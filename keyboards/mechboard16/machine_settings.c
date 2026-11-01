/**
 * ___  ___          _    ______                     _  __    ____
 * |  \/  |         | |   | ___ \                   | |/  |  / ___|
 * | .  . | ___  ___| |__ | |_/ / ___   __ _ _ __ __| |`| | / /___
 * | |\/| |/ _ \/ __| '_ \| ___ \/ _ \ / _` | '__/ _` | | | | ___ \
 * | |  | |  __/ (__| | | | |_/ / (_) | (_| | | | (_| |_| |_| \_/ |
 * \_|  |_/\___|\___|_| |_\____/ \___/ \__,_|_|  \__,_|\___/\_____/
 *
 * Copyright (c) 2024-2025 SukkoPera <software@sukkology.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

#include "common.h"
#include "machine_settings.h"
#include "pins.h"

#include QMK_KEYBOARD_H

const struct machine_settings machinePal PROGMEM = {
    SYS_PAL,
    ACIA_NORMAL,
    DISABLED,
    0,
    {255, 0, 0},
    false
};

const struct machine_settings machinePalJiffyDos PROGMEM = {
    SYS_PAL,
    ACIA_NORMAL,
    DISABLED,
    1,
    {0, 255, 0},
    false
};

const struct machine_settings machinePalJiffyDosFastAcia PROGMEM = {
    SYS_PAL,
    ACIA_DOUBLE,
    DISABLED,
    1,
    {0, 255, 127},
    false
};

const struct machine_settings machinePalJiffyDosMidiHack PROGMEM = {
    SYS_PAL,
    ACIA_MIDIHACK,
    DISABLED,
    1,
    {127, 255, 127},
    false
};

// const struct machine_settings machineNtsc PROGMEM {
//     SYS_NTSC,
//     ACIA_NORMAL,
//     DISABLED,
//     1,
//     {0, 0, 255},
//     false
// };

const struct machine_settings* const machine_settings[] PROGMEM = {
    &machinePal,
    &machinePalJiffyDos,
    &machinePalJiffyDosFastAcia,
    &machinePalJiffyDosMidiHack,
    // &machineNtsc
};

const uint8_t MACHINE_SETTINGS_NO = ARRAY_SIZE(machine_settings);

/**********************************************************************************************************************/

typedef union {
    uint32_t raw;

    struct {
        uint8_t machine_config :2;
    };
} user_config_t;

static user_config_t user_config;

void eeconfig_init_kb(void) {  // EEPROM is getting reset!
    user_config.raw = 0;
    user_config.machine_config = 0;
    eeconfig_update_user(user_config.raw); // Write default value to EEPROM now

    machine_configuration_set (0);
}

void machine_configuration_init(void) {
     // Read the user config from EEPROM
    user_config.raw = eeconfig_read_kb();

	if (user_config.machine_config <= MACHINE_SETTINGS_NO) {
		machine_configuration_set (user_config.machine_config);
	} else {
		// Default configuration
		machine_configuration_set (0);
	}
}

void machine_configuration_set (const uint8_t newConfiguration) {
	if (newConfiguration != user_config.machine_config && newConfiguration < MACHINE_SETTINGS_NO) {
		uprintf("Setting configuration %d\n", newConfiguration);
		const struct machine_settings *oldSettingsPgmPtr = (const struct machine_settings *) pgm_read_ptr (&machine_settings[user_config.machine_config]);
		const struct machine_settings *newSettingsPgmPtr = (const struct machine_settings *) pgm_read_ptr (&machine_settings[newConfiguration]);
		struct machine_settings oldSettings, newSettings;
		memcpy_P (&oldSettings, oldSettingsPgmPtr, sizeof(struct machine_settings));
		memcpy_P (&newSettings, newSettingsPgmPtr, sizeof(struct machine_settings));
        uint32_t start = timer_read32();

		if (newSettings.force_reset || newSettings.rom_slot != oldSettings.rom_slot) {
//			Log.info (F("Reset start\n"));
            gpio_set_pin_output(PIN_RESET);
            gpio_write_pin_low(PIN_RESET);
		}

		clockgen_set_clock (0, newSettings.clock0);
		clockgen_set_clock (1, newSettings.clock1);
		clockgen_set_clock (2, newSettings.clock2);

		gpio_write_pin (PIN_ROMSWITCH, newSettings.rom_slot == 0 ? 0 : 1);

//		SoftPWMSet (PIN_LED_R, newSettings.color.r);
//		SoftPWMSet (PIN_LED_G, newSettings.color.g);
//		SoftPWMSet (PIN_LED_B, newSettings.color.b);

		if (newSettings.force_reset || newSettings.rom_slot != oldSettings.rom_slot) {
            while (timer_elapsed32(start) < RESET_LENGTH_MS)
				;
//			Log.info (F("Reset end\n"));
            gpio_set_pin_input_high(PIN_RESET);
		}

		user_config.machine_config = newConfiguration;
		eeconfig_update_kb(user_config.raw);
	}
}
