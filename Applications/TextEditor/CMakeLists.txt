#
# Copyright (c) 2024-2026 Trian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.
#

set(TEXT_EDITOR_SOURCE_FILES
    Main.cpp
)

add_executable(TextEditor ${TEXT_EDITOR_SOURCE_FILES})
target_include_directories(TextEditor PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(TextEditor PUBLIC ${CMAKE_SOURCE_DIR}/Modules)
target_include_directories(TextEditor PUBLIC ${CMAKE_SOURCE_DIR}/Applications)

add_dependencies(TextEditor AND-Framework Module-GUI)
target_link_libraries(TextEditor PUBLIC AND-Framework Module-GUI)
