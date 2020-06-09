//
// Created by Zoltan Kuscsik on 4/21/20.
//

#include <BufferProducer.h>
#include <ByteBufferPool.h>
#include <thread>
#include <stdlib.h>
#include "test_main.h"

#include "gtest/gtest.h"

static unsigned int bufferReceivedCount_g = 0;

class TestProducer : public BufferProducer<buffer_chunk>{
public:

};

class TestConsumer : public BufferConsumer<buffer_chunk> {
public:
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

    size_t read(char *bufferChunks, size_t length, uint64_t offset,  size_t left_padding, size_t right_padding) override {
            return BufferPool<buffer_chunk >::read(bufferChunks, length, offset,  left_padding,
                     right_padding);
    }

    size_t readRandomAccess(char *data, size_t size, uint64_t offsetBytes) override {
        return BufferPool::readRandomAccess(data, size, offsetBytes);
    }
};

class TestCore : public ::testing::Test {
};


class BufferPoolTest : public TestCore {
protected:
    void SetUp() override {
        Test::SetUp();
        producer = ByteBufferPool::shared_producer_type(new TestProducer());
        consumer = ByteBufferPool::shared_consumer_type(new TestConsumer());
        pool = new TestBufferPool(producer, consumer, allocSize);

    }

    void TearDown() override {
        Test::TearDown();
        delete(pool);
    }

public:
    TestBufferPool  *pool;
    ByteBufferPool::shared_producer_type producer;
    ByteBufferPool::shared_consumer_type consumer;
    uint64_t allocSize = 10;
};

TEST_F(BufferPoolTest, PassSingleElement) {
    buffer_chunk chunk;

    producer->queueBuffer(chunk);
    ASSERT_EQ(bufferReceivedCount_g, 1);
    producer->queueBuffer(chunk);
    ASSERT_EQ(bufferReceivedCount_g, 2);
    ASSERT_EQ(pool->getSize(), 2);
    ASSERT_EQ(pool->getCapacity(), allocSize);
}

TEST_F(BufferPoolTest, ReadSingleElement) {
    buffer_chunk chunk;

    memset(chunk.data(), 9, chunk.size());
    producer->queueBuffer(chunk);
    buffer_chunk outPut;

    pool->read( (char*) outPut.data(), 1, 0, 0, 0);
    ASSERT_EQ(chunk, outPut);
}

TEST_F(BufferPoolTest, ReadMultipleElement) {
    buffer_chunk chunk1, chunk2;

    memset(chunk1.data(), 8, chunk1.size());
    producer->queueBuffer(chunk1);

    memset(chunk2.data(), 9, chunk2.size());
    producer->queueBuffer(chunk2);

    char outputBytes[2 * chunk1.size()];
    pool->read( outputBytes, 2, 0, 0, 0);


    ASSERT_EQ(memcmp(outputBytes, (const void*) chunk1.data(), chunk1.size()), 0 );

    ASSERT_EQ(memcmp(&outputBytes[chunk1.size()], (const void*) chunk2.data(), chunk2.size()), 0 );
}

TEST_F(BufferPoolTest, ReadUnalignedMultipleElement) {
    buffer_chunk chunk1, chunk2;
    int OFFS = 10;
    memset(chunk1.data(), 8, chunk1.size());
    producer->queueBuffer(chunk1);

    memset(chunk2.data(), 9, chunk2.size());
    producer->queueBuffer(chunk2);

    char outputBytes[chunk1.size()];
    {
        for(OFFS = 0; OFFS < chunk1.size(); OFFS++) {
            auto res = pool->readRandomAccess(outputBytes, chunk1.size(), OFFS);
            ASSERT_EQ(res, chunk1.size());
            ASSERT_EQ(memcmp(outputBytes, (const char *) (chunk1.data() + OFFS), OFFS), 0);
            ASSERT_EQ(memcmp(&outputBytes[chunk1.size() - OFFS], (const void *) chunk2.data(), OFFS), 0);
        }
    }
}


