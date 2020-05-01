//
// Created by Zoltan Kuscsik on 4/20/20.
//

#include <iostream>
#include "BufferPool.h"

template <>
void BufferPool<buffer_chunk>::read(
        BufferList* bufferChunks,
        size_t length,
        uint64_t offset)
{
    
}

template <>
void BufferPool<buffer_chunk>::pushBuffer(buffer_chunk& buffer) {
    mCircBuf.push_back(buffer);
    mTotalBufCount++;
    mConsumer->newBufferAvailable(mCircBuf.front());
}

template<typename T>
void BufferPool<T>::lockWaitForRead() {

}
