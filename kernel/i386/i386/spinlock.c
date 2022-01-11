/**
 * \file
 * \brief i386 implementation of Spinlock API
 *
 * This function contains the i386 internal implementation of the spinlocks
 * for NativeOS. A spinlock is a simple resource lock that allows a process
 * to mark a resource as busy. If a different thread tries to lock a locked
 * spinlock, it will loop (spin) until the lock can be locked.
 */

#include <sys/spinlock.h>
#include <sys/stdkern.h>

/**
 * \brief Replace the contents of the memory address with the given value.
 * \param memaddr the pointer whose value must be updated.
 * \param val the new value to place in the pointed memory address.
 * \return the old value for the memory address pointed here.
 * \internal
 */
static inline int
x86_xchg_dword(int *memaddr, int val)
{
	int prev;
	__asm__("xchgl %0, %1" : "=a"(prev) : "m"(*memaddr), "a"(val));
	return prev;
}

void
spinlock_init(struct spinlock *lock)
{
	lock->locked = 0;
}

void
spinlock_lock(struct spinlock *lock)
{
	/* If locked was already set to 1, this function will return 1. */
	while (x86_xchg_dword(&lock->locked, 1))
		;
}

void
spinlock_release(struct spinlock *lock)
{
	x86_xchg_dword(&lock->locked, 0);
}
