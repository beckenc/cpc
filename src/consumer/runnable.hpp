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
    using mq_t         = cpc::message_queue;
    using send_data_t  = std::function<void(std::span<const char, cpc::frame_size> const& output)>;
    using dispatcher_t = std::function<std::span<const char, cpc::frame_size>(cpc::frame& output)>;

    /** construct a consumer runnable
     * @param q     message queue, the runners synchronization point
     * @param sd    i/o interface for sending post processed data back to the hardware
     * @param dp    message dispatcher hook
     */
    runnable(mq_t& q, send_data_t sd, dispatcher_t dp);

    /** Wait for message arrival on the queue. 
     * 
     * Forward received message to the domain specifc 
     * dispatcher and than send it back to the
     * hardware
     */
    auto operator()() -> void;
    auto abort() -> void;

   private:
    mq_t&        queue;
    send_data_t  send_data;
    dispatcher_t dispatcher;
};
}  // namespace consumer
