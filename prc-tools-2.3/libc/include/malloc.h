#ifndef _332_MALLOC_H_
#define _332_MALLOC_H_

#warning This non-standard header is deprecated; it will be removed in \
the next prc-tools release; use stdlib.h instead

extern void *malloc (int size);
extern void free (void *ptr);

#ifdef __KERNEL__

void *kmalloc(int size, void *tcb);
int kfree(void *p);

struct mblock {
  struct mblock *next;
  struct mblock *prev;
  int size;
};

#endif /* __KERNEL__ */
#endif /* _332_MALLOC_H_ */
