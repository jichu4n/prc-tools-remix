#ifndef _332_ERRNO_H
#define _332_ERRNO_H

extern int errno;

#define ENOSYS 1	/* No syscall */
#define ENOENT 2	/* No entry   */
#define ESRCH  3        /* No process */
#define EAGAIN 4        /* (busy) Try again */

#endif /* _332_ERRNO_H */
