//
// Created by Zoltan Kuscsik on 4/20/20.
//

#include <iostream>
#include <glog/logging.h>
#include "BufferPool.h"
#include <boost/bind.hpp>
#include <cstring>
#include <algorithm>    // std::max



template <>
size_t BufferPool<buffer_chunk>::read(
        char* bufferChunks,
        size_t length,
        uint64_t offset,
        size_t left_padding,
        size_t right_padding)
{
    boost::mutex::scoped_lock lock(m_mutex);
    auto capacity = mCircBuf.capacity();

    auto readEnd = offset + length;

    if ( (mGotLastBuffer &&  offset >= mTotalBufCount)  ) {

        // producer closed the data stream.
        return 0;
    }

    if (left_padding >= BUFFER_CHUNK_SIZE ||
        right_padding >= BUFFER_CHUNK_SIZE ||
        (length - left_padding - right_padding) < 0 ) {

        return 0;
    }

    if ( (mTotalBufCount - offset)  > mCircBuf.capacity()
            || offset > mTotalBufCount
            || length > capacity
       ) {
        return 0;
    }

    if (readEnd > mTotalBufCount && !mGotLastBuffer) {
        mReadEnd = offset + length;
        mNotEnoughBytes.wait(lock, boost::bind(&BufferPool<buffer_chunk>::hasEnoughBytes, this));
    }

    {
        boost::mutex::scoped_lock lock(m_w_mutex);

        auto returnSize = length * BUFFER_CHUNK_SIZE;
        size_t lastItemCopySize;


        size_t leftSkip;
        size_t rightSkip;
        auto off = 0;

        for (size_t i = 0; i < length; i++) {

            if ( i == 0) {
                leftSkip = left_padding;
            } else {
                leftSkip = 0;
            }

            if (i == length - 1) {
                 rightSkip = right_padding;
            } else {
                 rightSkip = 0;
            }

            lastItemCopySize = BUFFER_CHUNK_SIZE - leftSkip - rightSkip;

            if (mGotLastBuffer && i == mTotalBufCount - 1) {
                lastItemCopySize = std::min(mLastBufferSize, lastItemCopySize);
            }

            auto endPos = std::min(mCircBuf.size(), capacity);
            auto startPos = endPos - (mTotalBufCount - offset) + i;

            memcpy(bufferChunks + off,
                       mCircBuf[startPos].data() + leftSkip,
                       lastItemCopySize);
            off += lastItemCopySize;
        }

        return off;
    }
}

template <>
size_t BufferPool<buffer_chunk>::readRandomAccess(char* data, size_t size, uint64_t offsetBytes)
{
    auto left_padding = offsetBytes % BUFFER_CHUNK_SIZE;
    auto right_padding = (BUFFER_CHUNK_SIZE - (left_padding + size) % BUFFER_CHUNK_SIZE) % BUFFER_CHUNK_SIZE;
    size_t length = (left_padding + right_padding + size) / BUFFER_CHUNK_SIZE;
    auto offset = offsetBytes / BUFFER_CHUNK_SIZE;
    return read(data, length, offset, left_padding, right_padding);
}

template <>
void BufferPool<buffer_chunk>::pushBuffer(buffer_chunk& buffer, bool lastBuffer, size_t lastBufferSize) {
    boost::mutex::scoped_lock lock(m_w_mutex);

    {
        mCircBuf.push_back(buffer);
        mTotalBufCount++;
        if(lastBuffer) {
            mGotLastBuffer = true;
            mLastBufferSize = lastBufferSize;
        }
        mNotEnoughBytes.notify_one();
    }

    mConsumer->newBufferAvailable(mCircBuf.front());
}

template<typename T>
void BufferPool<T>::lockWaitForRead() {

}