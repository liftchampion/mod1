cmake_minimum_required(VERSION 3.15)
project(mod1)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED FATAL_ERROR)    # TODO error
#find_package(glfw   REQUIRED FATAL_ERROR)   # TODO error

find_library(OPEN_GL OpenGl)
find_library(APP_KIT AppKit)

include_directories(${Vulkan_INCLUDE_DIR})  # TODO
include_directories(libraries/glfw-3.3.1.bin.MACOS/include)
include_directories(libraries/glm)

ADD_COMPILE_OPTIONS(-Wall -Wextra)

add_executable(mod1 src/main.cpp src/vulkaner.h)

target_link_libraries(mod1 "${CMAKE_CURRENT_SOURCE_DIR}/libraries/glfw-3.3.1.bin.MACOS/lib-macos/libglfw.3.dylib") # TODO
target_link_libraries(mod1 ${Vulkan_LIBRARY}) # TODO


