#include "vkl/vkl_buffer.h"
#include "vkl/vkl_buffer_view.h"
#include "vkl/vkl_command_buffer.h"
#include "vkl/vkl_command_pool.h"
#include "vkl/vkl_device.h"
#include "vkl/vkl_image.h"
#include "vkl/vkl_image_view.h"
#include "vkl/vkl_instance.h"
#include "vkl/vkl_memory.h"
#include "vkl/vkl_physical_device.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <cstring>
#include <iostream>

int32_t main(
   const int32_t /*argc*/,
   const char * const /*argv*/[] )
{
   const auto instance =
      vkl::CreateInstance(
         "vulkan-queues-and-commands",
         1,
         nullptr,
         0,
         1, 2, 182);

   if (!instance)
   {
      return -1;
   }

   const auto physical_gpu_devices =
      vkl::GetPhysicalGPUDevices(
         instance);

   if (physical_gpu_devices.empty())
   {
      return -2;
   }

   const auto queue_family_properties =
      vkl::GetPhysicalDeviceQueueFamilyProperties(
         physical_gpu_devices.front().second,
         VK_QUEUE_GRAPHICS_BIT,
         0);

   if (queue_family_properties.empty())
   {
      std::cerr
         << "No queue families with the graphics bit capability!"
         << std::endl;

      return -3;
   }

   const auto gpu_device =
      vkl::CreateDevice(
         physical_gpu_devices.front().second,
         0,
         queue_family_properties.front().first,
         queue_family_properties.front().second.queueCount);

   if (!gpu_device)
   {
      std::cerr
         << "GPU device not created!"
         << std::endl;

      return -4;
   }

   const auto buffer =
      vkl::CreateBuffer(
         gpu_device,
         1024 * 1024,
         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
         VK_SHARING_MODE_EXCLUSIVE);

   // need to do more to get the correct type index
   const auto buffer_memory =
      vkl::AllocateDeviceMemory(
         gpu_device,
         vkl::GetSize(buffer),
         0);

   const auto mapped_buffer_memory =
      vkl::MapDeviceMemory(
         gpu_device,
         buffer_memory,
         0,
         vkl::GetSize(buffer_memory),
         0);

   std::memset(
      *mapped_buffer_memory,
      0x7F,
      vkl::GetSize(mapped_buffer_memory));

   auto result =
      vkBindBufferMemory(
         *gpu_device,
         *buffer,
         *buffer_memory,
         vkl::GetSize(buffer_memory));

   if (result != VK_SUCCESS)
   {
      return -5;
   }

   const auto buffer_view =
      vkl::CreateBufferView(
         gpu_device,
         buffer,
         VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
         VK_FORMAT_R8_UNORM,
         0,
         vkl::GetSize(buffer));

   const auto image =
      vkl::CreateImage(
         gpu_device,
         0,
         VK_IMAGE_TYPE_2D,
         VK_FORMAT_R8G8B8A8_UNORM,
         { 1024, 1024, 1 },
         1,
         1,
         VK_SAMPLE_COUNT_8_BIT,
         VK_IMAGE_TILING_LINEAR,
         VK_IMAGE_USAGE_SAMPLED_BIT,
         VK_SHARING_MODE_EXCLUSIVE,
         VK_IMAGE_LAYOUT_UNDEFINED);

   // need to do more to get the correct type index
   const auto image_memory =
      vkl::AllocateDeviceMemory(
         gpu_device,
         1024 * 1024 * sizeof(uint32_t),
         0);

   const auto mapped_image_memory =
      vkl::MapDeviceMemory(
         gpu_device,
         image_memory,
         0,
         vkl::GetSize(image_memory),
         0);

   std::memset(
      *mapped_image_memory,
      0xF7,
      vkl::GetSize(mapped_image_memory));

   result =
      vkBindImageMemory(
         *gpu_device,
         *image,
         *image_memory,
         0);

   if (result != VK_SUCCESS)
   {
      return -6;
   }

   const auto image_view =
      vkl::CreateImageView(
         gpu_device,
         image,
         0,
         VK_IMAGE_VIEW_TYPE_2D,
         VK_FORMAT_R8G8B8A8_SNORM,
         { },
         { VK_IMAGE_ASPECT_COLOR_BIT });

   const auto command_pool =
      vkl::CreateCommandPool(
         gpu_device,
         VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
         VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
         queue_family_properties.front().first);

   vkl::ResetCommandPool(
      command_pool,
      VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

   const auto command_buffer =
      vkl::AllocateCommandBuffer(
         gpu_device,
         command_pool,
         VK_COMMAND_BUFFER_LEVEL_PRIMARY);

   vkl::BeginCommandBuffer(
      command_buffer,
      0);

   const VkBufferCopy copy {
      0,
      512,
      512
   };

   vkCmdCopyBuffer(
      *command_buffer,
      *buffer,
      *buffer,
      1,
      &copy);

   vkl::EndCommandBuffer(
      command_buffer);

   VkQueue queue { };

   vkGetDeviceQueue(
      *gpu_device,
      queue_family_properties.front().first,
      0,
      &queue);

   if (!queue)
   {
      return -7;
   }

   const VkSubmitInfo submit_info
   {
      VK_STRUCTURE_TYPE_SUBMIT_INFO,
      nullptr,
      0,
      nullptr,
      nullptr,
      1,
      command_buffer.get(),
      0,
      nullptr
   };

   result =
      vkQueueSubmit(
         queue,
         1,
         &submit_info,
         nullptr);

   if (result != VK_SUCCESS)
   {
      std::cerr
         << "Unable to submit command ("
         << result
         << ")!"
         << std::endl;

      return -8;
   }

   result =
      vkQueueWaitIdle(
         queue);

   if (result != VK_SUCCESS)
   {
      std::cerr
         << "Unable to wait idle on queue ("
         << result
         << ")!"
         << std::endl;

      return -9;
   }
   
   return 0;
}
