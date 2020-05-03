find_package(Boost 1.71.0 COMPONENTS core)

add_executable(
        unit_tests
        test_main.cpp
)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.71.0 COMPONENTS core filesystem thread)

SET(CMAKE_CXX_FLAGS_DEBUG "-O0")
SET(CMAKE_C_FLAGS_DEBUG "-O0")

target_link_libraries(
        unit_tests
        gtest_main
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