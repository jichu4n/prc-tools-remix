/* obj-res.c:  Dump out .prc compatible binary resource files from an object
 *
 * (c) 1996, 1997 Dionne & Associates
 * jeff@ryeham.ee.ryerson.ca
 *
 * This is Free Software, under the GNU Public Licence v2 or greater.
 *
 * Relocation added March 1997, Kresten Krab Thorup 
 * krab@california.daimi.aau.dk
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>

#ifndef PARAMS
#define PARAMS(x) x
#endif

#include <bfd.h>
#include "prc.h"

#ifdef __CYGWIN32__
#define O_PLATFORM O_BINARY
#else
#define O_PLATFORM 0
#endif

static char res_ext[] = ".grc";

static unsigned char exec_magic[] = { 0, 0, 0, 1 };
static unsigned char code_zero[] = { 0, 0,    0, 0x28,
				     0, 0,    0,    0, 
				     0, 0,    0,    8,
				     0, 0,    0, 0x20,
				     0, 0, 0x3f, 0x3c,
				     0, 1, 0xa9, 0xf0 };

static unsigned char pref[] = { 00, 0x1e, 00, 00, 0x10, 00, 00, 00, 0x10, 00 };

static unsigned char *compress_data(unsigned char *raw,
				    long data_len,
				    long bss_len,
				    long *comp_len)
{
  unsigned char *dp;
  unsigned char *wp;
  int block_len;
  int count;
  int total = 0;

  wp = dp = malloc(data_len * 2 + 64);  /* data could in theory grow a lot */

  /* Thanks to Darrin Massena for this algorithm */

  *(unsigned int *)wp = htonl(data_len);
  wp += 4;
  total += 4;

  if ((data_len + bss_len) > 0x7ffc) {
    fprintf(stderr, "error: .data and .bss too large for data #0 resource\n");
    exit(1);
  }

  /* A5 offset */
  *(unsigned int *)wp = htonl(-(((data_len + bss_len) + 3) & 0x7ffc));

  wp += 4;
  total += 4;

  count = data_len;
  while(count) {

    block_len = (count < 0x80) ? count : 0x80;
    *(wp++) = (block_len - 1) | 0x80;
    total++;

    memcpy(wp, raw, block_len);

    wp +=    block_len;
    raw +=   block_len;
    total += block_len;
    count -= block_len;
  }

  /* 3 separator bytes, and 2 more A5 offsets, all 0, = 11 more bytes */

  memset(wp, 0, 11);
  total += 11;

  /* 6 longs of 0 for future compatibility with MW relocation extensions */

  memset(wp, 0, 6*4);
  total += 6*4;

  *comp_len = total;
  return dp;
}

struct pilot_reloc {
  unsigned char  type;
  unsigned char  section;  
  unsigned short offset;
           long  value ;
} ;

#define TEXT_SECTION 't'
#define DATA_SECTION 'd'
#define BSS_SECTION  'b'

#define RELOC_ABS_32       0XBe

asymbol**
get_symbols (bfd *abfd, long *num)
{
  long storage_needed;
  asymbol **symbol_table;
  long number_of_symbols;
  long i;
  
  storage_needed = bfd_get_symtab_upper_bound (abfd);
	  
  if (storage_needed < 0)
    abort ();
      
  if (storage_needed == 0) {
    return ;
  }

  symbol_table = (asymbol **) malloc (storage_needed);

  number_of_symbols = bfd_canonicalize_symtab (abfd, symbol_table);
  
  if (number_of_symbols < 0) 
    abort ();

  *num = number_of_symbols;
  return symbol_table;
}
     
