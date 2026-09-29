/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2026 SukkoPera
 * 
 */

#include QMK_KEYBOARD_H

#include "max7221.h"

enum custom_layers {
    _BASE = 0,
    _CMD,
};

// POSITIONAL
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        /* ROW 0 */ KC_BSPC, KC_ENT,  KC_EQL,  KC_F8,   KC_F1,   KC_F2,             KC_F3,   KC_LBRC,
        /* ROW 1 */ KC_3,    KC_W,    KC_A,    KC_4,    KC_Z,    KC_S,              KC_E,    KC_LSFT,
        /* ROW 2 */ KC_5,    KC_R,    KC_D,    KC_6,    KC_C,    KC_F,              KC_T,    KC_X,
        /* ROW 3 */ KC_7,    KC_Y,    KC_G,    KC_8,    KC_B,    KC_H,              KC_U,    KC_V,
        /* ROW 4 */ KC_9,    KC_I,    KC_J,    KC_0,    KC_M,    KC_K,              KC_O,    KC_N,
        /* ROW 5 */ KC_DOWN, KC_P,    KC_L,    KC_UP,   KC_DOT,  KC_SCLN,           KC_MINS, KC_COMM,
        /* ROW 6 */ KC_LEFT, KC_BSLS, KC_QUOT, KC_RGHT, KC_GRV,  KC_INS,            KC_RBRC, KC_SLSH,
        /* ROW 7 */ KC_1,    KC_HOME, KC_TAB,  KC_2,    KC_SPC,  LT(_CMD, KC_LCTL), KC_Q,    KC_ESC
    ),

    [_CMD] = LAYOUT(
        /* ROW 0 */ EE_CLR,  KC_TRNS, LM_PREV, KC_TRNS, MS_BTN1, MS_BTN2, KC_TRNS, KC_MUTE,
        /* ROW 1 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 2 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 3 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 4 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        /* ROW 5 */ MS_DOWN, KC_TRNS, KC_TRNS, MS_UP,   LM_SPDU, LM_BRID, KC_VOLD, LM_SPDD,
        /* ROW 6 */ MS_LEFT, KC_TRNS, LM_BRIU, MS_RGHT, KC_TRNS, LM_NEXT, KC_VOLU, LM_TOGG,
        /* ROW 7 */ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    )
};

static uint8_t saved_mods;
static uint16_t translated_key = KC_NO;
static uint16_t translated_from = KC_NO;

//~ void keyboard_post_init_user(void) {
        //~ led_matrix_enable();
        //~ led_matrix_mode(LED_MATRIX_SOLID);
//~ }

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

    return true;
}
