/* This file is part of prc-tools.  */

#ifndef _PRC_TOOLS_STRING_H
#define _PRC_TOOLS_STRING_H

/* FIXME As we replace the somewhat borked functions from the old Linux
   sources, we'll move their declarations to this section.  Eventually
   the _LINUX_STRING_H_ section below will disappear.  */

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy (void *_dst, const void *_src, size_t _n);
void *memset (void *_s, int _c, size_t _n);

#ifdef __cplusplus
}
#endif

#endif

#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_

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
