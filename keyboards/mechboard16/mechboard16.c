/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2026 SukkoPera
 * 
 */

#include "clock_generator.h"
#include "machine_settings.h"
#include "pins.h"

#include QMK_KEYBOARD_H

void keyboard_post_init_kb(void) {
    // These are always outputs
    gpio_set_pin_output(PIN_RESET);
    gpio_set_pin_output(PIN_ROMSWITCH);
    gpio_set_pin_output(PIN_LED_R);
    gpio_set_pin_output(PIN_LED_G);
    gpio_set_pin_output(PIN_LED_B);

    // Init clock generator
    clockgen_init();

    // Set default machine configuration
    machine_configuration_init();

    // Continue with user init
    keyboard_post_init_user();
}
