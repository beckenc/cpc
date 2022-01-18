/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <boost/asio/deadline_timer.hpp>
#include <functional>
#include <iostream>
#include <span>

#include "cpc/message_queue.hpp"

namespace producer
{
class runnable
{
   public:
    using io_context = boost::asio::io_context;
    using tick_t     = boost::posix_time::milliseconds;
    using mq_t       = cpc::message_queue;
    using get_data_t = std::function<void(std::span<char, cpc::frame_size> const& output)>;

    runnable(io_context& ioc, mq_t& q, get_data_t gd, tick_t t);

    auto operator()() -> void;
    auto abort() -> void;
 
   private:
    void tick(const boost::system::error_code& ec);

    mq_t&                       queue;
    get_data_t                  get_data;
    tick_t                      expiry_time;
    boost::asio::deadline_timer timer;
    std::binary_semaphore       tick_sync{1};
};
}  // namespace producer
