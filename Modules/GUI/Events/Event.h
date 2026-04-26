/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Noncopyable.h>
#include <GUI/Forward.h>
#include <GUI/Object.h>

namespace GUI {

// clang-format off
#define GUI_ENUMERATE_EVENT_TYPES(x) \
    x(LayoutEvent, layout)     \
    x(MouseEvent, mouse)       \
    x(PaintEvent, paint)
// clang-format on

class Event {
    AND_MAKE_NONCOPYABLE(Event);
    AND_MAKE_NONMOVABLE(Event);

public:
    Event() = default;
    virtual ~Event() = default;
    virtual StringView class_name() const = 0;

#define _ENUMERATE_EVENT_TYPE(Type, type)                    \
    virtual bool is_##type##_event() const { return false; } \
    Type const& as_##type##_event() const                    \
    {                                                        \
        ASSERT(is_##type##_event());                         \
        return reinterpret_cast<Type const&>(*this);         \
    }
    GUI_ENUMERATE_EVENT_TYPES(_ENUMERATE_EVENT_TYPE)
#undef _ENUMERATE_EVENT_TYPE
};

#define GUI_EVENT(Type, type, base)                                        \
public:                                                                    \
    using Base = base;                                                     \
    virtual ~Type() override = default;                                    \
    virtual StringView class_name() const override { return VIEW(#Type); } \
    virtual bool is_##type##_event() const override { return true; }

} // namespace GUI
