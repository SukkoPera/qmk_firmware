/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2026 SukkoPera
 * 
 */

#include QMK_KEYBOARD_H

enum custom_layers {
    _BASE = 0,
    _CMD,
};

// POSITIONAL
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_c64(
        /* ROW 0 */ KC_BSPC, KC_ENT,  KC_RGHT, KC_F7,   KC_F1,   KC_F3,             KC_F5,   KC_DOWN,
        /* ROW 1 */ KC_3,    KC_W,    KC_A,    KC_4,    KC_Z,    KC_S,              KC_E,    KC_LSFT,
        /* ROW 2 */ KC_5,    KC_R,    KC_D,    KC_6,    KC_C,    KC_F,              KC_T,    KC_X,
        /* ROW 3 */ KC_7,    KC_Y,    KC_G,    KC_8,    KC_B,    KC_H,              KC_U,    KC_V,
        /* ROW 4 */ KC_9,    KC_I,    KC_J,    KC_0,    KC_M,    KC_K,              KC_O,    KC_N,
        /* ROW 5 */ KC_DOWN, KC_P,    KC_L,    KC_UP,   KC_DOT,  KC_SCLN,           KC_MINS, KC_COMM,
        /* ROW 6 */ KC_LEFT, KC_BSLS, KC_QUOT, KC_RGHT, KC_GRV,  KC_INS,            KC_RBRC, KC_SLSH,
        /* ROW 7 */ KC_1,    KC_HOME, KC_TAB,  KC_2,    KC_SPC,  LT(_CMD, KC_LCTL), KC_Q,    KC_ESC,
        
        // The following row is fictional, it does not exist in the matrix and is only used to simulate extra keys
        /* ROW 8 */ KC_CAPS, KC_GRV,  KC_NO,   KC_NO,   KC_NO,   KC_NO,             KC_NO,   KC_NO
    ),

    [_CMD] = LAYOUT_c64(
        /* ROW 0 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MUTE,
        /* ROW 1 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 2 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 3 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 4 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 5 */ MS_DOWN, KC_TRNS, KC_TRNS, MS_UP,   KC_TRNS, KC_TRNS, KC_VOLD, KC_TRNS,
        /* ROW 6 */ MS_LEFT, KC_TRNS, KC_TRNS, MS_RGHT, KC_TRNS, KC_TRNS, KC_VOLU, KC_TRNS,
        /* ROW 7 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        
        /* ROW 8 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    )
};
