//
// Created by Zoltan Kuscsik on 4/20/20.
//

#ifndef STREAMFS_BUFFERPRODUCER_H
#define STREAMFS_BUFFERPRODUCER_H

template <typename T> class BufferPool;

template <class T>
class BufferProducer {
    template <typename T1>
    friend  class BufferPool;

public:
    void queueBuffer(T &buffer);
protected:
    void setBufferPool(BufferPool<T> *bufferPool) { mBufferPool = bufferPool; }
private:
    BufferPool<T> *mBufferPool;
};


#endif //STREAMFS_BUFFERPRODUCER_H
