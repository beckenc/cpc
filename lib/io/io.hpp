/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <cstddef>
#include <span>

namespace io
{

constexpr size_t frame_size = 16 * 1024 * 1024;
void             get_data(std::span<char, frame_size> const& output);
void             send_data(std::span<const char, frame_size> const& output);
void             print_statistics();
}  // namespace io
