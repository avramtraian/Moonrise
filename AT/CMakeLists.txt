# Copyright (c) 2024 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

set(AT_SOURCE_FILES
    Assertion.cpp
    Assertion.h
    CoreDefines.h
    CoreTypes.h
    DistinctNumeric.h
    Error.cpp
    Error.h
    Format.cpp
    Format.h
    HashMap.h
    HashTable.h
    MemoryOperations.cpp
    MemoryOperations.h
    Optional.h
    ScopedValueRollback.h
    Span.h
    String.cpp
    String.h
    StringView.cpp
    StringView.h
    TypeTraits.h
    Utf8.cpp
    Utf8.h
    Vector.h
)

add_library(AT-Framework SHARED ${AT_SOURCE_FILES})

target_include_directories(AT-Framework PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_definitions(AT-Framework PRIVATE "AT_EXPORT_API=1")