/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2026 SukkoPera
 * 
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

enum clock {
    DISABLED = 0,
    SYS_PAL,
    SYS_NTSC,
    ACIA_NORMAL,			// Normal ACIA clock resulting in max 19200 bps
    ACIA_DOUBLE,			// Double ACIA clock, 38400 bps theoretically possible
    ACIA_QUAD,				// 76800 bps ;)
    ACIA_MIDIHACK,			/* This allows reaching the 31250 bps required for midi configuring the acia for 19200 bps
                             * (divisor = 96 => 3000000/96 = 31250)
                             */
};

bool clockgen_init(void);
void clockgen_set_clock (uint8_t clockId, enum clock clock);
