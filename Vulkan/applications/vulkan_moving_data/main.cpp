#include "vkl/vkl_buffer.h"
#include "vkl/vkl_command_buffer.h"
#include "vkl/vkl_command_pool.h"
#include "vkl/vkl_device.h"
#include "vkl/vkl_image.h"
#include "vkl/vkl_instance.h"
#include "vkl/vkl_memory.h"
#include "vkl/vkl_physical_device.h"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <tuple>
#include <utility>

std::optional<
   std::tuple<
      vkl::BufferHandle,
      vkl::DeviceMemoryHandle > >
CreateBuffer(
   const vkl::DeviceHandle & device,
   const VkDeviceSize size,
   const VkBufferUsageFlags usage_flags,
   const VkSharingMode sharing_mode )
{
   std::optional<
      std::tuple<
         vkl::BufferHandle,
         vkl::DeviceMemoryHandle > >
      optional_buffer;

   auto buffer =
      vkl::CreateBuffer(
         device,
         size,
         usage_flags,
         sharing_mode);

   vkl::DeviceMemoryHandle buffer_memory {
      nullptr };

   if (buffer)
   {
      // need to do more to get the correct type index
      buffer_memory =
         vkl::AllocateDeviceMemory(
            device,
            vkl::GetSize(buffer),
            0);

      if (buffer_memory)
      {
         auto result =
            vkBindBufferMemory(
               *device,
               *buffer,
               *buffer_memory,
               vkl::GetSize(buffer));

         if (result == VK_SUCCESS)
         {
            optional_buffer.emplace(
               std::move(buffer),
               std::move(buffer_memory));
         }
      }
   }

   return
      optional_buffer;
}

std::optional<
   std::tuple<
      vkl::ImageHandle,
      vkl::DeviceMemoryHandle > >
CreateImage(
   const vkl::DeviceHandle & device,
   const VkImageCreateFlags create_flags,
   const VkImageType image_type,
   const VkFormat image_format,
   const VkExtent3D image_extents,
   const uint32_t mip_levels,
   const uint32_t array_layers,
   const VkSampleCountFlagBits sample_count,
   const VkImageTiling image_tiling,
   const VkImageUsageFlags image_usage,
   const VkSharingMode sharing_mode,
   const VkImageLayout image_layout )
{
   std::optional<
      std::tuple<
         vkl::ImageHandle,
         vkl::DeviceMemoryHandle > >
      optional_image;

   auto image =
      vkl::CreateImage(
         device,
         create_flags,
         image_type,
         image_format,
         image_extents,
         mip_levels,
         array_layers,
         sample_count,
         image_tiling,
         image_usage,
         sharing_mode,
         image_layout);

   if (image)
   {
      // need to do more to get the correct type index
      // need to do more to get the correct internal type size
      // for now, just assume the internal type size is 4 bytes
      auto image_memory =
         vkl::AllocateDeviceMemory(
            device,
            image_extents.width *
            image_extents.height *
            image_extents.depth *
            sizeof(uint32_t),
            0);

      if (image_memory)
      {
         const auto result =
            vkBindImageMemory(
               *device,
               *image,
               *image_memory,
               0);

         if (result == VK_SUCCESS)
         {
            optional_image.emplace(
               std::move(image),
               std::move(image_memory));
         }
      }
   }

   return
      optional_image;
}

std::optional<
   std::tuple<
      vkl::CommandBufferHandle,
      vkl::CommandPoolHandle > >
CreateCommandBuffer(
   const vkl::DeviceHandle & device,
   const VkCommandPoolCreateFlags command_pool_create_flags,
   const uint32_t queue_family_index )
{
   std::optional<
      std::tuple<
         vkl::CommandBufferHandle,
         vkl::CommandPoolHandle > >
      optional_command_buffer;

   auto command_pool =
      vkl::CreateCommandPool(
         device,
         command_pool_create_flags,
         queue_family_index);

   if (command_pool)
   {
      auto command_buffer =
         vkl::AllocateCommandBuffer(
            device,
            command_pool,
            VK_COMMAND_BUFFER_LEVEL_PRIMARY);

      if (command_buffer)
      {
         optional_command_buffer.emplace(
            std::move(command_buffer),
            std::move(command_pool));
      }
   }

   return
      optional_command_buffer;
}

