/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef UA_UTIL_H_
#define UA_UTIL_H_

#include "ua_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Assert */
#include <assert.h>
#define UA_assert(ignore) assert(ignore)

/* BSD Queue Macros */
#include "queue.h"

/* container_of */
#define container_of(ptr, type, member) \
    (type *)((uintptr_t)ptr - offsetof(type,member))

/* Thread Local Storage */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
# define UA_THREAD_LOCAL _Thread_local /* C11 */
#elif defined(__GNUC__)
# define UA_THREAD_LOCAL __thread /* GNU extension */
#elif defined(_MSC_VER)
# define UA_THREAD_LOCAL __declspec(thread) /* MSVC extension */
#else
# define UA_THREAD_LOCAL
# warning The compiler does not allow thread-local variables. \
  The library can be built, but will not be thread-safe.
#endif

/* Integer Shortnames
 * ------------------
 * These are not exposed on the public API, since many user-applications make
 * the same definitions in their headers. */

typedef UA_Byte u8;
typedef UA_SByte i8;
typedef UA_UInt16 u16;
typedef UA_Int16 i16;
typedef UA_UInt32 u32;
typedef UA_Int32 i32;
typedef UA_UInt64 u64;
typedef UA_Int64 i64;

/* Atomic Operations
 * -----------------
 * Atomic operations that synchronize across processor cores (for
 * multithreading). Only the inline-functions defined next are used. Replace
 * with architecture-specific operations if necessary. */
#ifndef UA_ENABLE_MULTITHREADING
# define UA_atomic_sync()
#else
# ifdef _MSC_VER /* Visual Studio */
#  define UA_atomic_sync() _ReadWriteBarrier()
# else /* GCC/Clang */
#  define UA_atomic_sync() __sync_synchronize()
# endif
#endif

static UA_INLINE void *
UA_atomic_xchg(void * volatile * addr, void *newptr) {
#ifndef UA_ENABLE_MULTITHREADING
    void *old = *addr;
    *addr = newptr;
    return old;
#else
# ifdef _MSC_VER /* Visual Studio */
    return _InterlockedExchangePointer(addr, newptr);
# else /* GCC/Clang */
    return __sync_lock_test_and_set(addr, newptr);
# endif
#endif
}

static UA_INLINE void *
UA_atomic_cmpxchg(void * volatile * addr, void *expected, void *newptr) {
#ifndef UA_ENABLE_MULTITHREADING
    void *old = *addr;
    if(old == expected) {
        *addr = newptr;
    }
    return old;
#else
# ifdef _MSC_VER /* Visual Studio */
    return _InterlockedCompareExchangePointer(addr, expected, newptr);
# else /* GCC/Clang */
    return __sync_val_compare_and_swap(addr, expected, newptr);
# endif
#endif
}

static UA_INLINE uint32_t
UA_atomic_add(volatile uint32_t *addr, uint32_t increase) {
#ifndef UA_ENABLE_MULTITHREADING
    *addr += increase;
    return *addr;
#else
# ifdef _MSC_VER /* Visual Studio */
    return _InterlockedExchangeAdd(addr, increase) + increase;
# else /* GCC/Clang */
    return __sync_add_and_fetch(addr, increase);
# endif
#endif
}

/* Utility Functions
 * ----------------- */

/* Convert given byte string to a positive number. Returns the number of valid
 * digits. Stops if a non-digit char is found and returns the number of digits
 * up to that point. */
size_t UA_readNumber(u8 *buf, size_t buflen, u32 *number);

#define MIN(A,B) (A > B ? B : A)
#define MAX(A,B) (A > B ? A : B)

/* The typename string can be disabled to safe memory */
#ifdef UA_ENABLE_TYPENAMES
# define UA_TYPENAME(name) name,
#else
# define UA_TYPENAME(name)
#endif

#ifdef UA_DUMP_HEX_PKG
void UA_EXPORT UA_dump_hex_pkg(UA_Byte* buffer, size_t bufferLen);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* UA_UTIL_H_ */
