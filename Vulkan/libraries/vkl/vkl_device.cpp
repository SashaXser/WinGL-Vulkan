#include "vkl_device.h"
#include "vkl_allocator.h"

#include <iostream>
#include <new>

#if _DEBUG
#include <vector>
#endif

namespace vkl
{

void DestoryDeviceHandle(
   const VkDevice * const device )
{
   if (device)
   {
      if (*device)
      {
         vkDestroyDevice(
            *device,
            DefaultAllocator());
      }

      delete []
         (reinterpret_cast< const size_t * >(
            device) - 1);
   }
}

DeviceHandle CreateDevice(
   const VkPhysicalDevice physical_device,
   const VkDeviceQueueCreateFlags create_flags,
   const uint32_t queue_family_index,
   const uint32_t queue_count )
{
   DeviceHandle device {
      nullptr,
      &DestoryDeviceHandle };

   if (physical_device)
   {
      VkPhysicalDeviceProperties properties { };

      vkGetPhysicalDeviceProperties(
         physical_device,
         &properties);

      std::cout
         << "Creating Device On "
         << properties.deviceName
         << std::endl;

      VkDeviceQueueCreateInfo queue_create_info;
      queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_create_info.pNext = nullptr;
      queue_create_info.flags = create_flags;
      queue_create_info.queueFamilyIndex = queue_family_index;
      queue_create_info.queueCount = queue_count;
      queue_create_info.pQueuePriorities = nullptr;

#if _DEBUG
      uint32_t ilayer_count { };

      const VkResult enumerated =
         vkEnumerateInstanceLayerProperties(
            &ilayer_count,
            nullptr);

      if (enumerated != VK_SUCCESS)
      {
         std::cerr
            << "Unable to enumerate instance layer properties ("
            << enumerated
            << ")!"
            << std::endl;
      }

      std::vector< const char * > ilayers;
      std::vector< VkLayerProperties > il_properties {
         ilayer_count, VkLayerProperties { } };

      if (ilayer_count)
      {
         vkEnumerateInstanceLayerProperties(
            &ilayer_count,
            il_properties.data());

         for (const auto & ilayer : il_properties)
         {
            ilayers.push_back(ilayer.layerName);
         }
      }
#endif // _DEBUG

      VkPhysicalDeviceFeatures supported_features { };

      vkGetPhysicalDeviceFeatures(
         physical_device,
         &supported_features);

      VkDeviceCreateInfo create_info;
      create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      create_info.pNext = nullptr;
      create_info.flags = 0;
      create_info.queueCreateInfoCount = 1;
      create_info.pQueueCreateInfos = &queue_create_info;

#if _DEBUG
      create_info.enabledLayerCount = 0;
      create_info.ppEnabledLayerNames = nullptr;

      if (!ilayers.empty())
      {
         create_info.enabledLayerCount = ilayer_count;
         create_info.ppEnabledLayerNames = ilayers.data();
      }
#else // _DEBUG
      create_info.enabledLayerCount = 0;
      create_info.ppEnabledLayerNames = nullptr;
#endif // _DEBUG

      create_info.enabledExtensionCount = 0;
      create_info.ppEnabledExtensionNames = nullptr;
      create_info.pEnabledFeatures = &supported_features;

      size_t * const context =
         new (std::nothrow) size_t[2] { };

      if (context)
      {
         *context =
            reinterpret_cast< size_t >(
               physical_device);

         device.reset(
            reinterpret_cast< VkDevice * >(
               context + 1));

         const VkResult created =
            vkCreateDevice(
               physical_device,
               &create_info,
               DefaultAllocator(),
               device.get());

         if (created != VK_SUCCESS)
         {
            std::cerr
               << "Unable to create vulkan logical device ("
               << created
               << ")!"
               << std::endl;

            device.reset(
               nullptr);
         }
         else
         {
            std::cout
               << "Device Created 0x"
               << std::hex
               << *device
               << std::dec
               << std::endl;
         }
      }
   }

   return device;
}

VkPhysicalDevice GetPhysicalDevice(
   const DeviceHandle & device )
{
   VkPhysicalDevice physical_device { };

   if (device && *device)
   {
      physical_device =
         *reinterpret_cast< const VkPhysicalDevice * >(
            reinterpret_cast< const size_t * >(
               device.get()) - 1);
   }

   return physical_device;
}

} // namespace vkl