int
output_relocs (bfd *input_bfd, char *name, asymbol **symbols)
{
  int fd;

  asection *data_section = bfd_get_section_by_name (input_bfd, ".data");
  asection *bss_section = bfd_get_section_by_name (input_bfd, ".bss");
  asection *text_section = bfd_get_section_by_name (input_bfd, ".text");

  /* we only support relocations in the data section */
  asection *input_section = data_section;
  long reloc_size = bfd_get_reloc_upper_bound (input_bfd, input_section);
  arelent **reloc_vector = NULL;
  long reloc_count, count;

  struct pilot_reloc *pilot_relocs = (struct pilot_reloc*)malloc ((input_section->reloc_count + 1)
					  * sizeof (struct pilot_reloc));
  long num_prelocs = 0;

  if (reloc_size < 0)
    return;

  reloc_vector = (arelent **) malloc ((size_t) reloc_size);
  if (reloc_vector == NULL && reloc_size != 0)
    abort ();

  reloc_count = bfd_canonicalize_reloc (input_bfd,
					input_section,
					reloc_vector,
					symbols);
  if (reloc_count < 0)
    abort ();

  for (count = 0; count < reloc_count; count++)
    {
      arelent *r = &(*reloc_vector)[count];
      asymbol *s = *r->sym_ptr_ptr;
      struct pilot_reloc  *p = &pilot_relocs[num_prelocs];

#ifdef DEBUG
      printf ("DATA %s: ", r->howto->name);
      printf ("addr=0x%x ", r->address);
      printf ("sym=%s+%x", s->name, s->value);
      printf ("\n");
#endif

      if (!strncmp (r->howto->name, "32", 2))
	{
	  pilot_relocs[num_prelocs].type = RELOC_ABS_32;
	  num_prelocs += 1;
	}
      else
	{
	  fprintf (stderr, "warning: skipping relocation");
	  fprintf (stderr, "DATA %s: ", r->howto->name);
	  fprintf (stderr, "addr=0x%x ", r->address);
	  fprintf (stderr, "sym=%s+%x", s->name, s->value);
	  fprintf (stderr, "\n");
	  continue;
	}

      /* offset into data section... */
      p->offset = htons (r->address - data_section->vma); 

      /* store the value of the symbol */
      p->value   = htonl (s->value);

      /* store value of... */
      if (s->section == text_section)
	p->section = TEXT_SECTION;

      else if (s->section == data_section)
	p->section = DATA_SECTION;

      else if (s->section == bss_section)
	p->section = BSS_SECTION;

      else
	abort ();
    }

  if ((fd = open (name, O_WRONLY | O_PLATFORM | O_CREAT | O_TRUNC, 0644)) < 0) {
    fprintf (stderr, "Can't open output file %s\n", name);
    exit(4);
  }

  {
    short size = htons ((short) num_prelocs);

    write(fd, &size, 2);

    write(fd, &pilot_relocs[0], sizeof (struct pilot_reloc) * num_prelocs);
    close(fd);
  }
}

/* shared lib symbols stuff */

long
get_symbol(char *name, asection *sec, asymbol **symbol_table, long number_of_symbols)
{
  long i;
  for (i=0; i<number_of_symbols; i++) {
    if (symbol_table[i]->section == sec) {
      if (!strcmp(symbol_table[i]->name, name)) {
	return symbol_table[i]->value;
      }
    }
  }
  return -1;
}  

int
output_offset_table(int fd, char *ename, bfd *abfd, asymbol **symbol_table, long number_of_symbols)
{
  long i;
  FILE *ef;
  char buf[80];
  char libname[80];
  long etext_addr;
  long sym_addr;

  int foobar = 0;
  int count = 0;
  signed short *tab = malloc(32768); /* we don't know how many yet*/

  asection *text_section = bfd_get_section_by_name (abfd, ".text");

  if (!(ef = fopen(ename, "r"))) {
    fprintf (stderr,"Can't open %s\n",ename);
    exit(1);
  }

  fgets(libname, 80, ef);

  if (number_of_symbols < 0) {
    fprintf (stderr,"Corrupt symbol table!\n");
    exit(1);
  }

  if ((etext_addr = get_symbol("etext",
			       text_section,
			       symbol_table,
			       number_of_symbols)) == -1) {
    fprintf (stderr,"Can't find the symbol etext\n");
    exit(1);
  }

  fgets(buf, 80, ef);
  while (!feof(ef)) {
    buf[strlen(buf)-1] = 0; /* Arrrgh! linefeeds */

    if ((sym_addr = get_symbol(buf,
			       text_section,
			       symbol_table,
			       number_of_symbols)) == -1) {
      fprintf (stderr,"Can't find the symbol %s\n",buf);
      foobar++;
    } else {
      tab[++count] = htons(sym_addr - etext_addr);
    }
    fgets(buf, 80, ef);
  }

  fclose(ef);

  if (foobar) {
    fprintf (stderr,"*** %d symbols not found\n",foobar);
    exit(10);
  }

  strcpy((char *)&tab[++count],libname);
  tab[0] = htons(count * 2);
  write(fd, tab, count * 2 + strlen(libname) + 2);
  return 0;
}

