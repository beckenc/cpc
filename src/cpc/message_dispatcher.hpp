/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <chrono>
#include <span>
#include <string_view>

#include "cpc/message_queue.hpp"

namespace cpc
{
using namespace std::literals;

template <typename T = cpc::frame>
struct message_dispatcher
{
    auto operator()(auto&)  //
    {
        throw std::runtime_error("unknown message arrived");
    }
};

template <>
struct message_dispatcher<cpc::frame>
{
    auto operator()(auto& frame)
    {
        // dispatch the concrete type. video, audio, hw or network
        return std::visit(
            [](auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                message_dispatcher<T>{}(arg);
                return std::span<char, cpc::frame_size>{arg};
            },
            frame);
    }
};

template <>
struct message_dispatcher<video_frame>
{
    auto operator()(auto& frame)  //
    {
        constexpr auto s = "video_frame\0"sv;
        s.copy(frame.data(), s.size());
        std::this_thread::sleep_for(10ms); // simulate some load
    }
};

template <>
struct message_dispatcher<audio_frame>
{
    auto operator()(auto& frame)  //
    {
        constexpr auto s = "audio_frame\0"sv;
        s.copy(frame.data(), s.size());
        std::this_thread::sleep_for(20ms); // simulate some load
    }
};

template <>
struct message_dispatcher<hw_frame>
{
   public:
    auto operator()(auto& frame)  //
    {
        constexpr auto s = "hw_frame\0"sv;
        s.copy(frame.data(), s.size());
    }
};

template <>
struct message_dispatcher<network_frame>
{
   public:
    auto operator()(auto& frame)  //
    {
        constexpr auto s = "network_frame\0"sv;
        s.copy(frame.data(), s.size());
        std::this_thread::sleep_for(30ms);  // simulate some load
    }
};

}  // namespace cpc
