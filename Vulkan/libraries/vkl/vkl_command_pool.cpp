#include "vkl_command_pool.h"
#include "vkl_allocator.h"
#include "vkl_context_data.h"
#include "vkl_device.h"

#include <iostream>
#include <new>

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

      delete []
         (reinterpret_cast< const size_t * >(
            command_pool) - (CONTEXT_SIZE - 1));
   }
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
      size_t * const context =
         new (std::nothrow) size_t[CONTEXT_SIZE] { };

      if (context)
      {
         *(context + (CONTEXT_SIZE - PHYSICAL_DEVICE_INDEX - 1)) =
            reinterpret_cast< size_t >(
               GetPhysicalDevice(device));

         *(context + (CONTEXT_SIZE - DEVICE_INDEX - 1)) =
            reinterpret_cast< size_t >(
               *device);

         *(context + (CONTEXT_SIZE - COMMAND_POOL_CREATE_FLAGS_INDEX - 1)) =
            create_flags;

         *(context + (CONTEXT_SIZE - QUEUE_FAMILY_INDEX_INDEX - 1)) =
            queue_family_index;

         command_pool.reset(
            reinterpret_cast< VkCommandPool * >(
               context + (CONTEXT_SIZE - 1)));

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

            command_pool.reset(
               nullptr);
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
