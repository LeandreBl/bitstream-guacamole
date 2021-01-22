#include <stdlib.h>
#include <string.h>

#include "guacamole_internal.h"

int guacamole_create(guacamole_t *gm, size_t n_bits)
{
    memset(gm, 0, sizeof(*gm));
    if (guacamole_extend_array(gm, n_bits) == -1)
        return -1;
    return 0;
}

void guacamole_destroy(guacamole_t *gm)
{
    free(gm->_bits);
}

ssize_t guacamole_write(guacamole_t *gm, const void *bits_buffer, size_t bits)
{
    uint8_t bit;

    if (guacamole_extend_array(gm, bits) == -1)
        return -1;
    for (size_t i = 0; i < bits; ++i) {
        bit = guacamole_get_bit(bits_buffer, i);
        guacamole_set_bit(gm->_bits, gm->_bits_count + i, bit);
    }
    gm->_bits_count += bits;
    return bits;
}

ssize_t guacamole_read(guacamole_t *gm, void *bits_buffer, size_t bits)
{
    uint8_t bit;

    if (bits > gm->_bits_count)
        bits = gm->_bits_count;
    for (size_t i = 0; i < bits; ++i) {
        bit = guacamole_get_bit(gm->_bits, i);
        guacamole_set_bit(bits_buffer, i, bit);
    }
    guacamole_shift(
        gm->_bits,
        guacamole_round_up(gm->_bits_count, SBITS(uint8_t)) / SBITS(uint8_t),
        bits);
    gm->_bits_count -= bits;
    return bits;
}

size_t guacamole_discard(guacamole_t *gm, size_t bits_count)
{
    if (bits_count > gm->_bits_count)
        bits_count = gm->_bits_count;
    guacamole_shift(
        gm->_bits,
        guacamole_round_up(gm->_bits_count, SBITS(uint8_t)) / SBITS(uint8_t),
        bits_count);
    gm->_bits_count -= bits_count;
    return bits_count;
}