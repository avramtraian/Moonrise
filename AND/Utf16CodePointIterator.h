/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Optional.h"
#include "AND/Utf16.h"

namespace AND {

//=================================================================================================
// FORWARD ITERATOR.
//=================================================================================================

class Utf16CodePointIterator {
public:
    Utf16CodePointIterator(u16 const* code_units, usize code_unit_count);

    Utf16CodePointIterator(Utf16CodePointIterator const&) = default;
    Utf16CodePointIterator& operator=(Utf16CodePointIterator const&) = default;

public:
    NODISCARD u32 operator*() const;
    NODISCARD usize code_point_code_unit_count() const;

    Utf16CodePointIterator& operator++();
    Utf16CodePointIterator operator++(int);

    NODISCARD bool operator==(Utf16CodePointIterator const& other) const;
    NODISCARD bool operator!=(Utf16CodePointIterator const& other) const;

public:
    NODISCARD Optional<u32> peek(u32 offset_in_code_points = 1) const;
    NODISCARD bool is_done() const;

    NODISCARD bool code_point_is_valid() const;

private:
    u16 const* m_code_units;
    usize m_code_unit_count;
    Optional<Utf16DecodeResult> m_current_decode_result;
};

//=================================================================================================
// REVERSE ITERATOR.
//=================================================================================================

class Utf16ReverseCodePointIterator {
public:
    Utf16ReverseCodePointIterator(u16 const* code_units, usize code_unit_count);

    Utf16ReverseCodePointIterator(Utf16ReverseCodePointIterator const&) = default;
    Utf16ReverseCodePointIterator& operator=(Utf16ReverseCodePointIterator const&) = default;

public:
    NODISCARD u32 operator*() const;
    NODISCARD usize code_point_code_unit_count() const;

    Utf16ReverseCodePointIterator& operator++();
    Utf16ReverseCodePointIterator operator++(int);

    NODISCARD bool operator==(Utf16ReverseCodePointIterator const& other) const;
    NODISCARD bool operator!=(Utf16ReverseCodePointIterator const& other) const;

public:
    NODISCARD Optional<u32> peek(u32 offset_in_code_points = 1) const;
    NODISCARD bool is_done() const;

    NODISCARD bool code_point_is_valid() const;

private:
    u16 const* m_code_units;
    usize m_code_unit_count;
    Optional<Utf16DecodeResult> m_current_decode_result;
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::Utf16CodePointIterator;
using AND::Utf16ReverseCodePointIterator;
#endif
