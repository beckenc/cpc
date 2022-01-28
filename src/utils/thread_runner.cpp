/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#include "utils/thread_runner.hpp"

#include <iostream>
#include <utility>

namespace utils
{

thread_runner::thread_runner(std::string name, runable_t run, runabort_t abort)  //
    : name{std::move(name)}, runable{std::move(run)}, runabort(std::move(abort))
{
}

auto thread_runner::run() -> bool
{
    std::cout << "[" << name << "] Starting up\n";
    try
    {
        thread = std::jthread{[this](auto&& p) { this->run_fn(std::forward<decltype(p)>(p)); }};
    }
    catch (...)
    {
        std::cerr << "[" << name << "] Failed to create\n";
        return false;
    }

    return true;
}

void thread_runner::run_fn(const std::stop_token& stop_token)
{
    std::cout << "[" << name << "] Running\n";

    // Register a stop callback on the worker thread.
    std::stop_callback callback(stop_token, runabort);

    while (!stop_token.stop_requested())
    {
        try
        {
            runable();
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << "[" << name << "] " << e.what() << "\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << "[" << name << "] Something unforseen happened (" << e.what() << ")\n";
        }
    }

    std::cout << "[" << name << "] Shutdown\n";
}
}  // namespace utils
