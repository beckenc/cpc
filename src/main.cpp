/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#include <array>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <queue>
#include <string>

#include "consumer/runnable.hpp"
#include "cpc/message_dispatcher.hpp"
#include "cpc/message_queue.hpp"
#include "io/io.hpp"
#include "producer/runnable.hpp"
#include "utils/thread_runner.hpp"

namespace po = boost::program_options;

static constexpr int min_runtime    = 10;
static constexpr int min_throughput = 10;
static constexpr int max_throughput = 1000;

/**
 * Validate command line arguments
 */
static void check_args(const po::variables_map& vm)
{
    if (vm.count("domain"))
    {
        if (auto d = vm["domain"].as<std::string>(); d != "video" && d != "audio" && d != "hw" && d != "network")  //
            throw std::out_of_range("--domain argument is invalid");
        std::cout << "[args] Data processing domain was set to " << vm["domain"].as<std::string>() << "\n";
    }
    if (vm.count("throughput"))
    {
        if (auto tp = vm["throughput"].as<int>(); tp < min_throughput || tp > max_throughput)  //
            throw std::out_of_range("--throuput argument is out of range");
        std::cout << "[args] Data processing rate was set to " << vm["throughput"].as<int>() << " times per second\n";
    }
    if (vm.count("runtime"))
    {
        if (auto rt = vm["runtime"].as<int>(); rt < min_runtime)  //
            throw std::out_of_range("--runtime argument is out of range");
        std::cout << "[args] Program runtime was set to " << vm["runtime"].as<int>() << " seconds\n";
    }
}

auto main(int argc, char* argv[]) -> int
{
    //
    // 0) Command line Parsing
    //    program arguments variable map
    auto vm = po::variables_map{};

    try
    {
        auto ioc = boost::asio::io_context{};

        //
        // 1) Command line Parsing
        //
        auto desc = po::options_description{"Allowed options"};
        auto opt  = desc.add_options();
        opt("help,h",  //
            "produce help message");
        opt("domain,d", po::value<std::string>()->default_value("video"),  //
            "set the data processing domain, i.e. video, audio, hw, network");
        opt("throughput,t", po::value<int>()->default_value(min_throughput),  //
            "set the data processing rate between 10 and 1000 times per second");
        opt("runtime,r", po::value<int>()->default_value(min_runtime),  //
            "set the max program runtime to at least seconds");

        // Parse the command line
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return 1;
        }
        check_args(vm);

        //
        // 2) Signal Handling
        //    Construct a signal set registered for process termination. Wait for signals to occur.
        //
        auto signals = boost::asio::signal_set{ioc, SIGINT, SIGTERM};
        signals.async_wait(
            [&ioc](const boost::system::error_code&, int)
            {
                std::cout << "[asio] Stop IO context and wait for completion\n";
                ioc.stop();
            });

        //
        // 3) Program runtime observation
        //    Construct and start a deadline timer waiting for program timeout
        //
        auto dt = boost::asio::deadline_timer{ioc, boost::posix_time::seconds{vm["runtime"].as<int>()}};
        dt.async_wait([&ioc](const boost::system::error_code&) { std::raise(SIGTERM); });

        //
        // 4) Consumer / Producer
        //    start producing / consuming domain specific
        //
        auto cp_queue          = cpc::message_queue{};
        auto consumer_runnable = consumer::runnable{cp_queue, io::send_data, [](cpc::frame& frame) { cpc::message_dispatcher{}(frame); }};
        auto consumer          = utils::thread_runner{"consumer",                                       //
                                             [&consumer_runnable]() { consumer_runnable(); },  //
                                             [&consumer_runnable]() { consumer_runnable.abort(); }};

        auto producer_runnable = producer::runnable{ioc, cp_queue,
                                                    [&vm](cpc::frame& frame)
                                                    {
                                                        // TODO
                                                        // Avoid map lookup and std::string operations in the
                                                        // sending path to save some cpu cycles
                                                        if (vm["domain"].as<std::string>() == "video")
                                                            frame.emplace<cpc::video_frame>();
                                                        else if (vm["domain"].as<std::string>() == "audio")
                                                            frame.emplace<cpc::audio_frame>();
                                                        else if (vm["domain"].as<std::string>() == "hw")
                                                            frame.emplace<cpc::hw_frame>();
                                                        else if (vm["domain"].as<std::string>() == "network")
                                                            frame.emplace<cpc::network_frame>();
                                                        std::visit([](auto& raw_msg) { io::get_data(raw_msg); }, frame);
                                                    },
                                                    producer::runnable::tick_t{1000 / vm["throughput"].as<int>()}};
        auto producer          = utils::thread_runner{"producer",                                       //
                                             [&producer_runnable]() { producer_runnable(); },  //
                                             [&producer_runnable]() { producer_runnable.abort(); }};
        //
        // 5) start async event processing
        //
        consumer.run();
        producer.run();
        ioc.run();

        io::print_statistics();
    }
    catch (const std::exception& error)
    {
        std::cerr << "[main] Returned from io context. " << error.what() << "\n";
    }
    return 0;
}
