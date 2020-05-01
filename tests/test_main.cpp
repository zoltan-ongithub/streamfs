//
// Created by Zoltan Kuscsik on 4/21/20.
//

#include <BufferProducer.h>
#include <ByteBufferPool.h>
#include "test_main.h"

#include "gtest/gtest.h"

static unsigned int bufferReceivedCount_g = 0;

class TestProducer : public BufferProducer<buffer_chunk>{
public:

};

class TestConsumer : public BufferConsumer<buffer_chunk> {
    void newBufferAvailable(buffer_chunk &buffer) override {
        bufferReceivedCount_g ++;
    }
};

class TestBufferPool : public BufferPool<buffer_chunk > {
public:
    TestBufferPool(const ByteBufferPool::shared_producer_type& pProducer,
                   const ByteBufferPool::shared_consumer_type& pConsumer, uint64_t i)
            : BufferPool(pProducer, pConsumer, i) {

    }

    void read(BufferList *bufferChunks, size_t length, uint64_t offset) override {
            BufferPool::read(bufferChunks, length, offset);
    }

};
TEST(BufferPool, PassSingleElement) {
    ByteBufferPool::shared_producer_type producer = ByteBufferPool::shared_producer_type(new TestProducer());
    ByteBufferPool::shared_consumer_type consumer = ByteBufferPool::shared_consumer_type(new TestConsumer());
    buffer_chunk chunk;
    uint64_t allocSize = 1024;
    TestBufferPool  *pool = new TestBufferPool(producer, consumer, allocSize);
    producer->queueBuffer(chunk);
    ASSERT_EQ(bufferReceivedCount_g, 1);
    producer->queueBuffer(chunk);
    ASSERT_EQ(bufferReceivedCount_g, 2);
    ASSERT_EQ(pool->getSize(), 2);
    ASSERT_EQ(pool->getCapacity(), allocSize);
    delete(pool);
}