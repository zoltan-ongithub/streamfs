//
// Created by Zoltan Kuscsik on 4/21/20.
//

#include <streamfs/BufferProducer.h>
#include <streamfs/ByteBufferPool.h>
#include <thread>
#include <stdlib.h>
#include <boost/thread/condition.hpp>
#include "test_main.h"

#include "streamfs/BufferPoolThrottle.h"

#include "gtest/gtest.h"



#define BUFFER_POOL_CAPACITY (LIVE_POSITION_THRESHOLD_INDEX + READ_AHEAD_COUNT + 50)

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
    uint64_t allocSize = BUFFER_POOL_CAPACITY;
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

#ifdef BUFFER_CHUNK_READ_THROTTLING
TEST_F(BufferPoolTest, PlayerReadThrottleTest) {

    boost::condition_variable cv;

    // Buffer chunk rate in us.
    const uint32_t kBufferChunkRate      = 25000;
    // gstreamer buffer read size in bytes
    const uint32_t kPlayerReadSize       = 4096;
    // Number of buffer chunk to inject / produce.
    // We multiply by 1.75 to have more buffers produced than read back.
    // This is to ensure that we don't go below the live position
    // threshold during read back in the consumer thread, where throttle
    // stops being applied.
    const uint16_t kNumberOfBufferChunks = 1.75 * BUFFER_POOL_CAPACITY;
    // Tolerated time inaccuracy allowed when throttle is applied
    const uint32_t kAbsErrorThrottle     = 5000;
    // Tolerated time inaccuracy allowed when throttle is not expected
    const uint32_t kAbsErrorNoThrottle   = 5000;
    // Vector of reference buffer latencies
    std::vector<uint32_t> rate{0};

    // Create vector of alternating reference buffer rates
    uint32_t refRate = kBufferChunkRate;
    for (uint16_t i = 1; i < kNumberOfBufferChunks; ++i) {
        if (i % 5 == 0 ) {
            if (refRate == kBufferChunkRate) {
                refRate = kBufferChunkRate/2;
            } else {
                refRate = kBufferChunkRate;
            }
        }
        rate.push_back(refRate);
    }

    // Producer thread for injection of buffer_chunks into the buffer pool
    std::thread t1([&kBufferChunkRate, &rate, &cv, this]()
    {
        buffer_chunk chunk;
        for (uint16_t i = 0; i < kNumberOfBufferChunks; ++i) {
            memset(chunk.data(), i, chunk.size());
            std::this_thread::sleep_for(std::chrono::microseconds(rate[i]));
            producer->queueBuffer(chunk);
            // Notify the consumer theard when the index / number of buffer chunks pushed
            // to the buffer pool equals the sum of the live position threshold index and
            // read ahead count. This will ensure that the producer thread will start
            // reading from TSB, thus testing the TSB throttling.
            if (i == LIVE_POSITION_THRESHOLD_INDEX + READ_AHEAD_COUNT + 10) {
                cv.notify_one();
            }
        }
        producer->queueBuffer(chunk);
    });

    // Consumer thread for reading player buffers and evaluating throttling times.
    std::thread t2([&kBufferChunkRate, &rate, &cv, this]()
    {
        boost::mutex mutex;
        boost::unique_lock<boost::mutex> lock(mutex);
        // Wait for the producer to notify when the number of buffer chunks pushed to
        // the buffer pool equals the sum of the live position threshold index and
        // read ahead count. This is to ensure that we don't go below the live position
        // index after initial buffer read ahead has completed. This would otherwise
        // mean that the read position would be considered within the live window
        // meaning that throttling wouldn't be applied.
        cv.wait(lock);
        char outputBytes[kPlayerReadSize];
        for (uint64_t offset = 0; offset < (BUFFER_POOL_CAPACITY * BUFFER_CHUNK_SIZE) - kPlayerReadSize; offset += kPlayerReadSize) {
            std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
            auto res = pool->readRandomAccess(outputBytes, kPlayerReadSize, offset);
            std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();

            std::chrono::microseconds diff = (std::chrono::duration_cast<std::chrono::microseconds>(currentTime-lastTime));
            uint32_t readTime = diff.count();

            // Throttling time is evaluated the first time data is to be read from a new buffer chunk.
            // This is determined by calculating if the associated buffer data to read spans over
            // two adjacent buffer chunks OR if the left_padding is zero (meaning that data is read
            // from the start of a new buffer chunk)
            uint64_t bufferChunkIndexStart = offset / BUFFER_CHUNK_SIZE;
            uint64_t bufferChunkIndexEnd = (offset + kPlayerReadSize - 1) / BUFFER_CHUNK_SIZE;
            uint64_t left_padding = offset % BUFFER_CHUNK_SIZE;

            uint64_t playerBufferCount = (offset/kPlayerReadSize);

            if (playerBufferCount < READ_AHEAD_COUNT) {
                ASSERT_NEAR (0, readTime, kAbsErrorNoThrottle);
            } else {
                if (bufferChunkIndexStart != bufferChunkIndexEnd || left_padding == 0 && offset) {
                    ASSERT_NEAR (rate[bufferChunkIndexEnd], readTime, kAbsErrorThrottle);
                }
            }

        }
    });

    t1.join();
    t2.join();
}

