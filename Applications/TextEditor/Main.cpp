/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Log.h>
#include <GUI/Application.h>
#include <GUI/Containers/AlignBox.h>
#include <GUI/Containers/BorderBox.h>
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
        // m_message_timer.initialize(
        //     TimeDuration::from_seconds(1),
        //     [&] {
        //         dbgln("Hello from the timer callback!");
        //     });
        m_mouse_position = { 500, 500 };
    }

    virtual void on_paint_event(GUI::PaintEvent const& event) override
    {
        Base::on_paint_event(event);
        return;
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
    virtual Gfx::IntSize calculate_preferred_size() const override
    {
        return { 400, 300 };
    }

private:
    GUI::Timer m_message_timer;
    Gfx::IntPoint m_mouse_position;
};

int main(int argument_count, char** arguments)
{
    auto application = GUI::Application::construct(argument_count, arguments);
    auto window = application->add_window<GUI::Window>();
    auto outer_border = window->set_main_widget<GUI::BorderBox>();
    auto align_box = outer_border->set_widget<GUI::AlignBox>();
    auto inner_border = align_box->set_widget<GUI::BorderBox>();
    auto widget = inner_border->set_widget<TextEditorWidget>();

    outer_border->set_color(Gfx::Color::from_rgb(0.12F, 0.12F, 0.12F));
    outer_border->set_background(Gfx::Color::from_rgb(0.20F, 0.20F, 0.20F));
    outer_border->set_border(Gfx::IntBorder::uniform(4));

    inner_border->set_color(Gfx::Color::from_rgb(0.12F, 0.12F, 0.12F));
    inner_border->set_background(Gfx::Color::from_rgb(0.20F, 0.20F, 0.20F));
    inner_border->set_border(Gfx::IntBorder::uniform(4));

    align_box->set_preferred_width(GUI::Length::from_pixels(500));
    align_box->set_preferred_height(GUI::Length::from_percentage(0.5F));
    align_box->set_alignment(GUI::Alignment::BottomLeft);
    align_box->set_offset(100, 100);

    window->show();
    return application->execute();
}
