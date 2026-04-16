#
# Copyright (c) 2024-2026 Trian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.
#

set(AND_SOURCE_FILES
    ArrayIterator.h
    Assertions.cpp
    Assertions.h
    ByteBuffer.h
    CompareResult.h
    Defines.h
    Endian.h
    Forward.h
    Function.h
    Hash.h
    HashMap.h
    HashSet.h
    IterationDecision.h
    Log.cpp
    Log.h
    MatchResult.h
    MathUtilities.h
    MemoryOperations.cpp
    MemoryOperations.h
    Noncopyable.h
    NumberBase.h
    NumericLimits.h
    OpenAddressingBucketState.h
    OpenAddressingIterator.h
    Optional.h
    OwnPtr.h
    Platform.h
    PlatformWindows.cpp
    RefCounted.h
    RefPtr.h
    Span.h
    StringBuilder.cpp
    StringBuilder.h
    Types.h
    UnicodeCodePoint.cpp
    UnicodeCodePoint.h
    Utf16.cpp
    Utf16.h
    Utf16CodepointIterator.cpp
    Utf16CodepointIterator.h
    Utf16String.cpp
    Utf16String.h
    Utf16View.cpp
    Utf16View.h
    Utf8.cpp
    Utf8.h
    Utf8CodepointIterator.cpp
    Utf8CodepointIterator.h
    Utf8String.cpp
    Utf8String.h
    Utf8View.cpp
    Utf8View.h
    Vector.h
)

add_library(AND-Framework STATIC ${AND_SOURCE_FILES})
target_include_directories(AND-Framework PUBLIC ${CMAKE_SOURCE_DIR})

if (WIN32)
    # Libraries required to query the call stack.
    target_link_libraries(AND-Framework PRIVATE ntdll.lib)
    target_link_libraries(AND-Framework PRIVATE dbghelp.lib)
endif ()
