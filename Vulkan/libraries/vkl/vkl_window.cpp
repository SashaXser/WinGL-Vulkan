#include "vkl_window.h"
#include "vkl_context_data.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <utility>

#if _WIN32
#ifdef CreateWindow
#undef CreateWindow
#endif
#endif

namespace vkl
{

namespace window_system_init
{

static InitWindowSystemHandle::weak_type
   init_window_system_handle_;

struct Context final
{
};

} // namespace window_system_init

void TerminateWindowSystem(
   const uintptr_t * const * const initialized )
{
   if (initialized)
   {
      if (*initialized)
      {
         glfwTerminate();
      }

      vkl::internal::DeallocateContext(
         initialized);

      window_system_init::init_window_system_handle_.reset();
   }
}

InitWindowSystemHandle InitWindowSystem( )
{
   InitWindowSystemHandle init_win_sys_handle =
      window_system_init::init_window_system_handle_.lock();

   if (!init_win_sys_handle)
   {
      // if this fails, termination is automatically called
      const auto result =
         glfwInit();

      if (result == GLFW_TRUE)
      {
         init_win_sys_handle.reset(
            vkl::internal::AllocateContext<
               uintptr_t *,
               window_system_init::Context >(),
            &TerminateWindowSystem);

         if (!init_win_sys_handle)
         {
            glfwTerminate();
         }
         else
         {
            *init_win_sys_handle =
               reinterpret_cast< uintptr_t * >(1);

            window_system_init::init_window_system_handle_ =
               init_win_sys_handle;
         }
      }
   }

   return init_win_sys_handle;
}

void PollWindowSystem()
{
   glfwPollEvents();
}

bool PollWindowSystem(
   const std::initializer_list< WindowHandle > & windows )
{
   PollWindowSystem();

   bool window_closed { false };

   for (const auto & window : windows)
   {
      const auto window_imp =
         reinterpret_cast< GLFWwindow * >(
            *window);

      window_closed =
         glfwWindowShouldClose(
            window_imp);

      if (window_closed) break;
   }

   return window_closed;
}

namespace window
{

struct Context final
{
   WindowHandle::weak_type window_handle { };
   void * user_data { };
   KeyCallbackFunc key_callback { };
   MousePositionCallbackFunc mouse_position_callback { };
   MouseButtonCallbackFunc mouse_button_callback { };
   MouseScrollCallbackFunc mouse_scroll_callback { };
   WindowCloseCallbackFunc window_close_callback { };
   WindowResizeCallbackFunc window_resize_callback { };
};

} // namespace window

void DestroyWindow(
   uintptr_t * const * const window )
{
   if (window)
   {
      if (*window)
      {
         const auto window_imp =
            reinterpret_cast< GLFWwindow * >(
               *window);

         glfwDestroyWindow(
            window_imp);
      }

      vkl::internal::DeallocateContext(
         window);
   }
}

WindowHandle CreateWindow(
   const int32_t width,
   const int32_t height,
   const char * const title )
{
   WindowHandle window {
      vkl::internal::AllocateContext<
         uintptr_t *,
         window::Context >(),
      &DestroyWindow };

   if (window)
   {
      // do not allow opengl or opengl es to be used
      // we only want the native window system
      glfwWindowHint(GLFW_STEREO, GLFW_FALSE);
      glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

      const auto window_imp =
         glfwCreateWindow(
            width, height,
            title,
            nullptr,
            nullptr);

      if (!window_imp)
      {
         window.reset();
      }
      else
      {
         *window =
            reinterpret_cast< uintptr_t * >(
               window_imp);

         const auto context =
            vkl::internal::GetContextData<
               window::Context >(
                  window.get());

         if (context)
         {
            context->window_handle =
               window;

            glfwSetWindowUserPointer(
               window_imp,
               context);
         }
      }
   }

   return window;
}

bool SetWindowUserData(
   const WindowHandle & window,
   void * const data )
{
   bool set { false };

   if (window && *window)
   {
      const auto context =
         vkl::internal::GetContextData<
            window::Context >(
               window.get());

      if (context)
      {
         context->user_data =
            data;

         set = true;
      }
   }

   return set;
}

template <
   typename ContextCallbackT,
   typename ... ArgsT >
void ExecuteCallback(
   ContextCallbackT context_callback,
   GLFWwindow * const native_window,
   ArgsT && ... args )
{
   if (native_window)
   {
      const void * const user_data =
         glfwGetWindowUserPointer(
            native_window);

      if (user_data)
      {
         const auto context =
            reinterpret_cast<
               const window::Context * >(
                  user_data);

         const auto window_handle =
            context->window_handle.lock();

         if (window_handle &&
             context->*context_callback)
         {
            (context->*context_callback)(
               context->user_data,
               window_handle,
               std::forward< ArgsT >(args)...);
         }
      }
   }
}

template <
   typename CallbackFuncT,
   typename SetNativeCallbackFuncT,
   typename NativeCallbackFuncT >
bool SetupCallback(
   const WindowHandle & window,
   CallbackFuncT && callback,
   CallbackFuncT window::Context::* const context_callback,
   const SetNativeCallbackFuncT set_native_callback,
   const NativeCallbackFuncT native_callback )
{
   bool set { false };

   if (window && *window &&
       context_callback && set_native_callback)
   {
      const auto context =
         vkl::internal::GetContextData<
            window::Context >(
               window.get());

      if (context)
      {
         context->*context_callback =
            std::move(callback);

         const auto window_imp =
            reinterpret_cast< GLFWwindow * >(
               *window);

         if (context->*context_callback)
            set_native_callback(
               window_imp,
               native_callback);
         else
            set_native_callback(
               window_imp,
               nullptr);

         set = true;
      }
   }

   return set;
}

bool SetKeyCallback(
   const WindowHandle & window,
   KeyCallbackFunc && callback )
{
   const auto native_callback =
      [ ] (
         GLFWwindow * const native_window,
         const int32_t key,
         const int32_t scan_code,
         const int32_t action,
         const int32_t modifier_flags )
      {
         ExecuteCallback(
            &window::Context::key_callback,
            native_window,
            static_cast< const KeyboardKeys >(key),
            scan_code,
            static_cast< const InputActions >(action),
            static_cast< const ModifierKeyFlags >(modifier_flags));
      };

   return
      SetupCallback(
         window,
         std::move(callback),
         &window::Context::key_callback,
         &glfwSetKeyCallback,
         native_callback);
}

bool SetMousePositionCallback(
   const WindowHandle & window,
   MousePositionCallbackFunc && callback)
{
   const auto native_callback =
      [ ] (
         GLFWwindow * const native_window,
         const double x_pos,
         const double y_pos )
      {
         ExecuteCallback(
            &window::Context::mouse_position_callback,
            native_window,
            x_pos,
            y_pos);
      };

   return
      SetupCallback(
         window,
         std::move(callback),
         &window::Context::mouse_position_callback,
         &glfwSetCursorPosCallback,
         native_callback);
}

bool SetMouseButtonCallback(
   const WindowHandle & window,
   MouseButtonCallbackFunc && callback )
{
   const auto native_callback =
      [ ] (
         GLFWwindow * const native_window,
         const int32_t button,
         const int32_t action,
         const int32_t modifier_flags )
      {
         ExecuteCallback(
            &window::Context::mouse_button_callback,
            native_window,
            static_cast< const MouseButtons >(button),
            static_cast< const InputActions >(action),
            static_cast< const ModifierKeyFlags >(modifier_flags));
      };

   return
      SetupCallback(
         window,
         std::move(callback),
         &window::Context::mouse_button_callback,
         &glfwSetMouseButtonCallback,
         native_callback);
}

bool SetMouseScrollCallback(
   const WindowHandle & window,
   MouseScrollCallbackFunc && callback )
{
   const auto native_callback =
      [ ] (
         GLFWwindow * const native_window,
         const double x_offset,
         const double y_offset )
      {
         ExecuteCallback(
            &window::Context::mouse_scroll_callback,
            native_window,
            x_offset,
            y_offset);
      };

   return
      SetupCallback(
         window,
         std::move(callback),
         &window::Context::mouse_scroll_callback,
         &glfwSetScrollCallback,
         native_callback);
}

bool SetWindowCloseCallback(
   const WindowHandle & window,
   WindowCloseCallbackFunc && callback )
{
   const auto native_callback =
      [ ] (
         GLFWwindow * const native_window )
      {
         ExecuteCallback(
            &window::Context::window_close_callback,
            native_window);
      };

   return
      SetupCallback(
         window,
         std::move(callback),
         &window::Context::window_close_callback,
         &glfwSetWindowCloseCallback,
         native_callback);
}

bool SetWindowResizeCallback(
   const WindowHandle & window,
   WindowResizeCallbackFunc && callback )
{
   const auto native_callback =
      [ ] (
         GLFWwindow * const native_window,
         const int32_t width,
         const int32_t height )
      {
         ExecuteCallback(
            &window::Context::window_resize_callback,
            native_window,
            static_cast< uint32_t >(width),
            static_cast< uint32_t >(height));
      };

   return
      SetupCallback(
         window,
         std::move(callback),
         &window::Context::window_resize_callback,
         &glfwSetWindowSizeCallback,
         native_callback);
}

#if _WIN32

HWND GetWin32HWND(
   const WindowHandle & window )
{
   HWND hwnd { nullptr };

   if (window && *window)
   {
      const auto window_imp =
         reinterpret_cast< GLFWwindow * >(
            *window);

      hwnd =
         glfwGetWin32Window(
            window_imp);
   }

   return hwnd;
}

#endif // _WIN32

} // namespace vkl
