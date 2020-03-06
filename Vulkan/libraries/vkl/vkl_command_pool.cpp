#include "vkl_command_pool.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <cstddef>
#include <iostream>

namespace vkl
{

constexpr size_t DEVICE_INDEX = 1;
constexpr size_t PHYSICAL_DEVICE_INDEX = 2;
constexpr size_t COMMAND_POOL_CREATE_FLAGS_INDEX = 3;
constexpr size_t QUEUE_FAMILY_INDEX_INDEX = 4;
constexpr size_t CONTEXT_SIZE = 5;

void DestroyCommandPoolHandle(
   const VkCommandPool * const command_pool )
{
   if (command_pool)
   {
      if (*command_pool)
      {
         vkDestroyCommandPool(
            vkl::internal::GetContextData<
               VkDevice,
               DEVICE_INDEX >(
                  command_pool),
            *command_pool,
            DefaultAllocator());
      }

      vkl::internal::DeallocateContext<
         CONTEXT_SIZE >(
            command_pool);
   }
}

CommandPoolHandle SetCommandPoolContext(
   const vkl::internal::context_ptr_t context,
   const DeviceHandle & device,
   const VkCommandPoolCreateFlags create_flags,
   const uint32_t queue_family_index )
{
   vkl::internal::SetContextData<
      PHYSICAL_DEVICE_INDEX,
      CONTEXT_SIZE >(
         context,
         GetPhysicalDevice(device));

   vkl::internal::SetContextData<
      DEVICE_INDEX,
      CONTEXT_SIZE >(
         context,
         *device);

   vkl::internal::SetContextData<
      COMMAND_POOL_CREATE_FLAGS_INDEX,
      CONTEXT_SIZE >(
         context,
         create_flags);

   vkl::internal::SetContextData<
      QUEUE_FAMILY_INDEX_INDEX,
      CONTEXT_SIZE >(
         context,
         queue_family_index);

   return {
      vkl::internal::GetContextPointer<
         CONTEXT_SIZE,
         VkCommandPool >(
            context),
      &DestroyCommandPoolHandle
   };
}

CommandPoolHandle CreateCommandPool(
   const DeviceHandle & device,
   const VkCommandPoolCreateFlags create_flags,
   const uint32_t queue_family_index )
{
   CommandPoolHandle command_pool {
      nullptr, &DestroyCommandPoolHandle };

   if (device && *device)
   {
      const auto context =
         vkl::internal::AllocateContext<
            CONTEXT_SIZE >();

      if (context)
      {
         command_pool =
            SetCommandPoolContext(
               context,
               device,
               create_flags,
               queue_family_index);

         const VkCommandPoolCreateInfo info {
            VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            nullptr,
            create_flags,
            queue_family_index
         };

         const auto success =
            vkCreateCommandPool(
               *device,
               &info,
               DefaultAllocator(),
               command_pool.get());

         if (success != VK_SUCCESS)
         {
            std::cerr
               << "Command pool not created successfully ("
               << success
               << ")!"
               << std::endl;

            command_pool.reset();
         }
         else
         {
            std::cout
               << "Command pool created successfully!"
               << std::endl;
         }
      }
   }

   return command_pool;
}

bool ResetCommandPool(
   const CommandPoolHandle & command_pool,
   const VkCommandPoolResetFlags & reset_flags )
{
   bool reset = false;

   if (command_pool && *command_pool)
   {
      if (const auto device =
          GetDevice(command_pool))
      {
         const auto result =
            vkResetCommandPool(
               device,
               *command_pool,
               reset_flags);

         reset = result == VK_SUCCESS;

         if (result != VK_SUCCESS)
         {
            std::cerr
               << "Could not reset command pool ("
               << result
               << ")!"
               << std::endl;
         }
         else
         {
            std::cout
               << "Command pool reset!"
               << std::endl;
         }
      }
   }

   return reset;
}

VkDevice GetDevice(
   const CommandPoolHandle & command_pool )
{
   return
      vkl::internal::GetContextData<
         VkDevice,
         DEVICE_INDEX >(
            command_pool.get());
}

VkPhysicalDevice GetPhysicalDevice(
   const CommandPoolHandle & command_pool )
{
   return
      vkl::internal::GetContextData<
         VkPhysicalDevice,
         PHYSICAL_DEVICE_INDEX >(
            command_pool.get());
}

} // namespace vkl
