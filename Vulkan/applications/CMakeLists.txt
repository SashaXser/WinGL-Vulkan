cmake_minimum_required(VERSION 3.15.0)

set(VULKAN_APPLICATION_IDE_FOLDER
   "${VULKAN_IDE_FOLDER}/applications")

function(
   configure_user_file
   project_name )
   
   if (CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(path_suffix Bin32)
   elseif (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(path_suffix Bin)
   else ( )
      message(FATAL_ERROR "Platform type not known!")
   endif ( )
      
   find_path(
      validation_layer_path
      VkLayer_core_validation.json
      PATH_SUFFIXES ${path_suffix})

   set(
      VK_LAYER_PATH
      "${validation_layer_path}")
      
   if (CMAKE_GENERATOR_PLATFORM)
      set (VS_PLATFORM ${CMAKE_GENERATOR_PLATFORM})
   else ( )
      if (CMAKE_SIZEOF_VOID_P EQUAL 4)
         set(VS_PLATFORM Win32)
      elseif (CMAKE_SIZEOF_VOID_P EQUAL 8)
         set(VS_PLATFORM x64)
      else ( )
         message(FATAL_ERROR "Platform type not known!")
      endif ( )
   endif ( )

   configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/../project.vcxproj.user.in"
      "${CMAKE_CURRENT_BINARY_DIR}/${project_name}.vcxproj.user")

endfunction( configure_user_file )

add_subdirectory(./vulkan_instance)
add_subdirectory(./vulkan_physical_devices)
add_subdirectory(./vulkan_logical_devices)
add_subdirectory(./vulkan_layers_and_extensions)
add_subdirectory(./vulkan_memory_and_resources)
add_subdirectory(./vulkan_queues_and_commands)
add_subdirectory(./vulkan_moving_data)