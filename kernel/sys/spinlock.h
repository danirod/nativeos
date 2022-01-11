#pragma once

/**
 * \file
 * \brief Spinlock API
 */

/**
 * A spinlock. This data structure is open and not opaque because in order to
 * statically allocate spinlocks in the kernel source code, it is required to
 * provide an implementation for the kernel modules. However, the internal
 * fields of a spinlock should never be modified without using the functions
 * in the spinlocks API, which are atomic and safe to use.
 *
 * \ingroup spinlock.
 */
struct spinlock {
	/** The current state of the spinlock. */
	int locked;
};

/**
 * \defgroup spinlock
 * \brief Spinlock API
 *
 * A spinlock is a concurrent data structure that guarantees control to a
 * resource that cannot be shared at the same time among multiple threads
 * or processes.
 *
 * The spinlock has to primitives: lock and release. A process will lock
 * a resource before trying to access it, and it will release the lock when
 * it is done. If a concurrent process tries to lock a locked spinlock,
 * the nature of the spinlock makes the process loop (spin) until the other
 * process releases the lock.
 */

/**
 * Initialise and reset a spinlock to the unlocked state.
 * \ingroup spinlock
 * \param lock the spinlock to initialise
 */
void spinlock_init(struct spinlock *lock);

/**
 * Memory barrier that returns once the spinlock is acquired.
 * \ingroup spinlock
 * \param lock the spinlock to acquire
 */
void spinlock_lock(struct spinlock *lock);

/**
 * Releases a previously acquired spinlock.
 * \ingroup spinlock
 * \param lock the spinlock to release.
 */
void spinlock_release(struct spinlock *lock);
