cmake_minimum_required(VERSION 3.14)
project(streamlink)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package (Threads)

include(cmake/googletest.cmake)

include_directories(
        include
        export
        logging/src
        )


add_library(streamlink SHARED
        src/StreamPluginManager.cpp
        src/PluginManagerConfig.cpp
        src/BufferQueue.cpp
        src/PluginImpl.cpp
        src/StreamPluginManager.cpp
        src/StreamManager.cpp
        src/Stream.cpp
        src/MemAllocator.cpp include/MemAllocator.h
        src/ActiveBuffer.cpp include/ActiveBuffer.h
        src/BufferPool.cpp include/BufferPool.h
        src/BufferProducer.cpp
        src/BufferPool.cpp
        export/BufferProducer.h
        src/BufferConsumer.cpp
        src/ByteBufferPool.cpp
        include/BufferConsumer.h)

find_package (glog)

add_subdirectory(tests)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.71.0 COMPONENTS core)

fetch_googletest(
        ${PROJECT_SOURCE_DIR}/cmake
        ${PROJECT_BINARY_DIR}/googletest
)

add_library(sampleplugin SHARED
        sample/SamplePlugin.cpp
        tests/test_main.cpp tests/test_main.h src/ByteBufferPool.cpp export/ByteBufferPool.h export/MessageQueue.h export/PluginConfig.h include/PluginService.cpp include/PluginService.h export/ServiceRequest.h)

add_dependencies(streamlink sampleplugin)
target_link_libraries (sampleplugin
        ${CMAKE_THREAD_LIBS_INIT}
        glog)

target_link_libraries (streamlink
        PUBLIC
        dl
        ${CMAKE_THREAD_LIBS_INIT}
        glog)

target_link_libraries (streamlink
        ${Boost_LIBRARIES})

add_executable(streamfs main.cpp export/MessageQueue.h include/PluginService.cpp include/PluginService.h)
#rget_link_libraries(streamfs streamlink)

target_link_libraries (streamfs streamlink)
