#include <vulkan/vulkan.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <ios>
#include <iostream>
#include <map>
#include <memory>
#include <new>
#include <random>
#include <string>
#include <utility>
#include <vector>

class Allocator
{
public:
   static void * VKAPI_CALL Allocate(
      void * const user_data,
      const size_t size,
      const size_t alignment,
      const VkSystemAllocationScope scope );
   static void * VKAPI_CALL Reallocate(
      void * const user_data,
      void * const original_data,
      const size_t size,
      const size_t alignment,
      const VkSystemAllocationScope scope );
   static void VKAPI_CALL Free(
      void * const user_data,
      void * const original_data );

   static void VKAPI_CALL InternalAllocate(
      void * const user_data,
      const size_t size,
      const VkInternalAllocationType type,
      const VkSystemAllocationScope scope );
   static void VKAPI_CALL InternalFree(
      void * const user_data,
      const size_t size,
      const VkInternalAllocationType type,
      const VkSystemAllocationScope scope );

private:
   void * Allocate(
      const size_t size,
      const size_t alignment,
      const VkSystemAllocationScope scope );
   void * Reallocate(
      void * const original_data,
      const size_t size,
      const size_t alignment,
      const VkSystemAllocationScope scope );
   void Free(
      void * const original_data );

   void InternalAllocate(
      const size_t size,
      const VkInternalAllocationType type,
      const VkSystemAllocationScope scope );
   void InternalFree(
      const size_t size,
      const VkInternalAllocationType type,
      const VkSystemAllocationScope scope );

   size_t allocated_ { };

} g_allocator;

void * VKAPI_CALL Allocator::Allocate(
   void * const user_data,
   const size_t size,
   const size_t alignment,
   const VkSystemAllocationScope scope )
{
   return
      reinterpret_cast< Allocator * >(
         user_data)->Allocate(
            size,
            alignment,
            scope);
}

void * VKAPI_CALL Allocator::Reallocate(
   void * const user_data,
   void * const original_data,
   const size_t size,
   const size_t alignment,
   const VkSystemAllocationScope scope )
{
   return
      reinterpret_cast< Allocator * >(
         user_data)->Reallocate(
            original_data,
            size,
            alignment,
            scope);
}

void VKAPI_CALL Allocator::Free(
   void * const user_data,
   void * const original_data )
{
   reinterpret_cast< Allocator * >(
      user_data)->Free(
         original_data);
}

void VKAPI_CALL Allocator::InternalAllocate(
   void * const user_data,
   const size_t size,
   const VkInternalAllocationType type,
   const VkSystemAllocationScope scope )
{
   reinterpret_cast< Allocator * >(
      user_data)->InternalAllocate(
         size,
         type,
         scope);
}

void VKAPI_CALL Allocator::InternalFree(
   void * const user_data,
   const size_t size,
   const VkInternalAllocationType type,
   const VkSystemAllocationScope scope )
{
   reinterpret_cast< Allocator * >(
      user_data)->InternalFree(
         size,
         type,
         scope);
}

const char * DecodeScope(
   const VkSystemAllocationScope scope )
{
   const char * scope_name = "unknown";

   switch (scope)
   {
   case VK_SYSTEM_ALLOCATION_SCOPE_COMMAND: scope_name = "command"; break;
   case VK_SYSTEM_ALLOCATION_SCOPE_OBJECT: scope_name = "object"; break;
   case VK_SYSTEM_ALLOCATION_SCOPE_CACHE: scope_name = "cache"; break;
   case VK_SYSTEM_ALLOCATION_SCOPE_DEVICE: scope_name = "device"; break;
   case VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE: scope_name = "instance"; break;
   }

   return scope_name;
}

void * Allocator::Allocate(
   const size_t size,
   const size_t alignment,
   const VkSystemAllocationScope scope )
{
   // expected to be a power of two
   assert(
      alignment != 0 &&
      (alignment & (alignment - 1)) == 0);

   const double ratio =
      24.0 / alignment;
   const size_t header_size =
      ratio > 1.0 ?
      uint32_t(ratio + 1) * alignment :
      alignment;

   uint8_t * const header =
      reinterpret_cast< uint8_t * >(
         ::operator new(
            size + header_size,
            std::align_val_t{ alignment }));

   size_t * const data =
      reinterpret_cast< size_t * >(
         header + header_size);

   *(data - 1) =
      reinterpret_cast< size_t >(header);

   *(data - 2) = alignment;

   *(data - 3) = size;

   std::cout
      << "Allocating " << size
      << " bytes with alignment "
      << alignment << " and scope "
      << DecodeScope(scope)
      << ".  Allocated "
      << (allocated_ += size) / 1048576.0
      << " MiB (0x"
      << std::hex << data << ")" << std::dec
      << std::endl;

   return data;
}

void * Allocator::Reallocate(
   void * const original_data,
   const size_t size,
   const size_t alignment,
   const VkSystemAllocationScope scope )
{
   void * data = nullptr;

   if (!original_data)
      data = Allocate(size, alignment, scope);
   else if (size == 0)
      Free(original_data);
   else
   {
      // there are other rules but lets assume they are satisfied
      data = Allocate(size, alignment, scope);

      const size_t original_size =
         *(reinterpret_cast< size_t * >(original_data) - 3);

      std::memcpy(
         data,
         original_data,
         std::min(original_size, size));

      Free(original_data);
   }

   return data;
}

