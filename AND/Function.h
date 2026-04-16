/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Assertions.h"
#include "AND/Noncopyable.h"
#include "AND/Types.h"

namespace AND {

template<typename, typename...>
class Function { };

template<typename ReturnType, typename... Arguments>
class Function<ReturnType(Arguments...)> {
public:
    static constexpr usize inline_capacity = 24;
    static_assert(inline_capacity > 0);

public:
    class FunctorBase {
        AND_MAKE_NONCOPYABLE(FunctorBase);
        AND_MAKE_NONMOVABLE(FunctorBase);

    public:
        FunctorBase() = default;
        virtual ~FunctorBase() = default;

        virtual ReturnType invoke(Arguments...) const = 0;

        virtual void copy_to(void* dst_address) const = 0;
        virtual void move_to(void* dst_address) = 0;
    };

    template<typename FunctorType>
    class Functor : public FunctorBase {
    public:
        ALWAYS_INLINE explicit Functor(FunctorType const& functor)
            : m_functor(functor)
        {
        }

        ALWAYS_INLINE explicit Functor(FunctorType&& functor)
            : m_functor(move(functor))
        {
        }

        virtual ~Functor() override = default;

        virtual ReturnType invoke(Arguments... arguments) const override
        {
            return m_functor(arguments...);
        }

        virtual void copy_to(void* dst_address) const override
        {
            new (dst_address) Functor(m_functor);
        }

        virtual void move_to(void* dst_address) override
        {
            new (dst_address) Functor(move(m_functor));
        }

    private:
        FunctorType m_functor;
    };

public:
    ALWAYS_INLINE Function()
        : m_byte_count(0)
        , m_inline_buffer {}
    {
    }

    ALWAYS_INLINE ~Function()
    {
        clear();
    }

    ALWAYS_INLINE Function(Function const& other)
        : m_byte_count(other.m_byte_count)
    {
        void* dst_functor_address = m_inline_buffer;
        if (is_stored_on_heap()) {
            m_heap_functor = Function::allocate_memory(m_byte_count);
            dst_functor_address = m_heap_functor;
        }

        other.functor().copy_to(dst_functor_address);
    }

    ALWAYS_INLINE Function(Function&& other) noexcept
        : m_byte_count(other.m_byte_count)
    {
        if (is_stored_inline()) {
            other.functor().move_to(m_inline_buffer);
            other.clear();
        } else {
            m_heap_functor = other.m_heap_functor;
            other.m_heap_functor = nullptr;
            other.m_byte_count = 0;
        }
    }

    // FIXME: Introduce some 'require' clauses for 'FunctorType'. It seems really error-prone
    //        to treat any arbitrary type as a functor.
    template<typename FunctorType>
    /*implicit*/ ALWAYS_INLINE Function(FunctorType functor)
        : m_byte_count(sizeof(Functor<FunctorType>))
    {
        void* dst_functor_address = m_inline_buffer;
        if (is_stored_on_heap()) {
            m_heap_functor = Function::allocate_memory(m_byte_count);
            dst_functor_address = m_heap_functor;
        }

        new (dst_functor_address) Functor<FunctorType>(move(functor));
    }

    ALWAYS_INLINE Function& operator=(Function const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        // FIXME: If 'm_byte_count == other.m_byte_count' there is no point in releasing the heap buffer
        //        and then allocating a new one with the exact same size! The clear() API doesn't only
        //        destroy the functor object, but also releases the heap buffer.
        clear();
        m_byte_count = other.m_byte_count;

        void* dst_functor_address = m_inline_buffer;
        if (is_stored_on_heap()) {
            m_heap_functor = Function::allocate_memory(m_byte_count);
            dst_functor_address = m_heap_functor;
        }

        other.functor().copy_to(dst_functor_address);
        return *this;
    }

    ALWAYS_INLINE Function& operator=(Function&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        clear();
        m_byte_count = other.m_byte_count;

        if (is_stored_inline()) {
            other.functor().move_to(m_inline_buffer);
            other.clear();
        } else {
            m_heap_functor = other.m_heap_functor;
            other.m_heap_functor = nullptr;
            other.m_byte_count = 0;
        }

        return *this;
    }

    // FIXME: Introduce some 'require' clauses for 'FunctorType'. It seems really error-prone
    //        to treat any arbitrary type as a functor.
    template<typename FunctorType>
    ALWAYS_INLINE Function& operator=(FunctorType functor)
    {
        // FIXME: If 'm_byte_count == sizeof(Functor<FunctorType>)' there is no point in releasing the heap buffer
        //        and then allocating a new one with the exact same size! The clear() API doesn't only
        //        destroy the functor object, but also releases the heap buffer.
        clear();
        m_byte_count = sizeof(Functor<FunctorType>);

        void* dst_functor_address = m_inline_buffer;
        if (is_stored_on_heap()) {
            m_heap_functor = Function::allocate_memory(m_byte_count);
            dst_functor_address = m_heap_functor;
        }

        new (dst_functor_address) Functor<FunctorType>(move(functor));
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE bool is_empty() const
    {
        return (m_byte_count == 0);
    }

    ReturnType operator()(Arguments... arguments) const
    {
        if (is_empty())
            PANIC("Trying to invoke a Function that is empty!");

        return functor().invoke(arguments...);
    }

    ALWAYS_INLINE void clear()
    {
        usize byte_count = m_byte_count;
        m_byte_count = 0;

        if (byte_count > 0)
            functor().~FunctorBase();

        // NOTE: This happens when the destructor of the held functor object calls the
        //       destructor of some object captured by the lambda, which in turns assigns
        //       a new functor to this Function instance.
        if (m_byte_count != 0)
            PANIC("Re-entrant call stack caused by clearing a Function!");

        if (byte_count > inline_capacity)
            free_memory(m_heap_functor, byte_count);
        m_byte_count = 0;
    }

private:
    NODISCARD ALWAYS_INLINE static FunctorBase* allocate_memory(usize in_byte_count)
    {
        ASSERT(in_byte_count > inline_capacity);
        void* memory = operator new(in_byte_count);
        return static_cast<FunctorBase*>(memory);
    }

    ALWAYS_INLINE static void free_memory(FunctorBase* in_functor, MAYBE_UNUSED usize in_byte_count)
    {
        ASSERT(in_byte_count > inline_capacity);
        operator delete(in_functor);
    }

private:
    NODISCARD ALWAYS_INLINE bool is_stored_inline() const { return (m_byte_count <= inline_capacity); }
    NODISCARD ALWAYS_INLINE bool is_stored_on_heap() const { return (m_byte_count > inline_capacity); }

    NODISCARD ALWAYS_INLINE FunctorBase& functor()
    {
        if (is_stored_inline())
            return *reinterpret_cast<FunctorBase*>(m_inline_buffer);
        return *m_heap_functor;
    }

    NODISCARD ALWAYS_INLINE FunctorBase const& functor() const
    {
        if (is_stored_inline())
            return *reinterpret_cast<FunctorBase const*>(m_inline_buffer);
        return *m_heap_functor;
    }

private:
    usize m_byte_count;
    union {
        alignas(FunctorBase) RWByte m_inline_buffer[inline_capacity];
        FunctorBase* m_heap_functor;
    };
};

} // namespace AND
