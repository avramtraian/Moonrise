#
# Copyright (c) 2024-2026 Trian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.
#

set(TEXT_EDITOR_SOURCE_FILES
    Main.cpp
)

add_executable(TextEditor ${TEXT_EDITOR_SOURCE_FILES})
target_include_directories(TextEditor PUBLIC ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(TextEditor AND-Framework)
target_link_libraries(TextEditor PUBLIC AND-Framework)
