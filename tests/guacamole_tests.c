#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "bitstream_guacamole.h"
#include "guacamole_internal.h"

Test(shift, less_than_byte)
{
    uint8_t bits = 0b10110000;

    guacamole_shift(&bits, 1, 3);
    cr_assert_eq(bits, 0b10000000);
}

Test(shift, more_than_byte)
{
    uint8_t bits[] = { 0b10111010, 0b00100010 };

    guacamole_shift(bits, sizeof(bits) / sizeof(*bits), 12);
    cr_assert_eq(bits[0], 0b00100000);
    cr_assert_eq(bits[1], 0b00000000);
}

Test(bit_at, get)
{
    uint8_t bits = 0b10001011;

    cr_assert_eq(guacamole_get_bit(&bits, 0), 1);
    cr_assert_eq(guacamole_get_bit(&bits, 1), 0);
    cr_assert_eq(guacamole_get_bit(&bits, 2), 0);
    cr_assert_eq(guacamole_get_bit(&bits, 3), 0);
    cr_assert_eq(guacamole_get_bit(&bits, 4), 1);
    cr_assert_eq(guacamole_get_bit(&bits, 5), 0);
    cr_assert_eq(guacamole_get_bit(&bits, 6), 1);
    cr_assert_eq(guacamole_get_bit(&bits, 7), 1);
}

Test(bit_at, set)
{
    uint8_t bits = 0;

    guacamole_set_bit(&bits, 0, 1);
    cr_assert_eq(bits, 0b10000000);
    guacamole_set_bit(&bits, 1, 0);
    cr_assert_eq(bits, 0b10000000);
    guacamole_set_bit(&bits, 2, 1);
    cr_assert_eq(bits, 0b10100000);
    guacamole_set_bit(&bits, 3, 0);
    cr_assert_eq(bits, 0b10100000);
    guacamole_set_bit(&bits, 4, 0);
    cr_assert_eq(bits, 0b10100000);
    guacamole_set_bit(&bits, 5, 1);
    cr_assert_eq(bits, 0b10100100);
    guacamole_set_bit(&bits, 6, 1);
    cr_assert_eq(bits, 0b10100110);
    guacamole_set_bit(&bits, 7, 0);
}

Test(guacamole, write_sub_byte)
{
    guacamole_t gm;
    uint8_t bits = 0b10100000;
    const uint8_t *p;

    cr_assert_eq(guacamole_create(&gm, 0), 0);
    cr_assert_eq(guacamole_write(&gm, &bits, 3), 3);
    cr_assert_eq(gm._bits_count, 3);
    p = (typeof(p))gm._bits;
    cr_assert_eq(p[0], 0b10100000);
    bits = 0b11010000;
    cr_assert_eq(guacamole_write(&gm, &bits, 4), 4);
    cr_assert_eq(gm._bits_count, 7);
    p = (typeof(p))gm._bits;
    cr_assert_eq(p[0], 0b10111010);
    guacamole_destroy(&gm);
}

Test(guacamole, write_bytes)
{
    guacamole_t gm;
    uint8_t bits = 0b11100000;
    const uint8_t *p;

    cr_assert_eq(guacamole_create(&gm, 0), 0);
    cr_assert_eq(guacamole_write(&gm, &bits, 4), 4);
    cr_assert_eq(gm._bits_count, 4);
    p = (typeof(p))gm._bits;
    cr_assert_eq(p[0], 0b11100000);
    bits = 0b11100000;
    cr_assert_eq(guacamole_write(&gm, &bits, 3), 3);
    cr_assert_eq(gm._bits_count, 7);
    cr_assert_eq(p[0], 0b11101110);
    bits = 0b10100110;
    cr_assert_eq(guacamole_write(&gm, &bits, 8), 8);
    cr_assert_eq(gm._bits_count, 15);
    cr_assert_eq(guacamole_write(&gm, &bits, 8), 8);
    cr_assert_eq(gm._bits_count, 23);
    cr_assert_eq(guacamole_write(&gm, &bits, 8), 8);
    cr_assert_eq(gm._bits_count, 31);
    cr_assert_eq(guacamole_write(&gm, &bits, 8), 8);
    cr_assert_eq(gm._bits_count, 39);
    cr_assert_eq(guacamole_write(&gm, &bits, 8), 8);
    cr_assert_eq(gm._bits_count, 47);
    cr_assert_eq(guacamole_write(&gm, &bits, 8), 8);
    cr_assert_eq(gm._bits_count, 55);
    p = (typeof(p))gm._bits;
    cr_assert_eq(p[0], 0b11101111);
    cr_assert_eq(p[1], 0b01001101);
    cr_assert_eq(p[2], 0b01001101);
    cr_assert_eq(p[3], 0b01001101);
    cr_assert_eq(p[4], 0b01001101);
    cr_assert_eq(p[5], 0b01001101);
    cr_assert_eq(p[6], 0b01001100);
    guacamole_destroy(&gm);
}