main(int argc, char *argv[])
{
  int fd;
  bfd *bf;
  asection *s;
  char *ofile;
  char *fname = argv[argc - 1];
  int opt;
  int type;

  char *code0res="code0000.";
  char *code1res="code0001.";
  char *data0res="data0000.";
  char *rloc0res="rloc0000.";
  char *pref0res="pref0000.";

  asymbol **symbol_table;
  long number_of_symbols;

  unsigned char *data;
  unsigned char *comp;
  unsigned char *text;

  unsigned long data_len;
  unsigned long bss_len;
  unsigned long text_len;
  unsigned long comp_len;

  unsigned long data_vma;
  unsigned long bss_vma;
  unsigned long text_vma;

  if (argc < 2 || argc > 4 ) {
    fprintf(stderr, "Usage: %s [-l] [-L export.file] bfd.file\n",argv[0]);
    exit(1);
  }

  while ((opt = getopt(argc, argv, "lL")) != -1) {
    switch (opt) {

    case 'l':
      type = 1;
      code0res=(void *)0;
      code1res="GLib0000.";
      pref0res=(void *)0;
      break;
    case 'L':
      type = 2;
      code0res=(void *)0;
      code1res="libr0000.";
      data0res=(void *)0;
      rloc0res=(void *)0;
      pref0res=(void *)0;
      break;
    default:
      fprintf(stderr, "%s Unknown option\n", argv[0]);
      exit (2);
      break;
    }
  }

  if (!(bf = bfd_openr(argv[argc -1],0))) {
    fprintf (stderr,"Can't open %s\n",argv[argc -1]);
    exit(1);
  }

  if (bfd_check_format (bf, bfd_object) == 0) {
    printf("File is not an object file\n");
    exit(2);
  }

  symbol_table = get_symbols(bf, &number_of_symbols);

  s = bfd_get_section_by_name (bf, ".text");
  text_vma = s->vma;
  text_len = s->_raw_size + 4;  /* add space for that stupid 0 0 0 1 */
  text = malloc(text_len);
  memcpy(text, exec_magic, sizeof(exec_magic));

  if (bfd_get_section_contents(bf,
			       s, 
			       text + 4,
			       0,
			       s->_raw_size) == false) {
    fprintf(stderr, "read error section %s\n", s->name);
    exit(2);
  }

  s = bfd_get_section_by_name (bf, ".data");
  data = malloc(s->_raw_size + 32); /* Allows for up to 32 byte align */
  data_vma = s->vma;

  if (bfd_get_section_contents(bf,
			       s, 
			       data,
			       0,
			       s->_raw_size) == false) {
    fprintf(stderr, "read error section %s\n", s->name);
    exit(2);
  }

  s = bfd_get_section_by_name (bf, ".bss");
  bss_len = s->_raw_size;
  data_len = s->vma - data_vma;
  bss_vma = s->vma;

#ifdef DEBUG
  printf("0x%.4x .text, 0x%.4x .data, 0x%.4x bss\n",text_len,data_len,bss_len);
#endif

  comp = compress_data(data, data_len, bss_len, &comp_len);

  /* fill in the code #0 thing.  I still don't really know what to do here.
     Truth be known, I think it's mostly bogus. */

  *(unsigned int *)(&code_zero[4])=htonl(((data_len + bss_len) + 3) & 0x7ffc);

  ofile = malloc(strlen(fname) + 15); /* 15 to add prefixes and suffixes */

  if (code0res) {
    strcpy(ofile, code0res);
    strcat(ofile, fname);
    strcat(ofile, res_ext);

    if ((fd = open (ofile, O_WRONLY | O_PLATFORM | O_CREAT | O_TRUNC, 0644)) < 0) {
      fprintf (stderr, "Can't open output file %s\n", ofile);
      exit(4);
    }

    write(fd, code_zero, sizeof(code_zero));
    close(fd);
  }

  if (data0res) {
    strcpy(ofile, data0res);
    strcat(ofile, fname);
    strcat(ofile, res_ext);

    if ((fd = open (ofile, O_WRONLY | O_PLATFORM | O_CREAT | O_TRUNC, 0644)) < 0) {
      fprintf (stderr, "Can't open output file %s\n", ofile);
      exit(4);
    }

    write(fd, comp, comp_len);
    close(fd);
  }

  if (pref0res) {
    strcpy(ofile, pref0res);
    strcat(ofile, fname);
    strcat(ofile, res_ext);

    if ((fd = open (ofile, O_WRONLY | O_PLATFORM | O_CREAT | O_TRUNC, 0644)) < 0) {
      fprintf (stderr, "Can't open output file %s\n", ofile);
      exit(4);
    }

    write(fd, pref, 10);
    close(fd);
  }

  if (rloc0res) {
    /* generate data-relocations */
    strcpy(ofile, rloc0res);
    strcat(ofile, fname);
    strcat(ofile, res_ext);

    output_relocs (bf, ofile, symbol_table);
  }

  if (code1res) {
    strcpy(ofile, code1res);
    strcat(ofile, fname);
    strcat(ofile, res_ext);

    if ((fd = open (ofile, O_WRONLY | O_PLATFORM | O_CREAT | O_TRUNC, 0644)) < 0) {
      fprintf (stderr, "Can't open output file %s\n", ofile);
      exit(4);
    }

    write(fd, text, text_len);

    /* shared lib table generation */
    if (type == 2) {
      output_offset_table(fd, argv[2], bf, symbol_table, number_of_symbols);
    }
    close(fd);
  }
  exit(0);
}
