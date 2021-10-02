#include <vulkan/vulkan.h>

#include <cstdint>
#include <ios>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

void DestroyInstanceHandle(
   const VkInstance * const instance )
{
   if (instance && *instance)
   {
      vkDestroyInstance(
         *instance,
         nullptr);
   }

   delete instance;
}

using InstanceHandle =
   std::unique_ptr<
      VkInstance,
      decltype(&DestroyInstanceHandle) >;

void DestroyLogicalDeviceHandle(
   const VkDevice * const device )
{
   if (device && *device)
   {
      vkDestroyDevice(
         *device,
         nullptr);
   }

   delete device;
}

using LogicalDeviceHandle =
   std::unique_ptr<
      VkDevice,
      decltype(&DestroyLogicalDeviceHandle) >;

InstanceHandle CreateInstance( )
{
   VkApplicationInfo application_info;
   application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   application_info.pNext = nullptr;
   application_info.pApplicationName = "vulkan-logical-devices";
   application_info.applicationVersion = 1;
   application_info.pEngineName = nullptr;
   application_info.engineVersion = 0;
   application_info.apiVersion = VK_MAKE_VERSION(1, 2, 182);

   VkInstanceCreateInfo create_info;
   create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
   create_info.pNext = nullptr;
   create_info.flags = 0;
   create_info.pApplicationInfo = &application_info;
   create_info.enabledLayerCount = 0;
   create_info.ppEnabledLayerNames = nullptr;
   create_info.enabledExtensionCount = 0;
   create_info.ppEnabledExtensionNames = nullptr;

   VkAllocationCallbacks * allocator = nullptr;

   InstanceHandle instance {
      new VkInstance { nullptr },
      &DestroyInstanceHandle };

   const VkResult created =
      vkCreateInstance(
         &create_info,
         allocator,
         instance.get());

   if (created != VK_SUCCESS)
   {
      std::cerr
         << "Unable to create vulkan instance ("
         << created
         << ")!"
         << std::endl;

      instance = nullptr;
   }
   
   return instance;
}

std::map< uint32_t, std::vector< VkPhysicalDevice > >
GetGPUDevices( const InstanceHandle & instance )
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

   std::map< uint32_t, std::vector< VkPhysicalDevice > >
      gpu_devices;

   std::vector< VkPhysicalDevice > physical_devices(
      physical_count, { });

   vkEnumeratePhysicalDevices(
      *instance,
      &physical_count,
      physical_devices.data());

   for (const auto physical_device : physical_devices)
   {
      VkPhysicalDeviceProperties properties { };

      vkGetPhysicalDeviceProperties(
         physical_device,
         &properties);

      switch (properties.deviceType)
      {
      case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
         gpu_devices[0].push_back(physical_device); break;
      case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
         gpu_devices[1].push_back(physical_device); break;
      case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
         gpu_devices[2].push_back(physical_device); break;
      }
   }

   return gpu_devices;
}

std::vector< uint32_t >
GetGPUQueueIndices(
   const VkPhysicalDevice physical_device )
{
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

   std::vector< uint32_t > indices;

   for (const auto & property : properties)
   {
      if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      {
         indices.push_back(
            static_cast< uint32_t >(
               &property - properties.data()));
      }
   }

   return indices;
}

LogicalDeviceHandle CreateLogicalDevice(
   const VkPhysicalDevice physical_device )
{
   VkPhysicalDeviceProperties properties { };

   vkGetPhysicalDeviceProperties(
      physical_device,
      &properties);

   std::cout
      << "Creating Logical Device On "
      << properties.deviceName
      << std::endl;

   const auto gpu_queue_family_indices =
      GetGPUQueueIndices(
         physical_device);

   if (gpu_queue_family_indices.empty())
   {
      std::cerr
         << "Unable to create vulkan logical device! "
            "There are no GPU queue family indices!"
         << std::endl;

      return { nullptr, &DestroyLogicalDeviceHandle };
   }

   VkDeviceQueueCreateInfo queue_create_info;
   queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
   queue_create_info.pNext = nullptr;
   queue_create_info.flags = 0;
   queue_create_info.queueFamilyIndex = gpu_queue_family_indices.front();
   queue_create_info.queueCount = 1;
   queue_create_info.pQueuePriorities = nullptr;

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
   create_info.enabledLayerCount = 0;
   create_info.ppEnabledLayerNames = nullptr;
   create_info.enabledExtensionCount = 0;
   create_info.ppEnabledExtensionNames = nullptr;
   create_info.pEnabledFeatures = &supported_features;

   LogicalDeviceHandle logical_device = {
      new VkDevice { nullptr },
      &DestroyLogicalDeviceHandle };

   const VkResult created =
      vkCreateDevice(
         physical_device,
         &create_info,
         nullptr,
         logical_device.get());

   if (created != VK_SUCCESS)
   {
      std::cerr
         << "Unable to create vulkan logical device ("
         << created
         << ")!"
         << std::endl;

      logical_device = nullptr;
   }

   return logical_device;
}

int32_t main(
   const int32_t argc,
   const char* const argv[] )
{
   const auto instance = CreateInstance();

   if (!instance)
   {
      return -1;
   }

   std::cout
      << "Instance Created 0x"
      << std::hex
      << *instance
      << std::dec
      << std::endl;

   const auto gpu_devices = GetGPUDevices(instance);

   if (gpu_devices.empty())
   {
      return -2;
   }

   const auto logical_device =
      CreateLogicalDevice(
         gpu_devices.begin()->second.front());

   if (!logical_device)
   {
      return -3;
   }

   std::cout
      << "Logical Device Created 0x"
      << std::hex
      << *logical_device
      << std::dec
      << std::endl;

   return 0;
}

