#pragma once

#include "guacamole.h"

#define SBITS(v) (sizeof(v) * __CHAR_BIT__)

int guacamole_extend_array(guacamole_t *gm, size_t bits);

uint8_t guacamole_get_bit(const void *bits_buffer, size_t pos);

void guacamole_set_bit(void *bits_buffer, size_t pos, uint8_t value);

size_t guacamole_round_up(size_t value, size_t max);

void guacamole_shift(uint8_t *bits, size_t size, size_t n);