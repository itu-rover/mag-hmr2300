cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

option(TARGET_EMBEDDED "Target is embedded" OFF)

if (TARGET_EMBEDDED)
    set(_HMR2300_EMBEDDED ON)
endif()

if(NOT CMAKE_TOOLCHAIN_FILE AND TARGET_STM32)
    include(cmake/gcc-arm-none-eabi.cmake)
endif()

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(hmr2300-test)

# Include subprojects
add_subdirectory(lib)