int32_t main(
   const int32_t /*argc*/,
   const char * const /*argv*/[] )
{
   const auto instance =
      vkl::CreateInstance(
         "vulkan-moving-data",
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
      CreateBuffer(
         gpu_device,
         1024 * 1024,
         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
         VK_SHARING_MODE_EXCLUSIVE);

   if (!buffer)
   {
      return -5;
   }

   const auto image =
      CreateImage(
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

   if (!image)
   {
      return -6;
   }

   const auto command_buffer =
      CreateCommandBuffer(
         gpu_device,
         VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
         VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
         queue_family_properties.front().first);

   if (!command_buffer)
   {
      return -7;
   }

   const auto image_color_attachment =
      CreateImage(
         gpu_device,
         0,
         VK_IMAGE_TYPE_2D,
         VK_FORMAT_R64G64B64A64_SFLOAT,
         { 1024, 1024, 1 },
         1,
         1,
         VK_SAMPLE_COUNT_1_BIT,
         VK_IMAGE_TILING_OPTIMAL,
         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
         VK_SHARING_MODE_EXCLUSIVE,
         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

   if (!image_color_attachment)
   {
      return -8;
   }

   vkl::BeginCommandBuffer(
      std::get< 0 >(*command_buffer),
      0);

   // move an image that was previously a color attachment
   // and move it to be used by a shader for reading.
   const VkImageMemoryBarrier image_memory_barrier {
      VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      nullptr,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      VK_ACCESS_SHADER_READ_BIT,
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      VK_QUEUE_FAMILY_IGNORED,
      VK_QUEUE_FAMILY_IGNORED,
      *std::get< 0 >(*image_color_attachment),
      {
         VK_IMAGE_ASPECT_COLOR_BIT,
         0,
         VK_REMAINING_MIP_LEVELS,
         0,
         VK_REMAINING_ARRAY_LAYERS
      }
   };

   vkCmdPipelineBarrier(
      *std::get< 0 >(*command_buffer),
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0,
      0, nullptr,
      0, nullptr,
      1, &image_memory_barrier);

   // copy to the first half of the buffer the specified value
   // the starting and destination addresses must be multiple
   // of 4 bytes.
   vkCmdFillBuffer(
      *std::get< 0 >(*command_buffer),
      *std::get< 0 >(*buffer),
      0,
      vkl::GetSize(std::get< 0 >(*buffer)) / 2,
      0x10203040);

   const float fvalue { 123.456f };

   // copy to the second half of the buffer the specified
   // floating point value.  the starting and destination
   // addresses must be multiple of 4 bytes (unless whole
   // size is specifed for the destination).
   vkCmdFillBuffer(
      *std::get< 0 >(*command_buffer),
      *std::get< 0 >(*buffer),
      vkl::GetSize(std::get< 0 >(*buffer)) / 2,
      VK_WHOLE_SIZE,
      *reinterpret_cast< const uint32_t * >(&fvalue));

   const uint8_t copy_buffer[] =
      "some values to place into the buffer.";

   // copy the specified array data to the buffer...
   // the null terminator will not be copied...
   vkCmdUpdateBuffer(
      *std::get< 0 >(*command_buffer),
      *std::get< 0 >(*buffer),
      1024,
      sizeof(copy_buffer) - 1,
      copy_buffer);

   // copy from the current buffer from [0, 512) bytes and
   // place it into the same buffer from [512, 1024) bytes.
   const VkBufferCopy copy {
      0,
      512,
      512
   };

   vkCmdCopyBuffer(
      *std::get< 0 >(*command_buffer),
      *std::get< 0 >(*buffer),
      *std::get< 0 >(*buffer),
      1,
      &copy);

#if __cplusplus > 201703L
   const VkClearColorValue clear_color {
      .uint32 = { 0xFF, 0x00, 0x00, 0xFF }
   };
#else
   VkClearColorValue clear_color { };
   clear_color.uint32[0] = 0xFF;
   clear_color.uint32[1] = 0x00;
   clear_color.uint32[2] = 0x00;
   clear_color.uint32[3] = 0xFF;
#endif

   // clear all of the base image with the color red
   // it is recommended, even though this is not an
   // array image, to set the values for the subresource
   // range to 0 and 1.
   const VkImageSubresourceRange image_subresource_range {
      VK_IMAGE_ASPECT_COLOR_BIT,
      0, 1,
      0, 1
   };

   vkCmdClearColorImage(
      *std::get< 0 >(*command_buffer),
      *std::get< 0 >(*image),
      VK_IMAGE_LAYOUT_GENERAL,
      &clear_color,
      1,
      &image_subresource_range);

   // the most optimal format is one that has the memory
   // property set to device local and does not have the
   // memory property host local set.  this then requries
   // data to be staged into a buffer and then copied into
   // the actual texture buffer.
   const auto staged_buffer =
      CreateBuffer(
         gpu_device,
         256 * 256 * sizeof(uint32_t),
         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
         VK_SHARING_MODE_EXCLUSIVE);

   if (!staged_buffer)
   {
      return -9;
   }

   const auto mapped_staged_buffer =
      vkl::MapDeviceMemory(
         gpu_device,
         std::get< 1 >(*staged_buffer),
         0,
         vkl::GetSize(std::get< 0 >(*staged_buffer)),
         0);

   if (!mapped_staged_buffer)
   {
      return -10;
   }

   // make the entire image green
   std::for_each(
      reinterpret_cast< uint32_t * >(*mapped_staged_buffer),
      reinterpret_cast< uint32_t * >(*mapped_staged_buffer) +
      vkl::GetSize(std::get< 0 >(*staged_buffer)) / sizeof(uint32_t),
      [ ] ( uint32_t & data )
      {
         //       aabbggrr
         data = 0xFF00FF00;
      });

   // start with an undefined i
   const auto staged_image =
      CreateImage(
         gpu_device,
         0,
         VK_IMAGE_TYPE_2D,
         VK_FORMAT_R8G8B8A8_UNORM,
         { 256, 256, 1 },
         1,
         1,
         VK_SAMPLE_COUNT_1_BIT,
         VK_IMAGE_TILING_OPTIMAL,
         VK_IMAGE_USAGE_TRANSFER_DST_BIT,
         VK_SHARING_MODE_EXCLUSIVE,
         VK_IMAGE_LAYOUT_UNDEFINED);

   if (!staged_image)
   {
      return -11;
   }

   VkImageMemoryBarrier staged_image_barrier {
      VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      nullptr,
      0,
      VK_ACCESS_TRANSFER_WRITE_BIT,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_QUEUE_FAMILY_IGNORED,
      VK_QUEUE_FAMILY_IGNORED,
      *std::get< 0 >(*staged_image),
      {
         VK_IMAGE_ASPECT_COLOR_BIT,
         0, 1,
         0, 1
      }
   };

   // when a copy takes place from the buffer to
   // the image and completes, move the image to
   // layout transfer dst optimal.
   vkCmdPipelineBarrier(
      *std::get< 0 >(*command_buffer),
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      0,
      0, nullptr,
      0, nullptr,
      1, &staged_image_barrier);

   const VkBufferImageCopy staged_buffer_image_copy {
      0,
      256, 256,
      {
         VK_IMAGE_ASPECT_COLOR_BIT,
         0,
         0,
         1
      },
      { 0, 0, 0 },
      { 256, 256, 1 }
   };

   vkCmdCopyBufferToImage(
      *std::get< 0 >(*command_buffer),
      *std::get< 0 >(*staged_buffer),
      *std::get< 0 >(*staged_image),
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1,
      &staged_buffer_image_copy);

   staged_image_barrier.srcAccessMask =
      VK_ACCESS_TRANSFER_WRITE_BIT;
   staged_image_barrier.dstAccessMask =
      VK_ACCESS_SHADER_READ_BIT;

   // now that the copy is done, we need to make it
   // available for shaders to read when the shader
   // stage is executed.
   vkCmdPipelineBarrier(
      *std::get< 0 >(*command_buffer),
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0,
      0, nullptr,
      0, nullptr,
      1, &staged_image_barrier);

   staged_image_barrier.srcAccessMask =
      VK_ACCESS_SHADER_READ_BIT;
   staged_image_barrier.dstAccessMask =
      VK_ACCESS_TRANSFER_WRITE_BIT;
   staged_image_barrier.oldLayout =
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
   staged_image_barrier.newLayout =
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

   // move the image back to layout transfer source optimal
   // when a transfer stage is executed for the image
   vkCmdPipelineBarrier(
      *std::get< 0 >(*command_buffer),
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      0,
      0, nullptr,
      0, nullptr,
      1, &staged_image_barrier);

   const VkBufferImageCopy staged_image_buffer_copy {
      0,
      256,
      256,
      {
         VK_IMAGE_ASPECT_COLOR_BIT,
         0,
         0,
         1
      },
      { 0, 0, 0 },
      { 256, 256, 1 }
   };

   // copy the data back into the buffer
   vkCmdCopyImageToBuffer(
      *std::get< 0 >(*command_buffer),
      *std::get< 0 >(*staged_image),
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      *std::get< 0 >(*staged_buffer),
      1,
      &staged_image_buffer_copy);

   // VkImageCopy - same concept as copying a buffer
   // to an image and vice versa.  one image needs to
   // be in layout transfer source optimal or general
   // and the other needs to be in layout transfer
   // destination optimal or general.

   // normally images are setup with data and then the
   // transfer process will start. lets just put them
   // in the correct states and do a transfer.
   const auto image_blit_source =
      CreateImage(
         gpu_device,
         0,
         VK_IMAGE_TYPE_2D,
         VK_FORMAT_R8G8B8A8_UNORM,
         { 512, 512, 1 },
         1,
         1,
         VK_SAMPLE_COUNT_1_BIT,
         VK_IMAGE_TILING_OPTIMAL,
         VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
         VK_SHARING_MODE_EXCLUSIVE,
         VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

   const auto image_blit_destination =
      CreateImage(
         gpu_device,
         0,
         VK_IMAGE_TYPE_2D,
         VK_FORMAT_R8G8B8A8_UNORM,
         { 128, 128, 1 },
         1,
         1,
         VK_SAMPLE_COUNT_1_BIT,
         VK_IMAGE_TILING_OPTIMAL,
         VK_IMAGE_USAGE_TRANSFER_DST_BIT,
         VK_SHARING_MODE_EXCLUSIVE,
         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

   if (!image_blit_source || !image_blit_destination)
   {
      return -12;
   }

   // images should be in the correct states so just
   // perform the blit operation on it...
   const VkImageBlit image_blit {
      {
         VK_IMAGE_ASPECT_COLOR_BIT,
         0,
         0,
         1,
      },
      {
         { 0, 0, 1 },
         { 512, 512, 1 }
      },
      {
         VK_IMAGE_ASPECT_COLOR_BIT,
         0,
         0,
         1,
      },
      {
         { 0, 0, 1 },
         { 128, 128, 1 }
      }
   };

   vkCmdBlitImage(
      *std::get< 0 >(*command_buffer),
      *std::get< 0 >(*image_blit_source),
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      *std::get< 0 >(*image_blit_destination),
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1,
      &image_blit,
      VK_FILTER_LINEAR);

   vkl::EndCommandBuffer(
      std::get< 0 >(*command_buffer));

   VkQueue queue { };

   vkGetDeviceQueue(
      *gpu_device,
      queue_family_properties.front().first,
      0,
      &queue);

   if (!queue)
   {
      return -13;
   }

   const VkSubmitInfo submit_info
   {
      VK_STRUCTURE_TYPE_SUBMIT_INFO,
      nullptr,
      0,
      nullptr,
      nullptr,
      1,
      std::get< 0 >(*command_buffer).get(),
      0,
      nullptr
   };

   auto result =
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

      return -14;
   }

   // due to some of the copy operations, device may wait a few
   // seconds now while the copies are taking place...
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

      return -15;
   }
   
   return 0;
}
