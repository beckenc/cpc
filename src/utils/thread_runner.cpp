/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#include "utils/thread_runner.hpp"

#include <iostream>

namespace utils
{

thread_runner::~thread_runner() { shutdown(); }

thread_runner::thread_runner(std::string name, runable_t fn) : name{std::move(name)}, runable{fn} {}

void thread_runner::shutdown()
{
    running.store(false);
    if (thread.joinable())
    {
        std::cout << "[" << name << "] Shutdown\n";
        thread.join();
    }
}

bool thread_runner::run()
{
    std::cout << "[" << name << "] Starting up\n";
    try
    {
        running.store(true);
        thread = std::thread(&thread_runner::run_fn, this);
    }
    catch (...)
    {
        std::cerr << "[" << name << "] Failed to create\n";
        return false;
    }

    return true;
}

void thread_runner::run_fn()
{
    std::cout << "[" << name << "] Running\n";
    while (running.load())
    {
        try
        {
            runable();
        }
        catch (std::runtime_error& e)
        {
            std::cerr << "[" << name << "] " << e.what() << "\n";
        }
        catch (...)
        {
            std::cerr << "[" << name << "] Something unforseen happened.\n";
        }
    }
}
}  // namespace utils