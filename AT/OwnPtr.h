/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Assertion.h"
#include "AT/Defines.h"
#include "AT/Types.h"

namespace AT {

template<typename T>
class OwnPtr {
    AT_MAKE_NONCOPYABLE(OwnPtr);

    template<typename Q>
    friend OwnPtr<Q> adopt_own(Q*);
    template<typename Q>
    friend class OwnPtr;
    template<typename Q>
    friend class NonnullOwnPtr;

public:
    ALWAYS_INLINE OwnPtr()
        : m_instance(nullptr)
    {}

    ALWAYS_INLINE OwnPtr(OwnPtr&& other)
        : m_instance(other.m_instance)
    {
        other.m_instance = nullptr;
    }

    ALWAYS_INLINE OwnPtr& operator=(OwnPtr&& other)
    {
        clear();
        m_instance = other.m_instance;
        other.m_instance = nullptr;
        return *this;
    }

    ALWAYS_INLINE ~OwnPtr() { clear(); }

public:
    NODISCARD ALWAYS_INLINE bool is_valid() const { return (m_instance != nullptr); }
    NODISCARD ALWAYS_INLINE operator bool() const { return is_valid(); }

    NODISCARD ALWAYS_INLINE T& get() const
    {
        AT_ASSERT(is_valid());
        return *m_instance;
    }

    NODISCARD ALWAYS_INLINE T* operator->() const { return &get(); }
    NODISCARD ALWAYS_INLINE T& operator*() const { return get(); }

    NODISCARD ALWAYS_INLINE T* raw() const { return m_instance; }

    ALWAYS_INLINE void clear()
    {
        if (m_instance) {
            delete m_instance;
            m_instance = nullptr;
        }
    }

    template<typename Q>
    NODISCARD ALWAYS_INLINE OwnPtr<Q> as()
    {
        OwnPtr<Q> casted_own_ptr = OwnPtr<Q>(reinterpret_cast<Q*>(m_instance));
        m_instance = nullptr;
        return casted_own_ptr;
    }

private:
    ALWAYS_INLINE explicit OwnPtr(T* pointer_to_adopt)
        : m_instance(pointer_to_adopt)
    {}

private:
    T* m_instance;
};

template<typename T>
NODISCARD ALWAYS_INLINE OwnPtr<T> adopt_own(T* pointer_to_adopt)
{
    return OwnPtr<T>(pointer_to_adopt);
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE OwnPtr<T> make_own(Args&&... args)
{
    T* raw_pointer = new T(forward<Args>(args)...);
    return adopt_own(raw_pointer);
}

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::adopt_own;
using AT::make_own;
using AT::OwnPtr;
#endif // AT_INCLUDE_GLOBALLY