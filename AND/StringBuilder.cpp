/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/StringBuilder.h"
#include "AND/Assertions.h"
#include "AND/MathUtilities.h"
#include "AND/MemoryOperations.h"
#include "AND/String.h"
#include "AND/Utf16String.h"

#include <new>

namespace AND {

StringBuilder::StringBuilder()
    : m_encoding(Encoding::UTF8)
    , m_tail_block(&m_head_block)
{
    m_head_block.byte_count = inline_capacity;
}

StringBuilder::~StringBuilder()
{
    clear();
}

StringBuilder::StringBuilder(StringBuilder&& other) noexcept
    : m_encoding(other.m_encoding)
    , m_head_block(other.m_head_block)
    , m_tail_block(&m_head_block)
{
    other.m_head_block.next = nullptr;
    other.m_tail_block = &other.m_head_block;
    other.m_head_block.byte_offset = 0;
    zero_memory(other.m_head_block.buffer, inline_capacity);
}

StringBuilder& StringBuilder::operator=(StringBuilder&& other) noexcept
{
    // Handle the self-assignment case.
    if (this == &other)
        return *this;

    clear();

    m_encoding = other.m_encoding;
    m_head_block = other.m_head_block;
    m_tail_block = &m_head_block;

    other.m_head_block.next = nullptr;
    other.m_tail_block = &other.m_head_block;
    other.m_head_block.byte_offset = 0;
    zero_memory(other.m_head_block.buffer, inline_capacity);

    return *this;
}

void StringBuilder::set_encoding(Encoding encoding)
{
    ASSERTF(
        m_head_block.byte_offset == 0 && m_tail_block == &m_head_block,
        "Trying to change the encoding of a StringBuilder after it has been used!");
    m_encoding = encoding;
}

String StringBuilder::build_utf8() const
{
    ASSERTF(is_encoded_in_utf8(), "Trying to construct a UTF-8 string from a StringBuilder that is encoded in something else!");

    // Find the required string buffer size.
    usize byte_count = 0;
    for (Block const* current_block = &m_head_block; current_block; current_block = current_block->next)
        byte_count += current_block->byte_offset;

    // Find the destination string buffer by allocating a heap block if necessary.
    String string;
    string.m_byte_count = byte_count + sizeof('\0');
    WOBytes destination = string.m_inline_buffer;
    if (string.is_stored_on_heap()) {
        string.m_heap_block = String::allocate_memory(string.m_byte_count);
        string.m_heap_block->reference_count = 1;
        destination = string.m_heap_block->buffer;
    }

    // Copy the contents of the builder to the string destination buffer.
    usize destination_offset = 0;
    for (Block const* current_block = &m_head_block; current_block; current_block = current_block->next) {
        copy_memory(destination + destination_offset, current_block->buffer, current_block->byte_offset);
        destination_offset += current_block->byte_offset;
    }
    destination[destination_offset] = '\0';

    return string;
}

Utf16String StringBuilder::build_utf16() const
{
    ASSERTF(is_encoded_in_utf16(), "Trying to construct a UTF-16 string from a StringBuilder that is encoded in something else!");

    // Find the required string buffer size.
    usize code_unit_count = 0;
    for (Block const* current_block = &m_head_block; current_block; current_block = current_block->next)
        code_unit_count += current_block->byte_offset / sizeof(u16);

    // Find the destination string buffer by allocating a heap block if necessary.
    Utf16String string;
    string.m_code_unit_count = code_unit_count + 1;
    u16* destination = string.m_inline_buffer;
    if (string.is_stored_on_heap()) {
        string.m_heap_block = Utf16String::allocate_memory(string.m_code_unit_count);
        string.m_heap_block->reference_count = 1;
        destination = string.m_heap_block->code_units;
    }

    // Copy the contents of the builder to the string destination buffer.
    usize destination_offset = 0;
    for (Block const* current_block = &m_head_block; current_block; current_block = current_block->next) {
        copy_memory(destination + destination_offset, current_block->buffer, current_block->byte_offset);
        destination_offset += current_block->byte_offset / sizeof(u16);
    }
    destination[destination_offset] = '\0';

    return string;
}

void StringBuilder::append_code_point(u32 code_point)
{
    if (m_encoding == Encoding::UTF8) {
        auto encoded = utf8_encode(code_point);
        if (!encoded.has_value())
            PANIC("Trying to append an invalid Unicode code point to a StringBuilder in StringBuilder::append_code_point()!");
        push_bytes(encoded.value().encoded, encoded.value().byte_width);
    } else if (m_encoding == Encoding::UTF16) {
        auto encoded = utf16_encode(code_point);
        if (!encoded.has_value())
            PANIC("Trying to append an invalid Unicode code point to a StringBuilder in StringBuilder::append_code_point()!");
        push_bytes(reinterpret_cast<ROBytes>(encoded.value().code_units), encoded.value().code_unit_count * sizeof(u16));
    } else {
        PANIC("TODO");
    }
}

void StringBuilder::append_code_point_repeated(u32 code_point, usize count)
{
    for (usize i = 0; i < count; i++)
        append_code_point(code_point);
}

void StringBuilder::append_utf8(StringView const& view)
{
    if (m_encoding == Encoding::UTF8) {
        push_bytes(view.bytes(), view.byte_count());
    } else {
        for (u32 code_point : view)
            append_code_point(code_point);
    }
}

void StringBuilder::append_utf16(Utf16View const& view)
{
    if (m_encoding == Encoding::UTF16) {
        push_bytes(view.bytes(), view.byte_count());
    } else {
        for (u32 code_point : view)
            append_code_point(code_point);
    }
}

void StringBuilder::append_repeated(StringView const& view, usize repeat_count)
{
    for (usize i = 0; i < repeat_count; i++)
        append_utf8(view);
}

void StringBuilder::append_repeated(Utf16View const& view, usize repeat_count)
{
    for (usize i = 0; i < repeat_count; i++)
        append_utf16(view);
}

struct NumberToStringResult {
    u32 digit_count { 0 };
    static constexpr u32 max_digit_count = 64;
    u32 digit_code_points[max_digit_count] {};
    u32 padding_count { 0 };
    u32 padding_code_point { 0 };
};

static NumberToStringResult number_to_string(
    u64 value,
    NumberBase number_base,
    Optional<u32> min_digit_count,
    PaddingCharacter padding_character)
{
    NumberToStringResult result;
    if (value == 0)
        result.digit_count = 1;

    u64 temp_value = value;
    while (temp_value > 0) {
        temp_value /= to_underlying(number_base);
        result.digit_count++;
    }

    // clang-format off
    static constexpr u32 digit_code_point_map[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F',
    };
    // clang-format on

    temp_value = value;
    for (usize offset = 1; offset <= result.digit_count; offset++) {
        u32 digit = temp_value % to_underlying(number_base);
        temp_value /= to_underlying(number_base);
        result.digit_code_points[result.digit_count - offset] = digit_code_point_map[digit];
    }

    if (min_digit_count.has_value() && *min_digit_count > result.digit_count) {
        result.padding_count = *min_digit_count - result.digit_count;
        switch (padding_character) {
        case PaddingCharacter::Whitespace:
            result.padding_code_point = ' ';
            break;
        case PaddingCharacter::Zero:
            result.padding_code_point = '0';
            break;
        }
    }

    return result;
}

static void append_sign(StringBuilder& builder, s64 value, PlusSign plus_sign)
{
    if (value < 0)
        builder.append_code_point('-');
    else if (plus_sign == PlusSign::Always)
        builder.append_code_point('+');
    else if (plus_sign == PlusSign::ReplaceWithWhitespace)
        builder.append_code_point(' ');
}

void StringBuilder::append_unsigned(
    s64 value,
    NumberBase number_base,
    Optional<u32> min_digit_count,
    PaddingCharacter padding_character,
    PlusSign plus_sign,
    SignAlignment sign_alignment)
{
    auto number = number_to_string(value, number_base, min_digit_count, padding_character);

    if (sign_alignment == SignAlignment::Left) {
        if (plus_sign == PlusSign::Always)
            append_code_point('+');
        else if (plus_sign == PlusSign::ReplaceWithWhitespace)
            append_code_point(' ');
    }

    append_code_point_repeated(number.padding_code_point, number.padding_count);

    if (sign_alignment == SignAlignment::Right) {
        if (plus_sign == PlusSign::Always)
            append_code_point('+');
        else if (plus_sign == PlusSign::ReplaceWithWhitespace)
            append_code_point(' ');
    }

    for (u32 digit_index = 0; digit_index < number.digit_count; digit_index++)
        append_code_point(number.digit_code_points[digit_index]);
}

void StringBuilder::append_signed(
    s64 value,
    NumberBase number_base,
    Optional<u32> min_digit_count,
    PaddingCharacter padding_character,
    PlusSign plus_sign,
    SignAlignment sign_alignment)
{
    u64 unsigned_value = value >= 0 ? static_cast<u64>(value) : static_cast<u64>(-value);
    auto number = number_to_string(unsigned_value, number_base, min_digit_count, padding_character);

    if (sign_alignment == SignAlignment::Left)
        append_sign(*this, value, plus_sign);

    append_code_point_repeated(number.padding_code_point, number.padding_count);

    if (sign_alignment == SignAlignment::Right)
        append_sign(*this, value, plus_sign);

    for (u32 digit_index = 0; digit_index < number.digit_count; digit_index++)
        append_code_point(number.digit_code_points[digit_index]);
}

void StringBuilder::append_float(f64 value)
{
    // FIXME: This implementation of floating point number formatting is nothing to be
    //        proud of. It has very limited display precision and it is incomplete regarding
    //        NaNs and infinite values (or any kind of special state actually!)

    static constexpr u32 precision_number_of_decimals = 4;
    static constexpr f64 precision = 1e4;

    s64 whole_part = static_cast<s64>(value);
    f64 decimal_part = value - static_cast<f64>(whole_part);
    u64 display_decimal_part = static_cast<u64>(decimal_part * precision);

    append_signed(whole_part);
    append_code_point('.');
    append_unsigned(display_decimal_part, NumberBase::Decimal, precision_number_of_decimals);
}

void StringBuilder::append_newline(LineSeparator line_separator)
{
    switch (line_separator) {
    case LineSeparator::LF:
        append_code_point('\n');
        break;
    case LineSeparator::CRLF:
        append_code_point('\r');
        append_code_point('\n');
    case LineSeparator::CR:
        append_code_point('\r');
        break;
    }
}

void StringBuilder::append_indentation(u32 indentation_level, u32 indentation_size)
{
    u32 whitespace_count = indentation_level * indentation_size;
    append_code_point_repeated(' ', whitespace_count);
}

void StringBuilder::clear()
{
    Block* current_block = m_head_block.next;
    while (current_block != nullptr) {
        Block* next_block = current_block->next;
        StringBuilder::free_memory(current_block);
        current_block = next_block;
    }

    m_head_block.next = nullptr;
    m_tail_block = &m_head_block;
    m_head_block.byte_offset = 0;
    zero_memory(m_head_block.buffer, inline_capacity);
}

StringBuilder::Block* StringBuilder::allocate_memory(usize in_byte_count)
{
    ASSERT(in_byte_count > inline_capacity);
    usize heap_byte_count = in_byte_count - inline_capacity;
    usize allocation_size = sizeof(Block) + heap_byte_count;
    void* memory = ::operator new(allocation_size);

    Block* block = new (memory) Block();
    block->next = nullptr;
    block->byte_count = in_byte_count;
    block->byte_offset = 0;

    return block;
}

void StringBuilder::free_memory(Block* block)
{
    usize heap_byte_count = block->byte_count - inline_capacity;
    MAYBE_UNUSED usize allocation_size = sizeof(Block) + heap_byte_count;
    ::operator delete(block);
}

void StringBuilder::append_block(usize min_byte_count)
{
    usize next_byte_count = m_tail_block->byte_count * growth_factor_numerator / growth_factor_denominator;
    if (next_byte_count < min_byte_count)
        next_byte_count = min_byte_count;

    Block* new_block = StringBuilder::allocate_memory(next_byte_count);
    m_tail_block->next = new_block;
    m_tail_block = new_block;
}

void StringBuilder::push_bytes(ROBytes src_bytes, usize src_byte_count)
{
    usize available = m_tail_block->byte_count - m_tail_block->byte_offset;
    usize copy_size = min(src_byte_count, available);
    copy_memory(m_tail_block->buffer + m_tail_block->byte_offset, src_bytes, copy_size);
    m_tail_block->byte_offset += copy_size;

    if (copy_size < src_byte_count) {
        usize remaining = src_byte_count - copy_size;
        append_block(remaining);

        copy_memory(m_tail_block->buffer, src_bytes + copy_size, remaining);
        m_tail_block->byte_offset += remaining;
    }
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, u8 const& value)
{
    u64 unsigned_value = static_cast<u64>(value);
    builder.append_unsigned(unsigned_value, NumberBase::Decimal);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, u16 const& value)
{
    u64 unsigned_value = static_cast<u64>(value);
    builder.append_unsigned(unsigned_value, NumberBase::Decimal);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, u32 const& value)
{
    u64 unsigned_value = static_cast<u64>(value);
    builder.append_unsigned(unsigned_value, NumberBase::Decimal);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, u64 const& value)
{
    u64 unsigned_value = static_cast<u64>(value);
    builder.append_unsigned(unsigned_value, NumberBase::Decimal);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, s8 const& value)
{
    s64 signed_value = static_cast<s64>(value);
    builder.append_signed(signed_value, NumberBase::Decimal);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, s16 const& value)
{
    s64 signed_value = static_cast<s64>(value);
    builder.append_signed(signed_value, NumberBase::Decimal);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, s32 const& value)
{
    s64 signed_value = static_cast<s64>(value);
    builder.append_signed(signed_value, NumberBase::Decimal);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, s64 const& value)
{
    s64 signed_value = static_cast<s64>(value);
    builder.append_signed(signed_value, NumberBase::Decimal);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, f32 const& value)
{
    f64 float_value = static_cast<f64>(value);
    builder.append_float(float_value);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, f64 const& value)
{
    f64 float_value = static_cast<f64>(value);
    builder.append_float(float_value);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, bool const& value)
{
    if (value)
        builder.append_utf8(VIEW("true"));
    else
        builder.append_utf8(VIEW("false"));
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, void* const& value)
{
    builder.append_utf8(VIEW("0x"));
    builder.append_unsigned(reinterpret_cast<uintptr>(value), NumberBase::Hexadecimal, 2 * sizeof(void*));
}

} // namespace AND
