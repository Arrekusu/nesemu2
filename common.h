#pragma once

#include <cstring>
#include <cerrno>

static error_t memcpy_s(void *dest, size_t dest_size, const void *src, size_t count)
{
    if (dest_size < count)
        return ERANGE;

    memcpy(dest, src, count);

    return 0;
}
