#
# Copyright (c) 2024-2026 Trian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.
#

set(MODULE_GUI_SOURCE_FILES
    Application.cpp
    Application.h
    Containers/AlignBox.cpp
    Containers/AlignBox.h
    Containers/BorderBox.cpp
    Containers/BorderBox.h
    Containers/HorizontalGroup.cpp
    Containers/HorizontalGroup.h
    Cursor.cpp
    Cursor.h
    DockTree.cpp
    DockTree.h
    Events/MouseEvent.cpp
    Events/MouseEvent.h
    Events/LayoutEvent.h
    Events/PaintEvent.h
    Forward.h
    Length.cpp
    Length.h
    Object.cpp
    Object.h
    Panel.cpp
    Panel.h
    Platform/EventLoop.cpp
    Platform/EventLoop.h
    Platform/NativeHandles.h
    Platform/WindowManager.cpp
    Platform/WindowManager.h
    Platform/WindowsEventLoop.cpp
    Platform/WindowsEventLoop.h
    Platform/WindowsWindowManager.cpp
    Platform/WindowsWindowManager.h
    SizePolicy.h
    Timer.cpp
    Timer.h
    Widget.cpp
    Widget.h
    Window.cpp
    Window.h
)

add_library(Module-GUI STATIC ${MODULE_GUI_SOURCE_FILES})
target_include_directories(Module-GUI PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Module-GUI PUBLIC "${CMAKE_SOURCE_DIR}/Modules")

add_dependencies(Module-GUI Module-Gfx)
target_link_libraries(Module-GUI PUBLIC Module-Gfx)
