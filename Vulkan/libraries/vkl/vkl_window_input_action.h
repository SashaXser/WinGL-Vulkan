#ifndef _VKL_WINDOW_INPUT_ACTION_H_
#define _VKL_WINDOW_INPUT_ACTION_H_

#include <cstdint>

namespace vkl
{

enum class InputActions : uint8_t
{
   RELEASE = 0,
   PRESS = 1,
   REPEAT = 2
};

} // namespace vkl

#endif // _VKL_WINDOW_INPUT_ACTION_H_
