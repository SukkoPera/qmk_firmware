/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2026 SukkoPera
 * 
 */

#include "max7221.h"
#include "quantum.h"

#define MAX7221_SCK_PORT PORTF
#define MAX7221_SCK_DDR  DDRF
#define MAX7221_SCK_PIN  PF7

#define MAX7221_MOSI_PORT PORTF
#define MAX7221_MOSI_DDR  DDRF
#define MAX7221_MOSI_PIN  PF6

#define MAX7221_SS_PORT PORTF
#define MAX7221_SS_DDR  DDRF
#define MAX7221_SS_PIN  PF5

static uint8_t led_buffer[8];

static inline void sck_high(void)
{
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

static void max7221_write(uint8_t reg, uint8_t data)
{
    ss_low();

    max7221_write_byte(reg);
    max7221_write_byte(data);

    ss_high();
}

void max7221_init(void)
{
    MAX7221_SCK_DDR  |= (1 << MAX7221_SCK_PIN);
    MAX7221_MOSI_DDR |= (1 << MAX7221_MOSI_PIN);
    MAX7221_SS_DDR   |= (1 << MAX7221_SS_PIN);

    sck_low();
    mosi_low();
    ss_high();

    /* Decode mode: none */
    max7221_write(0x09, 0x00);

    /* Scan all 8 digits */
    max7221_write(0x0B, 0x07);

    /* Normal operation */
    max7221_write(0x0C, 0x01);

    /* Medium brightness */
    max7221_write(0x0A, 0x08);

    /* Clear display */
    max7221_set_all(0x00);
}

void max7221_set_led(uint8_t row, uint8_t col, bool on)
{
    if (row >= 8 || col >= 8)
        return;

    if (on)
        led_buffer[row] |= (1 << col);
    else
        led_buffer[row] &= ~(1 << col);
}

void max7221_set_row(uint8_t row, uint8_t value)
{
    if (row >= 8)
        return;

    led_buffer[row] = value;
}

void max7221_set_all(uint8_t value)
{
    for (uint8_t row = 0; row < 8; row++)
        led_buffer[row] = value;

    max7221_flush();
}

void max7221_set_brightness(uint8_t brightness)
{
    if (brightness > 15)
        brightness = 15;

    max7221_write(0x0A, brightness);
}

void max7221_flush(void)
{
    for (uint8_t row = 0; row < 8; row++)
        max7221_write(row + 1, led_buffer[row]);
}
