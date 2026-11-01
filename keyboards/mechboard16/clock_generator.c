/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2026 SukkoPera
 * 
 */

#include "si5351.h"
#include "clock_generator.h"

static const uint32_t QUARTZ_FREQUENCY = 25000000UL;

// Keep these in the same order as enum clock from header file
static const uint64_t CLOCK_FREQUENCIES[] = {
	0,				// Disabled
	17734475ULL,	// System clock, PAL
	14318181ULL,	// System clock, NTSC
	1843200,		// ACIA Normal
	3686400,		// ACIA Double
	7372800,		// ACIA Quad
	3000000,		// ACIA Midi Hack
};

bool clockgen_init(void) {
	bool ret;
	
	if ((ret = si5351_init (SI5351_CRYSTAL_LOAD_8PF, QUARTZ_FREQUENCY, 0))) {
		si5351_drive_strength (SI5351_CLK0, SI5351_DRIVE_8MA);
		si5351_drive_strength (SI5351_CLK1, SI5351_DRIVE_8MA);
		si5351_drive_strength (SI5351_CLK2, SI5351_DRIVE_8MA);

		//~ uprintf("Clock generator initialized\n");
	} else {
		//~ uprintf("Cannot init clock generator\n");
	}

	return ret;
}

static void clockgen_set_frequency (enum si5351_clock clockId, uint64_t freq) {
	if (freq == 0) {
		si5351_set_freq (freq, clockId);
		si5351_output_enable (clockId, 0);
		si5351_set_clock_pwr (clockId, 0);
	} else {
		si5351_set_clock_pwr (clockId, 1);
		si5351_set_freq (freq * SI5351_FREQ_MULT, clockId);
		si5351_output_enable (clockId, 1);
	}
}

void clockgen_set_clock (uint8_t clockId, enum clock clock) {
	//~ Log.debug (F("Setting clock %d to %d\n"), static_cast<int> (clockId), static_cast<int> (clock));
	switch (clockId) {
		case 0:
			clockgen_set_frequency(SI5351_CLK0, CLOCK_FREQUENCIES[clock]);
			break;
		case 1:
			clockgen_set_frequency(SI5351_CLK1, CLOCK_FREQUENCIES[clock]);
			break;
		case 2:
			clockgen_set_frequency(SI5351_CLK2, CLOCK_FREQUENCIES[clock]);
			break;
		default:
			// Fail silently
			break;
	}
}
