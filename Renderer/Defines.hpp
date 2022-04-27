#ifndef DEFINES_HPP
#define DEFINES_HPP

#include <cassert>

#define VK_CHECK(val)                  \
    do                                 \
    {                                  \
        if (val != VK_SUCCESS)         \
        {                              \
            assert(val == VK_SUCCESS); \
        }                              \
    } while (false)

#endif // DEFINES_HPP