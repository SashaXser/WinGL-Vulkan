#include "vkl_surface.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_physical_device.h"

#if VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#endif

#include <iostream>

namespace vkl
{

#if VK_USE_PLATFORM_WIN32_KHR
extern HWND GetWin32HWND(
   const WindowHandle & );
#endif

namespace
{

struct Context final
{
   InstanceHandle instance;
   PhysicalDeviceHandle physical_device;
   WindowHandle window;
};

} // namespace

void DestroySurface(
   const VkSurfaceKHR * const surface )
{
   if (surface)
   {
      if (*surface)
      {
         const auto context =
            vkl::internal::GetContextData<
               Context >(
                  surface);

         if (context &&
             context->instance && *context->instance)
         {
            vkDestroySurfaceKHR(
               *context->instance,
               *surface,
               DefaultAllocator());
         }
      }

      vkl::internal::DeallocateContext(
         surface);
   }
}

SurfaceHandle CreateSurface(
   const PhysicalDeviceHandle & physical_device,
   const WindowHandle & window )
{
   SurfaceHandle surface {
      nullptr, &DestroySurface };

   if (physical_device && *physical_device &&
       window && *window)
   {
#if VK_USE_PLATFORM_WIN32_KHR
      const auto instance =
         vkl::GetInstance(
            physical_device);

      if (instance && *instance)
      {
         surface.reset(
            vkl::internal::AllocateContext<
               VkSurfaceKHR,
               Context >(
                  instance,
                  physical_device,
                  window),
            &DestroySurface);

         if (surface)
         {
            const HWND hwnd =
               GetWin32HWND(
                  window);

            if (!hwnd)
            {
               surface.reset();
            }
            else
            {
               const VkWin32SurfaceCreateInfoKHR info {
                  VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                  nullptr,
                  0,
                  GetModuleHandle(nullptr),
                  hwnd
               };

               const auto result =
                  vkCreateWin32SurfaceKHR(
                     *instance,
                     &info,
                     DefaultAllocator(),
                     surface.get());

               if (result != VK_SUCCESS)
               {
                  std::cerr
                     << "Unable to create surface ("
                     << result
                     << ")!"
                     << std::endl;

                  surface.reset();
               }
               else
               {
                  std::cout
                     << "Surface created successfully!"
                     << std::endl;
               }
            }
         }
      }

#else
#error "Define for this platform!"
#endif
   }

   return surface;
}

InstanceHandle GetInstance(
   const SurfaceHandle & surface )
{
   return
      vkl::internal::GetContextData(
         surface.get(),
         &Context::instance);
}

WindowHandle GetWindow(
   const SurfaceHandle & surface )
{
   return
      vkl::internal::GetContextData(
         surface.get(),
         &Context::window);
}

} // namespace vkl