/* emit-prc.c:  Routines to deal with writing .prc files
 *
 * (c) 1996, Dionne & Associates
 * This is Free Software, under the GNU Public Licence v2 or greater.
 */

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "prc.h"

#ifdef CYGWIN32
#define O_PLATFORM O_BINARY
#else
#define O_PLATFORM 0
#endif


static char prc_exec[] = PRC_EXEC;
static char prc_pad1[] = PRC_PAD1;
static char prc_appl[] = PRC_APPL;
static char prc_pad2[] = PRC_PAD2;

static char prc_time[] = PRC_TIME;

static char prc_sect_pad1[] = PRC_SECT_PAD1;

pfd_t *openw_pfd(char *aname,
		 char *id,
		 char *fname,
		 char *type)
{
  pfd_t *pfd;

  if (!(pfd = (pfd_t *)malloc(sizeof(pfd_t)))) return (void *)0;

  memset(pfd, 0, sizeof(pfd_t));

  if (!(pfd->_fd = open(fname, O_PLATFORM | O_WRONLY | O_CREAT | O_TRUNC, 0644))) {
    free(pfd);
    return (void *)0;
  }

  if (!(pfd->prc = (prc_t *)malloc(sizeof(prc_t)))) {
    close(pfd->_fd);
    free(pfd);
    return (void *)0;
  }

  memset(pfd->prc, 0, sizeof(prc_t));

  memcpy(pfd->prc->c_exec, prc_exec, sizeof(prc_exec));
  memcpy(pfd->prc->c_pad1, prc_pad1, sizeof(prc_pad1));
  memcpy(pfd->prc->c_appl, type, sizeof(prc_appl));
  memcpy(pfd->prc->c_pad2, prc_pad2, sizeof(prc_pad2));

  strcpy(pfd->prc->name, aname);
  memcpy(pfd->prc->id, id, 4);

  memcpy(pfd->prc->time, prc_time, sizeof(prc_time));
  pfd->prc->n_sect = htons(0);

  return pfd;
}

int add_section_pfd(pfd_t *pfd,
		    char *name,
		    short id,
		    char *sc,
		    int len)
{
  sdata_t *sd;

  if (!(sd = (sdata_t *)malloc(sizeof(sdata_t)))) {
    return -1;
  }
  if (!(sd->sect = (sect_t *)malloc(sizeof(sect_t)))) {
    return -1;
  }

  sd->len = len;
  sd->data = sc;
  sd->next = pfd->sdata;
  pfd->sdata = sd;

  memcpy(sd->sect->name, name, 4);
  sd->sect->id = htons(id);

  pfd->prc->n_sect = htons(ntohs(pfd->prc->n_sect) + 1);

  return 0;
}
 
int write_pfd(pfd_t *pfd)
{
  unsigned int total;
  unsigned int count;
  int i;
  sdata_t *sd;

  count = sizeof(prc_t) + sizeof(sect_t) * ntohs(pfd->prc->n_sect) + 2;

  for (sd = pfd->sdata; sd; sd=sd->next) {
    sd->sect->offseth = htons((count >> 16) & 0xffff);
    sd->sect->offsetl = htons(count & 0xffff);
    count += sd->len;
  }

  /* All records are now complete, we can write the image */

  total = write(pfd->_fd, pfd->prc, sizeof(prc_t));

  for (sd = pfd->sdata; sd; sd=sd->next) {
    total += write(pfd->_fd, sd->sect, sizeof(sect_t));
  }

  total += write(pfd->_fd, prc_sect_pad1, sizeof(prc_sect_pad1));

  for (sd = pfd->sdata; sd; sd=sd->next) {
    total += write(pfd->_fd, sd->data, sd->len);
  }

  return total;
}
