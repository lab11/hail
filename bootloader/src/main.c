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

#include <ioport.h>
extern void bl_init(void);
extern void bl_testloop(void);
extern void bl_loop_poll(void);
extern void bl_spi_test_loop(void);

#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include <wdt_sam4l.h>
#include <sysclk.h>
#include "bootloader.h"
#include "ASF/common/services/ioport/sam/ioport_gpio.h"
#include "ASF/common/services/ioport/ioport.h"
void board_init(void)
{
	/*
	struct wdt_dev_inst wdt_inst;
	struct wdt_config   wdt_cfg;
	wdt_get_config_defaults(&wdt_cfg);
	wdt_init(&wdt_inst, WDT, &wdt_cfg);
	wdt_disable(&wdt_inst);
	*/


	/* Initialize IOPORT */
	ioport_init();
	ioport_set_pin_dir(PIN_PA19, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_PB06, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_PB06, IOPORT_MODE_PULLUP | IOPORT_MODE_GLITCH_FILTER);
	bpm_set_clk32_source(BPM, BPM_CLK32_SOURCE_RC32K);
	sysclk_init();
}

int load_calib(void)
{
    uint32_t osc_calib;
    if (*((volatile uint32_t*)(0xfe00)) != 0x69C0FFEE)
    {
        return 1;
    }
    osc_calib = *((volatile uint32_t*)(0xfe04));
    *((volatile uint32_t*)(0x400F0418)) = 0xAA000028;
    *((volatile uint32_t*)(0x400F0428)) = osc_calib;
    *((volatile uint32_t*)(0x400F0418)) = 0xAA000024;
    *((volatile uint32_t*)(0x400F0424)) = 0x87; //enable temp compensation
    return 2;
}

extern void jump_into_user_code(void)  __attribute__((noreturn));

int main (void)
{
    uint32_t stable;
    board_init();
    load_calib();
/*
    //pa19 as gclk0 (peripheral E)
    //DFLL/48
    *((volatile uint32_t*)(0x400E0800 + 0x074)) = 0x00170201; //GCLK0=dfll/48
    //RCSYS NO DIV
    //*((volatile uint32_t*)(0x400E0800 + 0x074)) = 0x00170001;
    //RC32
    //*((volatile uint32_t*)(0x400E0800 + 0x074)) = 0x00170d01;
    *((volatile uint32_t*)(0x400E1000 + 0x008)) = (1<<19); //disable GPIO
    *((volatile uint32_t*)(0x400E1000 + 0x168)) = (1<<19); //disable ST
    *((volatile uint32_t*)(0x400E1000 + 0x018)) = (1<<19); //pmr0c
    *((volatile uint32_t*)(0x400E1000 + 0x028)) = (1<<19); //pmr1c
    *((volatile uint32_t*)(0x400E1000 + 0x034)) = (1<<19); //pmr2s
*/
    // Verify BL policy
    uint32_t active = 0;
    uint32_t inactive = 0;
    uint32_t samples = 10000;
    while(samples)
    {
        if (ioport_get_pin_level(PIN_PB06) == 0) active++;
        else inactive++;
        samples--;
    }

    if (active > inactive)
    {
        bl_init();
        while (1)
        {
            bl_loop_poll();
        }
    }
    else
    {
        jump_into_user_code();
    }
}