void Allocator::Free(
   void * const original_data )
{
   if (original_data)
   {
      size_t * const data =
         reinterpret_cast< size_t * >(original_data);

      size_t * const header =
         reinterpret_cast< size_t * >(*(data - 1));

      const size_t alignment = *(data - 2);

      const size_t size = *(data - 3);

      ::operator delete(
         header,
         std::align_val_t { alignment });

      std::cout
         << "Freeing " << size
         << " bytes with alignment "
         << alignment
         << ".  Allocated "
         << (allocated_ -= size) / 1048576.0
         << " MiB (0x"
         << std::hex << data << ")" << std::dec
         << std::endl;
   }
}

const char * DecodeType(
   const VkInternalAllocationType type )
{
   const char * type_name = "unknown";

   switch (type)
   {
   case VK_INTERNAL_ALLOCATION_TYPE_EXECUTABLE: type_name = "executable"; break;
   }

   return type_name;
}

void Allocator::InternalAllocate(
   const size_t size,
   const VkInternalAllocationType type,
   const VkSystemAllocationScope scope )
{
   std::cout
      << "Internally Allocating "
      << size
      << " bytes with scope "
      << DecodeScope(scope)
      << " and type "
      << DecodeType(type)
      << ".  Allocated "
      << (allocated_ += size) / 1048576.0
      << " MiB"
      << std::endl;
}

void Allocator::InternalFree(
   const size_t size,
   const VkInternalAllocationType type,
   const VkSystemAllocationScope scope )
{
   std::cout
      << "Internally Freeing "
      << size
      << " bytes with scope "
      << DecodeScope(scope)
      << " and type "
      << DecodeType(type)
      << ".  Allocated "
      << (allocated_ -= size) / 1048576.0
      << " MiB"
      << std::endl;
}

VkAllocationCallbacks CreateAllocator( )
{
   VkAllocationCallbacks callbacks { };
   callbacks.pUserData = &g_allocator;
   callbacks.pfnAllocation = &Allocator::Allocate;
   callbacks.pfnReallocation = &Allocator::Reallocate;
   callbacks.pfnFree = &Allocator::Free;
   callbacks.pfnInternalAllocation = &Allocator::InternalAllocate;
   callbacks.pfnInternalFree = &Allocator::InternalFree;

   return callbacks;
}

void DestroyInstanceHandle(
   const VkInstance * const instance )
{
   if (instance && *instance)
   {
      vkDestroyInstance(
         *instance,
         &CreateAllocator());
   }

   delete instance;
}

using InstanceHandle =
   std::unique_ptr<
      VkInstance,
      decltype(&DestroyInstanceHandle) >;

void DestroyLogicalDeviceHandle(
   const VkDevice * const device )
{
   if (device && *device)
   {
      vkDestroyDevice(
         *device,
         &CreateAllocator());
   }

   delete device;
}

using LogicalDeviceHandle =
   std::unique_ptr<
      VkDevice,
      decltype(&DestroyLogicalDeviceHandle) >;

InstanceHandle CreateInstance( )
{
   VkApplicationInfo application_info;
   application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   application_info.pNext = nullptr;
   application_info.pApplicationName = "vulkan-logical-devices";
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

   InstanceHandle instance {
      new VkInstance { nullptr },
      &DestroyInstanceHandle };

   const VkResult created =
      vkCreateInstance(
         &create_info,
         &CreateAllocator(),
         instance.get());

   if (created != VK_SUCCESS)
   {
      std::cerr
         << "Unable to create vulkan instance ("
         << created
         << ")!"
         << std::endl;

      instance = nullptr;
   }
   
   return instance;
}

std::map< uint32_t, std::vector< VkPhysicalDevice > >
GetGPUDevices( const InstanceHandle & instance )
{
   uint32_t physical_count { };

   const VkResult enumerated =
      vkEnumeratePhysicalDevices(
         *instance,
         &physical_count,
         nullptr);

   if (enumerated != VK_SUCCESS)
   {
      std::cerr
         << "Unable to enumerate physical devices ("
         << enumerated
         << ")!"
         << std::endl;
   }

   if (!physical_count)
   {
      std::cerr
         << "There are no physical devices installed on the system!"
         << std::endl;
   }

   std::map< uint32_t, std::vector< VkPhysicalDevice > >
      gpu_devices;

   std::vector< VkPhysicalDevice > physical_devices(
      physical_count, { });

   vkEnumeratePhysicalDevices(
      *instance,
      &physical_count,
      physical_devices.data());

   for (const auto physical_device : physical_devices)
   {
      VkPhysicalDeviceProperties properties { };

      vkGetPhysicalDeviceProperties(
         physical_device,
         &properties);

      switch (properties.deviceType)
      {
      case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
         gpu_devices[0].push_back(physical_device); break;
      case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
         gpu_devices[1].push_back(physical_device); break;
      case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
         gpu_devices[2].push_back(physical_device); break;
      }
   }

   return gpu_devices;
}

std::vector< uint32_t >
GetGPUQueueIndices(
   const VkPhysicalDevice physical_device )
{
   uint32_t queue_count { };

   vkGetPhysicalDeviceQueueFamilyProperties(
      physical_device,
      &queue_count,
      nullptr);

   std::vector< VkQueueFamilyProperties >
      properties { queue_count, VkQueueFamilyProperties { } };

   vkGetPhysicalDeviceQueueFamilyProperties(
      physical_device,
      &queue_count,
      properties.data());

   std::vector< uint32_t > indices;

   for (const auto & property : properties)
   {
      if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      {
         indices.push_back(
            static_cast< uint32_t >(
               &property - properties.data()));
      }
   }

   return indices;
}

