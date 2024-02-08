# Copyright (c) 2024 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

# NOTE(traian): The minimum required version is currently pretty arbitrary.
#               Feel free to change this value as you see fit.
cmake_minimum_required(VERSION 3.14)
project(Moonrise CXX C)

# Set the project global C++ configuration.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Bin-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Bin-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Bin-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}")
set(CMAKE_PDB_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Bin-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}")

# Make the CMake predefined targets appear nicer if using the Visual Studio toolchain.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_EXPORT_LIBRARY_PREFIX "")

add_subdirectory(AT)
add_subdirectory(MoonCore)
add_subdirectory(MoonLight)
add_subdirectory(MoonRise)