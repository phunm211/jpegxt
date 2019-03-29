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
** This class contains the marker that defines the JPEG LS thresholds.
**
** $Id: thresholds.cpp,v 1.8 2014/09/30 08:33:17 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "marker/thresholds.hpp"
#include "io/bytestream.hpp"
///

/// Thresholds::Thresholds
Thresholds::Thresholds(class Environ *env)
  : JKeeper(env), m_usMaxVal(255), 
    m_usT1(3), m_usT2(7), m_usT3(21), m_usReset(64)
{
}
///

/// Thresholds::~Thresholds
Thresholds::~Thresholds(void)
{
}
///

/// Thresholds::WriteMarker
// Write the marker contents to a LSE marker.
void Thresholds::WriteMarker(class ByteStream *io)
{
  io->PutWord(13); // length including the length byte
  io->Put(1);      // ID of the LSE marker
  io->PutWord(m_usMaxVal);
  io->PutWord(m_usT1);
  io->PutWord(m_usT2);
  io->PutWord(m_usT3);
  io->PutWord(m_usReset);
}
///

/// Thresholds::ParseMarker
// Parse the marker contents of a LSE marker.
void Thresholds::ParseMarker(class ByteStream *io,UWORD len)
{
  if (len != 13)
    JPG_THROW(MALFORMED_STREAM,"Thresholds::ParseMarker","LSE marker length is invalid");
  
  m_usMaxVal = io->GetWord();
  m_usT1     = io->GetWord();
  m_usT2     = io->GetWord();
  m_usT3     = io->GetWord();
  m_usReset  = io->GetWord();
}
///

/// Thresholds::InstallDefaults
// Install the defaults for a given bits per pixel value.
void Thresholds::InstallDefaults(UBYTE bpp,UWORD near)
{
  m_usMaxVal = (1 << bpp) - 1;
  
  if (m_usMaxVal >= 128) {
    UWORD factor = m_usMaxVal;
    if (factor > 4095)
      factor = 4095;
    factor = (factor + 128) >> 8;
    m_usT1 = factor * ( 3 - 2) + 2 + 3 * near;
    if (m_usT1 > m_usMaxVal || m_usT1 < near + 1) m_usT1 = near + 1;
    m_usT2 = factor * ( 7 - 3) + 3 + 5 * near;
    if (m_usT2 > m_usMaxVal || m_usT2 < m_usT1  ) m_usT2 = m_usT1;
    m_usT3 = factor * (21 - 4) + 4 + 7 * near;
    if (m_usT3 > m_usMaxVal || m_usT3 < m_usT2  ) m_usT3 = m_usT2;
  } else {
    UWORD factor = 256 / (m_usMaxVal + 1);
    m_usT1 = 3 / factor + 3 * near;
    if (m_usT1 < 2) m_usT1 = 2;
    if (m_usT1 > m_usMaxVal || m_usT1 < near + 1) m_usT1 = near + 1;
    m_usT2 = 7 / factor + 5 * near;
    if (m_usT2 < 3) m_usT2 = 3;
    if (m_usT2 > m_usMaxVal || m_usT2 < m_usT1  ) m_usT2 = m_usT1;
    m_usT3 = 21 / factor + 7 * near;
    if (m_usT3 < 4) m_usT3 = 4;
    if (m_usT3 > m_usMaxVal || m_usT3 < m_usT2  ) m_usT3 = m_usT2;
  }
  m_usReset  = 64;
}
///
