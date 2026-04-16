#
# Copyright (c) 2024-2026 Trian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.
#

cmake_minimum_required(VERSION 3.20)
project(moonrise LANGUAGES CXX C)

#==========================================================================================================================================#
#-------------------------------------------------------- BUILD CONFIGURATION TYPES -------------------------------------------------------#
#==========================================================================================================================================#

# NOTE: Don't allow multi-configuration generators to actually have multiple configurations at the same time.
#       This simplifies the build system since there are no differences between generators.
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

#==========================================================================================================================================#
#------------------------------------------------------------ OUTPUT DIRECTORY ------------------------------------------------------------#
#==========================================================================================================================================#

# All generated binaries should be stored directly under the CMake binary directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# NOTE: For multi-configuration generators (such as Visual Studio or XCode) there are separate output directories for each configuration.
#       Since we only have one configuration per project file there is no need for multiple directories.
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR})

#==========================================================================================================================================#
#------------------------------------------------------------ COMPILATION FLAGS -----------------------------------------------------------#
#==========================================================================================================================================#

# Compile using the C++20 standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -pedantic")
        add_compile_definitions("AND_CONFIGURATION_DEBUG=1")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -Wall")
        add_compile_definitions("AND_CONFIGURATION_RELEASE=1")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi /W3 /EHsc")
        add_compile_definitions("AND_CONFIGURATION_DEBUG=1")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /DNDEBUG /W3 /EHsc")
        add_compile_definitions("AND_CONFIGURATION_RELEASE=1")
    endif ()
endif ()

#==========================================================================================================================================#
#----------------------------------------------------------- TARGET DEFINITIONS -----------------------------------------------------------#
#==========================================================================================================================================#

add_subdirectory(AND)
add_subdirectory(Applications)
add_subdirectory(Modules)
