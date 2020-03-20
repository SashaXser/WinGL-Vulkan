#ifndef _VKL_WINDOW_MODIFIER_KEYS_H_
#define _VKL_WINDOW_MODIFIER_KEYS_H_

#include <cstdint>

namespace vkl
{

namespace ModifierKeyFlagBits
{
   constexpr uint16_t SHIFT = 0x0001;
   constexpr uint16_t CONTROL = 0x0002;
   constexpr uint16_t CTRL = CONTROL;
   constexpr uint16_t ALTERNATE = 0x0004;
   constexpr uint16_t ALT = ALTERNATE;
   constexpr uint16_t SUPER = 0x0008;
   constexpr uint16_t CAPS_LOCK = 0x0010;
   constexpr uint16_t NUM_LOCK = 0x0020;
};

using ModifierKeyFlags = uint16_t;

} // namespace vkl

#endif // _VKL_WINDOW_MODIFIER_KEYS_H_
