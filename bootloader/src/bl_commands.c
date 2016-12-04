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

#include "bootloader.h"
#include <stdlib.h>
#include <flashcalw.h>
#include <ioport.h>
#include <string.h>
#include "info.h"
// COMMAND HELPERS
// ===============

inline void _escape_cat(uint8_t *dat, uint16_t len);
inline void _escape_set(uint8_t *dat, uint16_t len, uint8_t cmd);
inline uint32_t _rx_u32(uint16_t offset);
inline uint16_t _rx_u16(uint16_t offset);

uint8_t _poll_xfbusy(void);

inline void _escape_cat(uint8_t *dat, uint16_t len)
{
    while(len-- && (tx_left < (TXBUFSZ-1)))
    {
        if (*dat == ESCAPE_CHAR)
            tx_stage_ram[tx_left++] = ESCAPE_CHAR;
        tx_stage_ram[tx_left++] = *dat;
        dat ++;
    }
}

inline void _escape_set(uint8_t *dat, uint16_t len, uint8_t cmd)
{
    tx_ptr = 0;
    tx_left = 2;
    tx_stage_ram[0] = ESCAPE_CHAR;
    tx_stage_ram[1] = cmd;
    if (len > 0)
    {
        _escape_cat(dat, len);
    }
}

inline uint32_t _rx_u32(uint16_t offset)
{
    uint32_t rv = ((uint32_t)rx_stage_ram[offset++]);
    rv |= ((uint32_t)rx_stage_ram[offset++] << 8);
    rv |= ((uint32_t)rx_stage_ram[offset++] << 16);
    rv |= ((uint32_t)rx_stage_ram[offset++] << 24);
    return rv;
}

inline uint16_t _rx_u16(uint16_t offset)
{
    uint16_t rv = ((uint16_t)rx_stage_ram[offset++]);
    rv |= ((uint16_t)rx_stage_ram[offset++] << 8);
    return rv;
}

uint8_t _poll_xfbusy()
{
    uint32_t timeout = 0;
    for (timeout = 50000; timeout > 0; timeout--)
    {
        uint8_t xf [] = {0xD7, 0x00, 0x00};
        bl_spi_xfer(xf,xf,3);
        if ((xf[2] >> 5) & 1) //Program/erase failure
            return 2;
        if (xf[2] >> 7)
            return 0;
    }
    return 1;
}

void _reset_xflash()
{
    uint8_t rst [] = {0xF0, 0x00, 0x00, 0x00};
    bl_spi_tx(rst, 4, 1);
}

// COMMANDS
// ========

void bl_c_ping()
{
    _escape_set(NULL, 0, RES_PONG);
}

void bl_c_info()
{
    char rv [193];
    memset(rv, 0, 193);
    uint8_t len = snprintf(&rv[1], 191, "StormLoader "BOOTLOADER_VERSION" ("BOOTLOADER_DATE")\n"
                      "Copyright 2014 Michael Andersen, UC Berkeley\n\n"
                      "This is free software, and you are welcome to redistribute it\n"
                      "See http://storm.pm/license for details\n");
    rv[0] = len;
    _escape_set((uint8_t*)&rv[0], 193, RES_INFO);
}

void bl_c_id()
{
    //Read the program info and stuff
    //Test holder for now
}

void bl_c_reset()
{
    tx_left = 0;
    rx_ptr = 0;
    tx_left = 0;
}

void bl_c_clkout()
{
    //pa19 as gclk0 (peripheral E)
    //DFLL/48
    //*((volatile uint32_t*)(0x400E0800 + 0x074)) = 0x00170203; //GCLK0=dfll/48
    //RCSYS NO DIV
    //*((volatile uint32_t*)(0x400E0800 + 0x074)) = 0x00170001;
    //RC32
    *((volatile uint32_t*)(0x400E0800 + 0x074)) = 0x00170d01;
    *((volatile uint32_t*)(0x400E1000 + 0x008)) = (1<<19); //disable GPIO
    *((volatile uint32_t*)(0x400E1000 + 0x168)) = (1<<19); //disable ST
    *((volatile uint32_t*)(0x400E1000 + 0x018)) = (1<<19); //pmr0c
    *((volatile uint32_t*)(0x400E1000 + 0x028)) = (1<<19); //pmr1c
    *((volatile uint32_t*)(0x400E1000 + 0x034)) = (1<<19); //pmr2s
    while(1);
}
void bl_c_epage()
{
    if (rx_ptr != (4))
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint32_t addr =   _rx_u32(0);

    uint32_t pagenum = addr >> 9;
    bool brv;

    if (addr < ALLOWED_FLASH_FLOOR || addr >= ALLOWED_FLASH_CEILING ||
            (addr & 511) != 0)
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }
    flashcalw_default_wait_until_ready();

    brv = flashcalw_quick_page_read(pagenum);
    if (brv)
    {   //This means the page is already erased
        _escape_set(NULL, 0, RES_OK);
        return;
    }
    flashcalw_default_wait_until_ready();
    brv = flashcalw_erase_page(pagenum, true);
    flashcalw_picocache_invalid_all();
    if (!brv)
    {
        _escape_set(NULL, 0, RES_INTERROR);
        return;
    }
    flashcalw_default_wait_until_ready();

    _escape_set(NULL, 0, RES_OK);
    return;

}

