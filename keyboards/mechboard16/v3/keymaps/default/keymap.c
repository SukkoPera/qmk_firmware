/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2026 SukkoPera
 * 
 */

#include <stdbool.h>
#include <stdint.h>

#include QMK_KEYBOARD_H

#include "process_rgb_matrix.h"

enum custom_layers {
    _BASE = 0,
    _CMD,
};

// POSITIONAL
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_c16(
        /* ROW 0 */ KC_BSPC, KC_ENT,  KC_EQL,  KC_F8,   KC_F1,   KC_F2,             KC_F3,   KC_LBRC,
        /* ROW 1 */ KC_3,    KC_W,    KC_A,    KC_4,    KC_Z,    KC_S,              KC_E,    KC_LSFT,
        /* ROW 2 */ KC_5,    KC_R,    KC_D,    KC_6,    KC_C,    KC_F,              KC_T,    KC_X,
        /* ROW 3 */ KC_7,    KC_Y,    KC_G,    KC_8,    KC_B,    KC_H,              KC_U,    KC_V,
        /* ROW 4 */ KC_9,    KC_I,    KC_J,    KC_0,    KC_M,    KC_K,              KC_O,    KC_N,
        /* ROW 5 */ KC_DOWN, KC_P,    KC_L,    KC_UP,   KC_DOT,  KC_SCLN,           KC_MINS, KC_COMM,
        /* ROW 6 */ KC_LEFT, KC_BSLS, KC_QUOT, KC_RGHT, KC_GRV,  KC_INS,            KC_RBRC, KC_SLSH,
        /* ROW 7 */ KC_1,    KC_HOME, KC_TAB,  KC_2,    KC_SPC,  LT(_CMD, KC_LCTL), KC_Q,    KC_ESC,
        
        // The following row is fictional, it does not exist in the matrix and is only used to simulate extra keys
        /* ROW 8 */ KC_CAPS, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_END
    ),

    [_CMD] = LAYOUT_c16(
        /* ROW 0 */ EE_CLR,  KC_TRNS, RM_PREV, KC_TRNS, MS_BTN1, MS_BTN2, KC_TRNS, KC_MUTE,
        /* ROW 1 */ RM_SATD, KC_TRNS, KC_TRNS, RM_SATU, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 2 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 3 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 4 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 5 */ MS_DOWN, KC_TRNS, KC_TRNS, MS_UP,   RM_SPDU, RM_VALD, KC_VOLD, RM_SPDD,
        /* ROW 6 */ MS_LEFT, KC_TRNS, RM_VALU, MS_RGHT, KC_TRNS, RM_NEXT, KC_VOLU, RM_TOGG,
        /* ROW 7 */ RM_HUED, KC_TRNS, KC_TRNS, RM_HUEU, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        
        /* ROW 8 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    )
};

static const uint8_t FAKE_KEYS_ROW = 8;
static const uint8_t FAKE_KEY_SHIFTLOCK_COL = 0;
static const uint8_t FAKE_KEY_RSHIFT_COL = 1;

bool dip_switch_update_user(uint8_t index, bool active) {
        switch (index) {
                case 0:
                        action_exec(MAKE_KEYEVENT(FAKE_KEYS_ROW, FAKE_KEY_SHIFTLOCK_COL, active));
                        break;
                default:
                        break;
        }

        return true;
}

static uint8_t saved_mods; 
static uint16_t translated_key = KC_NO;
static uint16_t translated_from = KC_NO;

static uint16_t get_shifted_key(uint16_t keycode) {
    switch (keycode) {
        case KC_F1:   return KC_F4;
        case KC_F2:   return KC_F5;
        case KC_F3:   return KC_F6;
        case KC_F8:   return KC_F7;
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

        /* Some keys span multiple LEDs and they must all light up together. The way we achieve this is by having fake
         * key + led matrix entries that we trigger manually. Note action_exec will NOT have the desired effect.
         */
        switch (keycode) {
                case KC_SPC:
                        for (uint8_t col = 2; col <= 5; ++col) {
                                rgb_matrix_handle_key_event(FAKE_KEYS_ROW, col, record->event.pressed);
                        }
                        break;
                case KC_LSFT:
                        rgb_matrix_handle_key_event(FAKE_KEYS_ROW, FAKE_KEY_RSHIFT_COL, record->event.pressed);
                        break;
                default:
                        break;
        }

        return true;
}
