#
# Copyright (c) 2024 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.
#

set(MOON_GUI_SOURCE_FILES
    Application.cpp
    Application.h
    GUI.h
)

add_library(Moon-GUI SHARED ${MOON_GUI_SOURCE_FILES})
add_dependencies(Moon-GUI AT-Framework Moon-Core)

target_link_libraries(Moon-GUI PUBLIC AT-Framework Moon-Core)
target_include_directories(Moon-GUI PUBLIC
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/Moons"
)

target_compile_definitions(Moon-GUI PRIVATE
    "MOON_GUI_EXPORT_API"
)