void bl_c_wpage()
{
    if (rx_ptr != (512+4))
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint32_t addr =   _rx_u32(0);

    uint32_t pagenum = addr >> 9;
    bool brv;

    if (addr < ALLOWED_FLASH_FLOOR || addr >= ALLOWED_FLASH_CEILING ||
            (addr & 511) != 0)
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }
    flashcalw_default_wait_until_ready();

    brv = flashcalw_erase_page(pagenum, true);
    flashcalw_picocache_invalid_all();
    if (!brv)
    {
        _escape_set(NULL, 0, RES_INTERROR);
        return;
    }
    flashcalw_default_wait_until_ready();

    flashcalw_clear_page_buffer();

    flashcalw_default_wait_until_ready();

    uint32_t *fp = (uint32_t*) addr;
    uint16_t i;
    for (i=4; i < 516; i+=8)
    {
        *fp = 0xFFFFFFFF;
        *(fp+1) = 0xFFFFFFFF;
        *fp = _rx_u32(i);
        *(fp+1) = _rx_u32(i+4);
        fp+=2;
    }

    flashcalw_default_wait_until_ready();

    flashcalw_write_page(pagenum);
    flashcalw_picocache_invalid_all();
    flashcalw_default_wait_until_ready();

    _escape_set(NULL, 0, RES_OK);
    return;

}

void bl_c_xeblock(void)
{
    if (rx_ptr != 4)
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint32_t baddr = _rx_u32(0);
    if (baddr < ALLOWED_XFLASH_FLOOR || baddr >= ALLOWED_XFLASH_CEILING
          ||  (baddr & (XBLOCKSIZE-1)) != 0)
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }
    uint8_t eblock [] = {0x50, 0, 0, 0};
    eblock[1] = (uint8_t) (baddr >> 16);
    eblock[2] = (uint8_t) (baddr >>  8);
    eblock[3] = (uint8_t) (baddr);
    bl_spi_tx(eblock, 4, 1);

    uint8_t iserr = _poll_xfbusy();
    if (iserr == 1)
    {
        _escape_set(NULL, 0, RES_XFTIMEOUT);
        return;
    }
    else if (iserr == 2)
    {
        _escape_set(NULL, 0, RES_XFEPE);
        return;
    }
    else
    {
        _escape_set(NULL, 0, RES_OK);
        return;
    }
}

void bl_c_xwpage(void)
{
    if (rx_ptr != 260)
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint32_t addr = _rx_u32(0);
    if (addr < ALLOWED_XFLASH_FLOOR || addr >= ALLOWED_XFLASH_CEILING
           || (addr & (XPAGESIZE-1)) != 0)
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }

    uint8_t startprog [] = {0x82, 0x00, 0x00, 0x00};
    startprog[1] = (uint8_t) (addr >> 16);
    startprog[2] = (uint8_t) (addr >>  8);
    startprog[3] = (uint8_t) (addr);
    bl_spi_tx(startprog, 4, 0);
    bl_spi_tx(&rx_stage_ram[4],256,1);
    uint8_t iserr = _poll_xfbusy();
    if (iserr == 1)
    {
        _escape_set(NULL, 0, RES_XFTIMEOUT);
        return;
    }
    else if (iserr == 2)
    {
        _escape_set(NULL, 0, RES_XFEPE);
        return;
    }
    else
    {
        _escape_set(NULL, 0, RES_OK);
        return;
    }
}

