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

PhysicalDeviceHandle GetPhysicalDevice(
   const SurfaceHandle & surface )
{
   return
      vkl::internal::GetContextData(
         surface.get(),
         &Context::physical_device);
}

WindowHandle GetWindow(
   const SurfaceHandle & surface )
{
   return
      vkl::internal::GetContextData(
         surface.get(),
         &Context::window);
}

std::optional< VkSurfaceCapabilitiesKHR >
GetSurfaceCapabilites(
   const SurfaceHandle & surface )
{
   std::optional< VkSurfaceCapabilitiesKHR > capabilities;

   if (surface && *surface)
   {
      const auto physical_device =
         GetPhysicalDevice(
            surface);

      if (physical_device && *physical_device)
      {
         VkSurfaceCapabilitiesKHR pdsc { };

         const auto result =
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
               *physical_device,
               *surface,
               &pdsc);

         if (result == VK_SUCCESS)
         {
            capabilities = pdsc;
         }
      }
   }

   return capabilities;
}

std::optional< std::vector< VkSurfaceFormatKHR > >
GetSurfaceFormats(
   const SurfaceHandle & surface )
{
   std::optional< std::vector< VkSurfaceFormatKHR > >
      surface_formats;

   if (surface && *surface)
   {
      const auto physical_device =
         GetPhysicalDevice(
            surface);

      if (physical_device && *physical_device)
      {
         uint32_t surface_format_count { 0 };

         auto result =
            vkGetPhysicalDeviceSurfaceFormatsKHR(
               *physical_device,
               *surface,
               &surface_format_count,
               nullptr);

         if (VK_SUCCESS == result)
         {
            std::vector< VkSurfaceFormatKHR >
               formats { surface_format_count };

            result =
               vkGetPhysicalDeviceSurfaceFormatsKHR(
                  *physical_device,
                  *surface,
                  &surface_format_count,
                  formats.data());

            if (VK_SUCCESS == result)
            {
               surface_formats = formats;
            }
         }
      }
   }

   return surface_formats;
}

} // namespace vkl
