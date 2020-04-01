#include "vkl_swap_chain.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"
#include "vkl_surface.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <utility>

namespace vkl
{

namespace
{

struct Context
{
   PhysicalDeviceHandle physical_device;
   DeviceHandle device;
   SurfaceHandle surface;
   VkSwapchainCreateInfoKHR swap_chain_create_info;

   // todo, this vector needs to change to use a
   // SwapChainImageHandle type instead of vkimage
   std::vector< VkImage > swap_chain_images;
};

} // namespace

void DestroySwapChainHandle(
   const VkSwapchainKHR * const swap_chain )
{
   if (swap_chain)
   {
      if (*swap_chain)
      {
         const auto device =
            vkl::internal::GetContextData(
               swap_chain,
               &Context::device);

         if (device && *device)
         {
            vkDestroySwapchainKHR(
               *device,
               *swap_chain,
               DefaultAllocator());
         }
      }

      vkl::internal::DeallocateContext(
         swap_chain);
   }
}

bool SupportsPresentation(
   const DeviceHandle & device,
   const SurfaceHandle & surface )
{
   VkBool32 supported { VK_FALSE };

   const auto physical_device =
      GetPhysicalDevice(device);
   const auto queue_family =
      GetQueueFamily(
         device);

   if (device && *device &&
       surface && *surface &&
       physical_device && *physical_device &&
       queue_family)
   {
      const auto result =
         vkGetPhysicalDeviceSurfaceSupportKHR(
            *physical_device,
            queue_family->first,
            *surface,
            &supported);
   }

   return
      supported == VK_TRUE;
}

VkPresentModeKHR GetDefaultPresentMode(
   const std::vector< VkPresentModeKHR > & present_modes )
{
   VkPresentModeKHR present_mode {
      VK_PRESENT_MODE_FIFO_KHR };

   auto mode =
      std::find(
         present_modes.cbegin(),
         present_modes.cend(),
         VK_PRESENT_MODE_MAILBOX_KHR);

   if (present_modes.cend() == mode)
   {
      mode =
         std::find(
            present_modes.cbegin(),
            present_modes.cend(),
            VK_PRESENT_MODE_IMMEDIATE_KHR);

      if (present_modes.cend() == mode)
      {
         present_mode =
            VK_PRESENT_MODE_IMMEDIATE_KHR;
      }

      mode =
         std::find(
            present_modes.cbegin(),
            present_modes.cend(),
            VK_PRESENT_MODE_FIFO_KHR);

      if (present_modes.cend() == mode)
      {
         if (!present_modes.empty())
         {
            present_mode =
               present_modes.front();
         }
      }
      else
      {
         present_mode =
            VK_PRESENT_MODE_FIFO_KHR;
      }
   }
   else
   {
      present_mode =
         VK_PRESENT_MODE_MAILBOX_KHR;
   }

   return present_mode;
}

std::vector< VkImage > GetSwapChainImages(
   const DeviceHandle & device,
   const SwapChainHandle & swap_chain )
{
   std::vector< VkImage > swap_chain_images;

   uint32_t swap_chain_image_count { };

   auto result =
      vkGetSwapchainImagesKHR(
         *device,
         *swap_chain,
         &swap_chain_image_count,
         nullptr);

   if (VK_SUCCESS == result &&
       swap_chain_image_count)
   {
      swap_chain_images.resize(
         swap_chain_image_count,
         { });

      auto result =
         vkGetSwapchainImagesKHR(
            *device,
            *swap_chain,
            &swap_chain_image_count,
            swap_chain_images.data());

      if (VK_SUCCESS != result)
      {
         swap_chain_images.clear();
      }
      else
      {
         // todo: create by assigning to SwapChainImageHandle
      }
   }

   return swap_chain_images;
}

SwapChainHandle CreateSwapChain(
   const DeviceHandle & device,
   const SurfaceHandle & surface )
{
   SwapChainHandle swap_chain {
      nullptr, &DestroySwapChainHandle };

   if (device && *device &&
       surface && *surface)
   {
      if (SupportsPresentation(device, surface))
      {
         const auto surface_capabilities =
            GetSurfaceCapabilites(surface);
         const auto surface_formats =
            GetSurfaceFormats(surface);
         const auto surface_present_modes =
            GetSurfacePresentModes(surface);

         if (surface_capabilities &&
             surface_formats &&
             surface_present_modes)
         {
            const uint32_t min_image_count =
               std::clamp(
                  static_cast< uint32_t >(3),
                  surface_capabilities->minImageCount,
                  surface_capabilities->maxImageCount);

            // the structure is missing some validity checks
            // this will need to be addressed at a later time
            const VkSwapchainCreateInfoKHR info {
               VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
               nullptr,
               0,
               *surface,
               min_image_count,
               surface_formats->front().format,
               surface_formats->front().colorSpace,
               surface_capabilities->currentExtent,
               1,
               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
               VK_SHARING_MODE_EXCLUSIVE,
               0,
               nullptr,
               VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
               VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
               GetDefaultPresentMode(*surface_present_modes),
               VK_FALSE,
               nullptr
            };

            swap_chain.reset(
               vkl::internal::AllocateContext<
                  VkSwapchainKHR,
                  Context >(
                     GetPhysicalDevice(device),
                     device,
                     surface,
                     info),
               &DestroySwapChainHandle);

            if (swap_chain)
            {
               const auto result =
                  vkCreateSwapchainKHR(
                     *device,
                     &info,
                     DefaultAllocator(),
                     swap_chain.get());

               if (result != VK_SUCCESS)
               {
                  std::cerr
                     << "Unable to create swap chain ("
                     << result
                     << ")!"
                     << std::endl;

                  swap_chain.reset();
               }
               else
               {
                  std::cout
                     << "Swap chain created successfully!"
                     << std::endl;

                  auto swap_chain_images =
                     GetSwapChainImages(
                        device,
                        swap_chain);

                  if (swap_chain_images.empty())
                  {
                     std::cerr
                        << "Unable to obtain swap chain images!"
                        << std::endl;

                     swap_chain.reset();
                  }
                  else
                  {
                     const bool set =
                        vkl::internal::SetContextData(
                           swap_chain.get(),
                           &Context::swap_chain_images,
                           std::move(swap_chain_images));

                     if (!set)
                     {
                        std::cerr
                           << "Unable to store swap chain images!"
                           << std::endl;

                        swap_chain.reset();
                     }
                  }
               }
            }
         }
      }
   }

   return swap_chain;
}

DeviceHandle GetDevice(
   const SwapChainHandle & swap_chain )
{
   return
      vkl::internal::GetContextData(
         swap_chain.get(),
         &Context::device);
}

PhysicalDeviceHandle GetPhysicalDevice(
   const SwapChainHandle & swap_chain )
{
   return
      vkl::internal::GetContextData(
         swap_chain.get(),
         &Context::physical_device);
}

SurfaceHandle GetSurface(
   const SwapChainHandle & swap_chain )
{
   return
      vkl::internal::GetContextData(
         swap_chain.get(),
         &Context::surface);
}

std::optional< VkFormat >
GetFormat(
   const SwapChainHandle & swap_chain )
{
   std::optional< VkFormat > format { };

   const auto * context =
      vkl::internal::GetContextData<
         Context >(
            swap_chain.get());

   if (context)
   {
      format =
         context->swap_chain_create_info.imageFormat;
   }

   return format;
}

std::optional< VkExtent2D >
GetExtent(
   const SwapChainHandle & swap_chain )
{
   std::optional< VkExtent2D > extent { };

   const auto * context =
      vkl::internal::GetContextData<
         Context >(
            swap_chain.get());

   if (context)
   {
      extent =
         context->swap_chain_create_info.imageExtent;
   }

   return extent;
}

std::optional< std::vector< VkImage > >
GetSwapChainImages(
   const SwapChainHandle & swap_chain )
{
   std::optional< std::vector< VkImage > >
      swap_chain_images;

   const auto * context =
      vkl::internal::GetContextData<
         Context >(
            swap_chain.get());

   if (context)
   {
      swap_chain_images =
         context->swap_chain_images;
   }

   return swap_chain_images;
}

} // namespace vkl
