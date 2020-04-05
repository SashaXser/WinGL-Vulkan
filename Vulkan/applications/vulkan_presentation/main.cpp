#include "vkl/vkl_command_buffer.h"
#include "vkl/vkl_command_pool.h"
#include "vkl/vkl_device.h"
#include "vkl/vkl_instance.h"
#include "vkl/vkl_physical_device.h"
#include "vkl/vkl_surface.h"
#include "vkl/vkl_swap_chain.h"
#include "vkl/vkl_window.h"

#include <vulkan/vulkan.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <thread>

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

   const auto queue_family_properties =
      vkl::GetPhysicalDeviceQueueFamilyProperties(
         gpu_physical_devices.front().second,
         VK_QUEUE_GRAPHICS_BIT,
         0);

   if (queue_family_properties.empty())
   {
      return -5;
   }

   const auto gpu_device =
      vkl::CreateDevice(
         gpu_physical_devices.front().second,
         0,
         queue_family_properties.front().first,
         queue_family_properties.front().second.queueCount);

   if (!gpu_device)
   {
      return -6;
   }

   const auto window_init =
      vkl::InitWindowSystem();

   if (!window_init)
   {
      return -7;
   }

   const auto window =
      vkl::CreateWindow(
         640, 480,
         "vulkan-presentation");

   if (!window)
   {
      return -8;
   }

   const auto surface =
      vkl::CreateSurface(
         gpu_device,
         window);

   if (!surface)
   {
      return -9;
   }

   const auto surface_capabilities =
      vkl::GetSurfaceCapabilites(
         surface);

   if (!surface_capabilities)
   {
      return -10;
   }

   const auto surface_formats =
      vkl::GetSurfaceFormats(
         surface);

   if (!surface_formats)
   {
      return -11;
   }

   const auto surface_present_modes =
      vkl::GetSurfacePresentModes(
         surface);

   if (!surface_present_modes)
   {
      return -12;
   }

   const auto swap_chain =
      vkl::CreateSwapChain(
         gpu_device,
         surface);

   if (!swap_chain)
   {
      return -13;
   }

   const auto swap_chain_images =
      vkl::GetSwapChainImages(
         swap_chain);

   if (!swap_chain_images)
   {
      return -14;
   }

   vkl::SetWindowResizeCallback(
      window,
      [ &swap_chain ] (
         void * const user_data,
         const vkl::WindowHandle & window,
         const uint32_t width,
         const uint32_t height )
      {
         if (width && height)
         {
            // todo, setup swap chain here
         }
      });

   // transition each of the images in the swap chain
   // to be VK_IMAGE_LAYOUT_PRESENT_SRC_KHR

   const auto command_pool =
      vkl::CreateCommandPool(
         gpu_device,
         VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
         0);

   std::vector< vkl::CommandBufferHandle > framebuffer_command_buffers {
      swap_chain_images->size()
   };

   for (auto & command_buffer : framebuffer_command_buffers)
   {
      command_buffer =
         vkl::AllocateCommandBuffer(
            gpu_device,
            command_pool,
            VK_COMMAND_BUFFER_LEVEL_PRIMARY);

      vkl::BeginCommandBuffer(
         command_buffer,
         VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

      for (const auto & swap_chain_image : *swap_chain_images)
      {
         VkImageMemoryBarrier image_memory_barrier
         {
            VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            nullptr,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED,
            swap_chain_image,
            {
               VK_IMAGE_ASPECT_COLOR_BIT,
               0,
               VK_REMAINING_MIP_LEVELS,
               0,
               VK_REMAINING_ARRAY_LAYERS
            }
         };

         vkCmdPipelineBarrier(
            *command_buffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &image_memory_barrier);

         std::uniform_real_distribution< float >
            color_distribution(0.0f, 1.0f);
         std::default_random_engine random_engine(
            (std::random_device())());

         const VkClearColorValue clear_color {
            color_distribution(random_engine),
            color_distribution(random_engine),
            color_distribution(random_engine),
            1.0f
         };

         const VkImageSubresourceRange image_subres_range {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0, 1,
            0, 1
         };

         vkCmdClearColorImage(
            *command_buffer,
            swap_chain_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            &clear_color,
            1, &image_subres_range);

         image_memory_barrier = {
            VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            nullptr,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED,
            swap_chain_image,
            {
               VK_IMAGE_ASPECT_COLOR_BIT,
               0,
               VK_REMAINING_MIP_LEVELS,
               0,
               VK_REMAINING_ARRAY_LAYERS
            }
         };

         vkCmdPipelineBarrier(
            *command_buffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &image_memory_barrier);
      }

      vkl::EndCommandBuffer(
         command_buffer);
   }

   VkFence image_ready_fence { nullptr };

   const VkFenceCreateInfo info {
      VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      nullptr,
      VK_FENCE_CREATE_SIGNALED_BIT
   };

   vkCreateFence(
      *gpu_device,
      &info,
      nullptr,
      &image_ready_fence);

   while (!vkl::PollWindowSystem({window}))
   {
      vkResetFences(
         *gpu_device,
         1,
         &image_ready_fence);

      uint32_t next_image_index { UINT32_MAX };

      auto result =
         vkAcquireNextImageKHR(
            *gpu_device,
            *swap_chain,
            UINT64_MAX,
            nullptr,
            image_ready_fence,
            &next_image_index);

      vkWaitForFences(
         *gpu_device,
         1,
         &image_ready_fence,
         VK_TRUE,
         UINT64_MAX);

      const auto command_buffer =
         framebuffer_command_buffers[next_image_index];

      VkQueue queue { };

      vkGetDeviceQueue(
         *gpu_device,
         0,
         0,
         &queue);

      const VkSubmitInfo submit_info {
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

      const VkPresentInfoKHR present_info {
         VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
         nullptr,
         0, nullptr,
         1, swap_chain.get(),
         &next_image_index,
         &result
      };

      const auto present_result =
         vkQueuePresentKHR(
            queue,
            &present_info);

      std::this_thread::sleep_for(
            std::chrono::milliseconds(500));
   }

   vkDeviceWaitIdle(
      *gpu_device);

   vkDestroyFence(
      *gpu_device,
      image_ready_fence,
      nullptr);

   return 0;
}
