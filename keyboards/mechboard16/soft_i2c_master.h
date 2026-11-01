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
#ifndef SOFT_I2C_MASTER_H
#define SOFT_I2C_MASTER_H
/**
 * @file
 * @brief AVR Software I2C library
 *
 * @defgroup softI2C Software I2C
 * @details  Software Two Wire Interface library.
 * @{
 */
#if defined(__AVR__) || defined(DOXYGEN)  // AVR only

#include <stdbool.h>
#include <stddef.h>

//#include <Arduino.h>
#include <util/delay_basic.h>
//#include "DigitalPin.h"
#include "I2cConstants.h"
//------------------------------------------------------------------------------
// State codes.

/** Stop condition transmitted. */
static const uint8_t STATE_STOP = 0;

/** Repeated start condition transmitted. */
static const uint8_t STATE_REP_START = 1;

/** Read data transfer active. */
static const uint8_t STATE_RX_DATA = 2;

/** Write data transfer active. */
static const uint8_t STATE_TX_DATA = 3;

/** Slave address plus read bit transmitted, NACK received. */
static const uint8_t STATE_RX_ADDR_NACK = 4;

/** Slave address plus write bit transmitted, NACK received. */
static const uint8_t STATE_TX_ADDR_NACK = 5;
/** Data byte transmitted, NACK received. */
static const uint8_t STATE_TX_DATA_NACK = 6;

#if 0
//==============================================================================
/**
 * @class I2cMasterBase
 * @brief Base class for FastI2cMaster, SoftI2cMaster
 */
  I2cMasterBase() : _state(STATE_STOP) {}
#endif

#if 0
  /** Read a byte
   *
   * @note This function should only be used by experts. Data should be
   *       accessed by calling transfer() and transferContinue()
   *
   * @param[in] last send a NACK to terminate read if last is true else
   *            send an ACK to continue the read.
   *
   * @return byte read from I2C bus
   */
  uint8_t i2c_read(uint8_t last);

  /** Issue a start condition
   *
   * @note This function should only be used by experts. Data should be
   *       accessed by calling transfer() and transferContinue()
   */
  void i2c_start(void);
  /** Issue a stop condition.
   *
   * @note This function should only be used by experts. Data should be
   *       accessed by calling transfer() and transferContinue()
   */
  void i2c_stop(void);
#endif

  bool i2c_transfer(uint8_t addressRW, void *buf, size_t nbyte);
  bool i2c_transfer_opts(uint8_t addressRW, void *buf, size_t nbyte, uint8_t option);

  bool i2c_transferContinue(void *buf, size_t nbyte);
  bool i2c_transferContinue_opts(void *buf, size_t nbyte, uint8_t option);

  /** Write a byte
   *
   * @note This function should only be used by experts. Data should be
   *       accessed by calling transfer() and transferContinue()
   *
   * @param[in] data byte to write
   * @return true for ACK or false for NACK */
  //~ bool i2c_write(uint8_t data);


//==============================================================================
/**
 * @class SoftI2cMaster
 * @brief AVR Software I2C master class
 */
void i2c_begin(void);

#if 0
 private:
  uint8_t _sclBit;
  uint8_t _sdaBit;
  volatile uint8_t* _sclDDR;
  volatile uint8_t* _sdaDDR;
  volatile uint8_t* _sdaInReg;
  //----------------------------------------------------------------------------
  bool readSda() {return *_sdaInReg & _sdaBit;}
  //----------------------------------------------------------------------------
  void sclDelay(uint8_t n) {_delay_loop_1(n);}
  //----------------------------------------------------------------------------
  void writeScl(bool value) {
    uint8_t s = SREG;
    noInterrupts();
    if (value == LOW) {
      // Pull scl low.
      *_sclDDR |= _sclBit;
    } else {
      // Put scl in high Z  input mode.
      *_sclDDR &= ~_sclBit;
    }
    SREG = s;
  }
  //----------------------------------------------------------------------------
  void writeSda(bool value) {
    uint8_t s = SREG;
    noInterrupts();
    if (value == LOW) {
      // Pull sda low.
      *_sdaDDR |= _sdaBit;
    } else {
      // Put sda in high Z input mode.
      *_sdaDDR &= ~_sdaBit;
    }
    SREG = s;
  }
};
#endif

#endif  // __AVR__
#endif  // SOFT_I2C_MASTER_H
/** @} */