LogicalDeviceHandle CreateLogicalDevice(
   const VkPhysicalDevice physical_device )
{
   VkPhysicalDeviceProperties properties { };

   vkGetPhysicalDeviceProperties(
      physical_device,
      &properties);

   std::cout
      << "Creating Logical Device On "
      << properties.deviceName
      << std::endl;

   const auto gpu_queue_family_indices =
      GetGPUQueueIndices(
         physical_device);

   if (gpu_queue_family_indices.empty())
   {
      std::cerr
         << "Unable to create vulkan logical device! "
            "There are no GPU queue family indices!"
         << std::endl;

      return { nullptr, &DestroyLogicalDeviceHandle };
   }

   VkDeviceQueueCreateInfo queue_create_info;
   queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
   queue_create_info.pNext = nullptr;
   queue_create_info.flags = 0;
   queue_create_info.queueFamilyIndex = gpu_queue_family_indices.front();
   queue_create_info.queueCount = 1;
   queue_create_info.pQueuePriorities = nullptr;

#if _DEBUG

   uint32_t ilayer_count { };

   const VkResult enumerated =
      vkEnumerateInstanceLayerProperties(
         &ilayer_count,
         nullptr);

   if (enumerated != VK_SUCCESS)
   {
      std::cerr
         << "Unable to enumerate instance layer properties ("
         << enumerated
         << ")!"
         << std::endl;
   }

   std::vector< const char * > ilayers;
   std::vector< VkLayerProperties > il_properties {
      ilayer_count, VkLayerProperties { } };

   if (ilayer_count)
   {
      vkEnumerateInstanceLayerProperties(
         &ilayer_count,
         il_properties.data());

      for (const auto & ilayer : il_properties)
      {
         ilayers.push_back(ilayer.layerName);
      }
   }

#endif // _DEBUG

   VkPhysicalDeviceFeatures supported_features { };

   vkGetPhysicalDeviceFeatures(
      physical_device,
      &supported_features);

   VkDeviceCreateInfo create_info;
   create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
   create_info.pNext = nullptr;
   create_info.flags = 0;
   create_info.queueCreateInfoCount = 1;
   create_info.pQueueCreateInfos = &queue_create_info;

#if _DEBUG
   
   create_info.enabledLayerCount = 0;
   create_info.ppEnabledLayerNames = nullptr;

   if (!ilayers.empty())
   {
      create_info.enabledLayerCount = ilayer_count;
      create_info.ppEnabledLayerNames = ilayers.data();
   }

#else // _DEBUG
   create_info.enabledLayerCount = 0;
   create_info.ppEnabledLayerNames = nullptr;
#endif // _DEBUG

   create_info.enabledExtensionCount = 0;
   create_info.ppEnabledExtensionNames = nullptr;
   create_info.pEnabledFeatures = &supported_features;

   LogicalDeviceHandle logical_device = {
      new VkDevice { nullptr },
      &DestroyLogicalDeviceHandle };

   const VkResult created =
      vkCreateDevice(
         physical_device,
         &create_info,
         &CreateAllocator(),
         logical_device.get());

   if (created != VK_SUCCESS)
   {
      std::cerr
         << "Unable to create vulkan logical device ("
         << created
         << ")!"
         << std::endl;

      logical_device = nullptr;
   }

   return logical_device;
}

void DestroyBufferHandle(
   VkBuffer * const buffer )
{
   size_t * const context =
      reinterpret_cast< size_t * >(
         buffer) - 1;

   if (buffer && *buffer)
   {
      VkDevice * const device =
         reinterpret_cast< VkDevice * >(
            context);

      vkDestroyBuffer(
         *device,
         *buffer,
         &CreateAllocator());
   }

   delete [] context;
}

using BufferHandle =
   std::unique_ptr<
      VkBuffer,
      decltype(&DestroyBufferHandle) >;

std::string DecodeBufferUsage(
   const VkBufferUsageFlags usage )
{
   const char * const bits[] =
   {
      "Transfer Source", "Transfer Destination",
      "Uniform Texel Buffer", "Storage Texel Buffer",
      "Uniform Buffer", "Storage Buffer",
      "Index Buffer", "Vertex Buffer",
      "Indirect Buffer", "Transform Feedback Buffer",
      "Transform Feedback Counter Buffer", "Conditional Rendering",
      "Ray Tracing", "Shader Device Address"
   };

   std::string usages("(");

   for (uint32_t i = 0; i < 32; ++i)
   {
      if (usage & 1 << i)
      {
         if (i <= sizeof(bits) / sizeof(*bits))
            usages += std::string(bits[i]) + ", ";
         else
            usages += "Unknown Usage " + std::to_string(i);
      }
   }

   return usages + ")";
}

const char * DecodeSharingMode(
   const VkSharingMode mode )
{
   const char * string_mode =
      "Unknown Sharing Mode";

   switch (mode)
   {
   case VK_SHARING_MODE_EXCLUSIVE: string_mode = "Sharing Mode Exclusive"; break;
   case VK_SHARING_MODE_CONCURRENT: string_mode = "Sharing Mode Concurrent"; break;
   }

   return string_mode;
}

