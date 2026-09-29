/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2026 SukkoPera
 * 
 */

#include "max7221.h"
#include "quantum.h"
#include "led_matrix_drivers.h"

#define MAX7221_SCK_PORT PORTF
#define MAX7221_SCK_DDR  DDRF
#define MAX7221_SCK_PIN  PF7

#define MAX7221_MOSI_PORT PORTF
#define MAX7221_MOSI_DDR  DDRF
#define MAX7221_MOSI_PIN  PF6

#define MAX7221_SS_PORT PORTF
#define MAX7221_SS_DDR  DDRF
#define MAX7221_SS_PIN  PF5

static uint8_t led_buffer[64];

static inline void sck_high(void) {
    MAX7221_SCK_PORT |= (1 << MAX7221_SCK_PIN);
}

static inline void sck_low(void)
{
    MAX7221_SCK_PORT &= ~(1 << MAX7221_SCK_PIN);
}

static inline void mosi_high(void)
{
    MAX7221_MOSI_PORT |= (1 << MAX7221_MOSI_PIN);
}

static inline void mosi_low(void)
{
    MAX7221_MOSI_PORT &= ~(1 << MAX7221_MOSI_PIN);
}

static inline void ss_high(void)
{
    MAX7221_SS_PORT |= (1 << MAX7221_SS_PIN);
}

static inline void ss_low(void)
{
    MAX7221_SS_PORT &= ~(1 << MAX7221_SS_PIN);
}

static void max7221_write_byte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++) {
        if (data & 0x80)
            mosi_high();
        else
            mosi_low();

        sck_high();
        sck_low();

        data <<= 1;
    }
}

static void max7221_write(uint8_t reg, uint8_t data) {
    ss_low();

    max7221_write_byte(reg);
    max7221_write_byte(data);

    ss_high();
}


/*********************/

static void max7221_set_value(int index, uint8_t value) {
    uprintf ("LED %d = %d\n", index, value);

    if (index < 0 || index >= 64)
        return;

    led_buffer[index] = value;
}

static void max7221_set_value_all(uint8_t value) {
    for (uint8_t i = 0; i < 64; i++) {
        led_buffer[i] = value;
    }
}

static void max7221_set_brightness(uint8_t value) {
    if (value > 15) {
        value = 15;
    }

    max7221_write(0x0A, value);
}

// col: 0=A 1=B 2=C 3=D 4=E 5=F 6=G 7=DP
//static const uint8_t seg_mask[8] = {
//    0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x80
////    1 << 6, 1 << 5, 1 << 4, 1 << 3, 1 << 2, 1 << 1, 1 << 0, 1 << 7
//};

static void max7221_flush(void)
{
    uint16_t sum = 0;
    uint8_t on = 0;

    for (uint8_t row = 0; row < 8; row++) {
        uint8_t data = 0;

        for (uint8_t col = 0; col < 8; col++) {
            uprintf ("row %d, col %d\n", row, col);
//            uint8_t effectiveRow = row;
//            uint8_t effectiveCol = (col + 8 - 1) % 8;
//            uint8_t v = led_buffer[effectiveRow * 8 + effectiveCol];
            uint8_t v = led_buffer[col * 8 + row];
            sum += v;
            if (v > 0) {
                ++on;
//                data |= seg_mask[col];
                data |= 1 << ((6 - col + 8) % 8);
            }
        }

        max7221_write(row + 1, data);
    }

    uint8_t avg = on ? sum / on : 0;     // media 0-255 su tutti i LED accesi
    uprintf ("sum = %u, on = %d, brightness = %d\n", sum, on, avg >> 4);
    max7221_set_brightness(avg >> 4);    // scala 0-255 -> 0-15
}

static void max7221_init(void)
{
    MAX7221_SCK_DDR  |= (1 << MAX7221_SCK_PIN);
    MAX7221_MOSI_DDR |= (1 << MAX7221_MOSI_PIN);
    MAX7221_SS_DDR   |= (1 << MAX7221_SS_PIN);

    sck_low();
    mosi_low();
    ss_high();

    max7221_write(0x09, 0x00);  // no decode
    max7221_write(0x0B, 0x07);  // scan digits 0..7
    max7221_write(0x0C, 0x01);  // normal operation
//    max7221_set_brightness(12);  // initial intensity

    /* Clear display */
    max7221_set_value_all(0);

    max7221_set_value(0, 255);

    max7221_flush();
}

const led_matrix_driver_t led_matrix_driver = {
    .init         = max7221_init,
    .set_value    = max7221_set_value,
    .set_value_all = max7221_set_value_all,
    .flush        = max7221_flush,
};


