/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Assertions.h"
#include "AND/Types.h"

#include <new>

namespace AND {

template<typename T>
class Optional {
public:
    ALWAYS_INLINE constexpr Optional()
        : m_value_storage {}
        , m_has_value(false)
    {
    }

    ALWAYS_INLINE constexpr Optional(Optional const& other)
        : m_value_storage {}
        , m_has_value(other.m_has_value)
    {
        if (m_has_value)
            new (m_value_storage) T(other.raw_value());
    }

    ALWAYS_INLINE constexpr Optional(Optional&& other) noexcept
        : m_value_storage {}
        , m_has_value(other.m_has_value)
    {
        if (m_has_value)
            new (m_value_storage) T(move(other.raw_value()));
        other.release();
    }

    /*implicit*/ ALWAYS_INLINE constexpr Optional(T value)
        : m_has_value(true)
    {
        new (m_value_storage) T(move(value));
    }

    ALWAYS_INLINE constexpr Optional& operator=(Optional const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        release();
        m_has_value = other.m_has_value;
        if (m_has_value)
            new (m_value_storage) T(other.raw_value());
        return *this;
    }

    ALWAYS_INLINE constexpr Optional& operator=(Optional&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        release();
        m_has_value = other.m_has_value;
        if (m_has_value)
            new (m_value_storage) T(move(other.raw_value()));
        other.release();
        return *this;
    }

    ALWAYS_INLINE constexpr Optional& operator=(T value)
    {
        release();
        m_has_value = true;
        new (m_value_storage) T(move(value));
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE constexpr bool has_value() const { return m_has_value; }

    NODISCARD ALWAYS_INLINE constexpr T& value()
    {
        ASSERTF(has_value(), "Trying to get the value of an empty Optional (in Optional::value())!");
        return raw_value();
    }

    NODISCARD ALWAYS_INLINE constexpr const T& value() const
    {
        ASSERTF(has_value(), "Trying to get the value of an empty Optional (in Optional::value())!");
        return raw_value();
    }

    NODISCARD ALWAYS_INLINE T value_or(T const& fallback_value) const
    {
        if (m_has_value)
            return raw_value();
        return fallback_value;
    }

    NODISCARD ALWAYS_INLINE constexpr T& operator*() { return value(); }
    NODISCARD ALWAYS_INLINE constexpr const T& operator*() const { return value(); }

    ALWAYS_INLINE constexpr void release()
    {
        if (m_has_value) {
            raw_value().~T();
            m_has_value = false;
        }
    }

    NODISCARD ALWAYS_INLINE constexpr bool operator==(Optional const& other) const
    {
        if (!has_value() && !other.has_value())
            return true;
        if (!has_value() || !other.has_value())
            return false;
        return raw_value() == other.raw_value();
    }

    NODISCARD ALWAYS_INLINE constexpr bool operator!=(Optional const& other) const
    {
        // Negate the result of the check equality operator.
        return !(*this == other);
    }

private:
    NODISCARD ALWAYS_INLINE constexpr T& raw_value() { return *reinterpret_cast<T*>(m_value_storage); }
    NODISCARD ALWAYS_INLINE constexpr const T& raw_value() const { return *reinterpret_cast<T const*>(m_value_storage); }

private:
    alignas(T) u8 m_value_storage[sizeof(T)];
    bool m_has_value;
};

template<typename T>
class Optional<T&> {
public:
    ALWAYS_INLINE constexpr Optional()
        : m_value(nullptr)
    {
    }

    ALWAYS_INLINE constexpr Optional(Optional const& other)
        : m_value(other.m_value)
    {
    }

    ALWAYS_INLINE constexpr Optional(Optional&& other) noexcept
        : m_value(other.m_value)
    {
        other.m_value = nullptr;
    }

    /*implicit*/ ALWAYS_INLINE constexpr Optional(T& value)
        : m_value(&value)
    {
    }

    ALWAYS_INLINE constexpr Optional& operator=(Optional const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_value = other.m_value;
        return *this;
    }

    ALWAYS_INLINE constexpr Optional& operator=(Optional&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_value = other.m_value;
        other.m_value = nullptr;
        return *this;
    }

    ALWAYS_INLINE constexpr Optional& operator=(T& value)
    {
        release();
        m_value = &value;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE constexpr bool has_value() const { return (m_value != nullptr); }

    NODISCARD ALWAYS_INLINE constexpr T& value()
    {
        ASSERTF(has_value(), "Trying to get the value of an empty Optional (in Optional::value())!");
        return *m_value;
    }

    NODISCARD ALWAYS_INLINE constexpr const T& value() const
    {
        ASSERTF(has_value(), "Trying to get the value of an empty Optional (in Optional::value())!");
        return *m_value;
    }

    NODISCARD ALWAYS_INLINE T value_or(T const& fallback_value) const
    {
        if (m_value != nullptr)
            return *m_value;
        return fallback_value;
    }

    NODISCARD ALWAYS_INLINE constexpr T& operator*() { return value(); }
    NODISCARD ALWAYS_INLINE constexpr const T& operator*() const { return value(); }

    ALWAYS_INLINE constexpr void release()
    {
        m_value = nullptr;
    }

    NODISCARD ALWAYS_INLINE constexpr bool operator==(Optional const& other) const
    {
        if (!has_value() && !other.has_value())
            return true;
        if (!has_value() || !other.has_value())
            return false;
        return m_value == other.m_value;
    }

    NODISCARD ALWAYS_INLINE constexpr bool operator!=(Optional const& other) const
    {
        // Negate the result of the check equality operator.
        return !(*this == other);
    }

private:
    T* m_value;
};

} // namespace AND