void bl_c_crcrx(void)
{
    uint8_t rv [6];
    rv[0] = (uint8_t) rx_ptr & 0xFF;
    rv[1] = (uint8_t) (rx_ptr >> 8);
    if (rx_ptr == 0)
    {
        rv[2] = rv[3] = rv[4] = rv[5] = 0xFF;
    }
    else
    {
        uint32_t crc = crc32(0, &rx_stage_ram[0],rx_ptr);
        rv[2] = (uint8_t) (crc & 0xFF); crc >>=8;
        rv[3] = (uint8_t) (crc & 0xFF); crc >>=8;
        rv[4] = (uint8_t) (crc & 0xFF); crc >>=8;
        rv[5] = (uint8_t) (crc & 0xFF);
    }
    _escape_set(rv, 6, RES_CRCRX);
}

void bl_c_rrange(void)
{
    if (rx_ptr != 6)
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint16_t len = _rx_u16(4);

    //We only go to half the buffer because of escape expanding
    if (rx_ptr != 6 || len >= (TXBUFSZ>>1))
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint32_t addr = _rx_u32(0);
    if ((addr+len) > ALLOWED_FLASH_CEILING+1)
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }
    flashcalw_picocache_invalid_all();
    uint8_t* p = (uint8_t*) addr;
    _escape_set(p, len, RES_RRANGE);
    return;
}

void bl_c_xrrange(void)
{
    if (rx_ptr != 6)
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint16_t len = _rx_u16(4);
    //We only go to half the buffer because of escape expanding
    if (rx_ptr != 6 || len >= (TXBUFSZ>>1))
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint32_t addr = _rx_u32(0);
    if ((addr+len) > ALLOWED_XFLASH_CEILING+1)
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }

    uint8_t rd [] = {0x1B, 0x00, 0x00, 0x00, 0x00, 0x00};
    rd[1] = (uint8_t) (addr >> 16);
    rd[2] = (uint8_t) (addr >>  8);
    rd[3] = (uint8_t) (addr);
    bl_spi_tx(rd,6,0);
    //We store the data in RX stage so we can escape it into tx
    bl_spi_rx(rx_stage_ram, len);
    _escape_set(rx_stage_ram, len, RES_XRRANGE);
    return;
}

void bl_c_sattr(void)
{
    uint8_t vlen = rx_stage_ram[9];
    if (rx_ptr != 10+vlen)
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    //Attributes are stored in the first four pages of flash.
    //Each attribute is 64 bytes long.
    uint8_t idx = rx_stage_ram[0];
    if (idx >= 16 || vlen >=56)
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }
    uint32_t addr = idx*64;
    uint8_t fcmd [68];
    fcmd[0] = 0x58;
    fcmd[1] = (uint8_t) (addr >> 16);
    fcmd[2] = (uint8_t) (addr >>  8);
    fcmd[3] = (uint8_t) (addr);
    uint8_t i;
    for (i=0;i<8;i++) fcmd[4+i] = rx_stage_ram[1+i];
    fcmd[12] = vlen;
    for (i=0;i<vlen;i++) fcmd[13+i] = rx_stage_ram[10+i];
    bl_spi_tx(fcmd,13+vlen,1);

    uint8_t iserr = _poll_xfbusy();
    if (iserr == 1)
    {
        _escape_set(NULL, 0, RES_XFTIMEOUT);
        return;
    }
    else if (iserr == 2)
    {
        _escape_set(NULL, 0, RES_XFEPE);
        return;
    }
    else
    {
        _escape_set(NULL, 0, RES_OK);
        return;
    }
}

void bl_c_gattr(void)
{
    if (rx_ptr != 1)
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    if (rx_stage_ram[0] >= 16)
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }
    uint32_t addr = rx_stage_ram[0]*64;
    uint8_t buf[64];

    uint8_t rd [] = {0x1B, 0x00, 0x00, 0x00, 0x00, 0x00};
    rd[1] = (uint8_t) (addr >> 16);
    rd[2] = (uint8_t) (addr >>  8);
    rd[3] = (uint8_t) (addr);
    bl_spi_tx(rd,6,0);
    //We store the data in RX stage so we can escape it into tx
    bl_spi_rx(buf, 64);
    _escape_set(buf, 64, RES_GATTR);
    return;
}

