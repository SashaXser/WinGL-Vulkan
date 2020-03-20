#ifndef _VKL_WINDOW_MOUSE_BUTTONS_H_
#define _VKL_WINDOW_MOUSE_BUTTONS_H_

#include <cstdint>

namespace vkl
{

enum class MouseButtons : uint8_t
{
   ONE = 0,
   TWO = 1,
   THREE = 2,
   FOUR = 3,
   FIVE = 4,
   SIX = 5,
   SEVEN = 6,
   EIGHT = 7,
   LEFT = ONE,
   RIGHT = TWO,
   MIDDLE = THREE
};

} // namespace vkl

#endif // _VKL_WINDOW_MOUSE_BUTTONS_H_