BufferHandle CreateBuffer(
   const LogicalDeviceHandle & device,
   const size_t size,
   const VkBufferUsageFlags usage,
   const VkSharingMode mode )
{
   BufferHandle buffer {
      nullptr,
      &DestroyBufferHandle };

   if (device && *device)
   {
      size_t * const context =
         new (std::nothrow) size_t[2] { };
      
      if (context)
      {
         *context =
            reinterpret_cast< size_t >(
               *device);
      
         buffer.reset(
            reinterpret_cast< VkBuffer * >(
               context + 1));
      
         const VkBufferCreateInfo info {
            VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            nullptr,
            0, /* no sparse buffers */
            size,
            usage,
            mode,
            0,
            nullptr
         };

         std::cout
            << "Creating buffer of size "
            << size / 1048576.0
            << " MiB, "
            << " usage "
            << DecodeBufferUsage(usage)
            << ", and mode "
            << DecodeSharingMode(mode)
            << std::endl;

         const auto result =
            vkCreateBuffer(
               *device,
               &info,
               &CreateAllocator(),
               buffer.get());

         if (result == VK_SUCCESS)
         {
            std::cout
               << "Buffer Created Successfully!"
               << std::endl;
         }
         else
         {
            std::cout
               << "Buffer Created Unsuccessfully!"
               << std::endl;
         }
      }
   }

   return buffer;
}

std::vector< VkFormat > GetVkFormats( )
{
   std::vector< VkFormat > formats;

   size_t ext_range_size =
      VK_FORMAT_ASTC_12x12_SRGB_BLOCK -
      VK_FORMAT_R4G4_UNORM_PACK8 + 1;

   for (size_t i = 0; i < ext_range_size; ++i)
   {
      formats.emplace_back(
         static_cast< VkFormat >(
            static_cast< size_t >(VK_FORMAT_R4G4_UNORM_PACK8) + i));
   }

   ext_range_size =
      VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM -
      VK_FORMAT_G8B8G8R8_422_UNORM + 1;

   for (size_t i = 0; i < ext_range_size; ++i)
   {
      formats.emplace_back(
         static_cast< VkFormat >(
            static_cast< size_t >(VK_FORMAT_G8B8G8R8_422_UNORM) + i));
   }

   ext_range_size =
      VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG -
      VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG + 1;

   for (size_t i = 0; i < ext_range_size; ++i)
   {
      formats.emplace_back(
         static_cast< VkFormat >(
            static_cast< size_t >(VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG) + i));
   }

   ext_range_size =
      VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT -
      VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT + 1;

   for (size_t i = 0; i < ext_range_size; ++i)
   {
      formats.emplace_back(
         static_cast< VkFormat >(
            static_cast< size_t >(VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT) + i));
   }

   ext_range_size =
      VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT -
      VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT + 1;

   for (size_t i = 0; i < ext_range_size; ++i)
   {
      formats.emplace_back(
         static_cast< VkFormat >(
            static_cast< size_t >(VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT) + i));
   }

   ext_range_size =
      VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT -
      VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT + 1;

   for (size_t i = 0; i < ext_range_size; ++i)
   {
      formats.emplace_back(
         static_cast< VkFormat >(
            static_cast< size_t >(VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT) + i));
   }

   return formats;
}

void DisplayPhysicalDeviceFormatProperties(
   const VkPhysicalDevice physical_device )
{
   std::cout
      << "*** DisplayPhysicalDeviceFormatProperties ***"
      << std::endl;

   const auto formats = GetVkFormats();

   std::default_random_engine engine(
      (std::random_device())());
   std::uniform_int_distribution< size_t > distribution(
      0, formats.size() - 1);

   for (size_t i = 0; i < 5; ++i)
   {
      const VkFormat format =
         formats[distribution(engine)];

      VkFormatProperties properties { };
      vkGetPhysicalDeviceFormatProperties(
         physical_device,
         format,
         &properties);

      std::cout
         << "Format " << format
         << std::endl;

      for (size_t j = 0; j < 3; ++j)
      {
         const char * const feature_string =
            j == 0 ?
            "Linear Tiling Feature" :
            j == 1 ?
            "Optimal Tiling Feature" :
            "Buffer Feature";
         const VkFormatFeatureFlags feature_flags =
            j == 0 ?
            properties.linearTilingFeatures :
            j == 1 ?
            properties.optimalTilingFeatures :
            properties.bufferFeatures;

         for (size_t k = 0; k < 32; ++k)
         {
            switch (feature_flags & 1 << k)
            {
            case VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT:
               std::cout
                  << feature_string << " Sampled Image"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT:
               std::cout
                  << feature_string << " Storage Image"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT:
               std::cout
                  << feature_string << " Storage Image Atomic"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT:
               std::cout
                  << feature_string << " Uniform Texel Buffer"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT:
               std::cout
                  << feature_string << " Storage Texel Buffer"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT:
               std::cout
                  << feature_string << " Storage Texel Buffer Atomic"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT:
               std::cout
                  << feature_string << " Vertex Buffer"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT:
               std::cout
                  << feature_string << " Color Attachment"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT:
               std::cout
                  << feature_string << " Color Attachment Blend"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT:
               std::cout
                  << feature_string << " Depth Stencil Attachment"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_BLIT_SRC_BIT:
               std::cout
                  << feature_string << " Blit Source"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_BLIT_DST_BIT:
               std::cout
                  << feature_string << " Blit Destination"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT:
               std::cout
                  << feature_string << " Sampled Image Filter Linear"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_TRANSFER_SRC_BIT:
               std::cout
                  << feature_string << " Transfer Source"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_TRANSFER_DST_BIT:
               std::cout
                  << feature_string << " Transfer Destination"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT:
               std::cout
                  << feature_string << " Midpoint Chroma Samples"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT:
               std::cout
                  << feature_string << " Sampled Image YCBCR Conversion Linear Filter"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT:
               std::cout
                  << feature_string << " Sampled Image YCBCR Conversion Separate Reconstruction Filter"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT:
               std::cout
                  << feature_string << " Sampled Image YCBCR Conversion Chroma Reconstruction Explicit"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT:
               std::cout
                  << feature_string << " Sampled Image YCBCR Conversion Chroma Reconstruction Explicit Forceable"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_DISJOINT_BIT:
               std::cout
                  << feature_string << " Disjoint"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT:
               std::cout
                  << feature_string << " Cosited Chroma Samples"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG:
               std::cout
                  << feature_string << " Sampled Image Filter Cubic"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT:
               std::cout
                  << feature_string << " Sampled Image Filter Min Max"
                  << std::endl;
               break;
            case VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT:
               std::cout
                  << feature_string << " Fragment Density Map"
                  << std::endl;
               break;
            case 0: break;
            default:
               std::cout
                  << feature_string << " Unknown Format"
                  << std::endl;
               break;
            }
         }

         std::cout << std::endl;
      }
   }
}

