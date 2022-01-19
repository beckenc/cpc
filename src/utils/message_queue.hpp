/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <semaphore>
#include <tuple>

namespace utils
{

template <typename T, size_t depth>
class message_queue
{
   public:
    using msg     = T;
    using msg_ptr = std::shared_ptr<msg>;  // use a shared pointer for a zero-copy dequeue mechanism
    using fifo_t  = std::queue<msg_ptr>;

    message_queue() = default;

    message_queue(const message_queue& other) = delete;
    auto operator=(const message_queue& rhs) -> message_queue& = delete;

    message_queue(message_queue&& rhs) noexcept = delete;
    auto operator=(message_queue&& rhs) noexcept -> message_queue& = delete;

    ~message_queue() = default;

    /** enqueue the payload and signal the consumer
     * @return false if there is no space left in the queue
     */
    [[nodiscard]] auto enqueue(msg_ptr&& payload) -> bool;

    /** dequeue and signal the producer available slots
     * return nullopt if there is nothing more to dequeue
     */
    auto dequeue() -> msg_ptr;

    /** abort and return to the caller from ::dequeue() immediately
     */
    auto abort_queue() -> void;

   private:
    std::counting_semaphore<depth> occupied_slots{0};
    std::counting_semaphore<depth> available_slots{depth};
    std::mutex                     operation;
    fifo_t                         fifo;
};

template <typename T, size_t depth>
inline auto message_queue<T, depth>::enqueue(msg_ptr&& payload) -> bool
{
    if (!available_slots.try_acquire())
    {
        return false;
    }

    {
        auto guard = std::lock_guard<std::mutex>{operation};
        fifo.emplace(std::forward<msg_ptr>(payload));
    }

    occupied_slots.release();
    return true;
}

template <typename T, size_t depth>
inline auto message_queue<T, depth>::dequeue() -> msg_ptr
{
    occupied_slots.acquire();  // blocking, till message arrives

    auto msg = fifo.front();
    if (!msg)
    {  // noting in the queue, must be the stop sequence
        return msg;
    }

    {
        auto g = std::lock_guard<std::mutex>{operation};
        fifo.pop();
    }

    available_slots.release();
    return msg;
}

template <typename T, size_t depth>
inline auto message_queue<T, depth>::abort_queue() -> void
{
    occupied_slots.release();
}

}  // namespace utils
