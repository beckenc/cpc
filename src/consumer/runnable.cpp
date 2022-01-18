/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#include "consumer/runnable.hpp"

namespace consumer
{
runnable::runnable(mq_t& q, send_data_t sd) : queue{q}, send_data{sd} {}

auto runnable::operator()() -> void
{
    if (auto msg = queue.dequeue(); msg)
    {
        // to something cracy
        send_data(*msg);
    }
}

auto runnable::abort() -> void { queue.abort_queue(); }

}  // namespace consumer