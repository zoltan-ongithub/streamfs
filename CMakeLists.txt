cmake_minimum_required(VERSION 3.14)
project(streamfs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


include_directories(
        include
        export
        logging/src
        )


add_executable(streamfs
        main.cpp
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
        include/BufferConsumer.h)

find_package (glog)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.71.0 COMPONENTS core)


add_library(sampleplugin SHARED
        sample/SamplePlugin.cpp
        )

add_dependencies(streamfs sampleplugin)
target_link_libraries (sampleplugin glog)
target_link_libraries (streamfs
        PUBLIC
        dl
        glog)

target_link_libraries (streamfs
        INTERFACE
        Boost::Core)
