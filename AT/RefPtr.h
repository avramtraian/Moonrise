/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Assertion.h>
#include <AT/Types.h>

namespace AT {

class RefCountedBase {
    AT_MAKE_NONCOPYABLE(RefCountedBase);
    AT_MAKE_NONMOVABLE(RefCountedBase);

protected:
    RefCountedBase() = default;
    virtual ~RefCountedBase() = default;
};

template<typename T>
class RefCounted : public RefCountedBase {
public:
    ALWAYS_INLINE RefCounted()
        : m_reference_count(0)
    {}
    virtual ~RefCounted() override = default;

    void increment_reference_count()
    {
        // TODO: Ensure that addition would not overflow.
        ++m_reference_count;
    }

    bool decrement_reference_count()
    {
        AT_ASSERT_DEBUG(m_reference_count > 0);

        if (--m_reference_count == 0) {
            auto* self = static_cast<T*>(this);
            delete self;
            return true;
        }

        return false;
    }

    u32 reference_count() const
    {
        AT_ASSERT(m_reference_count > 0);
        return m_reference_count;
    }

private:
    u32 m_reference_count;
};

template<typename T>
class RefPtr {
    template<typename Q>
    friend RefPtr<Q> adopt_ref(Q*);

public:
    ALWAYS_INLINE RefPtr()
        : m_instance(nullptr)
    {}

    ALWAYS_INLINE RefPtr(const RefPtr& other)
        : m_instance(other.m_instance)
    {
        if (m_instance) {
            increment_reference_count();
        }
    }

    ALWAYS_INLINE RefPtr(RefPtr&& other) noexcept
        : m_instance(other.m_instance)
    {
        other.m_instance = nullptr;
    }

    ALWAYS_INLINE ~RefPtr()
    {
        if (m_instance) {
            decrement_reference_count();
        }
    }

    ALWAYS_INLINE RefPtr& operator=(const RefPtr& other)
    {
        clear();
        m_instance = other.m_instance;
        if (m_instance) {
            increment_reference_count();
        }
        return *this;
    }

    ALWAYS_INLINE RefPtr& operator=(RefPtr&& other) noexcept
    {
        clear();
        m_instance = other.m_instance;
        other.m_instance = nullptr;
        return *this;
    }

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
            decrement_reference_count();
            m_instance = nullptr;
        }
    }

private:
    ALWAYS_INLINE explicit RefPtr(T* pointer_to_adopt)
        : m_instance(pointer_to_adopt)
    {
        if (m_instance) {
            increment_reference_count();
        }
    }

    ALWAYS_INLINE void increment_reference_count()
    {
        static_assert(is_derived_from<T, RefCountedBase>, "Trying to use RefPtr<T> when T is not derived from RefCounted<T>!");
        static_cast<RefCounted<T>*>(m_instance)->increment_reference_count();
    }

    ALWAYS_INLINE void decrement_reference_count()
    {
        static_assert(is_derived_from<T, RefCountedBase>, "Trying to use RefPtr<T> when T is not derived from RefCounted<T>!");
        static_cast<RefCounted<T>*>(m_instance)->decrement_reference_count();
    }

private:
    T* m_instance;
};

template<typename T>
NODISCARD ALWAYS_INLINE RefPtr<T> adopt_ref(T* pointer_to_adopt)
{
    return RefPtr<T>(pointer_to_adopt);
}

template<typename T>
NODISCARD ALWAYS_INLINE RefPtr<T> adopt_ref(T& pointer_to_adopt)
{
    return adopt_ref(&pointer_to_adopt);
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE RefPtr<T> make_ref(Args&&... args)
{
    auto* raw_pointer = new T(forward<Args>(args)...);
    return adopt_ref(raw_pointer);
}

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::RefCounted;
using AT::RefPtr;
#endif // AT_INCLUDE_GLOBALLY
