//
// Created by Zoltan Kuscsik on 4/20/20.
//

#include <iostream>
#include <glog/logging.h>
#include "streamfs/BufferPool.h"
#include <boost/bind.hpp>
#include <cstring>
#include <algorithm>    // std::max

template<>
size_t BufferPool<buffer_chunk>::read(
        char *bufferChunks,
        size_t length,
        uint64_t offset,
        size_t left_padding,
        size_t right_padding) {
    if (exitPending) {
        LOG(WARNING) << "BufferPool::read : return 0 : exitPending";
        return 0;
    }

    boost::mutex::scoped_lock lock(m_mutex);
    auto capacity = mCircBuf.capacity();

    if ((mGotLastBuffer && offset >= mTotalBufCount)) {
        // producer closed the data stream.
        LOG(WARNING) << "BufferPool::read : return 0 : offset : " << offset << " mTotalBufCount : " << mTotalBufCount;
        return 0;
    }

    if (left_padding >= BUFFER_CHUNK_SIZE ||
        right_padding >= BUFFER_CHUNK_SIZE ||
        (length - left_padding - right_padding) < 0) {
        LOG(WARNING) << "BufferPool::read : return 0 : length : " << length << " left_padding : " << left_padding<< " right_padding : " << right_padding;
        return 0;
    }

    if ((mTotalBufCount - offset) > mCircBuf.capacity()
        || offset > mTotalBufCount
        || length > capacity
            ) {
        LOG(WARNING) << "BufferPool::read : return 0 : length : " << length << " buffer-offset : " << offset << " mTotalBufCount : " << mTotalBufCount << " mCircBuf.capacity() : " << mCircBuf.capacity() << " capacity : " << capacity;
        return 0;
    }

    auto readEnd = offset + length;
    if (readEnd > mTotalBufCount && !mGotLastBuffer) {
        boost::chrono::duration<double> timeout = boost::chrono::milliseconds(BUFFER_POOL_READ_TIMEOUT_MS);
        if (!mNotEnoughBytes.wait_for(lock, timeout, [this, readEnd](){ return readEnd <= mTotalBufCount || mGotLastBuffer; })) {
            LOG(INFO) << __FUNCTION__ << ": no buffer chunks received before read timout";
            return 0;
        }
        mReadEnd = readEnd;
    }

    {
        boost::mutex::scoped_lock lock(m_w_mutex);

        size_t lastItemCopySize;


        size_t leftSkip;
        size_t rightSkip;
        auto off = 0;

        for (size_t i = 0; i < length; i++) {

            if (i == 0) {
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
            mLastReadLocation = startPos;
            memcpy(bufferChunks + off,
                   mCircBuf[startPos].data() + leftSkip,
                   lastItemCopySize);
            off += lastItemCopySize;
        }

        return off;
    }
}

template<>
size_t BufferPool<buffer_chunk>::readRandomAccess(char *data, size_t size, uint64_t offsetBytes) {
    auto left_padding = offsetBytes % BUFFER_CHUNK_SIZE;
    auto right_padding = (BUFFER_CHUNK_SIZE - (left_padding + size) % BUFFER_CHUNK_SIZE) % BUFFER_CHUNK_SIZE;
    size_t length = (left_padding + right_padding + size) / BUFFER_CHUNK_SIZE;
    auto offset = offsetBytes / BUFFER_CHUNK_SIZE;
    return read(data, length, offset, left_padding, right_padding);
}

template<>
void BufferPool<buffer_chunk>::clear() {
    boost::mutex::scoped_lock lock(m_w_mutex);
    mGotLastBuffer = false;
    mTotalBufCount = 0;
    mReadEnd = 0;
    mLastBufferSize = 0;
    mCircBuf.clear();
}

template<>
void BufferPool<buffer_chunk>::pushBuffer(const buffer_chunk &buffer, bool lastBuffer, size_t lastBufferSize) {
    if (exitPending)
        return;

    boost::mutex::scoped_lock lock(m_w_mutex);

    {
        mCircBuf.push_back(buffer);
        mTotalBufCount++;
        if (lastBuffer) {
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

template<>
void BufferPool<buffer_chunk>::clearToLastRead() {
    boost::mutex::scoped_lock lock(m_mutex);
    boost::mutex::scoped_lock lockWrite(m_w_mutex);

    if (mCircBuf.empty())
        return;

    auto numElements = mCircBuf.size();
    auto eraseCount = numElements - mLastReadLocation - 1;
    mCircBuf.erase_end(eraseCount);
    mTotalBufCount -= eraseCount;
}

template<>
void BufferPool<buffer_chunk>::abortAllOperations() {
    boost::mutex::scoped_lock lock(m_w_mutex);
    mProducer->stop();
    mNotEnoughBytes.notify_all();
}
