#include "vkl_device.h"
#include "vkl_context_data.h"
#include "vkl_allocator.h"

#include <iostream>
#include <vector>

namespace vkl
{

namespace
{

struct Context final
{
   PhysicalDeviceHandle physical_device;
   VkDeviceQueueCreateFlags create_flags;
   uint32_t queue_family_index;
   uint32_t queue_count;
};

} // namespace

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

      vkl::internal::DeallocateContext(
         device);
   }
}

DeviceHandle CreateDevice(
   const PhysicalDeviceHandle physical_device,
   const VkDeviceQueueCreateFlags create_flags,
   const uint32_t queue_family_index,
   const uint32_t queue_count )
{
   DeviceHandle device {
      nullptr,
      &DestoryDeviceHandle };

   if (physical_device && *physical_device)
   {
      VkPhysicalDeviceProperties properties { };

      vkGetPhysicalDeviceProperties(
         *physical_device,
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

      const std::vector< float > queue_priorities(
         queue_count, 1.0f);
      queue_create_info.pQueuePriorities =
         queue_priorities.data();

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
         *physical_device,
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

      const char * const extensions[] =
      {
         VK_KHR_SWAPCHAIN_EXTENSION_NAME
      };

      create_info.enabledExtensionCount =
         sizeof(extensions) / sizeof(*extensions);
      create_info.ppEnabledExtensionNames = extensions;

      create_info.pEnabledFeatures = &supported_features;

      device.reset(
         vkl::internal::AllocateContext<
            VkDevice,
            Context >(
               physical_device,
               create_flags,
               queue_family_index,
               queue_count),
         &DestoryDeviceHandle);

      if (device)
      {
         const VkResult created =
            vkCreateDevice(
               *physical_device,
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

            device.reset();
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

PhysicalDeviceHandle GetPhysicalDevice(
   const DeviceHandle & device )
{
   return
      vkl::internal::GetContextData(
         device.get(),
         &Context::physical_device);
}

std::optional<
   std::pair< uint32_t, uint32_t > >
GetQueueFamily(
   const DeviceHandle & device )
{
   std::optional<
      std::pair< uint32_t, uint32_t > >
      queue_family;

   if (device && *device)
   {
      const auto context =
         vkl::internal::GetContextData<
            Context >(
               device.get());

      if (context)
      {
         queue_family.emplace(
            context->queue_family_index,
            context->queue_count);
      }
   }

   return queue_family;
}

} // namespace vkl
