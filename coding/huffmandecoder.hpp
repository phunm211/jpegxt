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
** This class implements a decoder for a single group of bits in a huffman
** decoder.
**
** $Id: huffmandecoder.hpp,v 1.13 2014/11/12 21:24:45 thor Exp $
**
*/

#ifndef CODING_HUFFMANDECODER_HPP
#define CODING_HUFFMANDECODER_HPP

/// Includes
#include "tools/environment.hpp"
#include "io/bytestream.hpp"
#include "std/string.hpp"
///

/// class HuffmanDecoder
// This class decodes a group of bits from the IO stream, generating a symbol. This
// is the base class.
class HuffmanDecoder : public JKeeper {
  //
  // Decoder table: Delivers for each 8-bit value the symbol.
  UBYTE  m_ucSymbol[256];
  //
  // Decoder length: Delivers for each 8-bit value the length of the symbol in bits.
  UBYTE  m_ucLength[256];
  //
  // If 8 bits are not sufficient, here are pointers that each point to a 256 byte
  // array indexed by the LSBs.
  UBYTE *m_pucSymbol[256];
  //
  // And ditto for the length.
  UBYTE *m_pucLength[256];
  //
public:
  HuffmanDecoder(class Environ *env,
                 UBYTE *&symbols,UBYTE *&sizes,UBYTE **&lsbsymb,UBYTE **&lsbsize)
    : JKeeper(env)
  {
    symbols = m_ucSymbol;
    sizes   = m_ucLength;
    lsbsymb = m_pucSymbol;
    lsbsize = m_pucLength;

    // Fill the unused area with invalid sizes.
    memset(m_ucLength ,0xff,sizeof(m_ucLength));
    memset(m_pucSymbol,0   ,sizeof(m_pucSymbol));
    memset(m_pucLength,0   ,sizeof(m_pucLength));
  }
  //
  ~HuffmanDecoder(void)
  { 
    int i;

    for(i = 0;i < 256;i++) {
      if (m_pucSymbol[i]) m_pEnviron->FreeMem(m_pucSymbol[i],256 * sizeof(UBYTE));      
      if (m_pucLength[i]) m_pEnviron->FreeMem(m_pucLength[i],256 * sizeof(UBYTE));
    }
  }
  //
  // Decode the next symbol.
  UBYTE Get(BitStream<false> *io)
  {
    UWORD data;
    UBYTE symbol;
    UBYTE size;
    UBYTE lsb,msb;

    data   = io->PeekWord();
    msb    = data >> 8;
    if (likely(m_ucLength[msb])) {
      symbol = m_ucSymbol[msb];
      size   = m_ucLength[msb];
    } else {
      lsb    = data;
      symbol = m_pucSymbol[msb][lsb];
      size   = m_pucLength[msb][lsb];
    }
    
    io->SkipBits(size);

    return symbol;
  }
};
///

///
#endif
