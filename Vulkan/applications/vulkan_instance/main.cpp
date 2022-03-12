#include <vulkan/vulkan.h>

#include <cstdint>
#include <iostream>

int32_t main(
   const int32_t /*argc*/,
   const char* const /*argv*/[] )
{
   VkApplicationInfo application_info;
   application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   application_info.pNext = nullptr;
   application_info.pApplicationName = "vulkan-instance";
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

   VkInstance instance = nullptr;

   const VkResult created =
      vkCreateInstance(
         &create_info,
         allocator,
         &instance);

   if (created != VK_SUCCESS)
   {
      std::cerr
         << "Unable to create vulkan instance ("
         << created
         << ")!"
         << std::endl;
   }
   else
   {
      std::cout
         << "Vulkan instance created!"
         << std::endl;

      vkDestroyInstance(
         instance,
         nullptr);
   }

   return 0;
}