static int buffer_is_empty(char *buf, size_t size)
{
    char* zeros  = (char*) calloc(size, sizeof(char));
    int res = memcmp(zeros, buf, size);
    free(zeros);
    return res;
}

TEST_F(BufferPoolTest, ClearBufferEnd) {
    buffer_chunk chunk1, chunk2;
    int LAST_BUFFER_SIZE = 20;

    memset(chunk1.data(), 8, chunk1.size());
    /* Queue 2 times the first buffer */
    producer->queueBuffer(chunk1);
    producer->queueBuffer(chunk1);

    memset(chunk2.data(), 9, chunk2.size());

    char outputBytes[chunk1.size()];
    memset(outputBytes, 0, chunk1.size());

    int result = pool->read( outputBytes, 1, 0, 0, 0);

    ASSERT_EQ(memcmp(outputBytes, (const void*) chunk1.data(), chunk1.size()), 0 );

    pool->clearToLastRead();

    producer->queueBuffer(chunk2);;
    result = pool->read( outputBytes, 1, 1, 0, 0);
    ASSERT_EQ(memcmp(outputBytes, (const void*) chunk2.data(), chunk2.size()), 0 );

}


TEST_F(BufferPoolTest, ReadMultipleNonAlligned) {
    buffer_chunk chunk1, chunk2;
    int LAST_BUFFER_SIZE = 20;

    memset(chunk1.data(), 8, chunk1.size());
    producer->queueBuffer(chunk1);

    memset(chunk2.data(), 9, chunk2.size());
    producer->queueBuffer(chunk2, true, LAST_BUFFER_SIZE);;

    char outputBytes[2 * chunk1.size()];
    memset(outputBytes, 0, 2 * chunk1.size());
    ASSERT_EQ(buffer_is_empty(&outputBytes[LAST_BUFFER_SIZE], chunk2.size() - LAST_BUFFER_SIZE), 0);

    int result = pool->read( outputBytes, 2, 0, 0, 0);

    ASSERT_EQ(result, chunk1.size() + LAST_BUFFER_SIZE);

    ASSERT_EQ(memcmp(outputBytes, (const void*) chunk1.data(), chunk1.size()), 0 );
    ASSERT_EQ(memcmp(&outputBytes[chunk1.size()], (const void*) chunk2.data(), LAST_BUFFER_SIZE), 0 );

    ASSERT_EQ(buffer_is_empty(&outputBytes[chunk1.size() + LAST_BUFFER_SIZE],
                              chunk2.size() - LAST_BUFFER_SIZE), 0);
}

TEST_F(BufferPoolTest, LockUntilBufferAvailable) {
    buffer_chunk chunk1, chunk2;
    std::vector<std::thread> workers;

    char outputBytes[2 * chunk1.size()];
    memset(outputBytes, 0,  2 * chunk1.size());

    memset(chunk1.data(), 8, chunk1.size());
    producer->queueBuffer(chunk1);

    memset(chunk2.data(), 9, chunk2.size());

    workers.emplace_back([&chunk2, &chunk1, this]()
        {
           usleep(500000);
           producer->queueBuffer(chunk2);
        });

    pool->read( outputBytes, 2, 0, 0, 0);

    ASSERT_EQ(memcmp(outputBytes, (const void*) chunk1.data(), chunk1.size()), 0 );

    ASSERT_EQ(memcmp(&outputBytes[chunk2.size()], (const void*) chunk2.data(), chunk2.size()), 0 );
    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });
    workers.clear();
}

