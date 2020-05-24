add_executable(
        unit_tests
        test_main.cpp
)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS filesystem thread)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# SET(CMAKE_CXX_FLAGS_DEBUG "-O0")
# SET(CMAKE_C_FLAGS_DEBUG "-O0")

target_link_libraries(
        unit_tests
        gtest_main
        gtest
        streamlink
)

include_directories(
        include
        export
        logging/src
)

add_test(
        NAME
        unit
        COMMAND
        ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
)
