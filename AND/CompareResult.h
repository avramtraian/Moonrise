/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

namespace AND {

class CompareResult {
public:
    enum class State : u8 {
        Equal,
        Less,
        Greater,
    };

    NODISCARD ALWAYS_INLINE static constexpr CompareResult equal() { return CompareResult(State::Equal); }
    NODISCARD ALWAYS_INLINE static constexpr CompareResult less() { return CompareResult(State::Less); }
    NODISCARD ALWAYS_INLINE static constexpr CompareResult greater() { return CompareResult(State::Greater); }

public:
    CompareResult() = delete;
    CompareResult(CompareResult const&) = default;
    CompareResult& operator=(CompareResult const&) = default;

    NODISCARD ALWAYS_INLINE constexpr bool is_equal() const { return m_state == State::Equal; }
    NODISCARD ALWAYS_INLINE constexpr bool is_not_equal() const { return m_state != State::Equal; }
    NODISCARD ALWAYS_INLINE constexpr bool is_less() const { return m_state == State::Less; }
    NODISCARD ALWAYS_INLINE constexpr bool is_greater() const { return m_state == State::Greater; }
    NODISCARD ALWAYS_INLINE constexpr bool is_less_or_equal() const { return m_state == State::Equal || m_state == State::Less; }
    NODISCARD ALWAYS_INLINE constexpr bool is_greater_or_equal() const { return m_state == State::Equal || m_state == State::Greater; }

private:
    ALWAYS_INLINE constexpr explicit CompareResult(State state)
        : m_state(state)
    {
    }

    State m_state;
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::CompareResult;
#endif
