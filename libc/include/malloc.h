#ifndef _332_MALLOC_H_
#define _332_MALLOC_H_

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
