#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_

#include <sys/types.h>	/* for size_t */

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern char * ___strtok;
extern char * strcpy(char *,const char *);
extern char * strncpy(char *,const char *,size_t);
extern char * strcat(char *, const char *);
extern char * strncat(char *, const char *, size_t);
extern char * strchr(const char *,int);
extern char * strpbrk(const char *,const char *);
extern char * strtok(char *,const char *);
extern char * strstr(const char *,const char *);
extern size_t strlen(const char *);
extern size_t strnlen(const char *,size_t);
extern size_t strspn(const char *,const char *);
extern int strcmp(const char *,const char *);
extern int strncmp(const char *,const char *,size_t);
extern unsigned long strtoul(const char *cp,char **endp,unsigned int base);

extern signed long strtol(const char *cp,char **endp,unsigned int base);
extern int intodec(char * dest,signed int arg,unsigned short places,unsigned int base);

extern void * memset(void *,char,size_t);
extern void * memcpy(void *,const void *,size_t);
extern void * memmove(void *,const void *,size_t);
extern void * memscan(void *,int,size_t);
extern int memcmp(const void *,const void *,size_t);

/*
 * Include machine specific inline routines
 */

#ifdef __cplusplus
}
#endif

#endif /* _LINUX_STRING_H_ */