TEST(BufferPoolThrottle, RateTest) {
    // Buffer chunk rate in us.
    const uint32_t kBufferChunkRate      = 12500;
    // Tolerated time inaccuracy allowed when throttle is applied
    const uint32_t kAbsErrorThrottle     = 5000;
    // Tolerated time inaccuracy allowed when throttle is not expected
    const uint32_t kAbsErrorNoThrottle   = 5000;
    // Total number of buffer registrations
    const uint32_t kTotalBufferSize      = IDLE_LIMIT_COUNT + READ_AHEAD_COUNT + 110;

    // Create vector of alternating reference buffer rates
    std::vector<uint32_t> rate{0};
    uint32_t refRate = kBufferChunkRate;
    for (uint16_t pos = 1; pos < kTotalBufferSize; ++pos) {
        if (pos % 5 == 0 ) {
            if (refRate == kBufferChunkRate) {
                refRate = kBufferChunkRate/2;
            } else {
                refRate = kBufferChunkRate;
            }
        }
        rate.push_back(refRate);
    }

    BufferPoolThrottle throttle(1000);

    // Register buffer chunk using the reference rate defined above
    buffer_chunk chunk;
    for (uint16_t pos = 0; pos < kTotalBufferSize; ++pos) {
        memset(chunk.data(), pos, chunk.size());
        std::this_thread::sleep_for(std::chrono::microseconds(rate[pos]));
        throttle.registerTimePeriod();
    }

    // Validate throttle timing for the entire range of registered positions
    for (uint16_t pos = 0; pos < kTotalBufferSize; ++pos) {
        throttle.setReadPosition(pos);
        std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
        throttle.wait();
        std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();

        std::chrono::microseconds diff = (std::chrono::duration_cast<std::chrono::microseconds>(currentTime-lastTime));
        uint32_t readTime = diff.count();

        if (pos < READ_AHEAD_COUNT) {
            // Initial read ahead pos range. Buffer chunk rate should be close to zero
            ASSERT_NEAR (0, readTime, kAbsErrorNoThrottle);
        } else if (pos < kTotalBufferSize - LIVE_POSITION_THRESHOLD_INDEX) {
            // Throttle pos range. Buffer chunk rate should match the alternating rate
            ASSERT_NEAR (rate[pos], readTime, kAbsErrorThrottle);
        } else {
            // Live pos range. Buffer chunk rate should be close to zero
            ASSERT_NEAR (0, readTime, kAbsErrorNoThrottle);
        }
    }
}

#endif
