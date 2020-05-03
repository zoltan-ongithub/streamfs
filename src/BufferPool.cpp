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
        uint64_t offset)
{
    boost::mutex::scoped_lock lock(m_mutex);
    auto capacity = mCircBuf.capacity();

    auto readEnd = offset + length;

    if ( (mGotLastBuffer &&  offset >= mTotalBufCount)  ) {

        // producer closed the data stream.
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
        auto lastItemCopySize = BUFFER_CHUNK_SIZE;

        if (mGotLastBuffer && length >= (mTotalBufCount - offset)) {
            length = mTotalBufCount - offset;
            returnSize = (mTotalBufCount - offset - 1) * BUFFER_CHUNK_SIZE + mLastBufferSize;
            lastItemCopySize = mLastBufferSize;
        }

        for (size_t i = 0; i < length; i++) {
            auto off = BUFFER_CHUNK_SIZE * i;
            auto endPos = std::min(mCircBuf.size(), capacity);
            auto startPos = endPos - (mTotalBufCount - offset) + i;

            if (i == length - 1) {
                memcpy(bufferChunks + off,
                       mCircBuf[startPos].data(),
                       lastItemCopySize);
            } else {
                memcpy(bufferChunks + off,
                       mCircBuf[startPos].data(),
                       BUFFER_CHUNK_SIZE);
            }
        }

        return returnSize;
    }
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