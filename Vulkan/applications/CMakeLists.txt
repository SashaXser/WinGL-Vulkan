cmake_minimum_required(VERSION 3.15.0)

set(VULKAN_APPLICATION_IDE_FOLDER
   "${VULKAN_IDE_FOLDER}/applications")

add_subdirectory(./vulkan_instance)
add_subdirectory(./vulkan_physical_devices)
