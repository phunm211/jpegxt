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
** This class represents multiple boxes that all contain specifications
** on the DCT process. These boxes are only used by part-8.
**
** $Id: dctbox.cpp,v 1.3 2014/09/30 08:33:14 thor Exp $
**
*/

/// Includes
#include "boxes/box.hpp"
#include "boxes/dctbox.hpp"
#include "io/bytestream.hpp"
#include "io/memorystream.hpp"
///

/// DCTBox::ParseBoxContent
// Second level parsing stage: This is called from the first level
// parser as soon as the data is complete. Must be implemented
// by the concrete box. Returns true in case the contents is
// parsed and the stream can go away.
bool DCTBox::ParseBoxContent(class ByteStream *stream,UQUAD boxsize)
{
  LONG v;
  UBYTE t;

  if (boxsize != 1)
    JPG_THROW(MALFORMED_STREAM,"DCTBox::ParseBoxContent","Malformed JPEG stream - size of the DCT box is incorrect");

  v = stream->Get();

  t = v >> 4; // The DCT type.

  if (t != FDCT && t != IDCT && t != Bypass)
    JPG_THROW(MALFORMED_STREAM,"DCTBox::ParseBoxContent","Malformed JPEG stream - invalid DCT specified");

  m_ucDCTType = t;

  t = v & 0x0f;

  if (t != 0 && t != 1)
    JPG_THROW(MALFORMED_STREAM,"DCTBox::ParseBoxContent","Malformed JPEG stream - invalid noise shaping specified");

  if (t && m_ucDCTType != Bypass)
    JPG_THROW(MALFORMED_STREAM,"DCTBox::ParseBoxContent",
              "Malformed JPEG stream - cannot enable noise shaping without bypassing the DCT");

  m_bNoiseShaping = (t != 0)?true:false;

  return true;
}
///

/// DCTBox::CreateBoxContent
// Second level creation stage: Write the box content into a temporary stream
// from which the application markers can be created.
// Returns whether the box content is already complete and the stream
// can go away.
bool DCTBox::CreateBoxContent(class MemoryStream *target)
{
  assert(!(m_bNoiseShaping && m_ucDCTType != Bypass));

  target->Put((m_ucDCTType << 4) | (m_bNoiseShaping?1:0));

  return true;
}
///
