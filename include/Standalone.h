#ifndef STANDALONE_H
#define STANDALONE_H

#define STANDALONE_CODE_RESOURCE_ID(id) \
  static unsigned short _disposition \
    __attribute__ ((__section__ (".disposn"), __unused__)) = (id)

#define STANDALONE_CODE_RESOURCE_TYPE_ID(type, id) \
  static struct { unsigned short _id; char _type[4]; } _disposition \
    __attribute__ ((__section__ (".disposn"), __unused__)) = { (id), (type) }

#define HACKMASTER_TRAP(vector) \
  static unsigned short _trap_vector \
    __attribute__ ((__section__ (".trap"), __unused__)) = (vector)

#endif
