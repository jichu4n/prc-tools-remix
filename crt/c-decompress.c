void decompress(unsigned char *dataP, void *glob, long datasize) {
    unsigned char *readp;
    unsigned char *writep;
    int i;

    /* Decompress the data segment */
    readp = dataP + 4;
    for(i=0;i<3;++i) {
	unsigned long startat = *(readp++);
	startat <<= 8; startat += *(readp++);
	startat <<= 8; startat += *(readp++);
	startat <<= 8; startat += *(readp++);
	writep = ((unsigned char *)(glob)) + datasize + (long)startat;
	while(*readp) {
	    if (*readp & 0x80) {
		unsigned char j;
		unsigned char len = (*(readp++) & 0x7f);
		++len;
		for(j=0;j<len;++j) {
		    *(writep++) = *(readp++);
		}
	    } else if (*readp & 0x40) {
		unsigned char len = (*(readp++) & 0x3f);
		++len;
		writep += len;
	    } else if (*readp & 0x20) {
		unsigned char j;
		unsigned char len = (*(readp++) & 0x1f);
		unsigned char b = *(readp++);
		len += 2;
		for(j=0;j<len;++j) {
		    *(writep++) = b;
		}
	    } else if (*readp & 0x10) {
		unsigned char j;
		unsigned char len = (*(readp++) & 0x0f);
		++len;
		for(j=0;j<len;++j) {
		    *(writep++) = 0xff;
		}
	    } else if (*readp == 1) {
		writep += 4;
		*(writep++) = 0xff;
		*(writep++) = 0xff;
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
	    } else if (*readp == 2) {
		writep += 4;
		*(writep++) = 0xff;
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
	    } else if (*readp == 3) {
		*(writep++) = 0xa9;
		*(writep++) = 0xf0;
		writep += 2;
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
		writep += 1;
		*(writep++) = *(readp++);
	    } else if (*readp == 4) {
		*(writep++) = 0xa9;
		*(writep++) = 0xf0;
		writep += 1;
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
		writep += 1;
		*(writep++) = *(readp++);
	    }
	}
	++readp;
    }
  }
