/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Log.h>
#include <GUI/Application.h>
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
    }

    virtual void on_paint_event(GUI::PaintEvent const& event) override
    {
        Gfx::Painter painter { event.paint_buffer(), event.region() };

        auto left = event.region();
        left.set_width(left.width() / 2);

        auto right = event.region();
        right.move_x_by(left.width());
        right.set_width(event.region().width() - left.width());

        auto percentage = static_cast<float>(event.region().height()) / static_cast<float>(event.region().width());
        percentage = clamp(percentage, 0.0F, 1.0F);

        auto top = left;
        top.set_height(top.height() * percentage);
        top.move_y_by(left.height() - top.height());

        painter.fill_rect(left, Gfx::Color::from_rgb(0.1F, 0.2F, 0.8F));
        painter.fill_rect(right, Gfx::Color::from_rgb(0.8F, 0.3F, 0.2F));
        painter.fill_rect(top, Gfx::Color::from_rgb(0.3F, 0.9F, 0.2F));
    }

private:
    GUI::Timer m_message_timer;
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
