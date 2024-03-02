/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/RefPtr.h>

namespace AT {

template<typename T>
requires (is_derived_from<T, RefCounted<T>>)
class NonnullRefPtr {
    template<typename Q>
    friend NonnullRefPtr<Q> adopt_nonnull_ref(Q*);

public:
    ALWAYS_INLINE NonnullRefPtr(const NonnullRefPtr& other)
        : m_instance(other.m_instance)
    {
        AT_ASSERT(other.m_instance);
        increment_reference_count();
    }

    ALWAYS_INLINE NonnullRefPtr(NonnullRefPtr&& other) noexcept
        : m_instance(other.m_instance)
    {
        AT_ASSERT(other.m_instance);
        other.m_instance = nullptr;
    }

    ALWAYS_INLINE NonnullRefPtr(const RefPtr<T>& ref_ptr)
        : m_instance(ref_ptr.raw())
    {
        AT_ASSERT(ref_ptr.raw());
        increment_reference_count();
    }

    ALWAYS_INLINE ~NonnullRefPtr()
    {
        AT_ASSERT(m_instance);
        decrement_reference_count();
    }

    ALWAYS_INLINE NonnullRefPtr& operator=(const NonnullRefPtr& other)
    {
        AT_ASSERT(m_instance);
        decrement_reference_count();

        AT_ASSERT(other.m_instance);
        m_instance = other.m_instance;
        increment_reference_count();

        return *this;
    }

    ALWAYS_INLINE NonnullRefPtr& operator=(NonnullRefPtr&& other) noexcept
    {
        AT_ASSERT(m_instance);
        decrement_reference_count();

        AT_ASSERT(other.m_instance);
        m_instance = other.m_instance;
        other.m_instance = nullptr;

        return *this;
    }

    ALWAYS_INLINE NonnullRefPtr& operator=(const RefPtr<T>& ref_ptr)
    {
        AT_ASSERT(m_instance);
        decrement_reference_count();

        AT_ASSERT(ref_ptr.raw());
        m_instance = ref_ptr.raw();
        increment_reference_count();

        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE T& get() const { return *m_instance; }
    NODISCARD ALWAYS_INLINE T* raw() const { return m_instance; }

    NODISCARD ALWAYS_INLINE T* operator->() const { return &get(); }
    NODISCARD ALWAYS_INLINE T& operator*() const { return get(); }

    NODISCARD ALWAYS_INLINE RefPtr<T> as_ref_ptr() const { return adopt_ref<T>(m_instance); }
    ALWAYS_INLINE operator RefPtr<T>() const { return as_ref_ptr(); }

private:
    ALWAYS_INLINE explicit NonnullRefPtr(T* pointer_to_adopt)
        : m_instance(pointer_to_adopt)
    {
        AT_ASSERT(pointer_to_adopt);
        increment_reference_count();
    }

    void increment_reference_count() { static_cast<RefCounted<T>*>(m_instance)->increment_reference_count(); }
    void decrement_reference_count() { static_cast<RefCounted<T>*>(m_instance)->decrement_reference_count(); }

private:
    T* m_instance;
};

template<typename T>
NODISCARD ALWAYS_INLINE NonnullRefPtr<T> adopt_nonnull_ref(T* pointer_to_adopt)
{
    // NOTE: Checking that the pointer is not null is performed inside the constructor.
    return NonnullRefPtr<T>(pointer_to_adopt);
}

template<typename T>
NODISCARD ALWAYS_INLINE NonnullRefPtr<T> adopt_nonnull_ref(T& pointer_to_adopt)
{
    return NonnullRefPtr<T>(&pointer_to_adopt);
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE NonnullRefPtr<T> make_nonnull_ref(Args&&... args)
{
    auto* raw_pointer = new T(forward<Args>(args)...);
    return adopt_nonnull_ref(raw_pointer);
}

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::adopt_nonnull_ref;
using AT::make_nonnull_ref;
using AT::NonnullRefPtr;
#endif // AT_INCLUDE_GLOBALLY