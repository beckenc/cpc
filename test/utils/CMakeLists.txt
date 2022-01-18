# creates the executable
add_executable(utils_test utils.test.cpp message_queue.test.cpp thread_runner.test.cpp)
# indicates the include paths
target_include_directories(utils_test PRIVATE ${Boost_INCLUDE_DIRS})
# indicates the shared library variant
target_compile_definitions(utils_test PRIVATE "BOOST_TEST_DYN_LINK=1")
# indicates the link paths
target_link_libraries(utils_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} utils)

# declares a test with our executable
add_test(NAME utils_test COMMAND utils_test)
