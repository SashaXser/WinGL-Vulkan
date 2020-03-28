#include "vkl/vkl_instance.h"
#include "vkl/vkl_physical_device.h"
#include "vkl/vkl_surface.h"
#include "vkl/vkl_window.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <iostream>

int32_t main(
   const int32_t argc,
   const char * const argv[] )
{
   const auto instance =
      vkl::CreateInstance(
         "vulkan-presentation", 1,
         "", 1,
         1, 1, 121);

   if (!instance)
   {
      return -1;
   }

   const auto gpu_physical_devices =
      vkl::GetPhysicalGPUDevices(
         instance);

   if (gpu_physical_devices.empty())
   {
      return -2;
   }

   const auto gpu_physical_device =
      gpu_physical_devices.front().second;

   const auto physical_device_queue_family_properties =
      vkl::GetPhysicalDeviceQueueFamilyProperties(
         gpu_physical_device,
         VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT,
         0);

   if (physical_device_queue_family_properties.empty())
   {
      return -3;
   }

   const bool presentation_supported =
      vkl::PhysicalDeviceSupportsPresentation(
         gpu_physical_device,
         physical_device_queue_family_properties.front().first);

   std::cout
      << "Presentation "
      << (presentation_supported ? "" : "not")
      << "supported for queue family index "
      << physical_device_queue_family_properties.front().first
      << " on device "
      << gpu_physical_devices.front().first.deviceName
      << std::endl;

   if (!presentation_supported)
   {
      return -4;
   }

   const auto window_init =
      vkl::InitWindowSystem();

   if (!window_init)
   {
      return -5;
   }

   const auto window =
      vkl::CreateWindow(
         640, 480,
         "vulkan-presentation");

   if (!window)
   {
      return -6;
   }

   const auto surface =
      vkl::CreateSurface(
         gpu_physical_device,
         window);

   if (!surface)
   {
      return -7;
   }

   const auto surface_capabilities =
      vkl::GetSurfaceCapabilites(
         surface);

   if (!surface_capabilities)
   {
      return -8;
   }

   return 0;
}
