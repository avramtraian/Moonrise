/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Assertions.h>
#include <AND/Forward.h>
#include <AND/MathUtilities.h>
#include <AND/Types.h>

namespace GUI {

class Length {
public:
    enum class Unit : u8 {
        Pixels,
        Percentage,
    };

    static Length from_pixels(u32 pixels)
    {
        Length result;
        result.m_unit = Unit::Pixels;
        result.m_value.pixels = pixels;
        return result;
    }

    static Length from_percentage(float percentage)
    {
        Length result;
        result.m_unit = Unit::Percentage;
        result.m_value.percentage = max(percentage, 0.0F);
        return result;
    }

public:
    Length()
        : m_unit(Unit::Pixels)
        , m_value { .pixels = 0 }
    {
    }

    Length(Length const&) = default;
    Length& operator=(Length const&) = default;

    Unit unit() const { return m_unit; }
    bool is_pixels() const { return m_unit == Unit::Pixels; }
    bool is_percentage() const { return m_unit == Unit::Percentage; }

    u32 pixels() const
    {
        ASSERT(m_unit == Unit::Pixels);
        return m_value.pixels;
    }

    float percentage() const
    {
        ASSERT(m_unit == Unit::Percentage);
        return m_value.percentage;
    }

    u32 to_pixels(u32 total_length) const
    {
        switch (m_unit) {
        case Unit::Pixels:
            return m_value.pixels;
        case Unit::Percentage:
            return static_cast<u32>(total_length * m_value.percentage);
        default:
            ASSERT_NOT_REACHED;
        }
    }

    f32 to_percentage(u32 total_length) const
    {
        switch (m_unit) {
        case Unit::Pixels:
            return static_cast<f32>(m_value.pixels) / static_cast<f32>(total_length);
        case Unit::Percentage:
            return m_value.percentage;
        default:
            ASSERT_NOT_REACHED;
        }
    }

public:
    bool operator==(Length const& other) const
    {
        if (m_unit != other.m_unit)
            return false;

        switch (m_unit) {
        case Unit::Pixels:
            return m_value.pixels == other.m_value.pixels;
        case Unit::Percentage:
            return m_value.percentage == other.m_value.percentage;
        default:
            ASSERT_NOT_REACHED;
        }
    }

    bool operator!=(Length const& other) const
    {
        return !(*this == other);
    }

private:
    Unit m_unit;
    union {
        u32 pixels;
        float percentage;
    } m_value;
};

void append_to_builder(StringBuilder&, Optional<StringView> const&, Length const&);

} // namespace GUI
