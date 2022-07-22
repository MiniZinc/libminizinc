/*
ccommon.h - common header for encoding and decoding algorithm

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#ifndef BASE64_CCOMMON_H
#define BASE64_CCOMMON_H

#define BASE64_VER_MAJOR	2
#define BASE64_VER_MINOR	0

#ifndef HAVE_SIZE_T
  #ifdef _WIN32
    #include <crtdefs.h>
  #elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
    #include <stdlib.h>
  #else
    typedef unsigned long size_t;
  #endif
#endif

#endif /* BASE64_CCOMMON_H */
