/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <GUI/Object.h>

namespace GUI {

class Widget : public Object {
    GUI_OBJECT(Widget, Object);

public:
    virtual Optional<u32> calculate_min_size_x() const { return {}; }
    virtual Optional<u32> calculate_max_size_x() const { return {}; }

    virtual Optional<u32> calculate_min_size_y() const { return {}; }
    virtual Optional<u32> calculate_max_size_y() const { return {}; }
};

} // namespace GUI
