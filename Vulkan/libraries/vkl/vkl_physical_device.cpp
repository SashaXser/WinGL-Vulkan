#include "vkl_physical_device.h"

#include <iostream>

namespace vkl
{

PhysicalDevices GetPhysicalDevices(
   const InstanceHandle & instance )
{
   PhysicalDevices physical_devices;

   if (instance && *instance)
   {
      uint32_t physical_count { };
      const VkResult enumerated =
         vkEnumeratePhysicalDevices(
            *instance,
            &physical_count,
            nullptr);

      if (enumerated != VK_SUCCESS)
      {
         std::cerr
            << "Unable to enumerate physical devices ("
            << enumerated
            << ")!"
            << std::endl;
      }

      if (!physical_count)
      {
         std::cerr
            << "There are no physical devices installed on the system!"
            << std::endl;
      }

      std::vector< VkPhysicalDevice > devices(
         physical_count, { });

      vkEnumeratePhysicalDevices(
         *instance,
         &physical_count,
         devices.data());

      for (const auto device : devices)
      {
         VkPhysicalDeviceProperties properties { };

         vkGetPhysicalDeviceProperties(
            device,
            &properties);

         physical_devices.emplace_back(
            properties,
            device);
      }
   }

   return physical_devices;
}

PhysicalDevices GetPhysicalGPUDevices(
   const InstanceHandle & instnace )
{
   PhysicalDevices gpu_devices;

   const auto physical_devices =
      GetPhysicalDevices(
         instnace);

   const VkPhysicalDeviceType types[] =
   {
      VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
      VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
      VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU
   };

   for (const auto type : types)
   {
      for (const auto & device : physical_devices)
      {
         if (type == device.first.deviceType)
         {
            gpu_devices.emplace_back(
               device.first,
               device.second);
         }
      }
   }

   return gpu_devices;
}

PhysicalDeviceQueueFamilyProperties
GetPhysicalDeviceQueueFamilyProperties(
   const VkPhysicalDevice physical_device,
   const VkQueueFlags required,
   const VkQueueFlags preferred )
{
   PhysicalDeviceQueueFamilyProperties
      queue_family_properties;

   uint32_t queue_count { };
   vkGetPhysicalDeviceQueueFamilyProperties(
      physical_device,
      &queue_count,
      nullptr);

   std::vector< VkQueueFamilyProperties >
      properties { queue_count, VkQueueFamilyProperties { } };

   vkGetPhysicalDeviceQueueFamilyProperties(
      physical_device,
      &queue_count,
      properties.data());

   for (const auto & property : properties)
   {
      if ((property.queueFlags & preferred) == preferred ||
          (property.queueFlags & required) == required)
      {
         queue_family_properties.emplace_back(
            static_cast< uint32_t >(
               &property - properties.data()),
            property);
      }
   }

   return queue_family_properties;
}

bool PhysicalDeviceSupportsPresentation(
   const VkPhysicalDevice physical_device,
   const uint32_t queue_family_index )
{
   bool supported { false };

   if (physical_device)
   {
#if VK_USE_PLATFORM_WIN32_KHR
      supported =
         vkGetPhysicalDeviceWin32PresentationSupportKHR(
            physical_device,
            queue_family_index) ==
         VK_TRUE;
#else
#error "Define for this platform!"
#endif
   }

   return supported;
}

} // namespace vkl
