cmake_minimum_required(VERSION 3.14)
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
        src/BufferPool.cpp include/BufferPool.h
        src/BufferProducer.cpp
        src/BufferPool.cpp
        export/BufferProducer.h
        src/BufferConsumer.cpp
        src/ByteBufferPool.cpp
        include/VirtualFSProvider.cpp
        include/VirtualFSProvider.h
        src/fuse/IFuse.cpp
        include/fuse/IFuse.h
        include/BufferConsumer.h src/PluginCbImpl.cpp include/PluginCbImpl.h)

find_package (glog)

add_subdirectory(tests)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.45.0 COMPONENTS core thread filesystem)

find_package(FUSE REQUIRED)

fetch_googletest(
        ${PROJECT_SOURCE_DIR}/cmake
        ${PROJECT_BINARY_DIR}/googletest
)

add_library(sampleplugin SHARED
        sample/SamplePlugin.cpp
        export/FileInterface.h)

add_dependencies(streamlink sampleplugin)
target_link_libraries (sampleplugin
        ${CMAKE_THREAD_LIBS_INIT}
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
        include/PluginService.h
        )

target_link_libraries (streamfs
         streamlink
        )
