find_package(Boost 1.77 COMPONENTS unit_test_framework REQUIRED)

# indicates the location of the boost installation tree.
# hard-coded for our simple example.
set(BOOST_INCLUDE_DIRS $boost_installation_prefix/include)

add_subdirectory(utils)