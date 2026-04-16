/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#define AND_MAKE_NONCOPYABLE(type_name)   \
    type_name(type_name const&) = delete; \
    type_name& operator=(type_name const&) = delete;

#define AND_MAKE_NONMOVABLE(type_name)        \
    type_name(type_name&&) noexcept = delete; \
    type_name& operator=(type_name&&) noexcept = delete;
