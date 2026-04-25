#
# Copyright (c) 2024-2026 Trian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.
#

set(MODULE_GFX_SOURCE_FILES
    Bitmap.cpp
    Bitmap.h
    Border.h
    Color.cpp
    Color.h
    Forward.h
    Image.h
    ImageFormat.h
    PaintBuffer.cpp
    PaintBuffer.h
    PaintEngine.h
    Painter.cpp
    Painter.h
    Point.cpp
    Point.h
    Rect.cpp
    Rect.h
    RenderDriver.cpp
    RenderDriver.h
    Size.cpp
    Size.h
    SoftwareImage.cpp
    SoftwareImage.h
    SoftwarePaintEngine.cpp
    SoftwarePaintEngine.h
    SoftwareRenderDriver.cpp
    SoftwareRenderDriver.h
)

add_library(Module-Gfx STATIC ${MODULE_GFX_SOURCE_FILES})
target_include_directories(Module-Gfx PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Module-Gfx PUBLIC "${CMAKE_SOURCE_DIR}/Modules")
