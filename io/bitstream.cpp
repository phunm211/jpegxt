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
** This class allows to read individual bits from a stream of bytes.
** This class implements the bytestuffing as required.
**
** $Id: bitstream.cpp,v 1.8 2014/11/12 14:16:17 thor Exp $
**
*/

/// Includes
#include "io/bitstream.hpp"
///
      
/// Bitstream::Fill
// Fill the byte-buffer. Implement bit-stuffing.
template<bool bitstuffing>
void BitStream<bitstuffing>::Fill(void)
{
  assert(m_ucBits <= 24);
  
  do {
    LONG dt = m_pIO->Get();
    
    if (dt == 0xff) {
      // Possible bitstuffing or bytestuffing.
      m_pIO->LastUnDo();
      //
      if (bitstuffing) {
        if (m_pIO->PeekWord() < 0xff80) {
          // proper bitstuffing. Remove eight bits
          // for now, but...
          m_pIO->Get();
          if (m_pChk)
            m_pChk->Update(dt);
          //
          // ...the next byte has a filler-bit.
          m_ucNextBits = 7;
          m_ulB       |= ULONG(dt) << (24 - m_ucBits);
          m_ucBits    += 8;
        } else {
          m_bMarker    = true;
          m_ucBits    += 8;
          break;
        }
      } else {
        // Bytestuffing.
        if (m_pIO->PeekWord() == 0xff00) {
          // Proper bytestuffing. Remove the zero-byte
          m_pIO->GetWord();
          if (m_pChk) {
            m_pChk->Update(0xff);
            m_pChk->Update(0x00);
          }
          m_ulB       |= ULONG(dt) << (24 - m_ucBits);
          m_ucBits    += 8;
        } else {
          // A marker. Do not advance over the marker, but
          // rather stay at it so the logic upwards can fix it.
          m_bMarker    = true;
          m_ucBits    += 8;
          break;
        }
      }
    } else if (dt == ByteStream::EOF) {
      m_bEOF       = true;
      m_ucBits    += 8;
    } else if (bitstuffing) {
      assert(m_ucNextBits == 8 || dt < 128); // was checked before.
      if (m_pChk) m_pChk->Update(dt);
      m_ulB       |= ULONG(dt) << (32 - m_ucNextBits - m_ucBits);
      m_ucBits    += m_ucNextBits;
      m_ucNextBits = 8;
    } else {
      if (m_pChk) m_pChk->Update(dt);
      m_ulB       |= ULONG(dt) << (24 - m_ucBits);
      m_ucBits    += 8;
    }
  } while(m_ucBits <= 24);
}
///

/// BitStream::ReportError
// Report an error if not enough bits were available, depending on
// the error flag.
template<bool bitstuffing>
void BitStream<bitstuffing>::ReportError(void)
{
  class Environ *m_pEnviron = m_pIO->EnvironOf();
  
  if (m_bEOF)
    JPG_THROW(UNEXPECTED_EOF,"BitStream::ReportError",
              "invalid stream, found EOF within entropy coded segment");
  if (m_bMarker)
    JPG_THROW(UNEXPECTED_EOF,"BitStream::ReportError",
              "invalid stream, found marker in entropy coded segment");
  
  JPG_THROW(MALFORMED_STREAM,"BitStream::ReportError",
            "invalid stream, found invalid huffman code in entropy coded segment");
}
///

/// Explicit instanciations
template class BitStream<true>;
template class BitStream<false>;
///

