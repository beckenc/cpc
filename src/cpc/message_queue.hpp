/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <variant>

#include "io/io.hpp"
#include "utils/message_queue.hpp"

namespace cpc
{
constexpr size_t frame_size = io::frame_size;
constexpr size_t queue_size = 10;

struct raw_frame : public std::array<char, frame_size>
{
    raw_frame()                       = default;
    raw_frame(const raw_frame& other) = delete;
    raw_frame& operator=(const raw_frame& rhs) = delete;
    raw_frame(raw_frame&& rhs) noexcept        = delete;
    raw_frame& operator=(raw_frame&& rhs) noexcept = delete;
    ~raw_frame()                                   = default;
};

struct video_frame : public raw_frame
{
};

struct audio_frame : public raw_frame
{
};

struct hw_frame : public raw_frame
{
};

struct network_frame : public raw_frame
{
};

using frame         = std::variant<raw_frame, video_frame, hw_frame, audio_frame, network_frame>;
using message_queue = utils::message_queue<frame, queue_size>;
}  // namespace cpc
