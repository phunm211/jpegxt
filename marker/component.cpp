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
**
** This class represents a single component.
**
** $Id: component.cpp,v 1.14 2014/09/30 08:33:17 thor Exp $
**
*/

/// Includes
#include "marker/component.hpp"
#include "io/bytestream.hpp"
///

/// Component::Component
Component::Component(class Environ *env,UBYTE idx,UBYTE prec,UBYTE subx,UBYTE suby)
  : JKeeper(env), m_ucIndex(idx), m_ucID(idx), m_ucSubX(subx), m_ucSubY(suby), m_ucPrecision(prec)
{
}
///

/// Component::~Component
Component::~Component(void)
{
}
///

/// Component::WriteMarker
// Write the component information to the bytestream.
void Component::WriteMarker(class ByteStream *io)
{
  // Write the component identifier.
  io->Put(m_ucID);

  // Write the dimensions of the MCU
  assert(m_ucMCUWidth  < 16);
  assert(m_ucMCUHeight < 16);

  io->Put((m_ucMCUWidth << 4) | m_ucMCUHeight);

  // Write the quantization table index.
  io->Put(m_ucQuantTable);
}
///

/// Component::ParseMarker
void Component::ParseMarker(class ByteStream *io)
{
  LONG data;

  data = io->Get();
  if (data == ByteStream::EOF)
    JPG_THROW(MALFORMED_STREAM,"Component::ParseMarker","frame marker incomplete, no component identifier found");

  m_ucID = data;

  data = io->Get();
  if (data == ByteStream::EOF)
    JPG_THROW(MALFORMED_STREAM,"Component::ParseMarker","frame marker incomplete, subsamling information missing");

  m_ucMCUWidth  = data >> 4;
  m_ucMCUHeight = data & 0x0f;

  data = io->Get();
  if (data < 0 || data > 3)
    JPG_THROW(MALFORMED_STREAM,"Component::ParseMarker","quantization table identifier corrupt, must be >= 0 and <= 3");

  m_ucQuantTable = data;
}
///
