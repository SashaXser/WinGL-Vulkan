#include "vkl/vkl_command_buffer.h"
#include "vkl/vkl_command_pool.h"
#include "vkl/vkl_device.h"
#include "vkl/vkl_fence.h"
#include "vkl/vkl_instance.h"
#include "vkl/vkl_physical_device.h"
#include "vkl/vkl_surface.h"
#include "vkl/vkl_swap_chain.h"
#include "vkl/vkl_window.h"

#include <vulkan/vulkan.h>

#include <algorithm>
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

   auto swap_chain =
      vkl::CreateSwapChain(
         gpu_device,
         surface);

   if (!swap_chain)
   {
      return -13;
   }

   auto swap_chain_images =
      vkl::GetSwapChainImages(
         swap_chain);

   if (!swap_chain_images)
   {
      return -14;
   }

   // a helper utility that fills the framebuffer commands for clearing
   // the images associated with the swap chain...  each time a new swap
   // chain is created, this needs to be called to clear those images.
   const auto ClearSwapChainImages =
      [ ] (
         const vkl::CommandPoolHandle & command_pool,
         const vkl::DeviceHandle & gpu_device,
         const std::vector< VkImage > & swap_chain_images,
         std::vector< vkl::CommandBufferHandle > & framebuffer_command_buffers )
   {
      // resets the command pool and all of the buffers
      // associated with it.  whatever resources currently
      // allocated in the framebuffer commands will still
      // be there for reuse.
      const bool reset =
         vkl::ResetCommandPool(
            command_pool,
            0);

      if (reset)
      {
         framebuffer_command_buffers.resize(
            swap_chain_images.size());

         for (auto & command_buffer : framebuffer_command_buffers)
         {
            if (!command_buffer)
            {
               // allocate a command buffer since there was
               // not one since the resize of the buffers
               command_buffer =
                  vkl::AllocateCommandBuffer(
                     gpu_device,
                     command_pool,
                     VK_COMMAND_BUFFER_LEVEL_PRIMARY);
            }

            if (command_buffer)
            {
               // begin the command buffer operation and indicate that
               // the buffer will be used over and over without modification
               // this helps the driver allocate the data in the best
               // possible way in the command buffer...
               const bool begin =
                  vkl::BeginCommandBuffer(
                     command_buffer,
                     VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
               
               if (begin)
               {
                  for (const auto & swap_chain_image : swap_chain_images)
                  {
                     // need to move the image from undefined to transfer
                     // destination optimal.  this will allow the clear
                     // command to work correctly.  i still do not quite
                     // understand how to correctly identify the different
                     // access capabilities for source and destination.
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

                     // issue the layout change.  still need to understand
                     // how this operation works.
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

                     // issue the clear color for the image.  the image
                     // will be in the correct layout already.
                     vkCmdClearColorImage(
                        *command_buffer,
                        swap_chain_image,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        &clear_color,
                        1, &image_subres_range);

                     // need to move the image from transfer destination
                     // optimal to the presentation.  this allows the
                     // image to be visualized by the surface.
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

                     // still need to understand how memory barriers
                     // really work, but this performs the actual
                     // layout change.
                     vkCmdPipelineBarrier(
                        *command_buffer,
                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &image_memory_barrier);
                  }

                  const auto end =
                     vkl::EndCommandBuffer(
                        command_buffer);

                  if (!end)
                  {
                     command_buffer.reset();
                  }
               }
            }
         }
      }

      return
         std::all_of(
            framebuffer_command_buffers.cbegin(),
            framebuffer_command_buffers.cend(),
            [ ] ( const auto & command_buffer )
            {
               return
                  command_buffer != nullptr;
            });
   };

   // create the command pool that will be used to
   // allocate command buffers from.  we will set it
   // up such that the command pool can be reset,
   // resetting all of the command buffers allocated
   // at once.
   const auto command_pool =
      vkl::CreateCommandPool(
         gpu_device,
         VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
         queue_family_properties.front().first);

   // allocate an empty set of command buffer handles.
   // each clear command buffer will be associated with
   // a single swap chain image.
   std::vector< vkl::CommandBufferHandle >
      framebuffer_command_buffers {
         swap_chain_images->size()
      };

   const auto resize_set =
      vkl::SetWindowResizeCallback(
         window,
         [ & ] (
            void * const user_data,
            const vkl::WindowHandle & window,
            const uint32_t width,
            const uint32_t height )
         {
            if (width && height)
            {
               // need to wait for the gpu to be idle so
               // we can make sure the commands can be reset
               const auto complete =
                  vkDeviceWaitIdle(
                     *gpu_device);

               if (complete == VK_SUCCESS)
               {
                  // recreate the swap chain with the new
                  // dimensions from the resize...
                  swap_chain =
                     vkl::CreateSwapChain(
                        swap_chain);

                  if (!swap_chain)
                  {
                     swap_chain.reset();
                     swap_chain_images.reset();
                  }
                  else
                  {
                     // get the new swap chain images from the
                     // resize.
                     swap_chain_images =
                        vkl::GetSwapChainImages(
                           swap_chain);

                     if (swap_chain_images)
                     {
                        // create commands that will perform the
                        // clearing of the swap chain images.
                        const bool cleared =
                           ClearSwapChainImages(
                              command_pool,
                              gpu_device,
                              *swap_chain_images,
                              framebuffer_command_buffers);

                        if (!cleared)
                        {
                           swap_chain.reset();
                           swap_chain_images.reset();
                           framebuffer_command_buffers.clear();
                        }
                     }
                  }
               }
            }
         });

   if (resize_set)
   {
      const bool cleared =
         ClearSwapChainImages(
            command_pool,
            gpu_device,
            *swap_chain_images,
            framebuffer_command_buffers);

      if (cleared)
      {
         const auto image_ready_fence =
            vkl::CreateFence(
               gpu_device,
               false);

         if (image_ready_fence)
         {
            while (!vkl::PollWindowSystem({window}) &&
                   swap_chain &&
                   swap_chain_images &&
                   !framebuffer_command_buffers.empty())
            {
               const auto reset =
                  vkl::Reset(
                     image_ready_fence);

               if (reset)
               {
                  uint32_t next_image_index { UINT32_MAX };

                  // get the next image that can be processed, as the
                  // presentation engine has returned it...
                  auto result =
                     vkAcquireNextImageKHR(
                        *gpu_device,
                        *swap_chain,
                        UINT64_MAX,
                        nullptr,
                        *image_ready_fence,
                        &next_image_index);

                  if (VK_SUCCESS == result)
                  {
                     // wait for the image to become available for
                     // processing, as it may still be currently
                     // executing commands for it...
                     const auto wait_complete =
                        vkl::Wait(
                           VK_TRUE,
                           UINT64_MAX,
                           image_ready_fence);

                     if (wait_complete)
                     {
                        const auto command_buffer =
                           framebuffer_command_buffers[next_image_index];

                        VkQueue queue { };

                        vkGetDeviceQueue(
                           *gpu_device,
                           queue_family_properties.front().first,
                           0,
                           &queue);

                        // submit the command buffer to the graphics device,
                        // which will clear the requested swap chain image
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

                        if (VK_SUCCESS == result)
                        {
                           // tell the presentation engine that the swap chain
                           // image is ready to be shown.
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
                     }
                  }
               }
            }
         }
      }
   }

   // wait for all the commands to have been executed before
   // deallocation of the resources takes place.
   vkDeviceWaitIdle(
      *gpu_device);

   return 0;
}
