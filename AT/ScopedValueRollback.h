/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Defines.h"
#include "AT/Types.h"

namespace AT {

template<typename T>
class ScopedValueRollback {
    AT_MAKE_NONCOPYABLE(ScopedValueRollback);
    AT_MAKE_NONMOVABLE(ScopedValueRollback);

public:
    ALWAYS_INLINE ScopedValueRollback(T& variable_to_rollback)
        : m_variable_to_rollback(variable_to_rollback)
        , m_initial_value(variable_to_rollback)
    {}

    ALWAYS_INLINE ~ScopedValueRollback() { m_variable_to_rollback = move(m_initial_value); }

private:
    T& m_variable_to_rollback;
    T m_initial_value;
};

} // namespace AT