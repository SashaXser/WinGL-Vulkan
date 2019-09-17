cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_PREFIX_PATH
   ${CMAKE_PREFIX_PATH}
   "${CMAKE_CURRENT_SOURCE_DIR}/ext-src/vulkan-1.1.121.0")

set(VULKAN_IDE_FOLDER vulkan)

add_subdirectory(./ext-src)
