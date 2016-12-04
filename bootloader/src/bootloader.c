/*
 * This file is part of StormLoader, the Storm Bootloader
 *
 * StormLoader is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StormLoader is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with StormLoader.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2014, Michael Andersen <m.andersen@eecs.berkeley.edu>
 */

#include <usart.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <flashcalw.h>
#include <spi.h>
#include <tc.h>
#include <freqm.h>

#include "info.h"

#include "bootloader.h"
#include <wdt_sam4l.h>
#include "ASF/common/services/clock/sam4l/sysclk.h"
#include "ASF/common/services/ioport/ioport.h"

uint8_t byte_escape;
uint8_t tx_stage_ram [TXBUFSZ];
uint16_t tx_ptr;
uint16_t tx_left;
uint8_t rx_stage_ram [RXBUFSZ];
uint16_t rx_ptr;

const sam_usart_opt_t bl_settings = {
     115200,
     US_MR_CHRL_8_BIT,
     US_MR_PAR_NO, //TODO change
     US_MR_NBSTOP_1_BIT,
     US_MR_CHMODE_NORMAL
};

void write_clock_constants(uint32_t val, uint32_t freq) {
  uint32_t addr =   0xfe00;
  uint32_t pagenum = addr >> 9;
  uint32_t inccount = *((volatile uint32_t*)(0xfe0c)) + 1;
  uint32_t contents [] = {0x69C0FFEE, val, freq, inccount};
  uint32_t *fp = (uint32_t*) addr;
  uint16_t i;
  flashcalw_default_wait_until_ready();
  flashcalw_erase_page(pagenum, true);
  flashcalw_picocache_invalid_all();
  flashcalw_default_wait_until_ready();
  flashcalw_clear_page_buffer();
  flashcalw_default_wait_until_ready();
  for (i=0; i < 4; i+=2)
  {
      *fp = 0xFFFFFFFF;
      *(fp+1) = 0xFFFFFFFF;
      *fp = contents[i];
      *(fp+1) = contents[i+1];
      fp+=2;
  }
  flashcalw_default_wait_until_ready();
  flashcalw_write_page(pagenum);
  flashcalw_picocache_invalid_all();
  flashcalw_default_wait_until_ready();
}

void lock_in_clock() {
  uint32_t tmp;
  uint32_t freq;
  uint8_t clock_ok = 0;
  int8_t last_dir = 0;
  uint32_t clk_maj;
  uint32_t clk_min;
  uint8_t dirty = 0;
  // We are going to use the clock from the radio to calibrate the internal RC32
  // clock. Now we can't be 100% sure that the radio clock is active, because
  // we don't power cycle that chip, so in the case that the clock is NOT
  // active, we bug out. We use a TC measurement to check this.

  //First stick the external clock into a timer, and see that it is
  //moving. It is possible that we have warm-restarted and the payload
  //has disabled the external clock. Not a problem, just don't try
  //the FREQM
  struct genclk_config cfg11;
  struct genclk_config cfg5;
  struct freqm_config fcfg;
  struct freqm_dev_inst fdev;
  genclk_config_defaults(&cfg11, 11);
  genclk_config_set_source(&cfg11, GENCLK_SRC_OSC0);
  genclk_config_set_divider(&cfg11, 5000);
  genclk_config_defaults(&cfg5, 5);
  genclk_config_set_source(&cfg5, GENCLK_SRC_OSC0);
  if (!osc_is_ready(OSC_ID_OSC0)) {
    osc_enable(OSC_ID_OSC0);
    for(tmp=48000;tmp>0;tmp--) {
      if (osc_is_ready(OSC_ID_OSC0)) {
        break;
      }
    }
  }
  if (!osc_is_ready(OSC_ID_OSC0)) {
    //The oscillator did not start properly
    return;
  }

  genclk_enable_source(GENCLK_SRC_OSC0);
  genclk_enable(&cfg11, 11);
  genclk_enable(&cfg5, 5);
  sysclk_enable_peripheral_clock(TC0);
  tc_init(TC0, 0,
    TC_CMR_CAPTURE_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE_1); //gc5
  tc_start(TC0, 0);
  for(tmp=0;tmp<480;tmp++) {
    volatile uint32_t _unused = tmp;
  }
  tmp = tc_read_cv(TC0, 0);
  if (tmp == 0) {
    return; //Clock is not moving
  }
  tmp = *((volatile uint32_t*)(0x400F0428));
  clk_maj = tmp >> 16;
  clk_min = tmp & 0xff;
  while(!clock_ok) {
    //Okay the reference clock is good. Lets shove gc11 into the FREQM
    freqm_get_config_defaults(&fcfg);
    fcfg.ref_clk = 3; //GCLK11 (/5000)
    fcfg.msr_clk = 7; //CLK32k
    fcfg.duration = 10; // So 1MHz / 5000 * 10 = 0.05 second. Count should then be 1638
    freqm_init(&fdev, FREQM, &fcfg);
    freqm_enable(&fdev);
    freqm_start_measure(&fdev);
    if (freqm_get_result_blocking(&fdev, &freq) != STATUS_OK) {
      return;
    }
    //We only correct for errors > 50 counts (1Khz or 3%)
    //unless we have already made a change and the new change would
    //be in the same direction
    if (freq < 1638) {
      //Too low
      if ((last_dir == 0 && freq > 1588) || last_dir == -1) {
        //We are within hysterises, don't correct OR
        //We have overshot, stick with the last setting
        goto commit;
      }
      dirty = 1;
      clk_min -= 1;
      last_dir = 1;
      if (clk_min <= 4) {
        //Range change
        clk_maj -= 4;
        clk_min = 0x20;
        last_dir = 0;
      }
    } else {
      //Too high
      if ((last_dir == 0 && freq < 1688) || last_dir == 1) {
        //We are within hysterises, don't correct OR
        //We have overshot, stick with the last setting
        goto commit;
      }
      dirty = 1;
      clk_min += 1;
      last_dir = -1;
      if (clk_min >= 60) {
        //Range change
        clk_maj += 4;
        clk_min = 0x20;
        last_dir = 0;
      }
    }
    tmp = (clk_maj << 16) + clk_min;
    *((volatile uint32_t*)(0x400F0418)) = 0xAA000028;
    *((volatile uint32_t*)(0x400F0428)) = tmp;
    *((volatile uint32_t*)(0x400F0418)) = 0xAA000024;
    *((volatile uint32_t*)(0x400F0424)) = 0x87; //enable temp compensation
    while((*((volatile uint32_t*)(0x400F0424)) & 2) == 0);
  }
commit:
  if (dirty || (*((volatile uint32_t*)(0xfe00)) != 0x69C0FFEE)) {
    //Write new constants to flash
    write_clock_constants((clk_maj << 16) + clk_min, freq*20);
  }
}

