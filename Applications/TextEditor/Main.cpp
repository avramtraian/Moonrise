/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Log.h>
#include <GUI/Application.h>
#include <GUI/Events/MouseEvent.h>
#include <GUI/Events/PaintEvent.h>
#include <GUI/Timer.h>
#include <GUI/Window.h>
#include <Gfx/Painter.h>

class TextEditorWidget : public GUI::Widget {
    GUI_OBJECT(TextEditorWidget, GUI::Widget);

public:
    virtual void initialize() override
    {
        m_message_timer.initialize(
            TimeDuration::from_seconds(1),
            [&] {
                dbgln("Hello from the timer callback!");
            });
        m_mouse_position = { 500, 500 };
    }

    virtual void on_paint_event(GUI::PaintEvent const& event) override
    {
        Base::on_paint_event(event);
        Gfx::Painter painter { event.paint_buffer(), m_layout_region };

        auto left = m_layout_region;
        left.set_width(left.width() / 2);

        auto right = m_layout_region;
        right.move_x_by(left.width());
        right.set_width(m_layout_region.width() - left.width());

        auto percentage = static_cast<float>(m_layout_region.height()) / static_cast<float>(m_layout_region.width());
        percentage = clamp(percentage, 0.0F, 1.0F);

        auto top = left;
        top.set_height(static_cast<u32>(top.height() * percentage));
        top.move_y_by(left.height() - top.height());

        painter.fill_rect(left, Gfx::Color::from_rgb(0.1F, 0.2F, 0.8F));
        painter.fill_rect(right, Gfx::Color::from_rgb(0.8F, 0.3F, 0.2F));
        painter.fill_rect(top, Gfx::Color::from_rgb(0.3F, 0.9F, 0.2F));

        Gfx::IntRect cursor;
        cursor.set_size(20, 20);
        cursor.center_in(m_mouse_position);
        painter.fill_rect(cursor, Gfx::Color::from_rgb(0.9F, 0.9F, 0.9F));
    }

    virtual void on_mouse_moved_event(GUI::MouseEvent const& event) override
    {
        Base::on_mouse_moved_event(event);
        m_mouse_position = event.position();
        schedule_paint_event();
    }

private:
    GUI::Timer m_message_timer;
    Gfx::IntPoint m_mouse_position;
};

int main(int argument_count, char** arguments)
{
    auto application = GUI::Application::construct(argument_count, arguments);
    auto window = GUI::Window::construct();
    application->add_window(window);

    auto widget = TextEditorWidget::construct();
    window->set_main_widget(widget);

    window->show();
    return application->execute();
}
