#
# Copyright (c) 2024 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.
#

set(MOON_CORE_SOURCE_FILES
    Core.h
    Log.cpp
    Log.h
)

add_library(Moon-Core SHARED ${MOON_CORE_SOURCE_FILES})
add_dependencies(Moon-Core AT-Framework)

target_link_libraries(Moon-Core PUBLIC AT-Framework)
target_include_directories(Moon-Core PUBLIC
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/Moons"
)

target_compile_definitions(Moon-Core PRIVATE
    "MOON_CORE_EXPORT_API"
)
