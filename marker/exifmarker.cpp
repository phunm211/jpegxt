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
** This class represents the EXIF marker, placed in APP1. This
** marker is currently only a dummy and not actually used. 
**
** $Id: exifmarker.cpp,v 1.6 2014/09/30 08:33:17 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "io/bytestream.hpp"
#include "marker/exifmarker.hpp"
///

/// EXIFMarker::EXIFMarker
EXIFMarker::EXIFMarker(class Environ *env)
  : JKeeper(env)
{
}
///

/// EXIFMarker::~EXIFMarker
EXIFMarker::~EXIFMarker(void)
{
}
///

/// EXIFMarker::WriteMarker
// Write the marker to the stream.
void EXIFMarker::WriteMarker(class ByteStream *io)
{
  UWORD len = 2 + 4 + 2 + 2 + 2 + 4 + 2 + 4;
  const char *id = "Exif";

  io->PutWord(len);

  // Identifier code: ASCII "Exif"
  while(*id) {
    io->Put(*id);
    id++;
  }
  io->Put(0); // This comes with two terminating zeros.
  io->Put(0); // This comes with two terminating zeros.

  // Now a regular little-endian tiff header follows
  io->Put(0x49);
  io->Put(0x49);

  // Tiff version: 42. The meaning of life and everything.
  io->Put(42);
  io->Put(0);

  // Tiff IFD offset. Place it at offset 8 from the start of
  // the marker.
  io->Put(8);
  io->Put(0);  
  io->Put(0);
  io->Put(0);

  // Here the first IFD starts. Number of entries: Zero.
  io->Put(0);
  io->Put(0);

  // And the offset to the next IFD follows. There is none.
  io->Put(0);
  io->Put(0);
  io->Put(0);
  io->Put(0);
}
///

/// EXIFMarker::ParseMarker
// Parse the adobe marker from the stream
// This will throw in case the marker is not
// recognized. The caller will have to catch
// the exception.
void EXIFMarker::ParseMarker(class ByteStream *io,UWORD len)
{
  if (len < 2 + 4 + 2 + 2 + 2 + 4 + 2 + 4)
    JPG_THROW(MALFORMED_STREAM,"EXIFMarker::ParseMarker","misformed EXIF marker");

  //
  // The exif header has already been parsed off.
  len     -= 2 + 4 + 2;
  // Skip the rest of the marker.
  if (len > 0)
    io->SkipBytes(len);
}
///