void DisplayPhysicalDeviceImageFormatProperties(
   const VkPhysicalDevice physical_device )
{
   std::cout
      << "*** DisplayPhysicalDeviceImageFormatProperties ***"
      << std::endl;

   const auto formats = GetVkFormats();

   const std::vector<
      std::pair< VkImageType, const char * > >
      itypes = {
         { VK_IMAGE_TYPE_1D, "Image Type 1D" },
         { VK_IMAGE_TYPE_2D, "Image Type 2D" },
         { VK_IMAGE_TYPE_3D, "Image Type 3D" }
      };

   const std::vector<
      std::pair< VkImageTiling, const char * > >
      itilings = {
         { VK_IMAGE_TILING_OPTIMAL, "Image Tiling Optimal" },
         { VK_IMAGE_TILING_LINEAR, "Image Tiling Linear" },
         { VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT, "Image Tiling DRM Format Modifier EXT" }
      };

   const std::vector<
      std::pair< VkImageUsageFlags, const char * > >
      iusages = {
         { VK_IMAGE_USAGE_TRANSFER_SRC_BIT, "Image Usage Transfer Source" },
         { VK_IMAGE_USAGE_TRANSFER_DST_BIT, "Image usage Transfer Destination" },
         { VK_IMAGE_USAGE_SAMPLED_BIT, "Image usage Sampled" },
         { VK_IMAGE_USAGE_STORAGE_BIT, "Image Usage Storage" },
         { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, "Image usage Color Attachment" },
         { VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, "Image Usage Depth Stencil Attachment" },
         { VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, "Image Usage Transient Attachment" },
         { VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, "Image Usage Input Attachment" },
         { VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV, "Image Usage Shading Rate Image NV EXT" },
         { VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT, "Image Usage Fragment Density Map EXT" }
      };

   const std::vector<
      std::pair< VkImageCreateFlags, const char * > >
      icreates = {
         { VK_IMAGE_CREATE_SPARSE_BINDING_BIT, "Image Create Sparse Binding" },
         { VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT, "Image Create Sparse Residency" },
         { VK_IMAGE_CREATE_SPARSE_ALIASED_BIT, "Image Create Sparse Aliased" },
         { VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT, "Image Create Mutable Format" },
         { VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, "Image Create Cube Compatible" },
         { VK_IMAGE_CREATE_ALIAS_BIT, "Image Create Alias" },
         { VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT, "Image Create Split Instance Bind Regions" },
         { VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT, "Image Create 2D array Compatible" },
         { VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT, "Image Create Block Texel View Compatible" },
         { VK_IMAGE_CREATE_EXTENDED_USAGE_BIT, "Imate Create Extended Usage" },
         { VK_IMAGE_CREATE_PROTECTED_BIT, "Image Create Protected" },
         { VK_IMAGE_CREATE_DISJOINT_BIT, "Image Create Disjoint" },
         { VK_IMAGE_CREATE_CORNER_SAMPLED_BIT_NV, "Image Crate Corner Sampled NV EXT" },
         { VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT, "Image Create Sample Locations Compatible Depth EXT" },
         { VK_IMAGE_CREATE_SUBSAMPLED_BIT_EXT, "Image Create Subsampled EXT" }
      };

   std::default_random_engine engine(
      (std::random_device())());
   const std::uniform_int_distribution< size_t > formats_distribution(
      0, formats.size() - 1);

   for (size_t i = 0; i < 5; ++i)
   {
      const auto format =
         formats[formats_distribution(engine)];

      for (const auto & itype : itypes)
      {
         const std::uniform_int_distribution< size_t > tilings_distribution(
            0, itilings.size() - 1);

         const auto & tiling =
            itilings[tilings_distribution(engine)];

         const std::uniform_int_distribution< size_t > usages_distribution(
            0, iusages.size() - 1);

         const auto usage =
            iusages[usages_distribution(engine)].first |
            iusages[usages_distribution(engine)].first |
            iusages[usages_distribution(engine)].first;

         const std::uniform_int_distribution< size_t > creates_distribution(
            0, icreates.size() - 1);

         const auto create =
            icreates[creates_distribution(engine)].first |
            icreates[creates_distribution(engine)].first |
            icreates[creates_distribution(engine)].first;

         std::cout
            << "Format      : " << format << std::endl
            << "Image Type  : " << itype.second << std::endl
            << "Tiling Type : " << tiling.second << std::endl
            << "Usage Type  : ";

         for (const auto & iusage : iusages)
         {
            if (usage & iusage.first)
               std::cout << iusage.second << ", ";
         }

         std::cout
            << std::endl
            << "Create Type : ";

         for (const auto & icreate : icreates)
         {
            if (create & icreate.first)
               std::cout << icreate.second << ", ";
         }

         std::cout
            << std::endl << std::endl;

         VkImageFormatProperties properties { };

         const auto result =
            vkGetPhysicalDeviceImageFormatProperties(
               physical_device,
               format,
               itype.first,
               tiling.first,
               usage,
               create,
               &properties);

         if (result != VK_SUCCESS)
         {
            std::cerr
               << "Unable to obtain physical device image "
               "format properties!  Device returned error "
               "code " << result << "!"
               << std::endl;
         }
         else
         {
            std::cout
               << "Max Extents (WxHxD) : "
               << properties.maxExtent.width << " x "
               << properties.maxExtent.height << " x "
               << properties.maxExtent.depth
               << std::endl
               << "Max Mip Levels      : "
               << properties.maxMipLevels
               << std::endl
               << "Max Array Levels    : "
               << properties.maxArrayLayers
               << std::endl
               << "Sample Counts       : ";

            for (size_t i = 1;
                 i <= VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
                 i <<= 1)
            {
               switch (properties.sampleCounts & i)
               {
               case VK_SAMPLE_COUNT_1_BIT:
                  std::cout << "1, "; break;
               case VK_SAMPLE_COUNT_2_BIT:
                  std::cout << "2, "; break;
               case VK_SAMPLE_COUNT_4_BIT:
                  std::cout << "4, "; break;
               case VK_SAMPLE_COUNT_8_BIT:
                  std::cout << "8, "; break;
               case VK_SAMPLE_COUNT_16_BIT:
                  std::cout << "16, "; break;
               case VK_SAMPLE_COUNT_32_BIT:
                  std::cout << "32, "; break;
               case VK_SAMPLE_COUNT_64_BIT:
                  std::cout << "64, "; break;
               case 0: break;
               default:
                  std::cout
                     << "Unknown Sample "
                     << std::hex << "0x" << i << std::dec
                     << ", ";
                  break;
               }
            }

            std::cout
               << std::endl
               << "Max Resource Size : "
               << properties.maxResourceSize / 1048576.0
               << " MiB"
               << std::endl;
         }

         std::cout << std::endl;
      }
   }
}