Test(guacamole, read)
{
    // I use single byte to write, to avoid endianness problem is write
    guacamole_t gm;
    uint8_t bits = 0b10110111;
    uint8_t res;

    cr_assert_eq(guacamole_create(&gm, 0), 0);
    cr_assert_eq(guacamole_write(&gm, &bits, SBITS(bits)), SBITS(bits));
    cr_assert_eq(gm._bits_count, SBITS(bits));
    bits = 0b10001100;
    cr_assert_eq(guacamole_write(&gm, &bits, SBITS(bits)), SBITS(bits));
    cr_assert_eq(gm._bits_count, SBITS(bits) * 2);
    res = 0;
    cr_assert_eq(guacamole_read(&gm, &res, 3), 3);
    cr_assert_eq(gm._bits_count, 13);
    cr_assert_eq(res, 0b10100000);
    res = 0;
    cr_assert_eq(guacamole_read(&gm, &res, 7), 7);
    cr_assert_eq(gm._bits_count, 6);
    cr_assert_eq(res, 0b10111100);
    res = 0;
    cr_assert_eq(guacamole_read(&gm, &res, 8), 6);
    cr_assert_eq(gm._bits_count, 0);
    cr_assert_eq(res, 0b00110000);
}

Test(guacamole, discard)
{
    guacamole_t gm;
    uint8_t bits = 0b11100000;
    const uint8_t *p;

    cr_assert_eq(guacamole_create(&gm, 0), 0);
    cr_assert_eq(guacamole_write(&gm, &bits, 4), 4);
    cr_assert_eq(gm._bits_count, 4);
    bits = 0b11100000;
    cr_assert_eq(guacamole_write(&gm, &bits, 3), 3);
    cr_assert_eq(gm._bits_count, 7);
    cr_assert_eq(guacamole_discard(&gm, 5), 5);
    cr_assert_eq(gm._bits_count, 2);
    p = (typeof(p))gm._bits;
    cr_assert_eq(p[0], 0b11000000);
    cr_assert_eq(guacamole_discard(&gm, 1), 1);
    cr_assert_eq(gm._bits_count, 1);
    p = (typeof(p))gm._bits;
    cr_assert_eq(p[0], 0b10000000);
    cr_assert_eq(guacamole_discard(&gm, 4), 1);
    cr_assert_eq(gm._bits_count, 0);
    p = (typeof(p))gm._bits;
    cr_assert_eq(p[0], 0b00000000);
}

Test(guacamole_bitstream, write)
{
    bitstream_guacamole_t bsg;
    uint8_t bits = 0b11000000;
    FILE *file;

    cr_assert_eq(bitstream_guacamole_create_from_file(&bsg, "test.bits"), 0);
    cr_assert_eq(bitstream_guacamole_write(&bsg, &bits, 3), 0);
    bits = 0b01110000;
    cr_assert_eq(bitstream_guacamole_write(&bsg, &bits, 5), 8);
    bits = 0b11111111;
    cr_assert_eq(bitstream_guacamole_write(&bsg, &bits, 8), 8);
    file = fopen("test.bits", "r");
    cr_assert_neq(file, NULL);
    cr_assert_file_contents_eq_str(file, "\xCE\xFF");
    fclose(file);
    unlink("test.bits");
}