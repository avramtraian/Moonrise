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
    virtual ~Object() override = default;
    virtual StringView class_name() const = 0;
};

#define GUI_OBJECT(type, base_type)                                        \
public:                                                                    \
    using Base = base_type;                                                \
    type() = default;                                                      \
    virtual ~type() override = default;                                    \
    virtual StringView class_name() const override { return VIEW(#type); } \
                                                                           \
    template<typename... Args>                                             \
    static NonnullRefPtr<type> construct(Args&&... args)                   \
    {                                                                      \
        return make_ref<type>(std::forward<Args>(args)...);                \
    }

} // namespace GUI
