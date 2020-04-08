#include "vkl_fence.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"

#include <algorithm>
#include <array>
#include <cassert>

namespace vkl
{

namespace
{

struct Context
{
   DeviceHandle device;
};

} // namespace

void DestroyFenceHandle(
   const VkFence * const fence )
{
   if (fence)
   {
      if (*fence)
      {
         const auto device =
            vkl::internal::GetContextData(
               fence,
               &Context::device);

         if (device && *device)
         {
            vkDestroyFence(
               *device,
               *fence,
               DefaultAllocator());
         }
      }

      vkl::internal::DeallocateContext(
         fence);
   }
}

FenceHandle CreateFence(
   const DeviceHandle & device,
   const bool signaled )
{
   FenceHandle fence {
      nullptr, &DestroyFenceHandle };

   if (device && *device)
   {
      fence.reset(
         vkl::internal::AllocateContext<
            VkFence,
            Context >(
               device),
         &DestroyFenceHandle);

      if (fence)
      {
         const VkFenceCreateInfo info {
            VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            nullptr,
            signaled ?
            VK_FENCE_CREATE_SIGNALED_BIT :
            static_cast< VkFenceCreateFlags >(0)
         };

         const auto result =
            vkCreateFence(
               *device,
               &info,
               DefaultAllocator(),
               fence.get());

         if (result != VK_SUCCESS)
         {
            fence.reset();
         }
      }
   }

   return fence;
}

bool IsSignaled(
   const FenceHandle & fence )
{
   bool signaled { false };

   const auto device =
      vkl::internal::GetContextData(
         fence.get(),
         &Context::device);

   if (fence && *fence &&
       device && *device)
   {
      const auto result =
         vkGetFenceStatus(
            *device,
            *fence);

      signaled =
         result == VK_SUCCESS;
   }

   return signaled;
}

template < size_t N >
bool AllValidFences(
   const std::array< const FenceHandle, N > & fences )
{
   return
      std::all_of(
         fences.cbegin(),
         fences.cend(),
         [ ] ( const FenceHandle & fence )
         {
            bool valid { false };
         
            if (fence && *fence)
            {
               const auto device =
                  vkl::internal::GetContextData(
                     fence.get(),
                     &Context::device);
         
               valid =
                  device && *device;
            }
         
            return valid;
         });
}

template < size_t N >
bool AllFencesFromSameDevice(
   const DeviceHandle & device,
   const std::array< const FenceHandle, N > & fences )
{
   return
      std::all_of(
         fences.cbegin(),
         fences.cend(),
         [ device ] ( const FenceHandle & fence )
         {
            DeviceHandle fence_device { nullptr };
         
            if (fence && *fence)
            {
               fence_device =
                  vkl::internal::GetContextData(
                     fence.get(),
                     &Context::device);
            }
         
            return fence_device == device;
         });
}

template < size_t N >
bool Reset(
   const std::array< const FenceHandle, N > & fences )
{
   bool reset { false };

   const bool valid_fences =
      AllValidFences(fences);

   if (valid_fences)
   {
      const auto device =
         vkl::internal::GetContextData(
            fences.front().get(),
            &Context::device);

#if !NDEBUG

      assert(
         AllFencesFromSameDevice(
            device,
            fences));

#endif

      std::array< VkFence, N > vk_fences;

      std::for_each(
         vk_fences.begin(),
         vk_fences.end(),
         [ fence = std::cbegin(fences) ]
         ( VkFence & vk_fence ) mutable
         {
            vk_fence = **fence++;
         });

      const auto result =
         vkResetFences(
            *device,
            N,
            vk_fences.data());

      reset =
         result == VK_SUCCESS;
   }

   return reset;
}

#define DEFINE_FENCE_RESET_FUNC( size ) \
   template \
   bool Reset< size >( \
      const std::array< const FenceHandle, size > & fences )

DEFINE_FENCE_RESET_FUNC( 1 );
DEFINE_FENCE_RESET_FUNC( 2 );
DEFINE_FENCE_RESET_FUNC( 3 );
DEFINE_FENCE_RESET_FUNC( 4 );
DEFINE_FENCE_RESET_FUNC( 5 );

template < size_t N >
bool Wait(
   const std::array< const FenceHandle, N > & fences,
   const bool wait_for_all,
   const uint64_t timeout )
{
   bool success { false };

   const bool valid_fences =
      AllValidFences(fences);

   if (valid_fences)
   {
      const auto device =
         vkl::internal::GetContextData(
            fences.front().get(),
            &Context::device);

#if !NDEBUG

      assert(
         AllFencesFromSameDevice(
            device,
            fences));

#endif

      std::array< VkFence, N > vk_fences;

      std::for_each(
         vk_fences.begin(),
         vk_fences.end(),
         [ fence = std::cbegin(fences) ]
         ( VkFence & vk_fence ) mutable
         {
            vk_fence = **fence++;
         });

      const auto result =
         vkWaitForFences(
            *device,
            N,
            vk_fences.data(),
            wait_for_all,
            timeout);

      success =
         result == VK_SUCCESS;
   }

   return success;
}

#define DEFINE_FENCE_WAIT_FUNC( size ) \
   template bool Wait< size >( \
      const std::array< const FenceHandle, size > & fences, \
      const bool wait_for_all, \
      const uint64_t timeout )

DEFINE_FENCE_WAIT_FUNC( 1 );
DEFINE_FENCE_WAIT_FUNC( 2 );
DEFINE_FENCE_WAIT_FUNC( 3 );
DEFINE_FENCE_WAIT_FUNC( 4 );
DEFINE_FENCE_WAIT_FUNC( 5 );

} // namespace vkl
