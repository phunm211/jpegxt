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
** This box keeps the file type and compatible file types for JPEG XT.
** It is basically a profile information.
**
** $Id: filetypebox.hpp,v 1.5 2015/03/11 16:02:38 thor Exp $
**
*/

#ifndef BOXES_FILETYPEBOX_HPP
#define BOXES_FILETYPEBOX_HPP

/// Includes
#include "boxes/box.hpp"
///

/// FileTypeBox
// This box keeps the file type and compatible file types for JPEG XT.
// It is basically a profile information.
class FileTypeBox : public Box {
  //
  // The brand type. This must be jpxt
  ULONG m_ulBrand;
  //
  // Minor version.
  ULONG m_ulMinor;
  //
  // Size of the compatibility list.
  ULONG m_ulNumCompats;
  //
  // The compatibility list.
  ULONG *m_pulCompatible; 
  //
  // Second level parsing stage: This is called from the first level
  // parser as soon as the data is complete. Must be implemented
  // by the concrete box. Returns true in case the contents is
  // parsed and the stream can go away.
  virtual bool ParseBoxContent(class ByteStream *stream,UQUAD boxsize);
  //
  // Second level creation stage: Write the box content into a temporary stream
  // from which the application markers can be created.
  // Returns whether the box content is already complete and the stream
  // can go away.
  virtual bool CreateBoxContent(class MemoryStream *target);
  //
public:
  enum {
    Type          = MAKE_ID('f','t','y','p'), // box type
    XT_Brand      = MAKE_ID('j','p','x','t'),
    XT_IDR        = MAKE_ID('i','r','f','p'), // intermediate range coding following 18477-6
    XT_HDR_A      = MAKE_ID('x','r','d','d'), // Profile A of 18477-7
    XT_HDR_B      = MAKE_ID('x','r','x','d'), // Profile B of 18477-7
    XT_HDR_C      = MAKE_ID('x','r','a','d'), // Profile C of 18477-7
    XT_HDR_D      = MAKE_ID('x','r','r','f'), // Profile D of 18477-7
    XT_LS         = MAKE_ID('l','s','f','p'), // lossless range coding following 18477-8
    XT_ALPHA_FULL = MAKE_ID('a','c','f','p'), // alpha coding full profile, 18477-9
    XT_ALPHA_BASE = MAKE_ID('a','c','b','p')  // alpha coding base profile, 18477-9
  };
  //
  FileTypeBox(class Environ *env,class Box *&boxlist)
    : Box(env,boxlist,Type), m_ulBrand(XT_Brand), m_ulMinor(0),
      m_ulNumCompats(0), m_pulCompatible(NULL)
  { }
  //
  virtual ~FileTypeBox(void);
  //
  // Add an entry to the compatibility list.
  void addCompatibility(ULONG compat);
  //
  // Check whether this file is compatible to the listed profile ID.
  // It is always understood to be compatibile to the brand, otherwise reading
  // would fail.
  bool isCompatbileTo(ULONG compat) const;
  //
};
///

///
#endif
