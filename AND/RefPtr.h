/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/RefCounted.h"

namespace AND {

enum class RefIsNonnull {
    No,
    Yes,
};

template<typename T, RefIsNonnull is_nonnull = RefIsNonnull::No>
class RefPtr {
    template<typename FriendT, RefIsNonnull friend_is_nonnull>
    friend class RefPtr;

    template<typename FriendT>
    friend RefPtr<FriendT> adopt(FriendT*);

    template<typename FriendT>
    friend RefPtr<FriendT, RefIsNonnull::Yes> adopt_nonnull(FriendT&);

public:
    ALWAYS_INLINE constexpr RefPtr()
    requires(is_nonnull == RefIsNonnull::No)
        : m_pointer(nullptr)
    {
    }

    ALWAYS_INLINE ~RefPtr()
    {
        release_impl();
    }

    ALWAYS_INLINE RefPtr(RefPtr const& other)
        : m_pointer(nullptr)
    {
        copy_from(other);
    }

    template<RefIsNonnull other_is_nonnull>
    requires(is_nonnull != other_is_nonnull)
    /*implicit*/ ALWAYS_INLINE RefPtr(RefPtr<T, other_is_nonnull> const& other)
        : m_pointer(nullptr)
    {
        copy_from(other);
    }

    template<typename OtherT, RefIsNonnull other_is_nonnull>
    requires(is_convertible<OtherT*, T*> && !is_same<OtherT, T>)
    /*implicit*/ ALWAYS_INLINE RefPtr(RefPtr<OtherT, other_is_nonnull> const& other)
        : m_pointer(nullptr)
    {
        copy_from(other);
    }

    ALWAYS_INLINE RefPtr(RefPtr&& other) noexcept
        : m_pointer(nullptr)
    {
        move_from(move(other));
    }

    template<RefIsNonnull other_is_nonnull>
    requires(is_nonnull != other_is_nonnull)
    /*implicit*/ ALWAYS_INLINE RefPtr(RefPtr<T, other_is_nonnull>&& other) noexcept
        : m_pointer(nullptr)
    {
        move_from(move(other));
    }

    template<typename OtherT, RefIsNonnull other_is_nonnull>
    requires(is_convertible<OtherT*, T*> && !is_same<OtherT, T>)
    /*implicit*/ ALWAYS_INLINE RefPtr(RefPtr<OtherT, other_is_nonnull>&& other) noexcept
        : m_pointer(nullptr)
    {
        move_from(move(other));
    }

    ALWAYS_INLINE RefPtr& operator=(RefPtr const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        copy_from(other);
        return *this;
    }

    template<RefIsNonnull other_is_nonnull>
    requires(is_nonnull != other_is_nonnull)
    ALWAYS_INLINE RefPtr& operator=(RefPtr<T, other_is_nonnull> const& other)
    {
        copy_from(other);
        return *this;
    }

    template<typename OtherT, RefIsNonnull other_is_nonnull>
    requires(is_convertible<OtherT*, T*> && !is_same<OtherT, T>)
    ALWAYS_INLINE RefPtr& operator=(RefPtr<OtherT, other_is_nonnull> const& other)
    {
        copy_from(other);
        return *this;
    }

    ALWAYS_INLINE RefPtr& operator=(RefPtr&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        move_from(move(other));
        return *this;
    }

    template<RefIsNonnull other_is_nonnull>
    requires(is_nonnull != other_is_nonnull)
    ALWAYS_INLINE RefPtr& operator=(RefPtr<T, other_is_nonnull>&& other) noexcept
    {
        move_from(move(other));
        return *this;
    }

