#ifndef _WINDOW_DEFINES_H_
#define _WINDOW_DEFINES_H_

// std includes
#include <cstdint>

// defines a size structure
struct Size
{
   unsigned long  width;
   unsigned long  height;
};

// defines a point structure
struct Point
{
   intptr_t    x;
   intptr_t    y;
};

#endif // _WINDOW_DEFINES_H_
