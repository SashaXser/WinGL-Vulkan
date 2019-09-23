#include <vulkan/vulkan.h>

#include <cstdint>
#include <ios>
#include <iostream>
#include <sstream>
#include <vector>

int32_t main(
   const int32_t argc,
   const char* const argv[] )
{
   VkApplicationInfo application_info;
   application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   application_info.pNext = nullptr;
   application_info.pApplicationName = "vulkan-physical-devices";
   application_info.applicationVersion = 1;
   application_info.pEngineName = nullptr;
   application_info.engineVersion = 0;
   application_info.apiVersion = VK_MAKE_VERSION(1, 1, 121);

   VkInstanceCreateInfo create_info;
   create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
   create_info.pNext = nullptr;
   create_info.flags = 0;
   create_info.pApplicationInfo = &application_info;
   create_info.enabledLayerCount = 0;
   create_info.ppEnabledLayerNames = nullptr;
   create_info.enabledExtensionCount = 0;
   create_info.ppEnabledExtensionNames = nullptr;

   VkAllocationCallbacks * allocator = nullptr;

   VkInstance instance = nullptr;

   const VkResult created =
      vkCreateInstance(
         &create_info,
         allocator,
         &instance);

   if (created != VK_SUCCESS)
   {
      std::cerr
         << "Unable to create vulkan instance ("
         << created
         << ")!"
         << std::endl;

      return -1;
   }

   uint32_t physical_count { };

   const VkResult enumerated =
      vkEnumeratePhysicalDevices(
         instance,
         &physical_count,
         nullptr);

   if (enumerated != VK_SUCCESS)
   {
      std::cerr
         << "Unable to enumerate physical devices ("
         << enumerated
         << ")!"
         << std::endl;

      return -2;
   }

   if (!physical_count)
   {
      std::cerr
         << "There are no physical devices installed on the system!"
         << std::endl;

      return -3;
   }

   std::vector< VkPhysicalDevice > physical_devices(
      physical_count, { });

   vkEnumeratePhysicalDevices(
      instance,
      &physical_count,
      physical_devices.data());

   for (const auto physical_device : physical_devices)
   {
      VkPhysicalDeviceProperties physical_device_properties { };

      vkGetPhysicalDeviceProperties(
         physical_device,
         &physical_device_properties);

      std::cout
         << "- - - Physical Device Properties - - -" << std::endl
         << "API Version " << physical_device_properties.apiVersion << std::endl
         << "Driver Version " << physical_device_properties.driverVersion << std::endl
         << "Vender ID " << physical_device_properties.vendorID << std::endl
         << "Device ID " << physical_device_properties.deviceID << std::endl
         << "Device Type ";

      switch (physical_device_properties.deviceType)
      {
      case VK_PHYSICAL_DEVICE_TYPE_OTHER:
         std::cout << "Other" << std::endl; break;
      case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
         std::cout << "Integrated GPU" << std::endl; break;
      case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
         std::cout << "Discrete GPU" << std::endl; break;
      case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
         std::cout << "Virtual GPU" << std::endl; break;
      case VK_PHYSICAL_DEVICE_TYPE_CPU:
         std::cout << "CPU" << std::endl; break;
      default:
         std::cout
            << "Unknown ("
            << physical_device_properties.deviceType
            << ")"
            << std::endl;
         
         break;
      }

      std::cout
         << "Device Name " << physical_device_properties.deviceName << std::endl
         << "Pipeline Cache UUID ";

      for (const auto id : physical_device_properties.pipelineCacheUUID)
      {
         std::cout
            << std::hex
            << static_cast< uint32_t >(id)
            << " ";
      }

      const auto ReportSampleCountFlags =
         [ ] ( VkSampleCountFlags flags )
         {
            std::stringstream s;

            s
               << std::hex
               << "0x" << flags
               << " ( ";

            uint32_t bits { 1 };

            while (flags)
            {
               if (flags & 0x1)
                  s << bits << " bit ";

               flags >>= 1; bits <<= 1;
            }

            s
               << " )";

            return s.str();
         };

      std::cout
         << std::dec << std::endl << std::endl << std::endl
         << "Limits.Max Image Dim 1D "
         << physical_device_properties.limits.maxImageDimension1D << std::endl
         << "Limits.Max Image Dim 2D "
         << physical_device_properties.limits.maxImageDimension2D << std::endl
         << "Limits.Max Image Dim 3D "
         << physical_device_properties.limits.maxImageDimension3D << std::endl
         << "Limits.Max Image Dim Cube "
         << physical_device_properties.limits.maxImageDimensionCube << std::endl
         << "Limits.Max Image Array Layers "
         << physical_device_properties.limits.maxImageArrayLayers << std::endl
         << "Limits.Max Texel Buffer Elements "
         << physical_device_properties.limits.maxTexelBufferElements << std::endl
         << "Limits.Max Uniform Buffer Range "
         << physical_device_properties.limits.maxUniformBufferRange << std::endl
         << "Limits.Max Image Array Layers "
         << physical_device_properties.limits.maxImageArrayLayers << std::endl
         << "Limits.Max Storage Buffer Range "
         << physical_device_properties.limits.maxStorageBufferRange << std::endl
         << "Limits.Max Push Constant Size "
         << physical_device_properties.limits.maxPushConstantsSize << std::endl
         << "Limits.Max Memory Allocation Count "
         << physical_device_properties.limits.maxMemoryAllocationCount << std::endl
         << "Limits.Max Sampler Allocation Count "
         << physical_device_properties.limits.maxSamplerAllocationCount << std::endl
         << "Limits.Buffer Image Granularity "
         << physical_device_properties.limits.bufferImageGranularity << std::endl
         << "Limits.Sparse Address Space Size "
         << physical_device_properties.limits.sparseAddressSpaceSize << std::endl
         << "Limits.Max Bound Descriptor Sets "
         << physical_device_properties.limits.maxBoundDescriptorSets << std::endl
         << "Limits.Max Per Stage Descriptor Samplers "
         << physical_device_properties.limits.maxPerStageDescriptorSamplers << std::endl
         << "Limits.Max Per Stage Descriptor Uniform Buffers "
         << physical_device_properties.limits.maxPerStageDescriptorUniformBuffers << std::endl
         << "Limits.Max Per Stage Descriptor Storage Buffers "
         << physical_device_properties.limits.maxPerStageDescriptorStorageBuffers << std::endl
         << "Limits.Max Per Stage Descriptor Sampler Images "
         << physical_device_properties.limits.maxPerStageDescriptorSampledImages << std::endl
         << "Limits.Max Per Stage Descriptor Storage Images "
         << physical_device_properties.limits.maxPerStageDescriptorStorageImages << std::endl
         << "Limits.Max Per Stage Descriptor Input Attachments "
         << physical_device_properties.limits.maxPerStageDescriptorInputAttachments << std::endl
         << "Limits.Max Per Stage Resources "
         << physical_device_properties.limits.maxPerStageResources << std::endl
         << "Limits.Max Descriptor Set Samplers "
         << physical_device_properties.limits.maxDescriptorSetSamplers << std::endl
         << "Limits.Max Descriptor Set Uniform Buffers "
         << physical_device_properties.limits.maxDescriptorSetUniformBuffers << std::endl
         << "Limits.Max Descriptor Set Uniform Buffers Dynamic "
         << physical_device_properties.limits.maxDescriptorSetUniformBuffersDynamic << std::endl
         << "Limits.Max Descriptor Set Storage Buffers "
         << physical_device_properties.limits.maxDescriptorSetStorageBuffers << std::endl
         << "Limits.Max Descriptor Set Storage Buffers Dynamic "
         << physical_device_properties.limits.maxDescriptorSetStorageBuffersDynamic << std::endl
         << "Limits.Max Descriptor Set Sampled Images "
         << physical_device_properties.limits.maxDescriptorSetSampledImages << std::endl
         << "Limits.Max Descriptor Set Storage Images "
         << physical_device_properties.limits.maxDescriptorSetStorageImages << std::endl
         << "Limits.Max Descriptor Set Input Attachments "
         << physical_device_properties.limits.maxDescriptorSetInputAttachments << std::endl
         << "Limits.Max Vertex Input Attributes "
         << physical_device_properties.limits.maxVertexInputAttributes << std::endl
         << "Limits.Max Vertex Input Bindings "
         << physical_device_properties.limits.maxVertexInputBindings << std::endl
         << "Limits.Max Vertex Input Attribute Offset "
         << physical_device_properties.limits.maxVertexInputAttributeOffset << std::endl
         << "Limits.Max Vertex Input Binding Stride "
         << physical_device_properties.limits.maxVertexInputBindingStride << std::endl
         << "Limits.Max Vertex Output Components "
         << physical_device_properties.limits.maxVertexOutputComponents << std::endl
         << "Limits.Max Tessellation Generation Level "
         << physical_device_properties.limits.maxTessellationGenerationLevel << std::endl
         << "Limits.Max Tessellation Patch Size "
         << physical_device_properties.limits.maxTessellationPatchSize << std::endl
         << "Limits.Max Tessellation Control Per Vertex Input Components "
         << physical_device_properties.limits.maxTessellationControlPerVertexInputComponents << std::endl
         << "Limits.Max Tessellation Control Per Vertex Output Components "
         << physical_device_properties.limits.maxTessellationControlPerVertexOutputComponents << std::endl
         << "Limits.Max Tessellation Control Per Patch Output Components "
         << physical_device_properties.limits.maxTessellationControlPerPatchOutputComponents << std::endl
         << "Limits.Max Tessellation Control Total Output Components "
         << physical_device_properties.limits.maxTessellationControlTotalOutputComponents << std::endl
         << "Limits.Max Tessellation Evaluation Input Components "
         << physical_device_properties.limits.maxTessellationEvaluationInputComponents << std::endl
         << "Limits.Max Tessellation Evaluation Output Components "
         << physical_device_properties.limits.maxTessellationEvaluationOutputComponents << std::endl
         << "Limits.Max Geometry Shader Invocations "
         << physical_device_properties.limits.maxGeometryShaderInvocations << std::endl
         << "Limits.Max Geometry Input Components "
         << physical_device_properties.limits.maxGeometryInputComponents << std::endl
         << "Limits.Max Geometry Output Components "
         << physical_device_properties.limits.maxGeometryOutputComponents << std::endl
         << "Limits.Max Geometry Output Vertices "
         << physical_device_properties.limits.maxGeometryOutputVertices << std::endl
         << "Limits.Max Geometry Total Output Components "
         << physical_device_properties.limits.maxGeometryTotalOutputComponents << std::endl
         << "Limits.Max Fragment Input Components "
         << physical_device_properties.limits.maxFragmentInputComponents << std::endl
         << "Limits.Max Fragment Output Attachments "
         << physical_device_properties.limits.maxFragmentOutputAttachments << std::endl
         << "Limits.Max Fragment Dual Src Attachments "
         << physical_device_properties.limits.maxFragmentDualSrcAttachments << std::endl
         << "Limits.Max Fragment Combined Output Resources "
         << physical_device_properties.limits.maxFragmentCombinedOutputResources << std::endl
         << "Limits.Max Compute Shared Memory Size "
         << physical_device_properties.limits.maxComputeSharedMemorySize << std::endl
         << "Limits.Max Compute Work Group Count "
         << physical_device_properties.limits.maxComputeWorkGroupCount[0] << ", "
         << physical_device_properties.limits.maxComputeWorkGroupCount[1] << ", "
         << physical_device_properties.limits.maxComputeWorkGroupCount[2] << std::endl
         << "Limits.Max Compute Work Group Invocations "
         << physical_device_properties.limits.maxComputeWorkGroupInvocations << std::endl
         << "Limits.Max Compute Work Group Size "
         << physical_device_properties.limits.maxComputeWorkGroupSize[0] << ", "
         << physical_device_properties.limits.maxComputeWorkGroupSize[1] << ", "
         << physical_device_properties.limits.maxComputeWorkGroupSize[2] << std::endl
         << "Limits.Sub Pixel Precision Bits "
         << physical_device_properties.limits.subPixelPrecisionBits << std::endl
         << "Limits.Sub Texel Precision Bits "
         << physical_device_properties.limits.subTexelPrecisionBits << std::endl
         << "Limits.Mipmap Precision Bits "
         << physical_device_properties.limits.mipmapPrecisionBits << std::endl
         << "Limits.Max Draw Indexed Index Value "
         << physical_device_properties.limits.maxDrawIndexedIndexValue << std::endl
         << "Limits.Max Draw Indirect Count "
         << physical_device_properties.limits.maxDrawIndirectCount << std::endl
         << "Limits.Max Sampler LOD Bias "
         << physical_device_properties.limits.maxSamplerLodBias << std::endl
         << "Limits.Max Sampler Anisotropy "
         << physical_device_properties.limits.maxSamplerAnisotropy << std::endl
         << "Limits.Max Viewports "
         << physical_device_properties.limits.maxViewports << std::endl
         << "Limits.Max Viewport Dimensions "
         << physical_device_properties.limits.maxViewportDimensions[0] << ", "
         << physical_device_properties.limits.maxViewportDimensions[1] << std::endl
         << "Limits.Max Viewport Bounds Range "
         << physical_device_properties.limits.viewportBoundsRange[0] << ", "
         << physical_device_properties.limits.viewportBoundsRange[1] << std::endl
         << "Limits.Max Viewport Sub Pixel Bits "
         << physical_device_properties.limits.viewportSubPixelBits << std::endl
         << "Limits.Min Memory Map Alignment "
         << physical_device_properties.limits.minMemoryMapAlignment << std::endl
         << "Limits.Min Texel buffer Offset Alignment "
         << physical_device_properties.limits.minTexelBufferOffsetAlignment << std::endl
         << "Limits.Min Uniform Buffer Offset Alignment "
         << physical_device_properties.limits.minUniformBufferOffsetAlignment << std::endl
         << "Limits.Min Storage Buffer Offset Alignment "
         << physical_device_properties.limits.minStorageBufferOffsetAlignment << std::endl
         << "Limits.Min Texel Offset "
         << physical_device_properties.limits.minTexelOffset << std::endl
         << "Limits.Max Texel Offset "
         << physical_device_properties.limits.maxTexelOffset << std::endl
         << "Limits.Min Texel Gather Offset "
         << physical_device_properties.limits.minTexelGatherOffset << std::endl
         << "Limits.Max Texel Gather Offset "
         << physical_device_properties.limits.maxTexelGatherOffset << std::endl
         << "Limits.Min Interpolation Offset "
         << physical_device_properties.limits.minInterpolationOffset << std::endl
         << "Limits.Max Interpolation Offset "
         << physical_device_properties.limits.maxInterpolationOffset << std::endl
         << "Limits.Sub Pixel Interpolation Offset Bits "
         << physical_device_properties.limits.subPixelInterpolationOffsetBits << std::endl
         << "Limits.Max Framebuffer Width "
         << physical_device_properties.limits.maxFramebufferWidth << std::endl
         << "Limits.Max Framebuffer Height "
         << physical_device_properties.limits.maxFramebufferHeight << std::endl
         << "Limits.Max Framebuffer Layers "
         << physical_device_properties.limits.maxFramebufferLayers << std::endl
         << "Limits.Framebuffer Color Sample Counts "
         << ReportSampleCountFlags(
            physical_device_properties.limits.framebufferColorSampleCounts) << std::endl
         << "Limits.Framebuffer Depth Sample Counts "
         << ReportSampleCountFlags(
            physical_device_properties.limits.framebufferDepthSampleCounts) << std::endl
         << "Limits.Framebuffer Stencil Sample Counts "
         << ReportSampleCountFlags(
            physical_device_properties.limits.framebufferStencilSampleCounts) << std::endl
         << "Limits.Framebuffer No Attachments Sample Counts "
         << ReportSampleCountFlags(
            physical_device_properties.limits.framebufferNoAttachmentsSampleCounts) << std::endl
         << "Limits.Max Color Attachments "
         << physical_device_properties.limits.maxColorAttachments << std::endl
         << "Limits.Sampled Image Color Sample Counts "
         << ReportSampleCountFlags(
            physical_device_properties.limits.sampledImageColorSampleCounts) << std::endl
         << "Limits.Sampled Image Integer Sample Counts "
         << ReportSampleCountFlags(
            physical_device_properties.limits.sampledImageIntegerSampleCounts) << std::endl
         << "Limits.Sampled Image Depth Sample Counts "
         << ReportSampleCountFlags(
            physical_device_properties.limits.sampledImageDepthSampleCounts) << std::endl
         << "Limits.Sampled Image Stencil Sample Counts "
         << ReportSampleCountFlags(
            physical_device_properties.limits.sampledImageStencilSampleCounts) << std::endl
         << "Limits.Sampled Sample Counts "
         << ReportSampleCountFlags(
            physical_device_properties.limits.storageImageSampleCounts) << std::endl
         << "Limits.Max Sample Mask Words "
         << physical_device_properties.limits.maxSampleMaskWords << std::endl
         << "Limits.Timestamp Compute And Graphics "
         << physical_device_properties.limits.timestampComputeAndGraphics << std::endl
         << "Limits.Timestamp Period "
         << physical_device_properties.limits.timestampPeriod << std::endl
         << "Limits.Max Clip Distance "
         << physical_device_properties.limits.maxClipDistances << std::endl
         << "Limits.Max Cull Distance "
         << physical_device_properties.limits.maxCullDistances << std::endl
         << "Limits.Max Combined Clip And Cull Distance "
         << physical_device_properties.limits.maxCombinedClipAndCullDistances << std::endl
         << "Limits.Discrete Queue Priorities "
         << physical_device_properties.limits.discreteQueuePriorities << std::endl
         << "Limits.Point Size Range "
         << physical_device_properties.limits.pointSizeRange[0] << ", "
         << physical_device_properties.limits.pointSizeRange[1] << std::endl
         << "Limits.Line Width Range "
         << physical_device_properties.limits.lineWidthRange[0] << ", "
         << physical_device_properties.limits.lineWidthRange[1] << std::endl
         << "Limits.Point Size Granularity "
         << physical_device_properties.limits.pointSizeGranularity << std::endl
         << "Limits.Line Width Granularity "
         << physical_device_properties.limits.lineWidthGranularity << std::endl
         << "Limits.Strict Lines "
         << physical_device_properties.limits.strictLines << std::endl
         << "Limits.Standard Sample Locations "
         << physical_device_properties.limits.standardSampleLocations << std::endl
         << "Limits.Optimal Buffer Copy Offset Alignment "
         << physical_device_properties.limits.optimalBufferCopyOffsetAlignment << std::endl
         << "Limits.Optimal Buffer Copy Row Pitch Alignment "
         << physical_device_properties.limits.optimalBufferCopyRowPitchAlignment << std::endl
         << "Limits.Non Coherent Atom Size "
         << physical_device_properties.limits.nonCoherentAtomSize << std::endl

         << std::endl << std::endl
         << "Sparse Properties.Residency Standard 2D Block Shape "
         << physical_device_properties.sparseProperties.residencyStandard2DBlockShape << std::endl
         << "Sparse Properties.Residency Standard 2D Multisample Block Shape "
         << physical_device_properties.sparseProperties.residencyStandard2DMultisampleBlockShape << std::endl
         << "Sparse Properties.Residency Standard 3D Multisample Block Shape "
         << physical_device_properties.sparseProperties.residencyStandard3DBlockShape << std::endl
         << "Sparse Properties.Residency Aligned Mip Size "
         << physical_device_properties.sparseProperties.residencyAlignedMipSize << std::endl
         << "Sparse Properties.Residency Non Resident Strict "
         << physical_device_properties.sparseProperties.residencyNonResidentStrict << std::endl
         << std::endl
         << std::endl;

      VkPhysicalDeviceFeatures physical_device_features { };

      vkGetPhysicalDeviceFeatures(
         physical_device,
         &physical_device_features);

      std::cout
         << "Features.Robust Buffer Access "
         << physical_device_features.robustBufferAccess << std::endl
         << "Features.Full Draw Index uint32 "
         << physical_device_features.fullDrawIndexUint32 << std::endl
         << "Features.Image Cube Array "
         << physical_device_features.imageCubeArray << std::endl
         << "Features.Independent Blend "
         << physical_device_features.independentBlend << std::endl
         << "Features.Geometry Shader "
         << physical_device_features.geometryShader << std::endl
         << "Features.Tessellation Shader "
         << physical_device_features.tessellationShader << std::endl
         << "Features.Sample Rate Shading "
         << physical_device_features.sampleRateShading << std::endl
         << "Features.Dual Src Blend "
         << physical_device_features.dualSrcBlend << std::endl
         << "Features.Logic Op "
         << physical_device_features.logicOp << std::endl
         << "Features.Multi Draw Indirect "
         << physical_device_features.multiDrawIndirect << std::endl
         << "Features.Draw Indirect First Instance "
         << physical_device_features.drawIndirectFirstInstance << std::endl
         << "Features.Depth Clamp "
         << physical_device_features.depthClamp << std::endl
         << "Features.Depth Bias Clamp "
         << physical_device_features.depthBiasClamp << std::endl
         << "Features.Fill Mode Non Solid "
         << physical_device_features.fillModeNonSolid << std::endl
         << "Features.Depth Bounds "
         << physical_device_features.depthBounds << std::endl
         << "Features.Wide Lines "
         << physical_device_features.wideLines << std::endl
         << "Features.Large Points "
         << physical_device_features.largePoints << std::endl
         << "Features.Alpha To One "
         << physical_device_features.alphaToOne << std::endl
         << "Features.Multi Viewport "
         << physical_device_features.multiViewport << std::endl
         << "Features.Sampler Anisotropy "
         << physical_device_features.samplerAnisotropy << std::endl
         << "Features.Texture Compression ETC2 "
         << physical_device_features.textureCompressionETC2 << std::endl
         << "Features.Texture Compression ASTC LDR "
         << physical_device_features.textureCompressionASTC_LDR << std::endl
         << "Features.Texture Compression BC "
         << physical_device_features.textureCompressionBC << std::endl
         << "Features.Occlusion Query Precise "
         << physical_device_features.occlusionQueryPrecise << std::endl
         << "Features.Pipeline Statistics Query "
         << physical_device_features.pipelineStatisticsQuery << std::endl
         << "Features.Vertex Pipeline Stores And Atomics "
         << physical_device_features.vertexPipelineStoresAndAtomics << std::endl
         << "Features.Fragment Stores And Atomics "
         << physical_device_features.fragmentStoresAndAtomics << std::endl
         << "Features.Shader Tessellation And Geometry Point Size "
         << physical_device_features.shaderTessellationAndGeometryPointSize << std::endl
         << "Features.Shader Image Gather Extended "
         << physical_device_features.shaderImageGatherExtended << std::endl
         << "Features.Shader Storage Image Extended Formats "
         << physical_device_features.shaderStorageImageExtendedFormats << std::endl
         << "Features.Shader Storage Image Multisample "
         << physical_device_features.shaderStorageImageMultisample << std::endl
         << "Features.Shader Storage Image Read Without Format "
         << physical_device_features.shaderStorageImageReadWithoutFormat << std::endl
         << "Features.Shader Storage Image Write Without Format "
         << physical_device_features.shaderStorageImageWriteWithoutFormat << std::endl
         << "Features.Shader Uniform Buffer Array Dynamic Indexing "
         << physical_device_features.shaderUniformBufferArrayDynamicIndexing << std::endl
         << "Features.Shader Sampled Image Array Dynamic Indexing "
         << physical_device_features.shaderSampledImageArrayDynamicIndexing << std::endl
         << "Features.Shader Storage Buffer Array Dynamic Indexing "
         << physical_device_features.shaderStorageBufferArrayDynamicIndexing << std::endl
         << "Features.Shader Storage Image Array Dynamic Indexing "
         << physical_device_features.shaderStorageImageArrayDynamicIndexing << std::endl
         << "Features.Shader Clip Distance "
         << physical_device_features.shaderClipDistance << std::endl
         << "Features.Shader Cull Distance "
         << physical_device_features.shaderCullDistance << std::endl
         << "Features.Shader Float 64 "
         << physical_device_features.shaderFloat64 << std::endl
         << "Features.Shader Int 64 "
         << physical_device_features.shaderInt64 << std::endl
         << "Features.Shader Int 16 "
         << physical_device_features.shaderInt16 << std::endl
         << "Features.Shader Resource Residency "
         << physical_device_features.shaderResourceResidency << std::endl
         << "Features.Shader Resource Min LOD "
         << physical_device_features.shaderResourceMinLod << std::endl
         << "Features.Sparse Binding "
         << physical_device_features.sparseBinding << std::endl
         << "Features.Sparse Residency Buffer "
         << physical_device_features.sparseResidencyBuffer << std::endl
         << "Features.Sparse Residency Image 2D "
         << physical_device_features.sparseResidencyImage2D << std::endl
         << "Features.Sparse Residency Image 3D "
         << physical_device_features.sparseResidencyImage3D << std::endl
         << "Features.Sparse Residency Image 2 Samples "
         << physical_device_features.sparseResidency2Samples << std::endl
         << "Features.Sparse Residency Image 4 Samples "
         << physical_device_features.sparseResidency4Samples << std::endl
         << "Features.Sparse Residency Image 8 Samples "
         << physical_device_features.sparseResidency8Samples << std::endl
         << "Features.Sparse Residency Image 16 Samples "
         << physical_device_features.sparseResidency16Samples << std::endl
         << "Features.Sparse Residency Aliased "
         << physical_device_features.sparseResidencyAliased << std::endl
         << "Features.Variable Multisample Rate "
         << physical_device_features.variableMultisampleRate << std::endl
         << "Features.Inherited Queries "
         << physical_device_features.inheritedQueries << std::endl
         << std::endl
         << std::endl;

      VkPhysicalDeviceMemoryProperties
         physical_device_memory_properties { };

      vkGetPhysicalDeviceMemoryProperties(
         physical_device,
         &physical_device_memory_properties);

      std::cout
         << "Memory Properties.Memory Type Count "
         << physical_device_memory_properties.memoryTypeCount << std::endl;

      for (uint32_t i = 0;
           i < physical_device_memory_properties.memoryTypeCount;
           ++i)
      {
         std::cout
            << "Memory Properties.Memory Type [" << i << "].Heap Index "
            << physical_device_memory_properties.memoryTypes[i].heapIndex << std::endl;

         for (uint32_t j = 1;
              j <= VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
              j <<= 1)
         {
            switch (j & physical_device_memory_properties.memoryTypes[i].propertyFlags)
            {
            case 0: break;
            case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Property Flags "
                     "Device Local"
                  << std::endl;
               break;
            case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Property Flags "
                     "Host Visible"
                  << std::endl;
               break;
            case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Property Flags "
                     "Host Coherent"
                  << std::endl;
               break;
            case VK_MEMORY_PROPERTY_HOST_CACHED_BIT:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Property Flags "
                     "Host Cached"
                  << std::endl;
               break;
            case VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Property Flags "
                     "Lazily Allocated"
                  << std::endl;
               break;
            case VK_MEMORY_PROPERTY_PROTECTED_BIT:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Property Flags "
                     "Protected"
                  << std::endl;
               break;
            case VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Property Flags "
                     "Device Coherent AMD"
                  << std::endl;
               break;
            case VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Property Flags "
                     "Device Uncached AMD"
                  << std::endl;
               break;
            default:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Property Flags "
                     "Unknown Flag 0x"
                  << std::hex
                  << (j & physical_device_memory_properties.memoryTypes[j].propertyFlags)
                  << std::dec
                  << std::endl;
               break;
            }
         }
      }

      std::cout
         << "Memory Properties.Memory Heap Count "
         << physical_device_memory_properties.memoryHeapCount << std::endl;

      for (uint32_t i = 0;
           i < physical_device_memory_properties.memoryHeapCount;
           ++i)
      {
         std::cout
            << "Memory Properties.Memory Type [" << i << "].Size "
            << physical_device_memory_properties.memoryHeaps[i].size
            << " B ("
            << physical_device_memory_properties.memoryHeaps[i].size / 1024.0 / 1024.0 / 1024.0
            << " GiB)" <<std::endl;

         for (uint32_t j = 1;
              j <= VK_MEMORY_HEAP_FLAG_BITS_MAX_ENUM;
              j <<= 1)
         {
            switch (j & physical_device_memory_properties.memoryHeaps[i].flags)
            {
            case 0: break;
            case VK_MEMORY_HEAP_DEVICE_LOCAL_BIT:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Heap Flags "
                     "Device Local"
                  << std::endl;
               break;
            case VK_MEMORY_HEAP_MULTI_INSTANCE_BIT:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Heap Flags "
                     "Host Visible"
                  << std::endl;
               break;
            default:
               std::cout
                  << "Memory Properties.Memory Type [" << i << "].Heap Flags "
                     "Unknown Flag 0x"
                  << std::hex
                  << (j & physical_device_memory_properties.memoryHeaps[j].flags)
                  << std::dec
                  << std::endl;
               break;
            }
         }
      }

      std::cout
         << std::endl
         << std::endl;

      uint32_t queue_family_count { };

      vkGetPhysicalDeviceQueueFamilyProperties(
         physical_device,
         &queue_family_count,
         nullptr);

      std::cout
         << "Number Of Queue Family Properties "
         << queue_family_count << std::endl;

      std::vector< VkQueueFamilyProperties >
         physical_device_queue_family_properties(
            queue_family_count, VkQueueFamilyProperties { });

      vkGetPhysicalDeviceQueueFamilyProperties(
         physical_device,
         &queue_family_count,
         physical_device_queue_family_properties.data());

      for (const auto & physical_device_queue_family_property :
           physical_device_queue_family_properties)
      {
         const size_t index =
            &physical_device_queue_family_property -
            &physical_device_queue_family_properties.front();

         for (uint32_t i = 1;
              i <= VK_QUEUE_FLAG_BITS_MAX_ENUM;
              i <<= 1)
         {
            switch (i & physical_device_queue_family_property.queueFlags)
            {
            case 0: break;
            case VK_QUEUE_GRAPHICS_BIT:
               std::cout
                  << "Queue Family Properties[" << index << "].Queue Flag "
                  << "Graphics" << std::endl;
               break;
            case VK_QUEUE_COMPUTE_BIT:
               std::cout
                  << "Queue Family Properties[" << index << "].Queue Flag "
                  << "Compute" << std::endl;
               break;
            case VK_QUEUE_TRANSFER_BIT:
               std::cout
                  << "Queue Family Properties[" << index << "].Queue Flag "
                  << "Transfer" << std::endl;
               break;
            case VK_QUEUE_SPARSE_BINDING_BIT:
               std::cout
                  << "Queue Family Properties[" << index << "].Queue Flag "
                  << "Sparse Binding" << std::endl;
               break;
            case VK_QUEUE_PROTECTED_BIT:
               std::cout
                  << "Queue Family Properties[" << index << "].Queue Flag "
                  << "Protected" << std::endl;
               break;
            default:
               std::cout
                  << "Queue Family Properties[" << index << "].Queue Flag "
                  << "Unknown Flag 0x"
                  << std::hex
                  << (i & physical_device_queue_family_property.queueFlags)
                  << std::dec
                  << std::endl;
            }
         }

         std::cout
            << "Queue Family Properties[" << index << "].Queue Count "
            << physical_device_queue_family_property.queueCount << std::endl
            << "Queue Family Properties[" << index << "].Timestamp Valid Bits "
            << physical_device_queue_family_property.timestampValidBits << std::endl
            << "Queue Family Properties[" << index << "].Min Image Transfer Granularity "
            << physical_device_queue_family_property.minImageTransferGranularity.width
            << ", "
            << physical_device_queue_family_property.minImageTransferGranularity.height
            << ", "
            << physical_device_queue_family_property.minImageTransferGranularity.depth
            << std::endl;
      }
   }

   vkDestroyInstance(
      instance,
      nullptr);

   return 0;
}

