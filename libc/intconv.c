/* intconv.c: ISO/IEC 9899:1999  7.20.1.2 and 7.20.1.4.
	7.20.1.2  The atoi, atol, and atoll functions.
	7.20.1.4  The strtol, strtoll, strtoul, and strtoull functions.

   Placed in the public domain by John Marshall.  */

#include "stdlib.h"
#include "ctype.h"
#include "limits.h"

/* In addition to the usual Lfuncname selection macro, this file requires
   the Makefile to define LONG_LONG if we are building the "long long int"
   variant of a function.  Generally the source code is identical to that
   of the "long int" variant, so this trick avoids making duplicate copies
   of the code.  */

#ifdef LONG_LONG
#define value_t  long long int
#define fn(name,extra)  name##extra
#else
#define value_t  long int
#define fn(name,extra)  name
#endif

/* BOUNDARY is one more than the maximum signed value_t, expressed as an
   unsigned value_t.  */
#define BOUNDARY (((unsigned value_t) 1) << (CHAR_BIT * sizeof (value_t) - 1))

#ifdef Latoi

int
atoi (const char *s) {
  return (int) atol (s);
  }

#endif
#if defined Latol || defined Latoll

value_t
fn(atol,l) (const char *s) {
  value_t n = 0;
  char sign;

  while (isspace (*s))
    s++;

  sign = *s;
  if (sign == '+' || sign == '-')
    s++;

  while (isdigit (*s))
    n = 10 * n + *s++ - '0';

  return (sign == '-')? -n : +n;
  }

#endif

#define NEGATIVE  0x01
#define OVERFLOW  0x02

unsigned value_t fn(_Strtoul,l) (int *, const char *, char **, int);

#if defined L_Strtoul || defined L_Strtoull

static inline unsigned int
decode_digit (unsigned char c) {
  if (c >= 'a')
    return c - 'a' + 10;
  else if (c >= 'A')
    return c - 'A' + 10;
  else if (c >= '0' && c <= '9')
    return c - '0';
  else
    return 36;
  }

unsigned value_t
fn(_Strtoul,l) (int *statep, const char *nptr, char **endptr, int base) {
  unsigned value_t n = 0;
  const char *s, *subject;
  int digit, state = 0;

  s = nptr;
  while (isspace (*s))
    s++;

  if (*s == '+')
    s++;
  else if (*s == '-') {
    state |= NEGATIVE;
    s++;
    }

  if ((base == 0 || base == 16)
      && s[0] == '0' && (s[1] == 'x' || s[1] == 'X') && isxdigit (s[2])) {
    base = 16;
    s += 2;
    }

  if (base == 0)
    base = (s[0] == '0')? 8 : 10;

  subject = s;

  while ((digit = decode_digit (*s)) < base) {
    unsigned value_t newn = n * base + digit;
    if (newn < n)
      state |= OVERFLOW;
    n = newn;
    s++;
    }

  if (endptr)
    *endptr = (char *) ((s > subject)? s : nptr);

  *statep = state;
  return (state & OVERFLOW)? (BOUNDARY - 1) * 2 + 1 : n;
  }

#endif
#if defined Lstrtol || defined Lstrtoll

value_t
fn(strtol,l) (const char *nptr, char **endptr, int base) {
  int state;
  unsigned value_t nu = fn(_Strtoul,l) (&state, nptr, endptr, base);
  value_t n;

  if (state & NEGATIVE) {
    if (nu > BOUNDARY) {
      nu = BOUNDARY;
      }
    n = -nu;
    }
  else {
    if (nu >= BOUNDARY) {
      nu = BOUNDARY - 1;
      }
    n = +nu;
    }

  /* FIXME Set OVERFLOW above, and check it here and signal via errno.  */

  return n;
  }

#endif
#if defined Lstrtoul || defined Lstrtoull

unsigned value_t
fn(strtoul,l) (const char *nptr, char **endptr, int base) {
  int state;
  unsigned value_t n = fn(_Strtoul,l) (&state, nptr, endptr, base);

  /* FIXME Check OVERFLOW and signal it via errno.  */

  return (state & NEGATIVE)? -n : +n;
  }

#endif
