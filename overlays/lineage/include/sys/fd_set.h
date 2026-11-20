#ifndef LINEAGE_PSP_SYS_FD_SET_H
#define LINEAGE_PSP_SYS_FD_SET_H

/*
 * Compatibility definition for the legacy OSLib header.
 * Layout follows the 32-bit fd_set representation used by the PSP toolchain.
 */
#include <stdint.h>

#ifndef FD_SETSIZE
#define FD_SETSIZE 256
#endif

#define __LINEAGE_FD_BITS 32
#define __LINEAGE_FD_SHIFT 5
#define __LINEAGE_FD_MASK 31
#define __LINEAGE_FD_WORDS ((FD_SETSIZE + __LINEAGE_FD_BITS - 1) / __LINEAGE_FD_BITS)

typedef uint32_t __fd_mask;

typedef struct fd_set {
    __fd_mask fds_bits[__LINEAGE_FD_WORDS];
} fd_set;

#define FD_SET(n, p) \
    ((p)->fds_bits[(unsigned)(n) >> __LINEAGE_FD_SHIFT] |= \
     (1U << ((unsigned)(n) & __LINEAGE_FD_MASK)))

#define FD_CLR(n, p) \
    ((p)->fds_bits[(unsigned)(n) >> __LINEAGE_FD_SHIFT] &= \
     ~(1U << ((unsigned)(n) & __LINEAGE_FD_MASK)))

#define FD_ISSET(n, p) \
    ((p)->fds_bits[(unsigned)(n) >> __LINEAGE_FD_SHIFT] & \
     (1U << ((unsigned)(n) & __LINEAGE_FD_MASK)))

#define FD_ZERO(p) ((void)__builtin_memset((p), 0, sizeof(*(p))))

#endif