void bl_c_crcif(void)
{
    if (rx_ptr != 8)
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint32_t base = _rx_u32(0);
    uint32_t len = _rx_u32(4);
    if(base >= ALLOWED_FLASH_CEILING || (base+len) > ALLOWED_FLASH_CEILING+1
            || (len >= 512*1024))
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }
    flashcalw_picocache_invalid_all();
    uint8_t* p = (uint8_t*) base;
    uint32_t crc = crc32(0, p, len);
    uint8_t rv [4];
    rv[0] = (uint8_t) (crc & 0xFF); crc >>=8;
    rv[1] = (uint8_t) (crc & 0xFF); crc >>=8;
    rv[2] = (uint8_t) (crc & 0xFF); crc >>=8;
    rv[3] = (uint8_t) (crc & 0xFF);

    _escape_set(rv, 6, RES_CRCIF);
}

void bl_c_crcef(void)
{
    if (rx_ptr != 8)
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint32_t len = _rx_u16(4);
    uint32_t addr = _rx_u32(0);
    if ((addr+len) > ALLOWED_XFLASH_CEILING+1)
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }

    uint32_t left = len;
    uint32_t crc = 0;
    while(left > 0)
    {
        uint16_t toread = 4096;
        if (toread > left) toread = left;
        uint8_t rd [] = {0x1B, 0x00, 0x00, 0x00, 0x00, 0x00};
        rd[1] = (uint8_t) (addr >> 16);
        rd[2] = (uint8_t) (addr >>  8);
        rd[3] = (uint8_t) (addr);
        bl_spi_tx(rd,6,0);
        //We store the data in RX stage so we can escape it into tx
        bl_spi_rx(&rx_stage_ram[0], toread);
        uint16_t i;
        crc = crc32(crc, &rx_stage_ram[0], toread);
        left -= toread;
        addr += toread;
    }

    uint8_t rv [4];
    rv[0] = (uint8_t) (crc & 0xFF); crc >>=8;
    rv[1] = (uint8_t) (crc & 0xFF); crc >>=8;
    rv[2] = (uint8_t) (crc & 0xFF); crc >>=8;
    rv[3] = (uint8_t) (crc & 0xFF);

    _escape_set(rv, 4, RES_CRCXF);
    return;
}

void bl_c_xepage(void)
{
    if (rx_ptr != 4)
    {
        _escape_set(NULL, 0, RES_BADARGS);
        return;
    }
    uint32_t addr = _rx_u32(0);
    if (addr < ALLOWED_XFLASH_FLOOR || addr >= ALLOWED_XFLASH_CEILING
          ||  (addr & (XPAGESIZE-1)) != 0)
    {
        _escape_set(NULL, 0, RES_BADADDR);
        return;
    }
    uint8_t epage [] = {0x81, 0, 0, 0};
    epage[1] = (uint8_t) (addr >> 16);
    epage[2] = (uint8_t) (addr >>  8);
    epage[3] = (uint8_t) (addr);
    bl_spi_tx(epage, 4, 1);

    uint8_t iserr = _poll_xfbusy();
    if (iserr == 1)
    {
        _escape_set(NULL, 0, RES_XFTIMEOUT);
        return;
    }
    else if (iserr == 2)
    {
        _escape_set(NULL, 0, RES_XFEPE);
        return;
    }
    else
    {
        _escape_set(NULL, 0, RES_OK);
        return;
    }
}

void bl_c_xfinit(void)
{
    uint8_t switch_to_264b [] = {0x3D, 0x2A, 0x80, 0xA6};
    bl_spi_tx(switch_to_264b, 4, 1);
    _escape_set(NULL, 0, RES_OK);
    return;
}

void bl_c_wuser(void)
{
  if (rx_ptr != 8)
  {
      _escape_set(NULL, 0, RES_BADARGS);
      return;
  }
  bool brv;

  flashcalw_default_wait_until_ready();

  brv = flashcalw_erase_user_page(true);

  flashcalw_picocache_invalid_all();
  if (!brv)
  {
      _escape_set(NULL, 0, RES_INTERROR);
      return;
  }
  flashcalw_default_wait_until_ready();

  flashcalw_clear_page_buffer();

  flashcalw_default_wait_until_ready();

  *((volatile uint32_t*)(0x00800004)) = 0xFFFFFFFF;
  *((volatile uint32_t*)(0x00800000)) = 0xFFFFFFFF;
  *((volatile uint32_t*)(0x00800004)) = _rx_u32(0);
  *((volatile uint32_t*)(0x00800000)) = _rx_u32(4);

  flashcalw_default_wait_until_ready();

  flashcalw_write_user_page();
  flashcalw_picocache_invalid_all();
  flashcalw_default_wait_until_ready();

  _escape_set(NULL, 0, RES_OK);
  return;
}

void bl_c_unknown()
{
    _escape_set(NULL, 0, RES_UNKNOWN);
    return;
}
