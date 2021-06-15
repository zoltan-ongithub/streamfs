//
// Created by Zoltan Kuscsik on 4/17/20.
//

#pragma once

#include <array>
#include <boost/static_assert.hpp>

// Plugin location directory
#define STREAMFS_PLUGIN_DIRECTORY "plugins/"
#define TS_PACKAGE_SIZE 188

// Buffer chunk sizes
#define BUFFER_CHUNK_SIZE (8 * 6 * TS_PACKAGE_SIZE) /* 1k chunks */

BOOST_STATIC_ASSERT( BUFFER_CHUNK_SIZE % TS_PACKAGE_SIZE == 0);

// Max number of buffer chunks in a list
#define MAX_BUFFER_LIST_COUNT 256

// Buffer read timeout in milliseconds
#define BUFFER_POOL_READ_TIMEOUT_MS 2000

using buffer_chunk  = std::array<unsigned char, BUFFER_CHUNK_SIZE>;
