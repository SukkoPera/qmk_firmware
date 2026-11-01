/* Arduino DigitalIO Library
 * Copyright (C) 2013 by William Greiman
 *
 * This file is part of the Arduino DigitalIO Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino DigitalIO Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#if defined(__AVR__) || defined(DOXYGEN)  // AVR only
/**
 * @file
 * @brief AVR Software I2C library
 *
 * @defgroup softI2C Software I2C
 * @details  Software Two Wire Interface library.
 * @{
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "soft_i2c_master.h"

// Set pin numbers for your configuration.
//const uint8_t SDA_PIN = 31;             // Pin 33
//const uint8_t SCL_PIN = 13;             // Pin 32

enum {
	LOW = 0,
	HIGH
};

// SDA = PE2, SCL = PC7
static volatile uint8_t* _sdaDDR = &DDRE;
static volatile uint8_t* _sdaInReg = &PINE;
static volatile uint8_t* sdaOutReg = &PORTE;
static const uint8_t _sdaBit = 1 << 2;

static volatile uint8_t* _sclDDR = &DDRC;
static volatile uint8_t* sclOutReg = &PORTC;
static const uint8_t _sclBit = 1 << 7;

static uint8_t _state;

uint8_t i2c_read(uint8_t last);
void i2c_start(void);
void i2c_stop(void);
bool i2c_write(uint8_t b);

static bool readSda(void) {
	return *_sdaInReg & _sdaBit;
}

static void sclDelay(uint8_t n) {
	_delay_loop_1(n * 10);
}

static void writeScl(bool value) {
		uint8_t s = SREG;
		cli();
		if (value == LOW) {
			// Pull scl low.
			*_sclDDR |= _sclBit;
		} else {
			// Put scl in high Z  input mode.
			*_sclDDR &= ~_sclBit;
		}
		SREG = s;
}

static void writeSda(bool value) {
		uint8_t s = SREG;
		cli ();
		if (value == LOW) {
			// Pull sda low.
			*_sdaDDR |= _sdaBit;
		} else {
			// Put sda in high Z input mode.
			*_sdaDDR &= ~_sdaBit;
		}
		SREG = s;
	}

//------------------------------------------------------------------------------
/**
 * Start an I2C transfer with possible continuation.
 *
 * @param[in] addrRW    I2C slave address plus R/W bit.
 *                      The I2C slave address is in the high seven bits
 *                      and is ORed with on of the following:
 *                      - I2C_READ for a read transfer.
 *                      - I2C_WRITE for a write transfer.
 *                      .
 * @param[in,out] buf   Source or destination for transfer.
 * @param[in] nbytes    Number of bytes to transfer (may be zero).
 * @param[in] option    Option for ending the transfer, one of:
 *                      - I2C_STOP end the transfer with an I2C stop
 *                        condition.
 *                      - I2C_REP_START end the transfer with an I2C
 *                        repeated start condition.
 *                      - I2C_CONTINUE allow additional transferContinue()
 *                        calls.
 *                      .
 * @return true for success else false.
 */
bool i2c_transfer(uint8_t addrRW, void *buf, size_t nbytes) {
	return i2c_transfer_opts(addrRW, buf, nbytes, I2C_STOP);
}

bool i2c_transfer_opts(uint8_t addrRW, void *buf, size_t nbytes, uint8_t option) {
	if (_state != STATE_REP_START) {
		i2c_start();
	}
	if (!i2c_write(addrRW)) {
		_state = addrRW & I2C_READ ? STATE_RX_ADDR_NACK : STATE_TX_ADDR_NACK;
		return false;
	}
	_state = addrRW & I2C_READ ? STATE_RX_DATA : STATE_TX_DATA;
	return i2c_transferContinue_opts(buf, nbytes, option);
}

/**
 * Continue an I2C transfer.
 *
 * @param[in,out] buf   Source or destination for transfer.
 * @param[in] nbytes    Number of bytes to transfer (may be zero).
 * @param[in] option    Option for ending the transfer, one of:
 *                      - I2C_STOP end the transfer with an I2C stop
 *                        condition.
 *                      - I2C_REP_START end the transfer with an I2C
 *                        repeated start condition.
 *                      - I2C_CONTINUE allow additional transferContinue()
 *                        calls.
 *                      .
 * @return true for success else false.
 */
bool i2c_transferContinue(void *buf, size_t nbytes) {
	return i2c_transferContinue_opts(buf, nbytes, I2C_STOP);
}

