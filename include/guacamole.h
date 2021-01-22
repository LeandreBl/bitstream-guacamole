#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

typedef struct guacamole_s {
    uint8_t *_bits;
    size_t _bits_count;
    size_t _max_bytes;
} guacamole_t;

int guacamole_create(guacamole_t *gm, size_t preallocated_size);
void guacamole_destroy(guacamole_t *gm);
ssize_t guacamole_write(guacamole_t *gm, const void *bits_buffer, size_t bits_count);
ssize_t guacamole_read(guacamole_t *gm, void *bits_buffer, size_t bits_count);
size_t guacamole_discard(guacamole_t *gm, size_t bits_count);