#
# Copyright (c) 2024-2026 Trian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.
#

set(MODULE_GUI_SOURCE_FILES
    Application.cpp
    Application.h
    DockTree.cpp
    DockTree.h
    Forward.h
    Object.cpp
    Object.h
    Panel.cpp
    Panel.h
    Platform/WindowManager.cpp
    Platform/WindowManager.h
    Platform/WindowsWindowManager.cpp
    Platform/WindowsWindowManager.h
    Widget.cpp
    Widget.h
    Window.cpp
    Window.h
)

add_library(Module-GUI STATIC ${MODULE_GUI_SOURCE_FILES})
target_include_directories(Module-GUI PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Module-GUI PUBLIC "${CMAKE_SOURCE_DIR}/Modules")
