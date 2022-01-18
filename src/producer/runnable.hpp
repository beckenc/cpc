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
    using get_data_t = std::function<void(cpc::frame& output)>;

    /** construct a producer runnable
     * @param ioc   io context used to make the heartbeat of the runner using a deadline_timer
     * @param q     message queue, the runners synchronization point 
     * @param gd    i/o interface for gathering data chunks from the hardware
     * @param t     the rate of the heartbeate. This is the cycle with that we talk to the hardware
     */
    runnable(io_context& ioc, mq_t& q, get_data_t gd, tick_t t);

    /** Wait for a tick and than start gathering data. 
     * 
     * Gather raw data from the hardware and make domain specific message frames out of it.
     * Link the messates to the message queue.
     */
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

