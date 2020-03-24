#include "vkl_instance.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"

#include <ios>
#include <iostream>
#include <string>

namespace vkl
{

namespace
{

struct Context
{
   std::string application_name;
   uint32_t application_version;
   std::string engine_name;
   uint32_t engine_version;
   uint32_t vk_api_version_major;
   uint32_t vk_api_version_minor;
   uint32_t vk_api_version_patch;
};

} // namespace

void DestroyInstanceHandle(
   const VkInstance * const instance )
{
   if (instance)
   {
      if (*instance)
      {
         vkDestroyInstance(
            *instance,
            DefaultAllocator());
      }

      vkl::internal::DeallocateContext(
         instance);
   }
}

InstanceHandle CreateInstance(
   const char * const application_name,
   const uint32_t application_version,
   const char * const engine_name,
   const uint32_t engine_version,
   const uint32_t vk_api_version_major,
   const uint32_t vk_api_version_minor,
   const uint32_t vk_api_version_patch )
{
   InstanceHandle instance {
      vkl::internal::AllocateContext<
         VkInstance,
         Context >(
            application_name ? application_name : "",
            application_version,
            engine_name ? engine_name : "",
            engine_version,
            vk_api_version_major,
            vk_api_version_minor,
            vk_api_version_patch),
      &DestroyInstanceHandle };

   if (!instance)
   {
      std::cerr
         << "Unable to allocate memory for VkInstance!"
         << std::endl;
   }
   else
   {
      VkApplicationInfo application_info;
      application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      application_info.pNext = nullptr;
      application_info.pApplicationName = application_name;
      application_info.applicationVersion = application_version;
      application_info.pEngineName = engine_name;
      application_info.engineVersion = engine_version;
      application_info.apiVersion =
         VK_MAKE_VERSION(
            vk_api_version_major,
            vk_api_version_minor,
            vk_api_version_patch);

      VkInstanceCreateInfo create_info;
      create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      create_info.pNext = nullptr;
      create_info.flags = 0;
      create_info.pApplicationInfo = &application_info;
      create_info.enabledLayerCount = 0;
      create_info.ppEnabledLayerNames = nullptr;

      const char * const extensions[] =
      {
         VK_KHR_SURFACE_EXTENSION_NAME,
#if VK_USE_PLATFORM_WIN32_KHR
         VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#else
#error "Define for this platform!"
#endif
      };

      create_info.enabledExtensionCount =
         sizeof(extensions) / sizeof(*extensions);
      create_info.ppEnabledExtensionNames = extensions;

      const VkResult created =
         vkCreateInstance(
            &create_info,
            DefaultAllocator(),
            instance.get());

      if (created != VK_SUCCESS)
      {
         std::cerr
            << "Unable to create vulkan instance ("
            << created
            << ")!"
            << std::endl;

         instance.reset();
      }
      else
      {
         std::cout
            << "Instance Created 0x"
            << std::hex
            << *instance
            << std::dec
            << std::endl;
      }
   }

   return instance;
}

} // namespace vkl
