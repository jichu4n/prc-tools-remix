/* ptst.c:  build a .prc from a pile of files.
 *
 * (c) 1996, Dionne & Associates
 * (c) 1997, The Silver Hammer Group Ltd.
 * This is Free Software, under the GNU Public Licence v2 or greater.
 */

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "prc.h"

#ifdef __CYGWIN32__
#define O_PLATFORM O_BINARY
#else
#define O_PLATFORM 0
#endif

main(int argc, char *argv[])
{
  pfd_t *pf;
  int fd;
  int i;
  char *sd;
  char sname[5];
  int slen;
  int opt;
  char *type = "appl";
  int optind = 1;

  if (argc < 5) {
    fprintf(stderr, "Usage: %s [-Ll] [-t type] fname.prc 'App Name' apid resource resource ...\n", argv[0]);
    exit (1);
  }

  while (optind < argc && argv[optind][0] == '-') {
    opt = argv[optind++][1];

    switch (opt) {

    case 'l':
      type = "GLib";
      break;
    case 'L':
      type = "libr";
      break;
    case 't':
      type = argv[optind++];
      break;
    case '?':
    default :
      fprintf(stderr, "%s Unknown option\n", argv[0]);
      exit (2);
      break;
    }
  }

  if (!(pf = openw_pfd(argv[optind + 1], argv[optind + 2], argv[optind], type))) {
    printf ("can't open prc file\n");
    exit(2);
  }

  /* the .prc write routines write the resources in reverse order! */

  for (i=argc-1; i > optind + 2; i--) {
    if ((fd = open(argv[i], O_PLATFORM | O_RDONLY)) < 1) {
      fprintf(stderr, "Can't open resource file %s\n",argv[i]);
      exit(3);
    }

    slen = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    sd = malloc(slen);
    read(fd, sd, slen);
    close (fd);

    add_section_pfd(pf, argv[i], strtoul(argv[i] + 4, (void *)0, 16),
		    sd, slen);
  }

  write_pfd(pf);
  exit(0);
}

