/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Forward.h"
#include "AND/Noncopyable.h"
#include "AND/NumberBase.h"
#include "AND/Optional.h"
#include "AND/Utf16View.h"
#include "AND/Utf8View.h"

namespace AND {

enum class PaddingCharacter {
    Zero,
    Whitespace,
};

enum class PlusSign {
    Never,
    Always,
    ReplaceWithWhitespace,
};

enum class SignAlignment {
    Right,
    Left,
};

enum class LineSeparator {
    LF,
    CRLF,
    CR,
};

void append_to_builder(StringBuilder&, Optional<Utf8View>, u8 const&);
void append_to_builder(StringBuilder&, Optional<Utf8View>, u16 const&);
void append_to_builder(StringBuilder&, Optional<Utf8View>, u32 const&);
void append_to_builder(StringBuilder&, Optional<Utf8View>, u64 const&);

void append_to_builder(StringBuilder&, Optional<Utf8View>, s8 const&);
void append_to_builder(StringBuilder&, Optional<Utf8View>, s16 const&);
void append_to_builder(StringBuilder&, Optional<Utf8View>, s32 const&);
void append_to_builder(StringBuilder&, Optional<Utf8View>, s64 const&);

void append_to_builder(StringBuilder&, Optional<Utf8View>, f32 const&);
void append_to_builder(StringBuilder&, Optional<Utf8View>, f64 const&);

void append_to_builder(StringBuilder&, Optional<Utf8View>, bool const&);
void append_to_builder(StringBuilder&, Optional<Utf8View>, void* const&);

class StringBuilder {
    AND_MAKE_NONCOPYABLE(StringBuilder);

public:
    enum class Encoding : u8 {
        UTF8,
        UTF16,
        ASCII,
    };

    static constexpr usize inline_capacity = 128;
    static_assert(inline_capacity > 0);

    static constexpr usize growth_factor_numerator = 3;
    static constexpr usize growth_factor_denominator = 2;
    static_assert(growth_factor_numerator >= growth_factor_denominator && growth_factor_denominator > 0);

    struct Block {
        Block* next { nullptr };
        usize byte_count { 0 };
        usize byte_offset { 0 };
        RWByte buffer[inline_capacity] {};
    };

public:
    StringBuilder();
    ~StringBuilder();

    StringBuilder(StringBuilder&&) noexcept;
    StringBuilder& operator=(StringBuilder&&) noexcept;

    void set_encoding(Encoding);
    NODISCARD ALWAYS_INLINE Encoding encoding() const { return m_encoding; }
    NODISCARD ALWAYS_INLINE bool is_encoded_in_utf8() const { return m_encoding == Encoding::UTF8; }
    NODISCARD ALWAYS_INLINE bool is_encoded_in_utf16() const { return m_encoding == Encoding::UTF16; }
    NODISCARD ALWAYS_INLINE bool is_encoded_in_ascii() const { return m_encoding == Encoding::ASCII; }

    NODISCARD Utf8String build_utf8() const;
    NODISCARD Utf16String build_utf16() const;

    void clear();

public:
    void append_code_point(u32);
    void append_code_point_repeated(u32, usize);

    void append_utf8(Utf8View const&);
    void append_utf16(Utf16View const&);

    void append_repeated(Utf8View const&, usize);
    void append_repeated(Utf16View const&, usize);

    void append_unsigned(
        s64 value,
        NumberBase number_base = NumberBase::Decimal,
        Optional<u32> min_digit_count = {},
        PaddingCharacter padding_character = PaddingCharacter::Zero,
        PlusSign plus_sign = PlusSign::Never,
        SignAlignment sign_alignment = SignAlignment::Right);

    void append_signed(
        s64 value,
        NumberBase number_base = NumberBase::Decimal,
        Optional<u32> min_digit_count = {},
        PaddingCharacter padding_character = PaddingCharacter::Zero,
        PlusSign plus_sign = PlusSign::Never,
        SignAlignment sign_alignment = SignAlignment::Right);

    void append_float(f64 value);

    void append_newline(LineSeparator line_separator = LineSeparator::LF);
    void append_indentation(u32 indentation_level, u32 indentation_size);

    template<typename T>
    ALWAYS_INLINE void append(T const& value)
    {
        append_to_builder(*this, {}, value);
    }

    template<typename T, typename... Args>
    ALWAYS_INLINE void append(T const& value, Args&&... args)
    {
        append(value);
        append(forward<Args>(args)...);
    }

    template<typename... Args>
    ALWAYS_INLINE void append_formatted(Utf8View format, Args&&... args)
    {
        append_formatted_impl(format, forward<Args>(args)...);
    }

    template<typename... Args>
    ALWAYS_INLINE void join(u32 separator, Args&&... args)
    {
        join_impl(separator, forward<Args>(args)...);
    }

private:
    NODISCARD static Block* allocate_memory(usize in_byte_count);
    static void free_memory(Block*);

    void append_block(usize min_byte_count);
    void push_bytes(ROBytes, usize);

    ALWAYS_INLINE void append_formatted_impl(Utf8View format)
    {
        // Append the rest of the format string to the builder. Any format arguments will be copied directly.
        append_utf8(format);
    }

    template<typename T, typename... Args>
    ALWAYS_INLINE void append_formatted_impl(Utf8View format, T const& argument_value, Args&&... args)
    {
        // 1. Find the position of the format argument. If there is no valid format argument, we append
        // the entire of the format string directly.
        static constexpr u32 argument_start_token = '{';
        static constexpr u32 argument_end_token = '}';

        auto argument_start_offset = format.find(argument_start_token);
        if (!argument_start_offset.has_value()) {
            append_formatted_impl(format);
            return;
        }

        auto argument_end_offset = format.slice(*argument_start_offset).find(argument_end_token);
        if (!argument_end_offset.has_value()) {
            append_formatted_impl(format);
            return;
        }
        argument_end_offset = *argument_start_offset + *argument_end_offset;

        // 2. Append the code points until the format argument.
        append_utf8(format.slice(0, *argument_start_offset));

        // 3. Append the format argument.
        auto argument = format.slice(*argument_start_offset + sizeof(RWByte), *argument_end_offset);
        append_to_builder(*this, argument, argument_value);

        // 4. Process the rest of the format string.
        auto remaining_format = format.slice(*argument_end_offset + sizeof(RWByte));
        append_formatted_impl(remaining_format, forward<Args>(args)...);
    }

    ALWAYS_INLINE static constexpr void join_impl(u32)
    {
        // Base case for the templated version. There is nothing left to do.
    }

    template<typename T>
    ALWAYS_INLINE void join_impl(u32, T const& value)
    {
        append(value);
    }

    template<typename T, typename Q, typename... Args>
    ALWAYS_INLINE void join_impl(u32 separator, T const& left_value, Q&& right_value, Args&&... args)
    {
        append(left_value);
        append_code_point(separator);
        join_impl(separator, forward<Q>(right_value), forward<Args>(args)...);
    }

private:
    Encoding m_encoding;
    Block m_head_block;
    Block* m_tail_block;
};

} // namespace AND
