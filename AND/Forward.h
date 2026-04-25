/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Types.h>

namespace AND {

template<typename>
class Optional;

template<typename T, usize inline_capacity>
requires(!is_rvalue_reference<T>)
class Vector;

class StringBuilder;
class Utf16CodePointIterator;
class Utf16ReverseCodePointIterator;
class Utf16String;
class Utf16View;
class Utf8CodepointIterator;
class Utf8ReverseCodePointIterator;
class String;
class StringView;

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::Optional;
using AND::String;
using AND::StringBuilder;
using AND::StringView;
using AND::Utf16CodePointIterator;
using AND::Utf16ReverseCodePointIterator;
using AND::Utf16String;
using AND::Utf16View;
using AND::Utf8CodepointIterator;
using AND::Utf8ReverseCodePointIterator;
using AND::Vector;
#endif
