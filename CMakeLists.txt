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

set(PUBLIC_HEADERS
    export/BufferConsumer.h
    export/BufferPool.h
    export/BufferProducer.h
    export/ByteBufferPool.h
    export/config.h
    export/FileInterface.h
    export/MessageQueue.h
    export/PluginCallbackInterface.h
    export/PluginConfig.h
    export/PluginInterface.h
    export/PluginService.h
    export/ServiceRequest.h
)

include_directories(
        include
        export
        logging/src
        )

add_library(buqu SHARED
        src/BufferPool.cpp export/BufferPool.h
        src/BufferProducer.cpp
        src/BufferPool.cpp
        export/BufferProducer.h
        src/BufferConsumer.cpp
        src/ByteBufferPool.cpp
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


add_library(sampleplugin SHARED
        sample/SamplePlugin.cpp
        export/FileInterface.h)

add_dependencies(streamlink sampleplugin)

target_link_libraries (sampleplugin
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${CURL_LIBRARIES}
        glog)

target_link_libraries(buqu
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${CURL_LIBRARIES}
        glog
        )

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


set_target_properties(buqu PROPERTIES PUBLIC_HEADER  "${PUBLIC_HEADERS}")

INSTALL(TARGETS buqu
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/streamfs
)

INSTALL(TARGETS streamlink
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/streamfs
)

install (TARGETS streamfs
         RUNTIME DESTINATION bin)

