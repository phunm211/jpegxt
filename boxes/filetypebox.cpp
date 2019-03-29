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
** $Id: filetypebox.cpp,v 1.3 2014/09/30 08:33:14 thor Exp $
**
*/

/// Includes
#include "boxes/box.hpp"
#include "boxes/filetypebox.hpp"
#include "io/bytestream.hpp"
#include "io/memorystream.hpp"
///

/// FileTypeBox::~FileTypeBox
// Destroy a file type box
FileTypeBox::~FileTypeBox(void)
{
  if (m_pulCompatible) {
    m_pEnviron->FreeMem(m_pulCompatible,sizeof(ULONG) * m_ulNumCompats);
  }
}
///

/// FileTypeBox::ParseBoxContent
// Second level parsing stage: This is called from the first level
// parser as soon as the data is complete. Must be implemented
// by the concrete box. Returns true in case the contents is
// parsed and the stream can go away.
bool FileTypeBox::ParseBoxContent(class ByteStream *stream,UQUAD boxsize)
{
  LONG lo,hi;
  ULONG *cmp,cnt;
  //
  if (boxsize < 4 + 4) // at least brand and minor version must be there.
    JPG_THROW(MALFORMED_STREAM,"FileTypeBox::ParseBoxContent",
              "Malformed JPEG stream - file type box is too short to contain brand and minor version");
  //
  // Also, refuse too long boxes.
  if (boxsize > MAX_LONG / sizeof(LONG))
    JPG_THROW(MALFORMED_STREAM,"FileTypeBox::ParseBoxContent",
              "Malformed JPEG stream - file type box is too long or length is invalid");
  //
  assert(m_pulCompatible == NULL);
  //
  hi = stream->GetWord();
  lo = stream->GetWord();
  //
  // Check whether the brand is ok.
  if (((hi << 16) | lo) != XT_Brand)
    JPG_THROW(MALFORMED_STREAM,"FileTypeBox::ParseBoxContent",
              "Malformed JPEG stream - file is not compatible to JPEG XT and cannot be read by this software");
  //
  // Ignore the minor version.
  stream->GetWord();
  stream->GetWord();
  //
  // Remove bytes read so far.
  boxsize -= 4 + 4;
  //
  // Now check the number of entries.
  if (boxsize & 3)
    JPG_THROW(MALFORMED_STREAM,"FileTypeBox::ParseBoxContent",
              "Malformed JPEG stream - number of compatibilities is corrupted, "
              "box size is not divisible by entry size");
  //
  cnt = boxsize >> 2;
  m_ulNumCompats  = cnt;
  m_pulCompatible = (ULONG *)m_pEnviron->AllocMem(sizeof(ULONG) * cnt);
  cmp = m_pulCompatible;
  while(cnt) {
    hi     = stream->GetWord();
    lo     = stream->GetWord();
    *cmp++ = (hi << 16) | lo;
    cnt--;
  }

  return true;
}
///

/// FileTypeBox::CreateBoxContent
// Second level creation stage: Write the box content into a temporary stream
// from which the application markers can be created.
// Returns whether the box content is already complete and the stream
// can go away.
bool FileTypeBox::CreateBoxContent(class MemoryStream *target)
{
  const ULONG *cmp = m_pulCompatible;
  ULONG cnt = m_ulNumCompats;

  target->PutWord(m_ulBrand >> 16);
  target->PutWord(m_ulBrand & 0xffff);
  target->PutWord(m_ulMinor >> 16);
  target->PutWord(m_ulMinor & 0xffff);

  while(cnt) {
    target->PutWord(*cmp >> 16);
    target->PutWord(*cmp & 0xffff);
    cmp++;
    cnt--;
  }

  return true;
}
///

/// FileTypeBox::addCompatibility 
// Add an entry to the compatibility list.
void FileTypeBox::addCompatibility(ULONG compat)
{
  ULONG newcnt = m_ulNumCompats + 1;
  ULONG *p;

  if (newcnt <= m_ulNumCompats)
    JPG_THROW(OVERFLOW_PARAMETER,"FileTypeBox::addCompatibility",
              "too many compatible brands specified, cannot add another");

  p = (ULONG *)m_pEnviron->AllocMem(sizeof(ULONG) * newcnt);

  if (m_pulCompatible && m_ulNumCompats > 0) {
    memcpy(p,m_pulCompatible,m_ulNumCompats * sizeof(ULONG));
    m_pEnviron->FreeMem(m_pulCompatible,m_ulNumCompats * sizeof(ULONG));
    m_pulCompatible = NULL;
  }

  p[m_ulNumCompats] = compat;
  m_pulCompatible   = p;
  m_ulNumCompats    = newcnt;
}
///

/// FileTypeBox::isCompatbileTo
// Check whether this file is compatible to the listed profile ID.
// It is always understood to be compatibile to the brand, otherwise reading
// would fail.
bool FileTypeBox::isCompatbileTo(ULONG compat) const
{
  const ULONG *p = m_pulCompatible;
  ULONG cnt      = m_ulNumCompats;

  if (p) {
    while(cnt) {
      if (*p == compat)
        return true;
      p++;
      cnt--;
    }
  }

  return false;
}
///
