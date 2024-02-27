/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Defines.h"
#include "AT/Types.h"

#define AT_DEFINE_DISTINCT_NUMERIC(DistinctType, integral_type)          \
    struct DistinctType {                                                \
    public:                                                              \
        ALWAYS_INLINE DistinctType()                                     \
            : m_value(static_cast<integral_type>(0))                     \
        {}                                                               \
                                                                         \
        ALWAYS_INLINE DistinctType(const DistinctType& other)            \
            : m_value(other.m_value)                                     \
        {}                                                               \
                                                                         \
        ALWAYS_INLINE DistinctType(integral_type value)                  \
            : m_value(value)                                             \
        {}                                                               \
                                                                         \
        ALWAYS_INLINE DistinctType& operator=(const DistinctType& other) \
        {                                                                \
            m_value = other.m_value;                                     \
            return *this;                                                \
        }                                                                \
                                                                         \
        ALWAYS_INLINE DistinctType& operator=(integral_type value)       \
        {                                                                \
            m_value = value;                                             \
            return *this;                                                \
        }                                                                \
                                                                         \
    public:                                                              \
        NODISCARD ALWAYS_INLINE integral_type& value()                   \
        {                                                                \
            return m_value;                                              \
        }                                                                \
        NODISCARD ALWAYS_INLINE const integral_type& value() const       \
        {                                                                \
            return m_value;                                              \
        }                                                                \
        NODISCARD ALWAYS_INLINE operator integral_type() const           \
        {                                                                \
            return m_value;                                              \
        }                                                                \
                                                                         \
    private:                                                             \
        integral_type m_value;                                           \
    };