void DestroyImageHandle(
   VkImage * const image )
{
   size_t * const context =
      reinterpret_cast< size_t * >(
         image) - 1;

   if (image && *image)
   {
      VkDevice * const device =
         reinterpret_cast< VkDevice * >(
            context);

      vkDestroyImage(
         *device,
         *image,
         &CreateAllocator());
   }

   delete [] context;
}

using ImageHandle =
   std::unique_ptr<
      VkImage,
      decltype(&DestroyImageHandle) >;

ImageHandle CreateImage(
   const LogicalDeviceHandle & device,
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
   ImageHandle image {
      nullptr, &DestroyImageHandle };

   if (device && *device)
   {
      size_t * const context =
         new (std::nothrow) size_t[2] { };

      if (context)
      {
         *context =
            reinterpret_cast< size_t >(
               *device);

         image.reset(
            reinterpret_cast< VkImage * >(
               context + 1));

         std::cout
            << "Creating image of size "
            << image_extents.width << " x "
            << image_extents.height << " x "
            << image_extents.depth
            << std::endl;

         const VkImageCreateInfo info {
            VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            nullptr,
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
            0, nullptr,
            image_layout
         };

         const auto result =
            vkCreateImage(
               *device,
               &info,
               &CreateAllocator(),
               image.get());

         if (result == VK_SUCCESS)
         {
            std::cout
               << "Image Created Successfully!"
               << std::endl;
         }
         else
         {
            std::cout
               << "Image Created Unsuccessfully!"
               << std::endl;
         }
      }
   }

   return image;
}

void DestroyDeviceMemory(
   VkDeviceMemory * const memory )
{
   size_t * const context =
      reinterpret_cast< size_t * >(
         memory) - 1;

   if (memory && *memory)
   {
      VkDevice * const device =
         reinterpret_cast< VkDevice * >(
            context);

      vkFreeMemory(
         *device,
         *memory,
         &CreateAllocator());
   }

   delete [] context;
}

using DeviceMemoryHandle =
   std::unique_ptr<
      VkDeviceMemory,
      decltype(&DestroyDeviceMemory) >;

