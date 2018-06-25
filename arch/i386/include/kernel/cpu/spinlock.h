#pragma once

typedef struct spinlock {
    char name[10];
    int locked;
} spinlock_t;

/**
 * @brief Resets a spinlock, setting the state to unacquired.
 * @param lock the spinlock to reset.
 */
void spinlock_reset(spinlock_t* lock);

/**
 * @brief Locks a spinlock, blocking the thread until the spinlock is acquired.
 * @param lock the spinlock to acquire.
 */
void spinlock_lock(spinlock_t* lock, const char* name);

/**
 * @brief Tries to lock a spinlock, giving up after an amount of fails.
 * @param lock the spinlock to acquire.
 * @param tries how many times to try to acquire the lock.
 * @return non-zero unless the lock is successfully acquired.
 */
int spinlock_try_lock(spinlock_t* lock, int tries);

/**
 * @brief Releases a previously acquired spinlock.
 * @param lock the spinlock to release.
 */
void spinlock_release(spinlock_t* lock);
