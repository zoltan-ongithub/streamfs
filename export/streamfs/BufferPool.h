//
// Created by Zoltan Kuscsik on 4/20/20.
//

#ifndef STREAMFS_BUFFERPOOL_H
#define STREAMFS_BUFFERPOOL_H

#include <boost/circular_buffer.hpp>
#include <boost/thread/condition.hpp>

#include <sys/mman.h>
#include <atomic>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <glog/logging.h>
#include <atomic>
#include "config.h"
#include <array>
#include "BufferProducer.h"
#include "BufferConsumer.h"

using buffer_chunk  = std::array<unsigned char, BUFFER_CHUNK_SIZE>;

#ifdef BUFFER_CHUNK_READ_THROTTLING
class BufferPoolThrottle;
#endif

struct BufferList{
    buffer_chunk* chunks;
    uint64_t length;
};

/**
 * Custom allocator to disable memory from core dump.
 */
template<typename _Tp>
class streamfs_allocator {
public:
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef _Tp *pointer;
	typedef const _Tp *const_pointer;
	typedef _Tp &reference;
	typedef const _Tp &const_reference;
	typedef _Tp value_type;

	pointer allocate(size_type n, const void *hint = 0) {
		void *ptr;
		auto pageSize = getpagesize();
		auto totalSize = n * sizeof(_Tp);
		auto ret = posix_memalign(&ptr, pageSize, totalSize);
		if (0 != ret) {
			LOG(ERROR) << __FUNCTION__ << " posix_memalign failed. n : " << n
					<< " error : " << strerror(ret);
		}
		ret = madvise(ptr, totalSize, MADV_DONTDUMP);
		if (0 != ret) {
			LOG(ERROR) << __FUNCTION__ << " madvise failed. totalSize : "
					<< totalSize << " error : " << strerror(errno);
		}
		return (pointer) ptr;
	}

	void deallocate(pointer p, size_type n) {
		int ret = madvise(p, n * sizeof(_Tp), MADV_DODUMP);
		if (0 != ret) {
			LOG(WARNING) << __FUNCTION__ << "madvise failed. n : " << n
					<< " error : " << strerror(errno);
		}
		free(p);
	}
};

/**
 * Generic buffer pool.
 * Buffer pool is responsible to free the producer/consumer pointers.
 *
 * @tparam T
 */
template <typename T>
class BufferPool {

public:
    /**
     * Buffer pool
     * @param producer - pointer to producer.
     * @param consumer  - pointer to consumer
     * @param preAllocBufSize  - number of chunk memories to maintain.
     */
    explicit BufferPool (
            std::shared_ptr<BufferProducer<T>> producer,
            std::shared_ptr<BufferConsumer<T>> consumer,
            uint64_t preAllocBufSize);

    virtual ~BufferPool();

    virtual size_t read(char* bufferChunks, size_t length, uint64_t offset, size_t left_padding , size_t right_padding ) = 0;

    virtual size_t readRandomAccess(char* data, size_t size, uint64_t offsetBytes)  = 0;

    virtual void pushBuffer(const T& buffer, bool lastBuffer = false, size_t lastBufferSize = 0);

    /**
     * Clear buffer. Can be called only from the consumer thread
     */
    virtual void clear();

    /**
     * Clear buffer until the last read
     */
    virtual void clearToLastRead();

    /**
     * Get circular buffer maximum capacity
     * @return
     */
    size_t getCapacity() {
        return mCircBuf.capacity();
    }

    /**
     * Get current size of the ring buffer
     * @return
     */
    size_t getSize() {
        return mCircBuf.size();
    }

    /**
     * Get virtual size of the ring buffer
     * @return
     */
    uint64_t getTotalBufferCount() {
        return mTotalBufCount;
    }

    /**
     * producer stop
     */
    virtual void abortAllOperations();

private:
    BufferPool(const BufferPool&);
    BufferPool& operator = (const BufferPool&);

    void lockWaitForRead();

private:
    std::shared_ptr<BufferProducer<T>> mProducer;
    std::shared_ptr<BufferConsumer<T>> mConsumer;
    boost::circular_buffer<T , streamfs_allocator<T>> mCircBuf;
    std::atomic<uint64_t> mTotalBufCount;
    boost::mutex m_mutex;
    boost::mutex m_w_mutex;

    boost::condition_variable mNotEnoughBytes;
    uint64_t mLastReadLocation;

    uint64_t mReadEnd;
    bool  mGotLastBuffer;
    size_t mLastBufferSize;
    bool exitPending = false;
    uint64_t mLastStartPos = 0;
    std::atomic<bool> mAborting = {false};
#ifdef BUFFER_CHUNK_READ_THROTTLING
    std::unique_ptr<BufferPoolThrottle> mThrottle;
#endif
};

#endif //STREAMFS_BUFFERPOOL_H
