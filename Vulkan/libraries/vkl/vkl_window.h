#ifndef _VKL_WINDOW_H_
#define _VKL_WINDOW_H_

#include "vkl_window_fwds.h"

#include <cstdint>
#include <initializer_list>

namespace vkl
{

InitWindowSystemHandle InitWindowSystem( );

void PollWindowSystem( );
bool PollWindowSystem(
   const std::initializer_list< WindowHandle > & windows );

WindowHandle CreateWindow(
   const int32_t width,
   const int32_t height,
   const char * const title );

bool SetWindowUserData(
   const WindowHandle & window,
   void * const data );

bool SetKeyCallback(
   const WindowHandle & window,
   KeyCallbackFunc && callback );

bool SetMousePositionCallback(
   const WindowHandle & window,
   MousePositionCallbackFunc && callback );

bool SetMouseButtonCallback(
   const WindowHandle & window,
   MouseButtonCallbackFunc && callback );

bool SetMouseScrollCallback(
   const WindowHandle & window,
   MouseScrollCallbackFunc && callback );

bool SetWindowCloseCallback(
   const WindowHandle & window,
   WindowCloseCallbackFunc && callback );

bool SetWindowResizeCallback(
   const WindowHandle & window,
   WindowResizeCallbackFunc && callback );

} // namespace vkl

#endif // _VKL_WINDOW_H_
