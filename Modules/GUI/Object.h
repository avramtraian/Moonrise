/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/RefPtr.h>
#include <AND/StringView.h>

namespace GUI {

class Object : public RefCounted {
public:
    Object() = default;
    virtual ~Object() = default;
    virtual StringView class_name() const = 0;

    virtual void initialize() { }
    virtual void destroy() { }
};

#define GUI_ABSTRACT_OBJECT(type, base_type)                               \
public:                                                                    \
    using Base = base_type;                                                \
    type() = default;                                                      \
    virtual StringView class_name() const override { return VIEW(#type); } \
                                                                           \
    virtual ~type() override                                               \
    {                                                                      \
        destroy();                                                         \
    }

#define GUI_OBJECT(type, base_type)                      \
    GUI_ABSTRACT_OBJECT(type, base_type);                \
                                                         \
public:                                                  \
    template<typename... Args>                           \
    static NonnullRefPtr<type> construct(Args&&... args) \
    {                                                    \
        auto* instance = new type();                     \
        ASSERT(instance);                                \
        instance->initialize(forward<Args>(args)...);    \
        return adopt_nonnull(*instance);                 \
    }

} // namespace GUI
