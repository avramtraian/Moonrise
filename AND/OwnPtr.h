/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Assertions.h"
#include "AND/Noncopyable.h"
#include "AND/Types.h"

namespace AND {

enum class OwnIsNonnull {
    No,
    Yes,
};

template<typename T, OwnIsNonnull is_nonnull = OwnIsNonnull::No>
requires(!is_reference<T> && !is_const<T>)
class OwnPtr {
    AND_MAKE_NONCOPYABLE(OwnPtr);

    template<typename FriendT, OwnIsNonnull friend_is_nonnull>
    requires(!is_reference<FriendT> && !is_const<FriendT>)
    friend class OwnPtr;

    template<typename Q>
    requires(!is_reference<Q> && !is_const<Q>)
    friend OwnPtr<Q> adopt_own(Q*);

    template<typename Q>
    requires(!is_reference<Q> && !is_const<Q>)
    friend OwnPtr<Q, OwnIsNonnull::Yes> adopt_nonnull_own(Q&);

public:
    ALWAYS_INLINE constexpr OwnPtr()
    requires(is_nonnull == OwnIsNonnull::No)
        : m_pointer(nullptr)
    {
    }

    ALWAYS_INLINE ~OwnPtr()
    {
    }

    ALWAYS_INLINE OwnPtr(OwnPtr&& other) noexcept
        : m_pointer(other.m_pointer)
    {
        other.m_pointer = nullptr;

        if constexpr (is_nonnull == OwnIsNonnull::Yes) {
            if (m_pointer == nullptr)
                PANIC("Trying to construct a NonnullOwnPtr from another NonnullOwnPtr that was moved or leaked!");
        }
    }

    template<OwnIsNonnull other_is_nonnull>
    requires(is_nonnull != other_is_nonnull)
    /*implicit*/ ALWAYS_INLINE OwnPtr(OwnPtr<T, other_is_nonnull>&& other) noexcept
        : m_pointer(other.m_pointer)
    {
        other.m_pointer = nullptr;

        if constexpr (is_nonnull == OwnIsNonnull::Yes) {
            if (m_pointer == nullptr)
                PANIC("Trying to construct a NonnullOwnPtr from a null OwnPtr!");
        }
    }

    ALWAYS_INLINE OwnPtr& operator=(OwnPtr&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        release_impl();
        m_pointer = other.m_pointer;
        other.m_pointer = nullptr;

        if constexpr (is_nonnull == OwnIsNonnull::Yes) {
            if (m_pointer == nullptr)
                PANIC("Trying to assign a NonnullOwnPtr that was moved or leaked to another NonnullOwnPtr!");
        }

        return *this;
    }

    template<OwnIsNonnull other_is_nonnull>
    requires(is_nonnull != other_is_nonnull)
    ALWAYS_INLINE OwnPtr& operator=(OwnPtr<T, other_is_nonnull>&& other) noexcept
    {
        release_impl();
        m_pointer = other.m_pointer;
        other.m_pointer = nullptr;

        if constexpr (is_nonnull == OwnIsNonnull::Yes) {
            if (m_pointer == nullptr)
                PANIC("Trying to assign a null OwnPtr to a NonnullOwnPtr!");
        }

        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE bool is_valid() const
    requires(is_nonnull == OwnIsNonnull::No)
    {
        return (m_pointer != nullptr);
    }

    NODISCARD ALWAYS_INLINE T* raw()
    requires(is_nonnull == OwnIsNonnull::No)
    {
        return m_pointer;
    }

    NODISCARD ALWAYS_INLINE T const* raw() const
    requires(is_nonnull == OwnIsNonnull::No)
    {
        return m_pointer;
    }

    NODISCARD ALWAYS_INLINE T* get()
    {
        if (m_pointer == nullptr) {
            if constexpr (is_nonnull == OwnIsNonnull::No)
                PANIC("Trying to dereference a null OwnPtr!");
            else
                PANIC("Trying to dereference a NonnullOwnPtr after it was moved or leaked!");
        }

        return m_pointer;
    }

    NODISCARD ALWAYS_INLINE T const* get() const
    {
        if (m_pointer == nullptr) {
            if constexpr (is_nonnull == OwnIsNonnull::No)
                PANIC("Trying to dereference a null OwnPtr!");
            else
                PANIC("Trying to dereference a NonnullOwnPtr after it was moved or leaked!");
        }

        return m_pointer;
    }

    NODISCARD ALWAYS_INLINE T* operator->() { return get(); }
    NODISCARD ALWAYS_INLINE T const* operator->() const { return get(); }

    NODISCARD ALWAYS_INLINE T& operator*() { return *get(); }
    NODISCARD ALWAYS_INLINE T const& operator*() const { return *get(); }

public:
    ALWAYS_INLINE void release()
    requires(is_nonnull == OwnIsNonnull::No)
    {
        release_impl();
    }

    NODISCARD ALWAYS_INLINE T* leak_ptr()
    {
        T* pointer = m_pointer;
        m_pointer = nullptr;
        return pointer;
    }

private:
    ALWAYS_INLINE constexpr explicit OwnPtr(T* pointer)
    requires(is_nonnull == OwnIsNonnull::No)
        : m_pointer(pointer)
    {
    }

    ALWAYS_INLINE constexpr explicit OwnPtr(T& instance)
    requires(is_nonnull == OwnIsNonnull::Yes)
        : m_pointer(&instance)
    {
    }

    ALWAYS_INLINE void release_impl()
    {
        T* pointer = leak_ptr();
        if (pointer)
            delete pointer;

        // NOTE: This happens when the destructor invoked on the line above causes this OwnPtr
        //       instance to hold a new value. Besides being very confusing for an OwnPtr to still
        //       be valid right after calling 'release()' on it, re-entrant calls can crash the
        //       runtime, cause memory leaks, or at least cause undefined behavior.
        if (m_pointer != nullptr)
            PANIC("Re-entrant call stack caused by releasing an OwnPtr!");
    }

private:
    T* m_pointer;
};

template<typename T>
requires(!is_reference<T> && !is_const<T>)
using NonnullOwnPtr = OwnPtr<T, OwnIsNonnull::Yes>;

template<typename T>
requires(!is_reference<T> && !is_const<T>)
NODISCARD ALWAYS_INLINE OwnPtr<T> adopt_own(T* pointer)
{
    return OwnPtr<T>(pointer);
}

template<typename T>
requires(!is_reference<T> && !is_const<T>)
NODISCARD ALWAYS_INLINE NonnullOwnPtr<T> adopt_nonnull_own(T& instance)
{
    return NonnullOwnPtr<T>(instance);
}

template<typename T, typename... Args>
requires(!is_reference<T> && !is_const<T>)
NODISCARD ALWAYS_INLINE NonnullOwnPtr<T> make(Args&&... args)
{
    T* pointer = new T(forward<Args>(args)...);
    ASSERT(pointer);
    return adopt_nonnull_own(*pointer);
}

} // namespace AND
