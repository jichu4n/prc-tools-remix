/* 
   Copyright 1996 Thruput Data Systems, Inc.
   Written by: Andy Sackheim
   Report bugs to: andys@tds-inc.com
   Relased to the public domain through the GNU license program.
   All standard GNU license conditions apply.

   D.Jeff Dionne, changed byte swap macros Feb 1997

   This program can be compiled with cc -o txt2bitm txt2bitm.c
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>

#ifdef CYGWIN32
#define O_PLATFORM O_BINARY
#else
#define O_PLATFORM 0
#endif

#define TRUE 		1
#define FALSE 		0
#define YES			1
#define NO			0
#define LF			0x0a
#define ERROR		-1
#define BMSIZE		163


char Array[BMSIZE][(BMSIZE / 8) + 1];	/* Keep it off the stack */
char Buffer[BMSIZE + 5];
char Program[100];
int Verbose;
int Id;
char Type[10];

/* Pilot bitmap format (also format of icon) */

struct BITMAPHDR
  {
    unsigned short width;
    unsigned short height;
    unsigned short chars_row;
    unsigned short fieldflags;
    unsigned short pad[4];	/* Long alignment */
  };

main (argc, argv, envp)
     int argc;
     char *argv[];
     char *envp[];
{
  char fname[100];
  char temp[50];
  int i;

  Id = 1000;
  strcpy (Type, "tAIB");

  strcpy (Program, *argv++);
  argc--;

  for (i = 0; i < argc; i++, argv++)
    {
      strcpy (temp, *argv);

      if (temp[0] != '-')	/* No more arguments */
	break;

      switch (temp[1])
	{
	case 'V':
	case 'v':
	  Verbose = TRUE;
	  break;

	case 'I':		/* Resource Id */
	case 'i':
	  Id = atoi (&temp[2]);
	  break;

	case 'T':		/* Type of Resource */
	case 't':
	  strncpy (Type, &temp[2], 4);
	  break;

	default:
	  printf ("Bad option '%s' (ignored)\n", *argv);
	  break;
	}
    }

  argc -= i;

  if (argc != 1)
    usage ();

  strcpy (fname, *argv++);
  if (process (fname) && Verbose)
    printf ("Sucessfully procesed: %s\n", fname);
  else if (Verbose)
    printf ("Error processing: %s\n", fname);
  exit (0);
}

int
process (fname)
     char *fname;
{
  FILE *fd;
  int ofd;
  int maxwidth;
  int height;
  int cperrow;
  int packwidth;
  int packcntr;
  int i;
  char ofname[100];
  struct BITMAPHDR hdr;
  struct BITMAPHDR *bm;

  bm = &hdr;

  memset (bm, 0, sizeof (struct BITMAPHDR));
  cperrow = height = maxwidth = 0;

  sprintf (ofname, "%s%04x.bin", Type, Id);

  if ((ofd = open (ofname, (O_PLATFORM | O_RDWR | O_CREAT | O_TRUNC), 0644)) == ERROR)
    {
      printf ("Error opening output file %s\n", ofname);
      return (FALSE);
    }

  if ((fd = fopen (fname, "r")) == NULL)
    {
      printf ("Can't open input file '%s'", fname);
      return (FALSE);
    }

  while (fgets (Buffer, sizeof (Buffer) - 1, fd) != 0L)
    {
      if (height >= BMSIZE)
	{
	  printf ("%s: Height exceeded %s. Truncated.\n", Program, BMSIZE);
	  break;
	}

      packwidth = packcntr = 0;
      for (i = 0; i < BMSIZE; i++)	/* Could break early */
	{
	  if (Buffer[i] == LF)
	    break;

	  Array[height][packcntr] <<= 1;
	  if (Buffer[i] != ' ' && Buffer[i] != '-')	/* Blank & dash = white */
	    Array[height][packcntr] |= 1;

	  if (++packwidth >= 8)	/* Move to Next char */
	    {
	      packwidth = 0;
	      packcntr++;
	    }
	}

      height++;
      if (i > maxwidth)		/* Pick up max excursion */
	maxwidth = i;
    }
  cperrow = maxwidth >> 3;	/* Char per row = maxwidth / 8 */

  bm->width = htons (maxwidth);
  bm->height = htons (height);
  bm->chars_row = htons (cperrow);

  if (write (ofd, (char *) bm, sizeof (struct BITMAPHDR)) !=
      sizeof (struct BITMAPHDR))
    {
      printf ("%s: Error writing bit map header\n", Program);
      close (ofd);
      return (FALSE);
    }

  for (i = 0; i < height; i++)
    {
      if (write (ofd, &Array[i], cperrow) != cperrow)
	{
	  printf ("%s: Error writing bit data row %d\n", Program, i);
	  close (ofd);
	  return (FALSE);
	}
    }

  close (ofd);
  return (TRUE);
}

usage ()
{
  printf ("%s Usage: -I<resource id> -T<resource type> input file\n", Program);
  printf ("Default resource id 1000, default Type tAIB (icon)\n");
  exit (1);
}
