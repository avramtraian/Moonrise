# Copyright (c) 2024 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

set(AT_SOURCE_FILES
)

add_library(AT-Framework SHARED ${AT_SOURCE_FILES})

target_include_directories(AT-Framework PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_definitions(AT-Framework PRIVATE "AT_EXPORT_API=1")