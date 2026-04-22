/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/RefPtr.h>
#include <GUI/Object.h>
#include <GUI/Platform/NativeHandles.h>
#include <GUI/Widget.h>
#include <Gfx/Image.h>
#include <Gfx/PaintBuffer.h>
#include <Gfx/PaintEngine.h>

namespace GUI {

class Window : public Object {
    GUI_OBJECT(Window, Object);

public:
    void set_main_widget(NonnullRefPtr<Widget> const&);
    void show();

    bool should_close() const;
    RefPtr<Gfx::Image> back_buffer() const { return m_back_buffer; }
    NativeWindowHandle native_handle() const { return m_native_handle; }

    void on_close_requested_event();
    void on_close_event();
    void on_resize_event(Gfx::IntSize);

private:
    virtual void initialize() override;
    virtual void destroy() override;

    void on_paint_event();
    void on_back_buffer_resized_event(Gfx::IntSize);

private:
    NativeWindowHandle m_native_handle { invalid_native_window_handle };
    Gfx::ImageFormat m_back_buffer_format;
    RefPtr<Gfx::Image> m_back_buffer;
    RefPtr<Gfx::PaintEngine> m_paint_engine;
    RefPtr<Gfx::PaintBuffer> m_paint_buffer;

    RefPtr<Widget> m_main_widget;
};

} // namespace GUI
