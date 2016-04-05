#ifndef __332_STDIO_H
#define __332_STDIO_H

#include <stdarg.h>

#define stdin 0
#define stdout 1
#define stderr 2

extern int getchar ();
extern char *gets (char *buf);
extern int puts (unsigned char *string);
extern int putchar (unsigned int outch);

extern int vsprintf(char *buf, const char *fmt, va_list args);
extern int sprintf(char * buf, const char *fmt, ...);
extern int printf(const char *fmt, ...);

#endif /* __332_STDIO_H */
