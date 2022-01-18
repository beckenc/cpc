# cpc - Consumer Provider Challenge

A simple producer consumer scenario in C++.

The program consists of the following components:

* **producer** gathers fixed size chunks from different kind of I/O, prepares the domain specific message frame and transport it via a `zero copy message queue mechanism` to the consumer. The **producer** runs cyclic with a configurable rate triggered by a `boost::asio::deadline_timer` that signals a `std::binary_semaphore`.

* **consumer** dequeue the message and forward it to the domain specific dispatcher for post processing and then pass it to the I/O system to finish.

* **message_queue** zero copy inter-thread communication component based on `std::queue` and `std::counting_semaphore`. API with a blocking dequeue and a non-blocking enqueue method.

        using msg_ptr = std::shared_ptr<msg>;  // use a shared pointer for a zero-copy dequeue mechanism
        [[nodiscard]] auto enqueue(msg_ptr&& payload) -> bool;
        auto dequeue() -> msg_ptr;

* **message_dispatcher** post processing of domain specific data. To be honest, it just simulates some kind of load with sleep and write the message type into the front of the received message.

        auto operator()(auto& frame) -> void  //
        {
            constexpr auto s = "network_frame\0"sv;
            s.copy(frame.data(), s.size());
            std::this_thread::sleep_for(30ms);  // simulate some load        
        }

* **I/O** the low level interface meant to interact with the "hardware". Here `get_data` just delivers characters from 'a' to 'z'.

* **main routine** parse the commandline, set up the signal handler and the program runtime using `boost.asio`. Lot of glue code to set up the provider and the consumer as well as their dependencies. Should definitly be reworked to some kind of component setup / initialization module.

## Source code organization

    $ tree
    .
    ├── doc
    │   └── Challenge.md
    ├── lib
    │   └── io              // i/o interface (stubs)
    ├── src
    │   ├── cpc             // consumer-producer message handling
    │   ├── consumer        // consumer module
    │   ├── producer        // producer module
    │   └── utils           // generic clases
    └── test                // unit tests
        └── utils

### How to build
    $ mkdir build
    $ cd build
    $ cmake ..
    $ cmake --build .

### Run the application
    $ ./src/cpc -h
    Allowed options:
      -h [ --help ]                 produce help message
      -d [ --domain ] arg (=video)  set the data processing domain, i.e. video, 
                                    audio, hw, network
      -t [ --throughput ] arg (=10) set the data processing rate between 10 and 
                                    1000 times per second
      -r [ --runtime ] arg (=10)    set the max program runtime to at least seconds

### Run unit tests
    $ ctest

## Used techiques and Performance consideratons

### Inter-Thread-Communication
- `std::queue`
    FIFO with an underlying double-ended queue that offer the best performance compared to std::list / std::vector.
- `std::shared_ptr<std::array<char, 16*1024*1024>`
    The message type, is a shared pointer with an 16MB fixed size array inside.
    Its livetime starts in the **producer** and ends after dispatching in the **consumer**. We just pass the `shared_ptr` through the system which is very lightweight(zero-copy of the 16MB real payload).
- `std::variant`
    Domain (video, audio, hw, network) specific message type

### Threading and Synchronization Primitives
- `std::thread`
    Run the consumer and producer in parallel. `std::async` would work for the producer as well.
- `std::counting_semaphore`
    Lightweight synchronization primitive to sync consumer and producer queue operations.
- `std::binary_semaphore`
    Signal the producer to start gathering data form the hardware. Efficency optimized version of a `std::counting_semaphore`

### Async operations
- `boost::asio::deadline_timer`
    - Terminate the program after a configurable time
    - Release the producers `binary_semaphore`. Compared to the simple aproach with `std::this_thread::sleep_for` the producer gathers data in fixed cycles.
- `boost::asio::signal_set`
    Signal set registered for process termination (Ctrl+C)

## TODO
- Eliminate map lookup and `std::string` operations in the sending path to save some cpu cycles
- Think about a throttle mechanism for the producer in case of overload situations of the consumer
- Rework the system start and do not do everything in the main routine. Some kind of module setup / initialization
- Unit tests for message dispatching
- Fix linter warnings

## License

Boost Software License.

## Dependencies

- boost Libraries (asio, program_options, test)