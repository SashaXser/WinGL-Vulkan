#include "vkl/vkl_window.h"
#include "vkl/vkl_window_input_action.h"
#include "vkl/vkl_window_keyboard_keys.h"
#include "vkl/vkl_window_modifier_keys.h"
#include "vkl/vkl_window_mouse_buttons.h"

#include <cstdint>
#include <chrono>
#include <ios>
#include <iostream>
#include <string>
#include <thread>

bool escape_pressed_ { false };

const char * DecodeAction(
   const vkl::InputActions action )
{
   const char * name = "Unknown";

   switch (action)
   {
   case vkl::InputActions::PRESS: name = "Press"; break;
   case vkl::InputActions::RELEASE: name = "Release"; break;
   case vkl::InputActions::REPEAT: name = "Repeat"; break;
   }

   return name;
};

std::string DecodeModifierFlags(
   const vkl::ModifierKeyFlags modifier_flags )
{
   std::string value;

   if (modifier_flags & vkl::ModifierKeyFlagBits::ALT)
      value += "ALT ";

   if (modifier_flags & vkl::ModifierKeyFlagBits::CAPS_LOCK)
      value += "CAPS LOCK ";

   if (modifier_flags & vkl::ModifierKeyFlagBits::CTRL)
      value += "CTRL ";

   if (modifier_flags & vkl::ModifierKeyFlagBits::NUM_LOCK)
      value += "NUM LOCK ";

   if (modifier_flags & vkl::ModifierKeyFlagBits::SHIFT)
      value += "SHIFT ";

   if (modifier_flags & vkl::ModifierKeyFlagBits::SUPER)
      value += "SUPER ";

   return value;
};

void KeyCallback(
   void * const user_data,
   const vkl::WindowHandle & window,
   const vkl::KeyboardKeys key,
   const int32_t scan_code,
   const vkl::InputActions action,
   const vkl::ModifierKeyFlags modifier_flags )
{
   const auto DecodeKey =
      [ ] ( const vkl::KeyboardKeys key )
      {
         std::string value;

         if (vkl::KeyboardKeys::SPACE <= key &&
             key <= vkl::KeyboardKeys::GRAVE_ACCENT)
         {
            value =
               static_cast< char >(key);
         }
         else
         {
            value =
               std::to_string(
                  static_cast< uint32_t >(key));
         }

         return value;
      };

   if (vkl::KeyboardKeys::ESCAPE == key &&
       vkl::InputActions::PRESS == action)
   {
      escape_pressed_ = true;
   }

   std::cout
      << "Key Callback" << "\n"
      << "User Data : 0x" << std::hex << user_data << "\n"
      << "Window : 0x" << *window << "\n"
      << "Key : " << std::dec << DecodeKey(key) << "\n"
      << "Scan Code : " << scan_code << "\n"
      << "Action : " << DecodeAction(action) << "\n"
      << "Modifer Flags : " << DecodeModifierFlags(modifier_flags) << "\n"
      << std::endl;
}

void MousePositionCallback(
   void * const user_data,
   const vkl::WindowHandle & window,
   const double x_pos,
   const double y_pos )
{
   std::cout
      << "Mouse Position Callback" << "\n"
      << "User Data : 0x" << std::hex << user_data << "\n"
      << "Window : 0x" << *window << "\n"
      << "Position : " << std::dec << x_pos << ", " << y_pos << "\n"
      << std::endl;
}

