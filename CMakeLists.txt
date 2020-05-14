cmake_minimum_required(VERSION 3.10)
project(streamlink)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set( CMAKE_VERBOSE_MAKEFILE on )

include(CheckFileOffsetBits)
CHECK_FILE_OFFSET_BITS()
# fuse defines
ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)

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
        src/BufferPool.cpp export/BufferPool.h
        src/BufferProducer.cpp
        src/BufferPool.cpp
        export/BufferProducer.h
        src/BufferConsumer.cpp
        src/ByteBufferPool.cpp
        include/VirtualFSProvider.cpp
        include/VirtualFSProvider.h
        src/fuse/IFuse.cpp
        include/fuse/IFuse.h
        export/BufferConsumer.h
        src/PluginCbImpl.cpp
        include/PluginCbImpl.h

        )

find_package (glog)

add_subdirectory(tests)
# set(Boost_DEBUG 1)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME ON)

find_package(Boost REQUIRED COMPONENTS thread filesystem )

find_package(FUSE REQUIRED)
find_package(CURL REQUIRED)

include_directories(${CURL_INCLUDE_DIR})
include_directories( ${Boost_INCLUDE_DIRS} )


fetch_googletest(
        ${PROJECT_SOURCE_DIR}/cmake
        ${PROJECT_BINARY_DIR}/googletest
)

add_library(sampleplugin SHARED
        sample/SamplePlugin.cpp
        export/FileInterface.h)

add_dependencies(streamlink sampleplugin)

target_link_libraries (sampleplugin
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${CURL_LIBRARIES}
        glog)

target_link_libraries (streamlink
        PUBLIC
        dl
        ${CMAKE_THREAD_LIBS_INIT}
        ${FUSE_LIBRARIES}
        ${Boost_LIBRARIES}
        glog)


add_executable(streamfs main.cpp
        export/MessageQueue.h
        src/PluginService.cpp
        export/PluginService.h
        )

target_link_libraries (streamfs
        streamlink
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${FUSE_LIBRARIES}
        streamlink
        )
