#include <kernel/cpu/spinlock.h>
#include <sys/stdkern.h>

static inline int x86_xchg_dword(int* memaddr, int val)
{
    int oldval = *memaddr;
    *memaddr = val;
    return oldval;
}

void spinlock_reset(spinlock_t* lock)
{
    lock->locked = 0;
}

void spinlock_lock(spinlock_t* lock, const char* name)
{
    /*
     * Keep the spinlock spinning as long as it is locked. If the spinlock
     * is locked, exchanging 1 with the value of the spinlock equals to
     * exchanging 1 to 1, so it returns 1 and has no side effect into the
     * spinlock. Once the spinlock is not locked anymore, the lock has a
     * value of 0, so it's exchanging 0 to 1: the spinlock is set to 1 and
     * this sub returns 0. This has no particular side effects.
     */
    while (x86_xchg_dword(&lock->locked, 1));
    strncpy(lock->name, name, 9);
}

int spinlock_try_lock(spinlock_t* lock, int tries)
{
    while (tries-- > 0 && x86_xchg_dword(&lock->locked, 1));
    return tries == 0;
}

void spinlock_release(spinlock_t* lock)
{
    x86_xchg_dword(&lock->locked, 0);
}