void bl_init(void)
{

    struct wdt_dev_inst wdt_inst;
    struct wdt_config   wdt_cfg;
    wdt_get_config_defaults(&wdt_cfg);
    wdt_init(&wdt_inst, WDT, &wdt_cfg);
    wdt_disable(&wdt_inst);

    byte_escape = 0;

    tx_ptr = tx_left = 0;
    tx_ptr = tx_left = 0;
    rx_ptr = 0;

    //Enable BL USART
    ioport_set_pin_mode(PIN_PB10A_USART3_TXD, MUX_PB10A_USART3_TXD);
    ioport_disable_pin(PIN_PB10A_USART3_TXD);
    ioport_set_pin_mode(PIN_PB09A_USART3_RXD, MUX_PB09A_USART3_RXD);
    ioport_disable_pin(PIN_PB09A_USART3_RXD);
    sysclk_enable_peripheral_clock(USART3);
    usart_reset(USART3);
    usart_init_rs232(USART3, &bl_settings, sysclk_get_main_hz());
    usart_enable_tx(USART3);
    usart_enable_rx(USART3);

    //Enable flash SPI
    ioport_set_pin_mode(PIN_PC05A_SPI_MOSI, MUX_PC05A_SPI_MOSI);
    ioport_disable_pin(PIN_PC05A_SPI_MOSI);
    ioport_set_pin_mode(PIN_PC04A_SPI_MISO, MUX_PC04A_SPI_MISO);
    ioport_disable_pin(PIN_PC04A_SPI_MISO);
    ioport_set_pin_mode(PIN_PC06A_SPI_SCK, MUX_PC06A_SPI_SCK);
    ioport_disable_pin(PIN_PC06A_SPI_SCK);
    ioport_set_pin_mode(PIN_PC03A_SPI_NPCS0, MUX_PC03A_SPI_NPCS0); //FL CS
    ioport_disable_pin(PIN_PC03A_SPI_NPCS0);
    ioport_set_pin_mode(PIN_PC01A_SPI_NPCS3, MUX_PC01A_SPI_NPCS3); //RAD CS
    ioport_disable_pin(PIN_PC01A_SPI_NPCS3);
    spi_enable_clock(SPI);
    spi_reset(SPI);
    spi_set_master_mode(SPI);
    spi_disable_mode_fault_detect(SPI);
    spi_disable_loopback(SPI);
    spi_set_peripheral_chip_select_value(SPI, spi_get_pcs(0));
    //spi_set_fixed_peripheral_select(SPI);
    spi_set_variable_peripheral_select(SPI);
    spi_disable_peripheral_select_decode(SPI);
    spi_set_delay_between_chip_select(SPI, 0);

    //Flash
    spi_set_transfer_delay(SPI, 0, FLASH_POSTCS_DELAY, FLASH_IXF_DELAY);
    spi_set_bits_per_transfer(SPI, 0, 8);
    spi_set_baudrate_div(SPI, 0, spi_calc_baudrate_div(FLASH_BAUD_RATE, sysclk_get_cpu_hz()));
    spi_configure_cs_behavior(SPI, 0, SPI_CS_KEEP_LOW);
    spi_set_clock_polarity(SPI, 0, 0); //SPI mode 0
    spi_set_clock_phase(SPI, 0, 1);

    spi_enable(SPI);

    _reset_xflash();
    bl_c_xfinit();
    lock_in_clock();
}

