/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <stdexcept>
#include <thread>

namespace utils
{

class thread_runner
{
   public:
    using runable_t  = std::function<void()>;
    using runabort_t = std::function<void()>;

    thread_runner() = delete;

    thread_runner(const thread_runner& other) = delete;
    auto operator=(const thread_runner& rhs) -> thread_runner& = delete;

    thread_runner(thread_runner&& rhs) noexcept = delete;
    auto operator=(thread_runner&& rhs) noexcept -> thread_runner& = delete;

    ~thread_runner();

    thread_runner(std::string name, runable_t run, runabort_t abort);

    auto run() -> bool;
    auto shutdown() -> void;

   private:
    void run_fn();

    std::thread      thread;
    std::atomic_bool running;
    std::string      name;
    runable_t        runable;
    runabort_t       runabort;
};
}  // namespace utils
