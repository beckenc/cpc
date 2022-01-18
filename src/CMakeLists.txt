find_package( Boost 1.77 COMPONENTS program_options REQUIRED )

add_subdirectory(utils)
add_subdirectory(consumer)
add_subdirectory(producer)

add_executable(cpc main.cpp)

target_link_libraries( cpc LINK_PUBLIC ${Boost_LIBRARIES} pthread utils producer consumer io)