DeviceMemoryHandle AllocateMemory(
   const LogicalDeviceHandle & device,
   const VkDeviceSize size,
   const uint32_t type_index )
{
   DeviceMemoryHandle memory {
      nullptr, &DestroyDeviceMemory };

   if (device && *device)
   {
      size_t * const context =
         new (std::nothrow) size_t[2] { };

      if (context)
      {
         *context =
            reinterpret_cast< size_t >(
               *device);

         memory.reset(
            reinterpret_cast< VkDeviceMemory * >(
               context + 1));

         const VkMemoryAllocateInfo info {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            nullptr,
            size,
            type_index
         };

         const auto result =
            vkAllocateMemory(
               *device,
               &info,
               &CreateAllocator(),
               memory.get());

         if (result == VK_SUCCESS)
         {
            std::cout
               << "Memory Allocated Successfully!"
               << std::endl;
         }
         else
         {
            std::cout
               << "Memory Allocated Unsuccessfully!"
               << std::endl;
         }
      }
   }

   return memory;
}

void ReleaseMappedMemory(
   void * const memory )
{
   size_t * const context =
      reinterpret_cast< size_t * >(
         memory) - 2;

   if (memory &&
       *reinterpret_cast< size_t * >(memory))
   {
      VkDevice * const device =
         reinterpret_cast< VkDevice * >(
            context);
      VkDeviceMemory * const device_memory =
         reinterpret_cast< VkDeviceMemory * >(
            context + 1);

      vkUnmapMemory(
         *device,
         *device_memory);
   }

   delete [] context;
}

using MappedMemoryHandle =
   std::unique_ptr<
      void *,
      decltype(&ReleaseMappedMemory) >;

MappedMemoryHandle MapMemory(
   const LogicalDeviceHandle & device,
   const DeviceMemoryHandle & device_memory,
   const VkDeviceSize offset,
   const VkDeviceSize size,
   const VkMemoryMapFlags flags )
{
   MappedMemoryHandle mapped_memory {
      nullptr, &ReleaseMappedMemory };

   if (device && *device &&
       device_memory && *device_memory)
   {
      size_t * const context =
         new (std::nothrow) size_t[3] { };

      if (context)
      {
         *context =
            reinterpret_cast< size_t >(
               *device);

         *(context + 1) =
            reinterpret_cast< size_t >(
               *device_memory);

         mapped_memory.reset(
            reinterpret_cast< void ** >(
               context + 2));

         const auto result =
            vkMapMemory(
               *device,
               *device_memory,
               offset,
               size,
               flags,
               mapped_memory.get());

         if (result == VK_SUCCESS)
         {
            std::cout
               << "Memory Mapped Successfully!"
               << std::endl;
         }
         else
         {
            std::cout
               << "Memory Mapped Unsuccessfully!"
               << std::endl;
         }
      }
   }

   return mapped_memory;
}

void DestroyBufferViewHandle(
   VkBufferView * const view )
{
   size_t * const context =
      reinterpret_cast< size_t * >(
         view) - 1;

   if (view && *view)
   {
      VkDevice * const device =
         reinterpret_cast< VkDevice * >(
            context);

      vkDestroyBufferView(
         *device,
         *view,
         &CreateAllocator());
   }

   delete [] context;
}

using BufferViewHandle =
   std::unique_ptr<
      VkBufferView,
      decltype(&DestroyBufferViewHandle) >;

BufferViewHandle CreateBufferView(
   const LogicalDeviceHandle & device,
   const BufferHandle & buffer,
   const VkBufferViewCreateFlags create_flags,
   const VkFormat format,
   const VkDeviceSize offset,
   const VkDeviceSize range )
{
   BufferViewHandle view {
      nullptr, &DestroyBufferViewHandle };

   if (device && *device &&
       buffer && *buffer)
   {
      size_t * const context =
         new (std::nothrow) size_t[2] { };

      if (context)
      {
         *context =
            reinterpret_cast< size_t >(
               *device);

         view.reset(
            reinterpret_cast< VkBufferView * >(
               context + 1));

         const VkBufferViewCreateInfo info {
            VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
            nullptr,
            create_flags,
            *buffer,
            format,
            offset,
            range
         };

         const auto result =
            vkCreateBufferView(
               *device,
               &info,
               &CreateAllocator(),
               view.get());

         if (result == VK_SUCCESS)
         {
            std::cout
               << "Buffer View Created Successfully!"
               << std::endl;
         }
         else
         {
            std::cout
               << "Buffer View Created Unsuccessfully!"
               << std::endl;
         }
      }
   }

   return view;
}

void DestroyImageViewHandle(
   VkImageView * const view )
{
   size_t * const context =
      reinterpret_cast< size_t * >(
         view) - 1;

   if (view && *view)
   {
      VkDevice * const device =
         reinterpret_cast< VkDevice * >(
            context);

      vkDestroyImageView(
         *device,
         *view,
         &CreateAllocator());
   }

   delete [] context;
}

using ImageViewHandle =
   std::unique_ptr<
      VkImageView,
      decltype(&DestroyImageViewHandle) >;