bool i2c_transferContinue_opts(void *buf, size_t nbytes, uint8_t option) {
	uint8_t* p = (uint8_t *) buf;
	if (_state == STATE_RX_DATA) {
		for (size_t i = 0; i < nbytes; i++) {
				p[i] = i2c_read(i == (nbytes - 1) && option != I2C_CONTINUE);
		}
	} else if (_state == STATE_TX_DATA) {
		for (size_t i = 0; i < nbytes; i++) {
			if (!i2c_write(p[i])) {
				_state = STATE_TX_DATA_NACK;
				return false;
			}
		}
	} else {
		return false;
	}
	if (option == I2C_STOP) {
		i2c_stop();
		_state = STATE_STOP;
	} else if (option == I2C_REP_START) {
		i2c_start();
		_state = STATE_STOP;
	}
	return true;
}

//------------------------------------------------------------------------------
/**
 * Initialize SCL/SDA pins and set the bus high.
 *
 * @param[in] sdaPin The software SDA pin number.
 *
 * @param[in] sclPin The software SCL pin number.
 */
void i2c_begin(void) {
#if 0
	uint8_t port;

	// Get bit mask and address of scl registers.
	_sclBit = digitalPinToBitMask(sclPin);
	port = digitalPinToPort(sclPin);
	_sclDDR = portModeRegister(port);
	volatile uint8_t* sclOutReg = portOutputRegister(port);

	// Get bit mask and address of sda registers.
	_sdaBit = digitalPinToBitMask(sdaPin);
	port = digitalPinToPort(sdaPin);
	_sdaDDR = portModeRegister(port);
	_sdaInReg = portInputRegister(port);
	volatile uint8_t* sdaOutReg = portOutputRegister(port);
#endif

	// Clear PORT bit for scl and sda.
	uint8_t s = SREG;
	cli();
	*sclOutReg &= ~_sclBit;
	*sdaOutReg &= ~_sdaBit;
	SREG = s;

	// Set scl and sda high.
	writeScl(HIGH);
	writeSda(HIGH);

	_state = STATE_STOP;
}
//------------------------------------------------------------------------------
/* Read a byte and send ACK if more reads follow else NACK to terminate read.
 *
 * @param[in] last Set true to terminate the read else false.
 *
 * @return The byte read from the I2C bus.
 */
uint8_t i2c_read(uint8_t last) {
	uint8_t b = 0;

	// Set sda to high Z mode for read.
	writeSda(HIGH);
	// Read a byte.
	for (uint8_t i = 0; i < 8; i++) {
		// Don't change this loop unless you verify the change with a scope.
		b <<= 1;
		sclDelay(16);
		writeScl(HIGH);
		sclDelay(12);
		if (readSda()) b |= 1;
		writeScl(LOW);
	}
	// send ACK or NACK
	writeSda(last);
	sclDelay(12);
	writeScl(HIGH);
	sclDelay(18);
	writeScl(LOW);
	writeSda(LOW);
	return b;
}
//------------------------------------------------------------------------------
/* Issue a start condition. */
void i2c_start(void) {
	if (!readSda()) {
		writeSda(HIGH);
		writeScl(HIGH);
		sclDelay(20);
	}
	writeSda(LOW);
	sclDelay(20);
	writeScl(LOW);
}
//------------------------------------------------------------------------------
	/*  Issue a stop condition. */
void i2c_stop(void) {
	writeSda(LOW);
	sclDelay(20);
	writeScl(HIGH);
	sclDelay(20);
	writeSda(HIGH);
	sclDelay(20);
}
//------------------------------------------------------------------------------
/*
 * Write a byte.
 *
 * @param[in] data The byte to send.
 *
 * @return The value true, 1, if the slave returned an ACK or false for NACK.
 */
bool i2c_write(uint8_t data) {
	// write byte
	for (uint8_t m = 0X80; m != 0; m >>= 1) {
		// don't change this loop unless you verify the change with a scope
		writeSda(m & data);
		sclDelay(8);
		writeScl(HIGH);
		sclDelay(18);
		writeScl(LOW);
	}
	sclDelay(8);
	// Go to sda high Z mode for input.
	writeSda(HIGH);
	writeScl(HIGH);
	sclDelay(16);

	// Get ACK or NACK.
	uint8_t rtn = readSda();

	// pull scl low.
	writeScl(LOW);

	// Pull sda low.
	writeSda(LOW);
	return rtn == 0;
}
#endif  // __AVR__
/** @} */

