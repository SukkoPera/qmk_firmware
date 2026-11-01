/**
 * Copyright (c) 2024-2025 SukkoPera <software@sukkology.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define CB_SIZE 32   // capacity is CB_SIZE - 1 (one slot stays empty)

typedef struct {
    uint8_t rows;
    uint8_t cols;
} matrix_sample;

typedef struct {
    matrix_sample buf[CB_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} circ_buf_t;

static inline uint8_t cb_next(const circ_buf_t *cb) {
    return (cb->tail + 1) % CB_SIZE;
}

static inline void cb_begin(circ_buf_t *cb) {
    cb->head = 0;
    cb->tail = 0;
}

static inline bool cb_empty(const circ_buf_t *cb) {
    return cb->head == cb->tail;
}

static inline bool cb_full(const circ_buf_t *cb) {
    return cb_next(cb) == cb->head;
}

static inline uint8_t cb_available(const circ_buf_t *cb) {
    return (cb->tail + CB_SIZE - cb->head) % CB_SIZE;
}

static inline uint8_t cb_free(const circ_buf_t *cb) {
    return CB_SIZE - cb_available(cb);
}

static inline bool cb_put(circ_buf_t *cb, const matrix_sample *x) {
    if (cb_full(cb)) return false;
    cb->buf[cb->tail] = *x;
    cb->tail = cb_next(cb);
    return true;
}

static inline matrix_sample cb_get(circ_buf_t *cb) {
    matrix_sample x = cb->buf[cb->head];
    cb->head = (cb->head + 1) % CB_SIZE;
    return x;
}

static inline matrix_sample cb_peek(const circ_buf_t *cb) {
    return cb->buf[cb->head];
}

static inline matrix_sample cb_peek_n(const circ_buf_t *cb, uint8_t n) {
    return cb->buf[(cb->head + n) % CB_SIZE];
}

static inline matrix_sample *cb_pointer(circ_buf_t *cb) {
    return cb->buf + cb->head;
}
