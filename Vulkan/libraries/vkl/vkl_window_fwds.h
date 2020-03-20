#ifndef _VKL_WINDOW_FWDS_H_
#define _VKL_WINDOW_FWDS_H_

#include <cstdint>
#include <functional>
#include <memory>

namespace vkl
{

using ModifierKeyFlags = uint16_t;
enum class InputActions : uint8_t;
enum class KeyboardKeys : uint16_t;
enum class MouseButtons : uint8_t;

using InitWindowSystemHandle =
   std::shared_ptr< uintptr_t * >;

using WindowHandle =
   std::shared_ptr< uintptr_t * >;

using KeyCallbackFunc =
   std::function<
      void (
         void * const user_data,
         const WindowHandle & window,
         const KeyboardKeys key,
         const int32_t scan_code,
         const InputActions action,
         const ModifierKeyFlags modifier_flags ) >;

using MousePositionCallbackFunc =
   std::function<
      void (
         void * const user_data,
         const WindowHandle & window,
         const double x_pos,
         const double y_pos ) >;

using MouseButtonCallbackFunc =
   std::function<
      void (
         void * const user_data,
         const WindowHandle & window,
         const MouseButtons button,
         const InputActions action,
         const ModifierKeyFlags modifier_flags ) >;

using MouseScrollCallbackFunc =
   std::function<
      void (
         void * const user_data,
         const WindowHandle & window,
         const double x_offset,
         const double y_offset ) >;

using WindowCloseCallbackFunc =
   std::function<
      void (
         void * const user_data,
         const WindowHandle & window ) >;

using WindowResizeCallbackFunc =
   std::function<
      void (
         void * const user_data,
         const WindowHandle & window,
         const uint32_t width,
         const uint32_t height ) >;

} // namespace vkl

#endif // _VKL_WINDOW_FWDS_H_
