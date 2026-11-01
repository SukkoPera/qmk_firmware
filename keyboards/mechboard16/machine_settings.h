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

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <avr/pgmspace.h>

#include "clock_generator.h"

struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct machine_settings {
    enum clock clock0;			// System Clock
    enum clock clock1;			// ACIA Clock
    enum clock clock2;			// Unused
    uint8_t rom_slot;
    struct color color;			// LED Color
    bool force_reset;		    // Force reset upon entering this configuration, otherwise it's only performed when ROM slot changes
};

extern const struct machine_settings* const machine_settings[] PROGMEM;
extern const uint8_t MACHINE_SETTINGS_NO;

void machine_configuration_init(void);
void machine_configuration_set (const uint8_t newConfiguration);
