# Copyright (c) 2024 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

set(RISE_SOURCE_FILES
    GUI/Application/Application.cpp
    GUI/Application/Application.h
    GUI/Application/Window.cpp
    GUI/Application/Window.h
    GUI/Native/NativeWindow.cpp
    GUI/Native/NativeWindow.h
    GUI/Native/Windows/WindowsNativeWindow.cpp
    GUI/Native/Windows/WindowsNativeWindow.h
    Main.cpp
)

add_executable(MoonRise ${RISE_SOURCE_FILES})
add_dependencies(MoonRise AT-Framework MoonCore)

target_link_libraries(MoonRise PUBLIC "AT-Framework" "MoonCore")
target_include_directories(MoonRise PRIVATE "${CMAKE_SOURCE_DIR}")