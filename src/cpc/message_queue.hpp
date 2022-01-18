/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include "utils/message_queue.hpp"
#include "io/io.hpp"

namespace cpc
{
  constexpr size_t frame_size = io::frame_size;
  constexpr size_t queue_size = 10;
  using frame = std::array<char, frame_size>;
  using message_queue = utils::message_queue<frame, queue_size>;
}