uint16_t bl_spi_xfer(uint8_t* send, uint8_t* recv, uint16_t len)
{
    uint16_t i;
    uint16_t recvd = 0;
    uint32_t timeout;
    //Throw away existing buffer
    while(spi_is_rx_full(SPI))
    {
        spi_get(SPI);
    }
    for (i = 0; i < len; i++)
    {
        for(timeout = 100000; timeout > 0 && !spi_is_tx_ready(SPI);timeout--);
        spi_write(SPI, send[i], spi_get_pcs(0), i==(len-1));
        for(timeout = 100000; timeout > 0 && !spi_is_rx_ready(SPI);timeout--);
        uint16_t dw;
        dw = spi_get(SPI);
        recv[recvd] = (uint8_t) dw;
        recvd ++;
    }

    return recvd;
}

uint16_t bl_spi_tx(uint8_t* send, uint16_t len, uint8_t do_end)
{
    uint16_t i;
    uint16_t recvd = 0;
    uint32_t timeout;
    for (i = 0; i < len; i++)
    {
        for(timeout = 100000; timeout > 0 && !spi_is_tx_empty(SPI);timeout--);
        spi_write(SPI, send[i], spi_get_pcs(0), (i==(len-1))&&do_end);
    }
    for(timeout = 100000; timeout > 0 && !spi_is_tx_ready(SPI);timeout--);

    return recvd;
}

uint16_t bl_spi_rx(uint8_t* recv, uint16_t len)
{
    uint16_t i;
    uint16_t recvd = 0;
    uint32_t timeout;
    //Throw away existing buffer
    while(spi_is_rx_full(SPI))
    {
        spi_get(SPI);
    }
    for (i = 0; i < len; i++)
    {
        for(timeout = 100000; timeout > 0 && !spi_is_tx_ready(SPI);timeout--);
        spi_write(SPI, 0x00, spi_get_pcs(0), i==(len-1));
        for(timeout = 100000; timeout > 0 && !spi_is_rx_ready(SPI);timeout--);
        uint16_t dw;
        dw = spi_get(SPI);
        recv[recvd] = (uint8_t) dw;
        recvd ++;
    }
    return recvd;
}

void bl_loop_poll(void)
{
    if (usart_is_rx_ready(USART3))
    {

        uint32_t ch;
        usart_getchar(USART3, &ch);
        if (rx_ptr >= RXBUFSZ)
        {
            tx_ptr = 0;
            tx_left = 1;
            tx_stage_ram[0] = RES_OVERFLOW;
        }
        else
        {
            bl_rxb(ch);
        }
    }
    if (usart_is_tx_ready(USART3))
    {
        if (tx_left > 0)
        {
            bl_txb(tx_stage_ram[tx_ptr++]);
            tx_left--;
        }
    }
}

void bl_txb(uint8_t b)
{
    usart_putchar(USART3, b);
}

void bl_rxb(uint8_t b)
{
    if (byte_escape && b == ESCAPE_CHAR)
    {
        byte_escape = 0;
        rx_stage_ram[rx_ptr++] = b;
    }
    else if (byte_escape)
    {
        bl_cmd(b);
        byte_escape = 0;
    }
    else if (b == ESCAPE_CHAR)
    {
        byte_escape = 1;
    }
    else
    {
        rx_stage_ram[rx_ptr++] = b;
    }

}

void bl_cmd(uint8_t b)
{
    switch(b)
    {
        case CMD_PING:
            bl_c_ping();
            break;
        case CMD_INFO:
            bl_c_info();
            break;
        case CMD_ID:
            bl_c_id();
            break;
        case CMD_RESET:
            bl_c_reset();
            break;
        case CMD_WPAGE:
            bl_c_wpage();
            break;
        case CMD_EPAGE:
            bl_c_epage();
            break;
        case CMD_XEBLOCK:
            bl_c_xeblock();
            break;
        case CMD_XWPAGE:
            bl_c_xwpage();
            break;
        case CMD_CRCRX:
            bl_c_crcrx();
            break;
        case CMD_RRANGE:
            bl_c_rrange();
            break;
        case CMD_XRRANGE:
            bl_c_xrrange();
            break;
        case CMD_SATTR:
            bl_c_sattr();
            break;
        case CMD_GATTR:
            bl_c_gattr();
            break;
        case CMD_CRCIF:
            bl_c_crcif();
            break;
        case CMD_CRCEF:
            bl_c_crcef();
            break;
        case CMD_XEPAGE:
            bl_c_xepage();
            break;
        case CMD_XFINIT:
            bl_c_xfinit();
            break;
        case CMD_CLKOUT:
            bl_c_clkout();
            break;
        case CMD_WUSER:
            bl_c_wuser();
            break;
        default:
            bl_c_unknown();
            break;
    }
}
