/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Assertion.h"
#include "AT/CoreDefines.h"
#include "AT/CoreTypes.h"
#include "AT/MemoryOperations.h"

namespace AT {

class Error {
    AT_MAKE_NONCOPYABLE(Error);

    template<typename T>
    friend class ErrorOr;

public:
    enum Code : u32 {
        None = 0,

        BufferOverflow,
        IndexOutOfRange,
        InvalidEncoding,
        OutOfMemory,
    };

    enum class Kind : u8 {
        Code,
        StringLiteral,
        StringAllocated,
    };

public:
    AT_API static Error from_error_code(Code error_code);

private:
    Error() = default;
    Error(Error&& other) noexcept;
    Error& operator=(Error&& other) noexcept = delete;

private:
    union {
        Code code;
        const char* string_literal;
        struct {
            char* heap_buffer;
            usize byte_count;
        } string_allocated;

    } m_error;
    Kind m_error_kind;
};

template<typename T>
class NODISCARD ErrorOr {
    AT_MAKE_NONCOPYABLE(ErrorOr);
    AT_MAKE_NONMOVABLE(ErrorOr);

public:
    ALWAYS_INLINE ErrorOr(T value)
        : m_is_error(false)
        , m_value_storage(move(value))
    {}

    ALWAYS_INLINE ErrorOr(Error&& error)
        : m_is_error(true)
        , m_error_storage(move(error))
    {}

    ALWAYS_INLINE ErrorOr(Error::Code error_code)
        : m_is_error(true)
        , m_error_storage(Error::from_error_code(error_code))
    {}

    ALWAYS_INLINE ~ErrorOr()
    {
        if (m_is_error) {
            m_error_storage.~Error();
        }
        else {
            m_value_storage.~T();
        }
    }

public:
    NODISCARD ALWAYS_INLINE bool is_error() const { return m_is_error; }

    NODISCARD ALWAYS_INLINE T&& release_value() { return move(m_value_storage); }
    NODISCARD ALWAYS_INLINE Error&& release_error() { return move(m_error_storage); }

private:
    union {
        Error m_error_storage;
        T m_value_storage;
    };

    bool m_is_error;
};

template<typename T>
class NODISCARD ErrorOr<T&> {
    AT_MAKE_NONCOPYABLE(ErrorOr);
    AT_MAKE_NONMOVABLE(ErrorOr);

public:
    ALWAYS_INLINE ErrorOr(T& value)
        : m_is_error(false)
        , m_value_storage(&value)
    {}

    ALWAYS_INLINE ErrorOr(Error&& error)
        : m_is_error(true)
        , m_error_storage(move(error))
    {}

    ALWAYS_INLINE ErrorOr(Error::Code error_code)
        : m_is_error(true)
        , m_error_storage(Error::from_error_code(error_code))
    {}

    ALWAYS_INLINE ~ErrorOr()
    {
        if (m_is_error) {
            m_error_storage.~Error();
        }
    }

public:
    NODISCARD ALWAYS_INLINE bool is_error() const { return m_is_error; }

    NODISCARD ALWAYS_INLINE T& release_value() { return *m_value_storage; }
    NODISCARD ALWAYS_INLINE Error&& release_error() { return move(m_error_storage); }

private:
    union {
        Error m_error_storage;
        T* m_value_storage;
    };

    bool m_is_error;
};

template<>
class NODISCARD ErrorOr<void> {
    AT_MAKE_NONCOPYABLE(ErrorOr);
    AT_MAKE_NONMOVABLE(ErrorOr);

public:
    ALWAYS_INLINE ErrorOr()
        : m_is_error(false)
        , m_error_storage {}
    {}

    ALWAYS_INLINE ~ErrorOr()
    {
        if (m_is_error) {
            reinterpret_cast<Error*>(m_error_storage)->~Error();
        }
    }

    ALWAYS_INLINE ErrorOr(Error&& error)
        : m_is_error(true)
    {
        new (m_error_storage) Error(move(error));
    }

    ALWAYS_INLINE ErrorOr(Error::Code error_code)
        : m_is_error(true)
    {
        new (m_error_storage) Error(Error::from_error_code(error_code));
    }

public:
    NODISCARD ALWAYS_INLINE bool is_error() const { return m_is_error; }

    ALWAYS_INLINE void release_value() {}
    NODISCARD ALWAYS_INLINE Error&& release_error() { return move(*reinterpret_cast<Error*>(m_error_storage)); }

private:
    alignas(Error) u8 m_error_storage[sizeof(Error)];
    bool m_is_error;
};

} // namespace AT

//
// Checks if the result of the expression is an error and if so it will throw an assert.
// The value that is stored in the expression result, if it is not an error, will be ignored.
//
// This utility macro should only be used when actually handling errors is impossible or not useful. By throwing an
// assertion, the application will crash.
//
#define MUST(expression)                                                 \
    {                                                                    \
        auto _at_error_or_##variable = (expression);                     \
        if (_at_error_or_##variable.is_error()) [[unlikely]] {           \
            /* TODO: Log the expression that caused the must to fail. */ \
            AT_ASSERT(!#expression);                                     \
        }                                                                \
    }

//
// Same behaviour and use cases as the plain MUST macro, except that the value that is stored in the expression result,
// if it is not an error, will be moved from the ErrorOr structure into the given variable.
// The variable declaration can also be used to create a new variable and initialize it with the result value.
//
#define MUST_ASSIGN(variable_declaration, expression)                    \
    auto&& AT_CONCATENATE(_at_error_or, AT_LINE) = (expression);         \
    if (AT_CONCATENATE(_at_error_or, AT_LINE).is_error()) [[unlikely]] { \
        /* TODO: Log the expression that caused the must to fail. */     \
        AT_ASSERT(!#expression);                                         \
    }                                                                    \
    variable_declaration = AT_CONCATENATE(_at_error_or, AT_LINE).release_value();

//
// Checks if the result of the expression is an error and if so the function that called this macro will return.
// The calling must function must return an ErrorOr. If the expression result is an error, it will be released and
// returned, expecting to be handled by the next function in the callstack.
//
// Unlike the MUST macro, this can be used to propagate errors though the system, as it will not crash the application.
//
#define TRY(expression)                             \
    {                                               \
        auto _at_error_or = (expression);           \
        if (_at_error_or.is_error()) [[unlikely]] { \
            return _at_error_or.release_error();    \
        }                                           \
        _at_error_or.release_value();               \
    }

//
// Same behaviour and use cases as the plain TRY macro, except that the value that is stored in the expression result,
// if it is not an error, will be moved from the ErrorOr structure into the given variable.
// The variable declaration can also be used to create a new variable and initialize it with the result value.
//
#define TRY_ASSIGN(variable_declaration, expression)                     \
    auto&& AT_CONCATENATE(_at_error_or, AT_LINE) = (expression);         \
    if (AT_CONCATENATE(_at_error_or, AT_LINE).is_error()) [[unlikely]] { \
        return AT_CONCATENATE(_at_error_or, AT_LINE).release_error();    \
    }                                                                    \
    variable_declaration = AT_CONCATENATE(_at_error_or, AT_LINE).release_value();