void MouseButtonCallback(
   void * const user_data,
   const vkl::WindowHandle & window,
   const vkl::MouseButtons button,
   const vkl::InputActions action,
   const vkl::ModifierKeyFlags modifier_flags )
{
   const auto DecodeButton =
      [ ] ( const vkl::MouseButtons button )
      {
         const char * name = "Unknown";

         switch (button)
         {
         case vkl::MouseButtons::LEFT: name = "Left"; break;
         case vkl::MouseButtons::RIGHT: name = "Right"; break;
         case vkl::MouseButtons::MIDDLE: name = "Middle"; break;
         case vkl::MouseButtons::FOUR: name = "Four"; break;
         case vkl::MouseButtons::FIVE: name = "Five"; break;
         case vkl::MouseButtons::SIX: name = "Six"; break;
         case vkl::MouseButtons::SEVEN: name = "Seven"; break;
         case vkl::MouseButtons::EIGHT: name = "Eight"; break;
         }

         return name;
      };

   std::cout
      << "Mouse Button Callback" << "\n"
      << "User Data : 0x" << std::hex << user_data << "\n"
      << "Window : 0x" << *window << "\n"
      << "Button : " << std::dec << DecodeButton(button) << "\n"
      << "Action : " << DecodeAction(action) << "\n"
      << "Modifer Flags : " << DecodeModifierFlags(modifier_flags) << "\n"
      << std::endl;
}

void MouseScrollCallback(
   void * const user_data,
   const vkl::WindowHandle & window,
   const double x_offset,
   const double y_offset )
{
   std::cout
      << "Mouse Scroll Callback" << "\n"
      << "User Data : 0x" << std::hex << user_data << "\n"
      << "Window : 0x" << *window << "\n"
      << "X Offset : " << std::dec << x_offset << "\n"
      << "Y Offset : " << y_offset << "\n"
      << std::endl;
}

void WindowCloseCallback(
   void * const user_data,
   const vkl::WindowHandle & window )
{
   std::cout
      << "Window Close Callback" << "\n"
      << "User Data : 0x" << std::hex << user_data << "\n"
      << std::dec
      << std::endl;
}

void WindowResizeCallback(
   void * const user_data,
   const vkl::WindowHandle & window,
   const uint32_t width,
   const uint32_t height )
{
   std::cout
      << "Window Resize Callback" << "\n"
      << "User Data : 0x" << std::hex << user_data << std::dec << "\n"
      << "Width : " << width << "\n"
      << "Height : " << height << "\n"
      << std::endl;
}

void SetupCallbacks(
   const vkl::WindowHandle & window_handle,
   const bool clear_callbacks )
{
   vkl::SetKeyCallback(
      window_handle,
      !clear_callbacks ?
         &KeyCallback :
         nullptr);

   vkl::SetMousePositionCallback(
      window_handle,
      !clear_callbacks ?
         &MousePositionCallback :
         nullptr);

   vkl::SetMouseButtonCallback(
      window_handle,
      !clear_callbacks ?
         &MouseButtonCallback :
         nullptr);

   vkl::SetMouseScrollCallback(
      window_handle,
      !clear_callbacks ?
         &MouseScrollCallback :
         nullptr);

   vkl::SetWindowCloseCallback(
      window_handle,
      !clear_callbacks ?
         &WindowCloseCallback :
         nullptr);

   vkl::SetWindowResizeCallback(
      window_handle,
      !clear_callbacks ?
         &WindowResizeCallback :
         nullptr);
}

int32_t main(
   const int32_t argc,
   const char * const argv[] )
{
   const auto init_win_sys_handle =
      vkl::InitWindowSystem();
   
   if (!init_win_sys_handle)
   {
      return -1;
   }

   const auto window_handle =
      vkl::CreateWindow(
         640, 480,
         "vkl Window Test");

   if (!window_handle)
   {
      return -2;
   }

   vkl::SetWindowUserData(
      window_handle,
      reinterpret_cast< void * >(
         0x1234ABCD));

   SetupCallbacks(
      window_handle,
      false);

   auto timeout =
      std::chrono::high_resolution_clock::now() +
      std::chrono::hours(24);

   while (!vkl::PollWindowSystem({window_handle}))
   {
      if (escape_pressed_)
      {
         SetupCallbacks(
            window_handle,
            true);

         timeout =
            std::chrono::high_resolution_clock::now() +
            std::chrono::seconds(10);

         escape_pressed_ = false;
      }

      if (std::chrono::high_resolution_clock::now() >=
          timeout)
      {
         SetupCallbacks(
            window_handle,
            false);

         timeout =
            std::chrono::high_resolution_clock::now() +
            std::chrono::hours(24);
      }

      std::this_thread::sleep_for(
         std::chrono::milliseconds(33));
   }

   return 0;
}
