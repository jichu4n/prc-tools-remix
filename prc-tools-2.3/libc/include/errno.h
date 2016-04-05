#ifndef _332_ERRNO_H
#define _332_ERRNO_H

#warning This header is deprecated; because Palm OS has no useful errno \
facility, this header will be removed in the next prc-tools release

extern int errno;

#define ENOSYS 1	/* No syscall */
#define ENOENT 2	/* No entry   */
#define ESRCH  3        /* No process */
#define EAGAIN 4        /* (busy) Try again */

#endif /* _332_ERRNO_H */