    template<typename OtherT, RefIsNonnull other_is_nonnull>
    requires(is_convertible<OtherT*, T*>)
    ALWAYS_INLINE RefPtr& operator=(RefPtr<OtherT, other_is_nonnull>&& other) noexcept
    {
        move_from(move(other));
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE bool is_valid() const
    requires(is_nonnull == RefIsNonnull::No)
    {
        return (m_pointer != nullptr);
    }

    NODISCARD ALWAYS_INLINE T* raw()
    requires(is_nonnull == RefIsNonnull::No)
    {
        return m_pointer;
    }

    NODISCARD ALWAYS_INLINE T const* raw() const
    requires(is_nonnull == RefIsNonnull::No)
    {
        return m_pointer;
    }

    NODISCARD ALWAYS_INLINE T* get() const
    {
        if (m_pointer == nullptr) {
            if constexpr (is_nonnull == RefIsNonnull::No)
                PANIC("Trying to dereference a null RefPtr!");
            else
                PANIC("Trying to dereference a NonnullRefPtr after it was moved or leaked!");
        }

        return m_pointer;
    }

    NODISCARD ALWAYS_INLINE T* operator->() const { return get(); }
    NODISCARD ALWAYS_INLINE T& operator*() const { return *get(); }

public:
    ALWAYS_INLINE void release()
    requires(is_nonnull == RefIsNonnull::No)
    {
        release_impl();
    }

    NODISCARD ALWAYS_INLINE T* leak_ptr()
    {
        T* pointer = m_pointer;
        m_pointer = nullptr;
        return pointer;
    }

    template<typename Q>
    requires((is_convertible<Q*, T*> || is_convertible<T*, Q*>) && !is_same<T, Q>)
    NODISCARD ALWAYS_INLINE RefPtr<Q, is_nonnull> as() const
    {
        Q* casted_pointer = reinterpret_cast<Q*>(m_pointer);
        if constexpr (is_nonnull == RefIsNonnull::Yes)
            return RefPtr<Q, is_nonnull>(*casted_pointer);
        else
            return RefPtr<Q, is_nonnull>(casted_pointer);
    }

private:
    ALWAYS_INLINE static void try_increment_ref_count(T const* pointer)
    {
        if (pointer)
            pointer->increment_ref_count();
    }

    ALWAYS_INLINE void release_impl()
    {
        if (T* pointer = leak_ptr()) {
            if (pointer->decrement_ref_count())
                delete pointer;
        }

        // NOTE: This happens when decrementing the reference count causes the destructor to be
        //       invoked, which in turn assigns a new value to this RefPtr instance. Besides being
        //       very confusing for a RefPtr to still be valid right after calling 'release()' on it,
        //       re-entrant calls can crash the runtime, cause memory leaks, or at least cause
        //       undefined behavior.
        if (m_pointer != nullptr)
            ASSERTF(m_pointer == nullptr, "Re-entrant call stack caused by releasing a RefPtr!");
    }

    template<typename SourceT, RefIsNonnull source_is_nonnull>
    requires(is_convertible<SourceT*, T*>)
    ALWAYS_INLINE void copy_from(RefPtr<SourceT, source_is_nonnull> const& source)
    {
        release_impl();
        m_pointer = source.m_pointer;
        RefPtr::try_increment_ref_count(m_pointer);

        // Validate that the non-null semantics are satisfied.
        if constexpr (is_nonnull == RefIsNonnull::Yes) {
            if (m_pointer == nullptr) {
                if constexpr (source_is_nonnull == RefIsNonnull::Yes)
                    PANIC("Trying to copy-construct a NonnullRefPtr from another NonnullRefPtr that was moved or leaked!");
                else
                    PANIC("Trying to copy-construct a NonnullRefPtr from a null RefPtr!");
            }
        } else if (source_is_nonnull == RefIsNonnull::Yes)
            if (m_pointer == nullptr)
                PANIC("Trying to copy-construct a RefPtr from a NonnullRefPtr that was moved or leaked!");
    }

    template<typename SourceT, RefIsNonnull source_is_nonnull>
    requires(is_convertible<SourceT*, T*>)
    ALWAYS_INLINE void move_from(RefPtr<SourceT, source_is_nonnull>&& source) noexcept
    {
        release_impl();
        m_pointer = source.m_pointer;
        source.m_pointer = nullptr;

        // Validate that the non-null semantics are satisfied.
        if constexpr (is_nonnull == RefIsNonnull::Yes) {
            if (m_pointer == nullptr) {
                if constexpr (source_is_nonnull == RefIsNonnull::Yes)
                    PANIC("Trying to move-construct a NonnullRefPtr from another NonnullRefPtr that was moved or leaked!");
                else
                    PANIC("Trying to move-construct a NonnullRefPtr from a null RefPtr!");
            }
        } else if (source_is_nonnull == RefIsNonnull::Yes)
            if (m_pointer == nullptr)
                PANIC("Trying to move-construct a RefPtr from a NonnullRefPtr that was moved or leaked!");
    }

private:
    ALWAYS_INLINE explicit RefPtr(T* pointer)
    requires(is_nonnull == RefIsNonnull::No)
        : m_pointer(pointer)
    {
        RefPtr::try_increment_ref_count(m_pointer);
    }

    ALWAYS_INLINE explicit RefPtr(T& instance)
    requires(is_nonnull == RefIsNonnull::Yes)
        : m_pointer(&instance)
    {
        RefPtr::try_increment_ref_count(m_pointer);
    }

    T* m_pointer;
};

template<typename T, RefIsNonnull is_nonnull = RefIsNonnull::No>
using ConstRefPtr = RefPtr<T const, is_nonnull>;

template<typename T>
using NonnullRefPtr = RefPtr<T, RefIsNonnull::Yes>;

template<typename T>
using NonnullConstRefPtr = NonnullRefPtr<T const>;

template<typename T>
NODISCARD ALWAYS_INLINE RefPtr<T> adopt(T* pointer)
{
    return RefPtr<T>(pointer);
}

template<typename T>
NODISCARD ALWAYS_INLINE NonnullRefPtr<T> adopt_nonnull(T& instance)
{
    return NonnullRefPtr<T>(instance);
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE NonnullRefPtr<T> make_ref(Args&&... args)
{
    T* pointer = new T(forward<Args>(args)...);
    ASSERT(pointer);
    return adopt_nonnull(*pointer);
}

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::adopt;
using AND::adopt_nonnull;
using AND::ConstRefPtr;
using AND::make_ref;
using AND::NonnullConstRefPtr;
using AND::NonnullRefPtr;
using AND::RefIsNonnull;
using AND::RefPtr;
#endif
