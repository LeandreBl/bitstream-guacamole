#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "bitstream_guacamole.h"
#include "guacamole_internal.h"

int bitstream_guacamole_create(bitstream_guacamole_t *bsg, int fd)
{
    memset(bsg, 0, sizeof(*bsg));
    bsg->_fd = dup(fd);
    if (bsg->_fd == -1)
        return -1;
    return guacamole_create(&bsg->_gm, GUACAMOLE_DEFAULT_SIZE);
}

int bitstream_guacamole_create_from_file(
    bitstream_guacamole_t *bsg, const char *filename)
{
    int fd = open(filename, O_RDWR | O_CREAT, 0666);

    if (fd == -1 || bitstream_guacamole_create(bsg, fd) == -1)
        return -1;
    return 0;
}

void bitstream_guacamole_destroy(bitstream_guacamole_t *bsg)
{
    guacamole_destroy(&bsg->_gm);
    close(bsg->_fd);
}

ssize_t bitstream_guacamole_write(
    bitstream_guacamole_t *bsg, const void *bits_buffer, size_t bits_count)
{
    ssize_t wr = guacamole_write(&bsg->_gm, bits_buffer, bits_count);
    size_t writable;

    if (wr == -1)
        return -1;
    writable = bsg->_gm._bits_count / __CHAR_BIT__;
    if (writable > 0) {
        wr = write(bsg->_fd, bsg->_gm._bits, writable);
        if (wr == -1)
            return -1;
        guacamole_discard(&bsg->_gm, wr);
        return wr * SBITS(uint8_t);
    }
    return 0;
}

ssize_t bitstream_guacamole_read(
    bitstream_guacamole_t *bsg, void *bits_buffer, size_t bits_count)
{
    uint8_t buffer[GUACAMOLE_DEFAULT_SIZE] = { 0 };
    size_t total = 0;
    ssize_t rd;
    ssize_t wr;

    do {
        rd = read(bsg->_fd, buffer, sizeof(buffer));
        if (rd == -1)
            return -1;
        wr = guacamole_write(&bsg->_gm, buffer, rd * SBITS(uint8_t));
        if (wr == -1)
            return -1;
        total += wr;
    } while (rd == sizeof(buffer));
    return guacamole_read(&bsg->_gm, bits_buffer, bits_count);
}
