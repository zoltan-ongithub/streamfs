//
// Created by Zoltan Kuscsik on 4/20/20.
//

#include <streamfs/BufferPool.h>
#include "streamfs/BufferProducer.h"


template<>
void BufferProducer<buffer_chunk>::queueBuffer(const buffer_chunk &buffer, bool lastBuffer, size_t lastBufferSize) {
    mBufferPool->pushBuffer(buffer, lastBuffer, lastBufferSize);
}

template<>
void BufferProducer<buffer_chunk>::stop() {}
