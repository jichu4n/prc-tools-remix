#ifndef SDKTYPES_H
#define SDKTYPES_H

#ifdef SDK_VERSION
  /* Define v3.5 type names for the old SDKs.  */
  #if SDK_VERSION < 30
  typedef Byte UInt8;
  typedef SByte Int8;
  typedef Word UInt16;
  typedef SWord Int16;
  typedef DWord UInt32;
  typedef SDWord Int32;
  #endif

  #if SDK_VERSION < 35
  typedef VoidHand MemHandle;
  #endif
#endif

#endif