TEST_F(BufferPoolTest, LockUntilBufferAvailableStartEmpty) {
    buffer_chunk chunk1;
    std::vector<std::thread> workers;

    char outputBytes[chunk1.size()];
    memset(outputBytes, 0,   chunk1.size());

    memset(chunk1.data(), 8, chunk1.size());

    workers.emplace_back([&chunk1, this]()
                         {
                             usleep(500000);
                             producer->queueBuffer(chunk1);
                         });
    pool->read( outputBytes, 1, 0, 0, 0);

    ASSERT_EQ(memcmp(outputBytes, (const void*) chunk1.data(), chunk1.size()), 0 );

    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });

    workers.clear();
}

TEST_F(BufferPoolTest, ReadOverWriteBuffer) {
    buffer_chunk chunk1, chunk2;
    char outputBytes[chunk1.size()];
    std::vector<std::thread> workers;

    memset(outputBytes, 0,  chunk1.size());
    memset(chunk1.data(), 8, chunk1.size());
    memset(chunk2.data(), 9, chunk2.size());

    for(int i = 0; i < allocSize; i ++) {
        producer->queueBuffer(chunk1);
    }

    workers.emplace_back([&chunk2, &chunk1, this]()
                         {
                             usleep(500000);
                             producer->queueBuffer(chunk2);
                         });

    producer->queueBuffer(chunk2);

    auto result = pool->read( outputBytes, 1, allocSize, 0, 0);

    ASSERT_EQ(result, 1 * BUFFER_CHUNK_SIZE);

    ASSERT_EQ(memcmp(outputBytes, (const void*) chunk2.data(), chunk2.size()), 0 );
    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });
    workers.clear();
}

TEST_F(BufferPoolTest, ReadOverWriteBufferWithLock) {
    buffer_chunk chunk1, chunk2, chunk3;
    auto outSize = 2 * chunk1.size();
    char outputBytes[outSize];
    std::vector<std::thread> workers;

    memset(outputBytes, 0,  outSize);
    memset(chunk1.data(), 8, chunk1.size());
    memset(chunk2.data(), 9, chunk2.size());
    memset(chunk3.data(), 10, chunk2.size());

    for(int i = 0; i < allocSize; i ++) {
        producer->queueBuffer(chunk1);
    }
    workers.clear();

    workers.emplace_back([&chunk2, &chunk3, this]()
                         {
                             usleep(500000);
                             producer->queueBuffer(chunk2);
                             producer->queueBuffer(chunk3);
                         });

    auto result = pool->read( outputBytes, 2, allocSize, 0, 0);

    ASSERT_EQ(result, 2 * BUFFER_CHUNK_SIZE);

    ASSERT_EQ(memcmp(outputBytes, (const void*) chunk2.data(), chunk2.size()), 0 );
    ASSERT_EQ(memcmp(&outputBytes[chunk2.size()], (const void*) chunk3.data(), chunk3.size()), 0 );

    usleep(1000000);
    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });
    workers.clear();
}

TEST_F(BufferPoolTest, ReadWrongLenght) {
    buffer_chunk chunk1, chunk2;
    char outputBytes[2 * chunk1.size()];
    memset(outputBytes, 0,  2 * chunk1.size());
    memset(chunk1.data(), 8, chunk1.size());

    producer->queueBuffer(chunk1);

    auto result = pool->read( outputBytes, allocSize + 2, 0, 0, 0);
    ASSERT_EQ(result, 0);
}


TEST_F(BufferPoolTest, ReadAfterLastItemReturnsEmpty) {
    buffer_chunk chunk1, chunk2;
    int LAST_BUF_SIZE = 10;
    char outputBytes[2 * chunk1.size()];

    memset(outputBytes, 0,  2 * chunk1.size());

    memset(chunk1.data(), 8, chunk1.size());
    producer->queueBuffer(chunk1);

    producer->queueBuffer(chunk1, true, LAST_BUF_SIZE);;

    auto result = pool->read( outputBytes, 2, 0, 0, 0);

    ASSERT_EQ(result, LAST_BUF_SIZE + chunk1.size());

    result = pool->read( outputBytes, 2, 2, 0, 0);
    ASSERT_EQ(result, 0);
}

