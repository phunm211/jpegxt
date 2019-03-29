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
** This class represents the adobe color specification marker, placed
** in APP14. Used here to indicate the color space and to avoid a color
** transformation.
**
** $Id: adobemarker.cpp,v 1.9 2014/09/30 08:33:17 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "io/bytestream.hpp"
#include "marker/adobemarker.hpp"
///

/// AdobeMarker::AdobeMarker
AdobeMarker::AdobeMarker(class Environ *env)
  : JKeeper(env)
{
}
///

/// AdobeMarker::~AdobeMarker
AdobeMarker::~AdobeMarker(void)
{
}
///

/// AdobeMarker::WriteMarker
// Write the marker to the stream.
void AdobeMarker::WriteMarker(class ByteStream *io)
{
  UWORD len = 2 + 5 + 2 + 2 + 2 + 1;
  const char *id = "Adobe";

  io->PutWord(len);

  // Identifier code: ASCII "Adobe"
  while(*id) {
    io->Put(*id);
    id++;
  }

  io->PutWord(100); // version
  io->PutWord(0);   // flags 0
  io->PutWord(0);   // flags 1
  io->Put(m_ucColorSpace);
}
///

/// AdobeMarker::ParseMarker
// Parse the adobe marker from the stream
// This will throw in case the marker is not
// recognized. The caller will have to catch
// the exception.
void AdobeMarker::ParseMarker(class ByteStream *io,UWORD len)
{
  UWORD version;
  LONG color;

  if (len != 2 + 5 + 2 + 2 + 2 + 1)
    JPG_THROW(MALFORMED_STREAM,"AdobeMarker::ParseMarker","misformed Adobe marker");

  version = io->GetWord();
  if (version != 100) // Includes EOF
    JPG_THROW(MALFORMED_STREAM,"AdobeMarker::ParseMarker","Adobe marker version unrecognized");

  io->GetWord();
  io->GetWord(); // ignored.

  color  = io->Get();

  if (color < 0 || color > Last)
    JPG_THROW(MALFORMED_STREAM,"AdobeMarker::ParseMarker","Adobe color information unrecognized");

  m_ucColorSpace = color;
}
///