ImageViewHandle CreateImageView(
   const LogicalDeviceHandle & device,
   const ImageHandle & image,
   const VkImageViewCreateFlags create_flags,
   const VkImageViewType view_type,
   const VkFormat format,
   const VkComponentMapping & component_mapping,
   const VkImageSubresourceRange & subresource_range )
{
   ImageViewHandle view {
      nullptr, &DestroyImageViewHandle };

   if (device && *device &&
       image && *image)
   {
      size_t * const context =
         new (std::nothrow) size_t[2] { };

      if (context)
      {
         *context =
            reinterpret_cast< size_t >(
               *device);

         view.reset(
            reinterpret_cast< VkImageView * >(
               context + 1));

         const VkImageViewCreateInfo info {
            VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            nullptr,
            create_flags,
            *image,
            view_type,
            format,
            component_mapping,
            subresource_range
         };

         const auto result =
            vkCreateImageView(
               *device,
               &info,
               &CreateAllocator(),
               view.get());

         if (result == VK_SUCCESS)
         {
            std::cout
               << "Image View Created Successfully!"
               << std::endl;
         }
         else
         {
            std::cout
               << "Image View Created Unsuccessfully!"
               << std::endl;
         }
      }
   }

   return view;
}

int32_t main(
   const int32_t argc,
   const char* const argv[] )
{
   const auto instance =
      CreateInstance();

   if (!instance)
   {
      return -1;
   }

   std::cout
      << "Instance Created 0x"
      << std::hex
      << *instance
      << std::dec
      << std::endl;

   const auto gpu_devices =
      GetGPUDevices(instance);

   if (gpu_devices.empty())
   {
      return -2;
   }

   const auto logical_device =
      CreateLogicalDevice(
         gpu_devices.begin()->second.front());

   if (!logical_device)
   {
      return -3;
   }

   std::cout
      << "Logical Device Created 0x"
      << std::hex
      << *logical_device
      << std::dec
      << std::endl;

   const auto buffer1 =
      CreateBuffer(
         logical_device,
         1024 * 1024,
         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
         VK_SHARING_MODE_EXCLUSIVE);

   const auto buffer2 =
      CreateBuffer(
         logical_device,
         1024 * 1024,
         VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
         VK_SHARING_MODE_EXCLUSIVE);

   DisplayPhysicalDeviceFormatProperties(
      gpu_devices.begin()->second.front());

   DisplayPhysicalDeviceImageFormatProperties(
      gpu_devices.begin()->second.front());

   const auto image1 =
      CreateImage(
         logical_device,
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

   const auto image2 =
      CreateImage(
         logical_device,
         0,
         VK_IMAGE_TYPE_2D,
         VK_FORMAT_R8G8B8A8_SNORM,
         { 2048, 2048, 1 },
         1,
         1,
         VK_SAMPLE_COUNT_1_BIT,
         VK_IMAGE_TILING_OPTIMAL,
         VK_IMAGE_USAGE_SAMPLED_BIT,
         VK_SHARING_MODE_EXCLUSIVE,
         VK_IMAGE_LAYOUT_UNDEFINED);

   const auto buffer3 =
      CreateBuffer(
         logical_device,
         1024 * 1024,
         VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
         VK_SHARING_MODE_EXCLUSIVE);

   // need to do more to get the correct type index
   const auto buffer3_memory =
      AllocateMemory(
         logical_device,
         1024 * 1024,
         0);

   const auto buffer3_mapped_memory =
      MapMemory(
         logical_device,
         buffer3_memory,
         0,
         1024 * 1024,
         0);

   std::memset(
      *buffer3_mapped_memory,
      0x05,
      1024 * 1024);

   auto result =
      vkBindBufferMemory(
         *logical_device,
         *buffer3,
         *buffer3_memory,
         0);

   if (result == VK_SUCCESS)
   {
      std::cout
         << "Buffer Bound Successfully!"
         << std::endl;
   }
   else
   {
      std::cout
         << "Buffer Bound Unsuccessfully!"
         << std::endl;
   }

   const auto buffer3_view =
      CreateBufferView(
         logical_device,
         buffer3,
         VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
         VK_FORMAT_R8_UNORM,
         0,
         1024 * 1024);

   const auto image3 =
      CreateImage(
         logical_device,
         0,
         VK_IMAGE_TYPE_2D,
         VK_FORMAT_R8G8B8A8_SNORM,
         { 2048, 2048, 1 },
         1,
         1,
         VK_SAMPLE_COUNT_1_BIT,
         VK_IMAGE_TILING_OPTIMAL,
         VK_IMAGE_USAGE_SAMPLED_BIT,
         VK_SHARING_MODE_EXCLUSIVE,
         VK_IMAGE_LAYOUT_UNDEFINED);

   // need to do more to get the correct type index
   const auto image3_memory =
      AllocateMemory(
         logical_device,
         2048 * 2048 * sizeof(uint32_t),
         0);

   const auto image3_mapped_memory =
      MapMemory(
         logical_device,
         image3_memory,
         0,
         2048 * 2048 * sizeof(uint32_t),
         0);

   std::memset(
      *image3_mapped_memory,
      0x10,
      2048 * 2048 * sizeof(uint32_t));

   result =
      vkBindImageMemory(
         *logical_device,
         *image3,
         *image3_memory,
         0);

   if (result == VK_SUCCESS)
   {
      std::cout
         << "Image Bound Successfully!"
         << std::endl;
   }
   else
   {
      std::cout
         << "Image Bound Unsuccessfully!"
         << std::endl;
   }

   const auto image3_view =
      CreateImageView(
         logical_device,
         image3,
         0,
         VK_IMAGE_VIEW_TYPE_2D,
         VK_FORMAT_R8G8B8A8_SNORM,
         { },
         { VK_IMAGE_ASPECT_COLOR_BIT });
   
   return 0;
}
