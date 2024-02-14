# Copyright (c) 2024 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

set(CORE_SOURCE_FILES
    Core.h
    Log.cpp
    Log.h
)

add_library(MoonCore SHARED ${CORE_SOURCE_FILES})
add_dependencies(MoonCore AT-Framework)

target_link_libraries(MoonCore PUBLIC "AT-Framework")
target_include_directories(MoonCore PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_definitions(MoonCore PRIVATE "CORE_EXPORT_API=1")