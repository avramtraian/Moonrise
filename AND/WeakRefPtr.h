/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/RefCounted.h>
#include <AND/RefPtr.h>

namespace AND {

template<NonConstNonReferenceTypename T>
class WeakRefPtr {
    template<NonConstNonReferenceTypename FriendT>
    friend class WeakRefPtr;

public:
    ALWAYS_INLINE constexpr WeakRefPtr()
        : m_pointer(nullptr)
    {
    }

    ALWAYS_INLINE ~WeakRefPtr()
    {
        release();
    }

    ALWAYS_INLINE WeakRefPtr(WeakRefPtr const& other)
        : m_pointer(nullptr)
    {
        copy_from(other);
    }

    template<typename OtherT>
    requires(is_convertible<OtherT*, T*> && !is_same<OtherT, T>)
    /*implicit*/ ALWAYS_INLINE WeakRefPtr(WeakRefPtr<OtherT> const& other)
        : m_pointer(nullptr)
    {
        copy_from(other);
    }

    ALWAYS_INLINE WeakRefPtr(WeakRefPtr&& other) noexcept
        : m_pointer(nullptr)
    {
        move_from(move(other));
    }

    template<typename OtherT>
    requires(is_convertible<OtherT*, T*> && !is_same<OtherT, T>)
    /*implicit*/ ALWAYS_INLINE WeakRefPtr(WeakRefPtr<OtherT>&& other) noexcept
        : m_pointer(nullptr)
    {
        move_from(move(other));
    }

    template<typename OtherT, RefIsNonnull is_nonnull>
    requires(is_convertible<OtherT*, T*>)
    /*implicit*/ ALWAYS_INLINE WeakRefPtr(RefPtr<OtherT, is_nonnull> const& other)
        : m_pointer(nullptr)
    {
        if constexpr (is_nonnull == RefIsNonnull::Yes) {
            // Validate that the non-null semantics are satisfied.
            if (other.m_pointer == nullptr)
                PANIC("Trying to construct a WeakRefPtr from a NonnullRefPtr that was moved or leaked!");

            m_pointer = other.m_pointer;
            m_pointer->increment_weak_ref_count();
        } else {
            if (other.is_valid()) {
                m_pointer = other.m_pointer;
                m_pointer->increment_weak_ref_count();
            }
        }
    }

    ALWAYS_INLINE WeakRefPtr& operator=(WeakRefPtr const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        copy_from(other);
        return *this;
    }

    template<typename OtherT>
    requires(is_convertible<OtherT*, T*> && !is_same<OtherT, T>)
    ALWAYS_INLINE WeakRefPtr& operator=(WeakRefPtr<OtherT> const& other)
    {
        copy_from(other);
        return *this;
    }

    ALWAYS_INLINE WeakRefPtr& operator=(WeakRefPtr&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        move_from(move(other));
        return *this;
    }

    template<typename OtherT>
    requires(is_convertible<OtherT*, T*> && !is_same<OtherT, T>)
    ALWAYS_INLINE WeakRefPtr& operator=(WeakRefPtr<OtherT>&& other) noexcept
    {
        move_from(move(other));
        return *this;
    }

    template<typename OtherT, RefIsNonnull is_nonnull>
    requires(is_convertible<OtherT*, T*>)
    ALWAYS_INLINE WeakRefPtr& operator=(RefPtr<OtherT, is_nonnull> const& other)
    {
        release();
        if (other.is_valid()) {
            m_pointer = other.m_pointer;
            m_pointer->increment_weak_ref_count();
        }
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE bool is_valid() const
    {
        if (m_pointer == nullptr)
            return false;
        return m_pointer->has_strong_ref_count();
    }

    NODISCARD ALWAYS_INLINE T* raw() const
    {
        return reinterpret_cast<T*>(m_pointer);
    }

    NODISCARD ALWAYS_INLINE T* get() const
    {
        if (!is_valid())
            return nullptr;
        return raw();
    }

    NODISCARD ALWAYS_INLINE T* operator->() const { return get(); }
    NODISCARD ALWAYS_INLINE T& operator*() const { return *get(); }

public:
    NODISCARD ALWAYS_INLINE T* leak_ptr()
    {
        T* pointer = reinterpret_cast<T*>(m_pointer);
        m_pointer = nullptr;
        return pointer;
    }

    ALWAYS_INLINE void release()
    {
        WeakRefCounted* pointer = leak_ptr();
        if (!pointer)
            return;

        if (pointer->decrement_weak_ref_count()) {
            if (!pointer->has_strong_ref_count()) {
                // NOTE: Since the strong reference count is zero, the pointer does not reference an instance
                //       of the 'T' class, but a final instance of the 'WeakRefCounted' class. We must delete
                //       the pointer as a 'WeakRefCounted*' to avoid invoking the destructor of 'T' twice.
                delete pointer;
            }
        }
    }

private:
    template<typename OtherT>
    requires(is_convertible<OtherT*, T*>)
    ALWAYS_INLINE void copy_from(WeakRefPtr<OtherT> const& source)
    {
        release();
        if (source.is_valid()) {
            m_pointer = source.m_pointer;
            m_pointer->increment_weak_ref_count();
        }
    }

    template<typename OtherT>
    requires(is_convertible<OtherT*, T*>)
    ALWAYS_INLINE void move_from(WeakRefPtr<OtherT>&& source)
    {
        release();
        m_pointer = source.m_pointer;
        source.m_pointer = nullptr;
    }

private:
    WeakRefCounted* m_pointer;
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::WeakRefPtr;
#endif
