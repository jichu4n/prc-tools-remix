#ifndef STANDALONE_H
#define STANDALONE_H

#define STANDALONE_CODE_RESOURCE_ID(id) \
  static unsigned short _disposition \
    __attribute__ ((__section__ (".disposn"), __unused__)) = (id)

/* FIXME Ideally STANDALONE_CODE_RESOURCE_TYPE_ID could detect which of
   'TYPE' and "TYPE" it is given and evaluate it accordingly.  This will
   become possible with GCC 3 and its __builtin_choose_expr; in the meantime,
   we provide two separate macros and depend on the user.  */

#define _standalone_unpack(x, d)  (((unsigned long)(x)) >> (8 * d)) & 0xff

#define STANDALONE_CODE_RESOURCE_TYPE_ID(type, id) \
  static struct { unsigned short _id; unsigned char _type[4]; } _disposition \
    __attribute__ ((__section__ (".disposn"), __unused__)) = { (id), \
      { _standalone_unpack ((type), 3), _standalone_unpack ((type), 2), \
	_standalone_unpack ((type), 1), _standalone_unpack ((type), 0) } }

#define STANDALONE_CODE_RESOURCE_TYPESTR_ID(type, id) \
  static struct { unsigned short _id; char _type[4]; } _disposition \
    __attribute__ ((__section__ (".disposn"), __unused__)) = { (id), (type) }

#define HACKMASTER_TRAP(vector) \
  static unsigned short _trap_vector \
    __attribute__ ((__section__ (".trap"), __unused__)) = (vector)

#endif
