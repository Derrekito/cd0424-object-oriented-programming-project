cmake_minimum_required(VERSION 3.10)
project(monitor)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(monitor ${SOURCES})

set_property(TARGET monitor PROPERTY CXX_STANDARD 17)
target_link_libraries(monitor ${CURSES_LIBRARIES})
# TODO: Run -Werror in CI.
target_compile_options(monitor PRIVATE -Wall -Wextra)

# Unit tests: each test target lists only the sources it needs, so tests
# build and run even while the full monitor target doesn't compile yet.
enable_testing()
find_package(GTest REQUIRED)

add_executable(format_test tests/format_test.cpp src/format.cpp)
set_property(TARGET format_test PROPERTY CXX_STANDARD 17)
target_compile_options(format_test PRIVATE -Wall -Wextra)
target_link_libraries(format_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(format_test)
