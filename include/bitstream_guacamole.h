#pragma once

#include <stdbool.h>

#include "guacamole.h"

typedef struct bitstream_guacamole_s {
    int _fd;
    guacamole_t _gm;
} bitstream_guacamole_t;

#define GUACAMOLE_DEFAULT_SIZE 1024

int bitstream_guacamole_create(bitstream_guacamole_t *bsg, int fd);
int bitstream_guacamole_create_from_file(bitstream_guacamole_t *bsg, const char *filename);
void bitstream_guacamole_destroy(bitstream_guacamole_t *bsg);
ssize_t bitstream_guacamole_write(bitstream_guacamole_t *bsg, const void *bits_buffer, size_t bits_count);
ssize_t bitstream_guacamole_read(bitstream_guacamole_t *bsg, void *bits_buffer, size_t bits_count);