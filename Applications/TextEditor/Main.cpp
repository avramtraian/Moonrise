/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Application.h>
#include <GUI/Containers/AlignBox.h>
#include <GUI/Containers/BorderBox.h>
#include <GUI/Containers/HorizontalGroup.h>
#include <GUI/Window.h>

static NonnullRefPtr<GUI::BorderBox> add_child(NonnullRefPtr<GUI::HorizontalGroup> group)
{
    auto child = group->add_widget<GUI::BorderBox>();
    child->set_border(Gfx::IntBorder::uniform(4));
    child->set_color(Gfx::Color::from_rgb(0.1F, 0.1F, 0.1F));
    child->set_background(Gfx::Color::from_rgb(0.15F, 0.15F, 0.15F));
    group->set_widget_size_policy(child, GUI::SizePolicy::Fill);
    return child;
}

int main(int argc, char** argv)
{
    auto app = GUI::Application::construct(argc, argv);
    auto window = app->add_window<GUI::Window>();

    auto group = window->set_main_widget<GUI::HorizontalGroup>();
    group->set_widget_preferred_width(add_child(group), GUI::Length::from_pixels(50));

    auto align = group->add_widget<GUI::AlignBox>();
    group->set_widget_size_policy(align, GUI::SizePolicy::Fill);

    auto child = align->set_widget<GUI::BorderBox>();
    child->set_border(Gfx::IntBorder::uniform(4));
    child->set_color(Gfx::Color::from_rgb(0.1F, 0.1F, 0.1F));
    child->set_background(Gfx::Color::from_rgb(0.15F, 0.15F, 0.15F));
    align->set_alignment(GUI::Alignment::Center);
    align->set_width_policy(GUI::SizePolicy::Fill);
    align->set_preferred_height(GUI::Length::from_pixels(300));

    group->set_widget_preferred_width(add_child(group), GUI::Length::from_pixels(100));

    window->show();
    app->execute();
    return 0;
}
