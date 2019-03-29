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
** This box keeps all the information for opacity coding, the alpha mode
** and the matte color.
**
** $Id: alphabox.cpp,v 1.3 2016/01/22 10:09:54 thor Exp $
**
*/

/// Includes
#include "boxes/box.hpp"
#include "boxes/alphabox.hpp"
#include "io/bytestream.hpp"
#include "io/memorystream.hpp"
///

/// AlphaBox::ParseBoxContent
// Second level parsing stage: This is called from the first level
// parser as soon as the data is complete. Must be implemented
// by the concrete box. Returns true in case the contents is
// parsed and the stream can go away.
bool AlphaBox::ParseBoxContent(class ByteStream *stream,UQUAD boxsize)
{
  UBYTE mode1,mode2;
  
  if (boxsize != 2 + 4 * 2)
    JPG_THROW(MALFORMED_STREAM,"AlphaBox::ParseBoxContent",
              "Malformed JPEG stream, the alpha channel composition box size is invalid");

  mode1 = stream->Get();
  mode2 = stream->Get();

  if ((mode1 >> 4) > MatteRemoval)
    JPG_THROW(MALFORMED_STREAM,"AlphaBox::ParseBoxContent",
              "Malformed JPEG stream, the alpha composition method is invalid");

  m_ucAlphaMode = (mode1 >> 4);

  if (((mode1 & 0x0f) != 0) || mode2 != 0)
    JPG_THROW(MALFORMED_STREAM,"AlphaBox::ParseBoxContent",
              "Malformed JPEG stream, found invalid values for reserved fields");

  m_ulMatteRed   = stream->GetWord();
  m_ulMatteGreen = stream->GetWord();
  m_ulMatteBlue  = stream->GetWord();
  
  stream->GetWord();

  return true;
}
///

/// AlphaBox::CreateBoxContent
// Second level creation stage: Write the box content into a temporary stream
// from which the application markers can be created.
// Returns whether the box content is already complete and the stream
// can go away.
bool AlphaBox::CreateBoxContent(class MemoryStream *target)
{
  //
  // First write the mode bytes.
  target->Put(m_ucAlphaMode << 4);
  target->Put(0);
  //
  // Write the matte colors.
  target->PutWord(m_ulMatteRed);
  target->PutWord(m_ulMatteGreen);
  target->PutWord(m_ulMatteBlue);
  target->PutWord(0);

  return true;
}
///
