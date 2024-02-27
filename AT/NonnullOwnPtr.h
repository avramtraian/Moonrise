/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/OwnPtr.h"

namespace AT {

template<typename T>
class NonnullOwnPtr {
    AT_MAKE_NONCOPYABLE(NonnullOwnPtr);

    template<typename Q>
    friend NonnullOwnPtr<Q> adopt_nonnull_own(Q*);

public:
    ALWAYS_INLINE NonnullOwnPtr(NonnullOwnPtr&& other) noexcept
    {
        // NOTE: Usually caused by using the smart pointer after moving it somewhere else.
        AT_ASSERT(other.m_instance);
        m_instance = other.m_instance;
        other.m_instance = nullptr;
    }

    ALWAYS_INLINE NonnullOwnPtr(OwnPtr<T>& own_ptr)
    {
        AT_ASSERT(own_ptr.is_valid());
        m_instance = own_ptr.m_instance;
        own_ptr.m_instance = nullptr;
    }

    ALWAYS_INLINE NonnullOwnPtr& operator=(NonnullOwnPtr&& other) noexcept
    {
        // NOTE: Usually caused by using the smart pointer after moving it somewhere else.
        AT_ASSERT(other.m_instance);

        delete m_instance;
        m_instance = other.m_instance;
        other.m_instance = nullptr;

        return *this;
    }

    ALWAYS_INLINE NonnullOwnPtr& operator=(OwnPtr<T>& own_ptr)
    {
        AT_ASSERT(own_ptr.is_valid());

        delete m_instance;
        m_instance = own_ptr.m_instance;
        own_ptr.m_instance = nullptr;

        return *this;
    }

    ALWAYS_INLINE ~NonnullOwnPtr() { delete m_instance; }

public:
    NODISCARD ALWAYS_INLINE T& get() const { return *m_instance; }
    NODISCARD ALWAYS_INLINE T* operator->() const { return &get(); }
    NODISCARD ALWAYS_INLINE T& operator*() const { return get(); }

    NODISCARD ALWAYS_INLINE T* raw() const { return m_instance; }

    ALWAYS_INLINE operator const OwnPtr<T>&() const
    {
        // NOTE: Both OwnPtr and NonnullOwnPtr have the same fields and memory layout, so casting
        //       one to another would simply be a pointer reinterpretation.
        const auto& this_as_own_ptr = reinterpret_cast<const OwnPtr<T>&>(*this);
        return this_as_own_ptr;
    }

private:
    ALWAYS_INLINE explicit NonnullOwnPtr(T* pointer_to_adopt) { m_instance = pointer_to_adopt; }

private:
    T* m_instance;
};

template<typename T>
NODISCARD ALWAYS_INLINE NonnullOwnPtr<T> adopt_nonnull_own(T* pointer_to_adopt)
{
    AT_ASSERT(pointer_to_adopt != nullptr);
    return NonnullOwnPtr<T>(pointer_to_adopt);
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE NonnullOwnPtr<T> make_nonnull_own(Args&&... args)
{
    T* raw_pointer = new T(forward<Args>(args)...);
    return adopt_nonnull_own(raw_pointer);
}

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::adopt_nonnull_own;
using AT::make_nonnull_own;
using AT::NonnullOwnPtr;
#endif // AT_INCLUDE_GLOBALLY