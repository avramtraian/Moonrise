/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/Utf16View.h"

namespace AND {

Utf16View Utf16View::from_code_units(Span<u16 const> code_units)
{
    Utf16View view = Utf16View { code_units.elements(), code_units.count() };
    ASSERT(view.validate());
    return view;
}

Utf16View Utf16View::from_code_units(u16 const* code_units, usize code_unit_count)
{
    return Utf16View::from_code_units(ConstSpan { code_units, code_unit_count });
}

Utf16View Utf16View::from_null_terminated(char16_t const* characters)
{
    usize code_unit_count = 0;
    u16 const* code_units = reinterpret_cast<u16 const*>(characters);
    while (*code_units) {
        ++code_units;
        ++code_unit_count;
    }

    code_units = reinterpret_cast<u16 const*>(characters);
    return Utf16View::from_code_units(code_units, code_unit_count);
}

bool Utf16View::validate() const
{
    u16 const* code_units = m_code_units;
    usize code_unit_count = m_code_unit_count;

    while (code_unit_count > 0) {
        auto decode = utf16_decode(code_units, code_unit_count);
        if (!decode.has_value())
            return false;
        code_units += decode.value().code_unit_count;
        code_unit_count -= decode.value().code_unit_count;
    }

    return true;
}

usize Utf16View::calculate_length() const
{
    usize length = 0;
    for (MAYBE_UNUSED u32 code_point : *this)
        ++length;
    return length;
}

void Utf16View::clear()
{
    m_code_units = nullptr;
    m_code_unit_count = 0;
}

} // namespace AND
