//
// Created by Zoltan Kuscsik on 4/20/20.
//

#include <iostream>
#include "BufferPool.h"

template <>
void BufferPool<buffer_chunk >::readHead(BufferList& bufferChunks, const BufferList* lastChunks)
{

}

template <>
void BufferPool<buffer_chunk>::pushBuffer(buffer_chunk& buffer) {
    mCircBuf.push_back(buffer);
    mConsumer->newBufferAvailable(mCircBuf.front());
}
