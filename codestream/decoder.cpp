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
** This class parses the markers and holds the decoder together.
**
** $Id: decoder.cpp,v 1.27 2017/02/21 15:48:21 thor Exp $
**
*/

/// Include
#include "codestream/decoder.hpp"
#include "io/bytestream.hpp"
#include "std/assert.hpp"
#include "codestream/tables.hpp"
#include "marker/frame.hpp"
#include "codestream/image.hpp"

///

/// Decoder::Decoder
// Construct the decoder
Decoder::Decoder(class Environ *env)
  : JKeeper(env), m_pImage(NULL)
{
}
///

/// Decoder::~Decoder
// Delete the decoder
Decoder::~Decoder(void)
{
  delete m_pImage;
}
///

/// Decoder::ParseHeaderIncremental
// Parse off the header, return the image in case
// the header was parsed off completely.
class Image *Decoder::ParseHeaderIncremental(class ByteStream *io)
{
  
  if (m_pImage) {
    //
    // Continue parsing the header.
    if (m_pImage->TablesOf()->ParseTablesIncremental(io,NULL) == false) {
      //
      // Parsing the header is done, return the image. All remaining
      // parsing is done by the image.
      return m_pImage;
    }
  } else {
    LONG marker = io->GetWord();

    if (marker != 0xffd8) // SOI
      JPG_THROW(MALFORMED_STREAM,"Decoder::ParseHeader",
                "stream does not contain a JPEG file, SOI marker missing");

    m_pImage  = new(m_pEnviron) class Image(m_pEnviron);
    //
    // The checksum is not going over the headers but starts at the SOF.
    m_pImage->TablesOf()->ParseTablesIncrementalInit();
  }
  //
  return NULL;
}
///

/// Decoder::ParseTags
// Accept decoder options.
void Decoder::ParseTags(const struct JPG_TagItem *tags)
{
  if (tags->GetTagData(JPGTAG_MATRIX_LTRAFO,JPGFLAG_MATRIX_COLORTRANSFORMATION_YCBCR) == 
      JPGFLAG_MATRIX_COLORTRANSFORMATION_NONE) {
    if (m_pImage) {
      m_pImage->TablesOf()->ForceColorTrafoOff();
    }
  }
  if (tags->GetTagData(JPGTAG_IMAGE_LOSSLESSDCT,false)) {
    if (m_pImage) {
      m_pImage->TablesOf()->ForceIntegerDCT();
    }
  }
}
///
