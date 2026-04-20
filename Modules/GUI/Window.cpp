/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Log.h>
#include <GUI/Application.h>
#include <GUI/Platform/WindowManager.h>
#include <GUI/Window.h>

#include "Gfx/RenderDriver.h"

namespace GUI {

void Window::initialize()
{
    m_native_id = WindowManager::the().create_window();
    m_back_buffer_format = Gfx::ImageFormat::BGRA_8888;
    m_paint_engine = Gfx::RenderDriver::the().create_render_device();
}

void Window::destroy()
{
    m_paint_engine.release();
    m_back_buffer.release();
    WindowManager::the().destroy_window(m_native_id);
    m_native_id = invalid_window_id;
}

void Window::set_main_widget(NonnullRefPtr<Widget> const& widget)
{
    m_main_widget = widget;
}

void Window::show()
{
    WindowManager::the().show_window(m_native_id);
    Optional<Gfx::IntSize> window_size = WindowManager::the().get_window_size(m_native_id);
    ASSERT(window_size.has_value());
    on_back_buffer_resized(window_size.value());
}

bool Window::should_close() const
{
    return WindowManager::the().window_should_close(m_native_id);
}

void Window::on_back_buffer_resized(Gfx::IntSize new_size)
{
    if (m_back_buffer.is_valid() && m_back_buffer->size() == new_size)
        return;

    m_back_buffer.release();
    m_back_buffer = Gfx::RenderDriver::the().create_image(m_back_buffer_format, new_size);
    m_paint_engine->set_render_target(m_back_buffer);
}

} // namespace GUI
