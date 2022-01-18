add_library(utils STATIC thread_runner.cpp)
target_link_libraries(utils LINK_PRIVATE pthread)