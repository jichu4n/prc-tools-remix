/* prc.h:  Pilot resource file stuff */

/* File header */

typedef struct prc {
  char name[32];
  char c_exec[4];
  char time[12];
  char c_pad1[12];
  char c_appl[4];
  char id[4];
  char c_pad2[8];
  short n_sect;
} prc_t;

#define PRC_EXEC { 0, 1, 0, 1 }
#define PRC_PAD1 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#define PRC_APPL { 'a', 'p', 'p', 'l' }
#define PRC_PAD2 { 0x28, 0, 0, 0, 0, 0 }
#define PRC_TIME { 0xad, 0xc0, 0xbe, 0xa0, \
                   0xad, 0xc0, 0xbe, 0xa0, \
                   0x00, 0x00, 0x00, 0x00 };


/* Section header */

typedef struct sect {
  char name[4];
  short id;
  short offseth;
  short offsetl;
} sect_t;

#define PRC_SECT_PAD1 { 0, 0 }

typedef struct sdata {
  char *data;
  int len;
  struct sect *sect;
  struct sdata *next;
} sdata_t;

typedef struct pfd {
  int _fd;
  struct prc *prc;
  struct sdata *sdata;
} pfd_t;

pfd_t *openw_pfd(char *aname,
		 char *id,
		 char *fname,
                 char *type);

int write_pfd(pfd_t *pfd);

int add_section_pfd(pfd_t *pfd,
		    char *name,
		    short id,
		    char *sc,
		    int len);
