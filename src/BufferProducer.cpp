//
// Created by Zoltan Kuscsik on 4/20/20.
//

#include "BufferProducer.h"

template<class T>
void BufferProducer<T>::queueBuffer(T &buffer) {
    mBufferPool->pushBuffer(buffer);
}