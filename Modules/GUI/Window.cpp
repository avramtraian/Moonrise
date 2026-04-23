/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Events/LayoutEvent.h>
#include <GUI/Events/MouseEvent.h>
#include <GUI/Events/PaintEvent.h>
#include <GUI/Platform/WindowManager.h>
#include <GUI/Window.h>
#include <Gfx/RenderDriver.h>
#include <Gfx/SoftwareImage.h>

namespace GUI {

void Window::initialize()
{
    m_native_handle = WindowManager::the().create_window();
    m_back_buffer_format = Gfx::ImageFormat::BGRA_8888;
    m_paint_engine = Gfx::RenderDriver::the().create_render_device();
    m_paint_buffer = make_ref<Gfx::PaintBuffer>();
}

void Window::destroy()
{
    m_paint_buffer.release();
    m_paint_engine.release();
    m_back_buffer.release();
    WindowManager::the().destroy_window(m_native_handle);
    m_native_handle = invalid_native_window_handle;
}

void Window::set_main_widget(NonnullRefPtr<Widget> const& widget)
{
    m_main_widget = widget;
}

void Window::show()
{
    WindowManager::the().show_window(m_native_handle);
    Optional<Gfx::IntSize> window_size = WindowManager::the().get_window_size(m_native_handle);
    ASSERT(window_size.has_value());
    on_resize_event(window_size.value());
}

bool Window::should_close() const
{
    return WindowManager::the().window_should_close(m_native_handle);
}

void Window::on_close_requested_event()
{
}

void Window::on_close_event()
{
}

void Window::on_resize_event(Gfx::IntSize new_size)
{
    // Don't propagate the event if the new window size is invalid.
    if (new_size.width() == 0 || new_size.height() == 0)
        return;

    // Don't propagate the event if the current back buffer size already matches the new window size.
    if (m_back_buffer.is_valid() && m_back_buffer->size() == new_size)
        return;

    if (m_main_widget.is_valid()) {
        LayoutEvent layout_event { Gfx::IntRect { Gfx::IntPoint::zero(), new_size } };
        m_main_widget->on_layout_event(layout_event);
    }

    on_back_buffer_resized_event(new_size);
    on_paint_event();
}

void Window::on_mouse_moved_event(Gfx::IntPoint relative_position)
{
    if (!m_main_widget.is_valid())
        return;

    MouseEvent mouse_event;
    mouse_event.set_relative_position(relative_position, {});
    m_main_widget->on_mouse_moved_event(mouse_event);
}

void Window::on_paint_event()
{
    m_paint_buffer->clear();
    m_paint_engine->begin_frame();

    if (m_main_widget.is_valid()) {
        PaintEvent paint_event { m_paint_buffer, m_back_buffer->rect() };
        m_main_widget->on_paint_event(paint_event);
    }

    m_paint_engine->execute_paint_buffer(m_paint_buffer);
    m_paint_engine->end_frame();

    // FIXME: The GUI::Window object should probably not care about the type of render driver being used.
    //        Also, this hard-coded cast is really ugly. However, since GPU rendering is very far in the
    //        future, is makes things really simple and convenient...
    auto software_back_buffer = m_back_buffer.as<Gfx::SoftwareImage>();
    WindowManager::the().present_back_buffer(m_native_handle, software_back_buffer->bitmap());
}

void Window::on_back_buffer_resized_event(Gfx::IntSize new_size)
{
    if (m_back_buffer.is_valid() && m_back_buffer->size() == new_size)
        return;

    m_back_buffer.release();
    m_back_buffer = Gfx::RenderDriver::the().create_image(m_back_buffer_format, new_size);
    m_paint_engine->set_render_target(m_back_buffer);
}

} // namespace GUI
