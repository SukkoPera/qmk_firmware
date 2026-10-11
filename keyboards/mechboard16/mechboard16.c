/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2026 SukkoPera
 * 
 */

#include <stdbool.h>
#include <avr/io.h>

#include "CircularBuffer.h"

#include QMK_KEYBOARD_H

/* volatile */ static circ_buf_t matrix_samples;

static bool use_passive_scanner = false;

static const pin_t pins_port_b[] = {
    B0, B1, B2, B3, B4, B5, B6, B7
};

static const pin_t pins_port_d[] = {
    D0, D1, D2, D3, D4, D5, D6, D7
};


// Pin-change ISR, called whenever the C16 polls the keyboard, all we do is snoop what is going on ;)
ISR (PCINT0_vect) {
    matrix_sample ms = {
	.rows = PINB,
	.cols = PIND
    };

    if (ms.rows != 0xFF) {
	cb_put(&matrix_samples, &ms);
    }
}

void matrix_init_passive(void) {
    /* Keyboard polling: TED drives the rows, which we have on PORT B, while the keyboard "outputs" the columns, which
     * we have on PORT D. They are all INPUTs by default se all we have to do is to enable the pin-change interrupts on
     * all pins of PORT B.
     */
    cli();
    PCMSK0 = (1 << PCINT7) | (1 << PCINT6) | (1 << PCINT5) | (1 << PCINT4) |
	     (1 << PCINT3) | (1 << PCINT2) | (1 << PCINT1) | (1 << PCINT0);
    PCICR |= (1 << PCIE0);
    // TODO: Clear pending interrupt flag?
    sei();
}

void matrix_init_active(void) {
/*    DDRB  = 0x00;   // Port B all inputs...
    PORTB = 0xFF;   // ... with pull-ups

   // Same for port D (the actual input port)
   PORTD = 0xFF;
   DDRD = 0x00;
*/
}

static unsigned int countSetBits (int n) {
    unsigned int count = 0;

    while (n) {
	n &= n - 1;
	++count;
    }

    return count;
}

void matrix_init_custom(void) {
    // These are always outputs
    gpio_set_pin_output(C6);
    gpio_set_pin_output(E6);
    gpio_set_pin_output(F1);
    gpio_set_pin_output(F2);
    gpio_set_pin_output(F4);

    // Start with all inputs with pull-ups
    for (uint8_t i = 0; i < ARRAY_SIZE(pins_port_b); ++i) {
        gpio_set_pin_input_high(pins_port_b[i]);
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(pins_port_d); ++i) {
        gpio_set_pin_input_high(pins_port_d[i]);
    }

    // Decide which keyboard scanner to use
    uint32_t start = timer_read32();
    while (timer_elapsed32(start) < 1500UL && !use_passive_scanner) {	// ms
        for (uint8_t i = 0; i < ARRAY_SIZE(pins_port_b); ++i) {
            if (gpio_read_pin(pins_port_b[i]) == 0) {
                // Activity on PORTB, assume the computer is scanning the matrix itself, so switch to the passive scanner
                use_passive_scanner = true;
		break;
            }
        }
    }

    if (use_passive_scanner) {
        uprintf("Using PASSIVE scanner\n");
        matrix_init_passive();
    } else {
	matrix_init_active();
    }
}

// Here 1 means pressed, 0 released
static bool matrix_scan_passive(matrix_row_t matrix[]) {
    bool matrix_has_changed = false;

    while (cb_available(&matrix_samples)) {
	matrix_sample sample = cb_get(&matrix_samples);

	/* When scanning the keyboard, the C16/+4 KERNAL first does a quick test to check if any key is pressed at all:
	 * it brings all the rows down and checks whether all cols are up or not. If at least one column is down, it
	 * goes on to check every individual row, as at least one key must be pressed and there's no other way to find
	 * out exactly which one, otherwise it takes no further action and just terminates the scan there.
	 */
	if (sample.rows == 0x00 && sample.cols == 0xFF) {
	    // All keys released
	    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
		if (matrix[row] != 0x00) {
		    matrix[row] = 0x00;
		    matrix_has_changed = true;
		}
	    }
	    uprintf ("deadscan\n");
	} else if (countSetBits (sample.rows) == 7) {
	    // Exactly one row is cleared, find out which one and update all its columns
	    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
		if ((sample.rows & (1 << row)) == 0) {
		    if ((matrix[row] & 0xFF) != (~sample.cols & 0xFF)) {
			uprintf ("Row %d, cols %u, mtx %u\n", row, (~sample.cols & 0xFF), (matrix[row] & 0xFF));
			matrix_has_changed = true;
			matrix[row] = ~sample.cols & 0xFF;
			break;		// There is necessarily only one row at 0
		    }
		}
	    }
	}
    }

    static bool last = false;
    if (last != matrix_has_changed) {
	uprintf("Scan, changed = %d\n", matrix_has_changed);
	last = matrix_has_changed;
    }

    return matrix_has_changed;
}

static bool matrix_scan_active(matrix_row_t matrix[]) {
    bool matrix_has_changed = false;

    /* Scan all rows */
    for (uint8_t row = 0; row < ARRAY_SIZE(pins_port_b); ++row) {
        // Set a single row to ground
	gpio_set_pin_output(pins_port_b[row]);
        gpio_write_pin_low(pins_port_b[row]);

        // Wait for things to settle and then read column output
        wait_us(30);
        matrix_row_t data = 0x00;
        for (uint8_t col = 0; col < ARRAY_SIZE(pins_port_d); ++col) {
            if (gpio_read_pin(pins_port_d[col]) == 0) {
                data |= (matrix_row_t) 1 << col;
            }
        }
        if (matrix[row] != data) {
            matrix_has_changed = true;
            matrix[row] = data;
        }

	gpio_set_pin_input_high(pins_port_b[row]);	// Back to Hi-Z
    }

    return matrix_has_changed;
}

bool matrix_scan_custom(matrix_row_t matrix[]) {
    if (use_passive_scanner) {
        return matrix_scan_passive(matrix);
    } else {
        return matrix_scan_active(matrix);
    }
}
