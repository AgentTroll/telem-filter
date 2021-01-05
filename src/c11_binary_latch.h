/**
 * @file
 */

#ifndef LIFTOFF_C11_BINARY_LATCH_H
#define LIFTOFF_C11_BINARY_LATCH_H

#include <condition_variable>
#include <mutex>

/**
 * @brief Represents a single-use latch which can have
 * multiple waiting threads which are released by the
 * release() method. This is implemented for C++11.
 */
class c11_binary_latch {
private:
    /**
     * Whether or not the release() method has been called.
     *
     * Access to this member is protected by the mutex.
     */
    bool released{false};
    /**
     * The mutex used to protect access to the released
     * member.
     */
    std::mutex mutex;

    /**
     * The condition variable used to block threads waiting
     * to be released.
     */
    std::condition_variable cond;

public:
    /**
     * Blocks until a thread calls release(), unless the
     * release() method has already been called.
     */
    void wait();

    /**
     * Releases all threads blocked on the call to wait().
     */
    void release();
};

#endif // LIFTOFF_C11_BINARY_LATCH_H
