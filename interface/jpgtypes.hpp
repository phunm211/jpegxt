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
 * System independent type definitions.
 * $Id: jpgtypes.hpp,v 1.9 2014/09/30 08:33:17 thor Exp $
 *
 * The following header defines basic types to be used in the JPG interface
 * routines.
 *
 * These are external definitions, visible to the outher world and part of
 * the jpeg interface.
 */


#ifndef JPGTYPES_HPP
#define JPGTYPES_HPP

#include "config.h"
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

/// Elementary types
#if defined(HAS_INT32_T) && defined(HAS_UINT32_T)
typedef int32_t           JPG_LONG;
typedef uint32_t          JPG_ULONG;
#else
# if SIZEOF_LONG == 4
typedef signed long int   JPG_LONG;    /* an 32 bit signed integer */
typedef unsigned long int JPG_ULONG;   /* an 32 bit unsigned integer */
# elif SIZEOF_INT == 4
typedef signed int        JPG_LONG;    /* an 32 bit signed integer */
typedef unsigned int      JPG_ULONG;   /* an 32 bit unsigned integer */
# else
#  error "No 32 bit integer type available"
# endif
#endif
///

// Floating point types, available by ANSI.
// Precision doesn't matter too much
typedef float             JPG_FLOAT;
///

/// Convenience boolean types
#define JPG_TRUE  (1)
#define JPG_FALSE (0)
///

/// Limits
/* Limits of the types defined above. It is rather important that
 * the specific implementation meets these limits.
 */

// Define the following only if not yet defined:
#define JPG_MIN_LONG ((JPG_LONG)(-0x80000000L))
#define JPG_MAX_LONG ((JPG_LONG)(0x7fffffffL))

#define JPG_MIN_ULONG ((JPG_ULONG)(0x00000000UL))
#define JPG_MAX_ULONG ((JPG_ULONG)(0xffffffffUL))
///

/// Pointers
// The next one defines a "generic" pointer ("A PoiNTeR").
#ifndef JPG_NOVOIDPTR
typedef void *JPG_APTR;
typedef const void *JPG_CPTR;
#else
#define JPG_APTR void *
#define JPG_CPTR const void *
#endif
///

///
#endif

