/*************************************************************************
** Written by Thomas Richter (THOR Software - thor@math.tu-berlin.de)   **
** Sponsored by Accusoft Corporation, Tampa, FL and                     **
** the Computing Center of the University of Stuttgart                  **
**************************************************************************

The copyright in this software is being made available under the
license included below. This software may be subject to other third
party and contributor rights, including patent rights, and no such
rights are granted under this license.
 
Copyright (c) 2013-2017, ISO
All rights reserved.

This software module was originally contributed by the parties as
listed below in the course of development of the ISO/IEC 18477 (JPEG
XT) standard for validation and reference purposes:

- University of Stuttgart
- Accusoft

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the University of Stuttgart or Accusoft nor
    the names of its contributors may be used to endorse or promote
    products derived from this software without specific prior written
    permission.
  * Redistributed products derived from this software must conform to
    ISO/IEC 18477 (JPEG XT) except that non-commercial redistribution
    for research and for furtherance of ISO/IEC standards is permitted.
    Otherwise, contact the contributing parties for any other
    redistribution rights for products derived from this software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*************************************************************************/

/*
** This is an Os abstraction of the stdlib
** include file. It might possibly contain fixes for
** various Os derivations from the intended stdlib.
**
** $Id: string.hpp,v 1.7 2014/09/30 08:33:18 thor Exp $
*/

#ifndef STRING_HPP
#define STRING_HPP
#include "config.h"

/// Standard headers
#ifdef DEPLOY_PICCLIB
# if defined(__cplusplus)
extern "C" {
# endif
# if defined(__IS_powerpc__)
#  include <string.h>
# endif
# include "pegasus/picclib.h"
# if defined(__cplusplus)
}
# endif
#else
# if HAVE_STRING_H
#  if !STDC_HEADERS && HAVE_MEMORY_H
#   include <memory.h>
#  endif
#  include <string.h>
# endif
# if HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif
///

/// strSize: XML compliant string size computation
// Compute the length of the string in bytes (without the NUL!) for
// XML encoded strings. This can be either a UTF-8 string, or a UTF-16
// string. In the latter case, the string is expected to start with a
// byte-order mark (0xfeff for big and 0xfffe for little endian).
inline static size_t strSize(const unsigned char *data)
{
  if (((data[0] == 0xff) && (data[1] == 0xfe)) ||
      ((data[0] == 0xfe) && (data[1] == 0xff))) {
    const unsigned char *p = data;
    // Repeat until a double 0 is found. Note that we cannot
    // cast to UWORD since the alignment isn't clear.
    do {
      p += 2; // The first one isn't NUL for sure!
    } while(p[0] | p[1]);
    //
    return p - data;
  } else {
    return strlen((const char *)data);
  }
}
///

/// Provide quicker replacements for memset/memcpy/memmove
#ifndef DEPLOY_PICCLIB
# ifdef HAVE_BUILTIN_MEMSET
#  undef memset
#  define memset __builtin_memset
# endif
# ifdef HAVE_BUILTIN_MEMCPY
#  undef memcpy
#  define memcpy __builtin_memcpy
# endif
# ifdef HAVE_BUILTIN_MEMMOVE
#  undef memmove
#  define memmove __builtin_memmove
# endif
#endif
///

/// Check for memchr function and provide a replacement if we don't have it.
#if !HAVE_MEMCHR
inline static void *memchr(const void *s, int c, size_t n) throw()
{
  const unsigned char *in = (const unsigned char *)s;

  if (n) {
    do {
      if (*in == c) return const_cast<void *>(s);
      in++;
    } while(--n);
  }
  return 0;
}
#endif
///

/// Check for the memmove function and provide a replacement if we don't have it.
#if !HAVE_MEMMOVE
// This is not overly efficient, but it should at least work...
inline static void *memmove(void *dest, const void *src, size_t n) throw()
{
  unsigned char       *d = (unsigned char *)dest;
  const unsigned char *s = (const unsigned char *)src;

  if (n) {
    if (s > d) {
      // Source is behind dest, copy ascending.
      do {
        *d = *s;
        d++,s++;
      } while(--n);
    } else if (s < d) {
      // Source is in front of dest, copy descending.
      s += n, d += n;
      do {
        --d,--s;
        *d = *s;
      } while(--n);
    }
  }
  return dest;
}
#endif
///

/// Check for the memset function and provide a replacement if we don't
#if !HAVE_MEMSET
inline static void *memset(void *s, int c, size_t n) throw()
{
  unsigned char *d = (unsigned char *)s;
  
  if (n) {
    do {
      *d = c;
      d++;
    } while(--n);
  }

  return const_cast<void *>(s);
}
#endif
///

/// Check for availibility of the strchr function and provide a replacement if there is none.
#if !HAVE_STRCHR
inline static char *strchr(const char *s, int c) throw()
{
  while(*s) {
    if (*s == c) return const_cast<char *>(s);
    s++;
  }
  return 0;
}
#endif
///

/// Check for strerror function
#if !HAVE_STRERROR
// we cannot really do much about it as this is Os depdendent. Yuck.
#define strerror(c) "unknown error"
#endif
///

/// Check for strrchr function and provide a replacement.
#if !HAVE_STRRCHR
inline static char *strrchr(const char *s, int c) throw()
{
  const char *t = s + strlen(s);
  
  while(t > s) {
    t--;
    if (*t == c) return const_cast<char *>(t);
  }
  return 0;
}
#endif
///

///
#endif
