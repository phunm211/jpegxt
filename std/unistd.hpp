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
** $Id: unistd.hpp,v 1.7 2014/09/30 08:33:18 thor Exp $
*/

#ifndef UNISTD_HPP
#define UNISTD_HPP
#include "config.h"

#ifdef HAVE_BSTRINGS_H
#include <bstrings.h>
#elif defined(HAVE_BSTRING_H)
#include <bstring.h>
#endif

#ifdef HAVE_UNISTD_H

#if defined(HAVE_LLSEEK) || defined(HAVE_LSEEK64)
# ifndef _LARGEFILE64_SOURCE
#  define _LARGEFILE64_SOURCE
# endif
# include <sys/types.h>
#endif

#include <unistd.h>
// *ix compatibility hack, or rather, win compatibility hack
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifndef O_BINARY
#define O_BINARY 0
#endif
#elif defined(HAVE_IO_H)
#include <io.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#endif

#if !defined(HAVE_OPEN) || !defined(HAVE_CLOSE) || !defined(HAVE_READ) || !defined(HAVE_WRITE)
#error "POSIX io functions are not available, won't compile without"
#endif

#ifndef HAVE_SLEEP
// Dummy implemented insite.
unsigned int sleep(unsigned int seconds);
#endif

// Provide definitions for STDIN_FILENO etc if not available.
#ifndef HAS_STDIN_FILENO
# define STDIN_FILENO 0
#endif

#ifndef HAS_STDOUT_FILENO
# define STDOUT_FILENO 1
#endif

#ifndef HAS_STDERR_FILENO
# define STDERR_FILENO 2
#endif

//
// A generic 64-bit version of seek
extern long long longseek(int fd,long long offset,int whence);

#endif
