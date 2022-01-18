/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#include "producer/runnable.hpp"

#include <array>

#include "io/io.hpp"



namespace producer
{

runnable::runnable(io_context& ioc, mq_t& q, get_data_t gd, tick_t t)  //
    : queue{q}, get_data{gd}, expiry_time{t}, timer{ioc, t}
{
    timer.async_wait([this](const auto& ec) { tick(ec); });
}

auto runnable::operator()() -> void
{
    using namespace std::chrono_literals;

    // wait for a signal from the tick by attempting to decrement the semaphore
    tick_sync.acquire();

    // copy io data into the transport frame
    // and move it to the queue
    auto frame_ptr = std::make_unique<cpc::frame>();
    get_data(*frame_ptr);

    if (!queue.enqueue(std::move(frame_ptr)))
    {
        // TODO
        // Think about a throttle mechanism to not overload the consumer
        throw std::runtime_error("Overload! failed to enqueue");
    }
}

auto runnable::abort() -> void  //
{
    tick_sync.release();
}

void runnable::tick(const boost::system::error_code& ec)
{
    if (!ec)
    {
        // trigger runner to start work,
        tick_sync.release();

        // Reschedule the timer
        timer.expires_at(timer.expires_at() + expiry_time);
        timer.async_wait([this](const auto& ec) { tick(ec); });
    }
    else
    {
        // shutdown sequence
        tick_sync.release();
    }
}
}  // namespace producer
