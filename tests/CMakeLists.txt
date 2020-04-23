find_package(Boost 1.71.0 COMPONENTS core)

add_executable(
        unit_tests
        test_main.cpp
)

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