/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once
#include <iostream>
#include <vector>

#include "cpc/message_queue.hpp"

namespace consumer
{
class runnable
{
   public:
    using mq_t        = cpc::message_queue;
    using send_data_t = std::function<void(std::span<const char, cpc::frame_size> const& output)>;

    runnable(mq_t& q, send_data_t sd);

    auto operator()() -> void;
    auto abort() -> void;

   private:
    mq_t&       queue;
    send_data_t send_data;
};
}  // namespace consumer
