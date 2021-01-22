#include <stdlib.h>
#include <string.h>

#include "guacamole_internal.h"

uint8_t guacamole_get_bit(const void *bits_buffer, size_t pos)
{
    const uint8_t *buffer = bits_buffer;
    uint8_t offset = __CHAR_BIT__ - 1 - (pos % SBITS(*buffer));

    return buffer[pos / SBITS(*buffer)] >> offset & 1UL;
}

void guacamole_set_bit(void *bits_buffer, size_t pos, uint8_t value)
{
    uint8_t *buffer = bits_buffer;
    uint8_t offset = __CHAR_BIT__ - 1 - (pos % SBITS(value));

    buffer[pos / 8] = (buffer[pos / 8] & ~(1UL << offset)) | (value << offset);
}

size_t guacamole_round_up(size_t value, size_t max)
{
    return value += max - value % max;
}

int guacamole_extend_array(guacamole_t *gm, size_t bits)
{
    size_t count = gm->_bits_count + bits;
    uint8_t *new;

    if (count <= gm->_max_bytes * __CHAR_BIT__)
        return 0;
    count = guacamole_round_up(count, SBITS(uint8_t)) / SBITS(uint8_t);
    new = reallocarray(gm->_bits, gm->_max_bytes + count, sizeof(uint8_t));
    if (new == NULL)
        return -1;
    gm->_bits = new;
    memset(
        gm->_bits + gm->_max_bytes,
        0,
        (count - gm->_max_bytes) * sizeof(uint8_t));
    gm->_max_bytes = count;
    return 0;
}

void guacamole_shift(uint8_t *bits, size_t size, size_t n)
{
    size_t offset = n % SBITS(uint8_t);
    size_t steps = n / SBITS(uint8_t);

    size -= steps;
    if (steps > 0) {
        memmove(bits, bits + steps, size);
        memset(bits + size, 0, steps);
    }
    for (size_t i = 0; i < size - 1; ++i) {
        bits[i] <<= offset;
        bits[i] |= bits[i + 1] >> (SBITS(uint8_t) - offset);
    }
    bits[size - 1] <<= offset;
}
