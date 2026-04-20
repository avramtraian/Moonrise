/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/RefPtr.h>
#include <GUI/Object.h>
#include <GUI/Platform/WindowManager.h>
#include <GUI/Widget.h>
#include <Gfx/Image.h>
#include <Gfx/PaintEngine.h>

namespace GUI {

class Window : public Object {
    GUI_OBJECT(Window, Object);

public:
    void set_main_widget(NonnullRefPtr<Widget> const&);
    void show();

    bool should_close() const;
    RefPtr<Gfx::Image> back_buffer() const { return m_back_buffer; }

private:
    virtual void initialize() override;
    virtual void destroy() override;

    void on_back_buffer_resized(Gfx::IntSize);

private:
    WindowID m_native_id { invalid_window_id };
    Gfx::ImageFormat m_back_buffer_format;
    RefPtr<Gfx::Image> m_back_buffer;
    RefPtr<Gfx::PaintEngine> m_paint_engine;

    RefPtr<Widget> m_main_widget;
};

} // namespace GUI
