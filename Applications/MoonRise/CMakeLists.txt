#
# Copyright (c) 2024 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.
#

set(MOON_RISE_SOURCE_FILES
    EntryPoint.cpp
)

add_executable(MoonRise ${MOON_RISE_SOURCE_FILES})
add_dependencies(MoonRise AT-Framework Moon-Core Moon-GUI)

target_link_libraries(MoonRise PRIVATE AT-Framework Moon-Core Moon-GUI)
target_include_directories(MoonRise PUBLIC
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/Moons"
    "${CMAKE_SOURCE_DIR}/Applications"
)
