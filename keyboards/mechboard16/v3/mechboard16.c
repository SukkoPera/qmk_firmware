/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2026 SukkoPera
 * 
 */

#include <stdbool.h>
#include <stdint.h>

#include QMK_KEYBOARD_H

static const uint8_t KEY_RESTORE_ROW = 8;
static const uint8_t KEY_RESTORE_COL = 0;

bool dip_switch_update_user(uint8_t index, bool active) {
    switch (index) {
        case 0:
			action_exec(MAKE_KEYEVENT(KEY_RESTORE_ROW, KEY_RESTORE_COL, active));
            break;
        case 1:
            if (active) {
                // switch 1 turned on
            } else {
                // switch 1 turned off
            }
            break;
        default:
			break;
    }
    
    return true;
}

static uint8_t saved_mods;
static uint16_t translated_key = KC_NO;
static uint16_t translated_from = KC_NO;

//~ void keyboard_post_init_user(void) {
//~ }

static uint16_t get_shifted_key(uint16_t keycode) {
    switch (keycode) {
        case KC_F1:   return KC_F2;
        case KC_F3:   return KC_F4;
        case KC_F5:   return KC_F6;
        case KC_F7:   return KC_F8;
        case KC_BSPC: return KC_INS;
        default:      return KC_NO;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        if (get_mods() & MOD_MASK_SHIFT) {
            uint16_t target = get_shifted_key(keycode);

            if (target != KC_NO) {
                saved_mods = get_mods();
                translated_key = target;
                translated_from = keycode;

                clear_mods();
                register_code(target);

                return false;
            }
        }
    } else if (keycode == translated_from) {
        unregister_code(translated_key);

        translated_key = KC_NO;
        translated_from = KC_NO;

        set_mods(saved_mods);

        return false;
    }

    return true;
}
