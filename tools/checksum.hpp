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
** This helper class keeps and updates the checksum over the legacy
** JPEG stream.
**
** $Id: checksum.hpp,v 1.7 2014/09/30 08:33:18 thor Exp $
**
*/

#ifndef TOOLS_CHECKSUM_HPP
#define TOOLS_CHECKSUM_HPP

/// Includes
#include "interface/types.hpp"
#include "tools/environment.hpp"
///

/// Defines
//#define TESTING
#ifdef TESTING
#include "std/stdio.hpp"
#endif
/// 

/// class Checksum
// This helper class keeps and updates the checksum over the legacy
// JPEG stream.
class Checksum : public JObject {
  //
  // Consists of two bytes.
  UBYTE m_ucCount1;
  UBYTE m_ucCount2;
  //
#ifdef TESTING
  FILE *tmpout;
  int line;
#endif
  //
public:
  Checksum(void)
  : m_ucCount1(0), m_ucCount2(0)
  { 
#ifdef TESTING
    tmpout = fopen("/tmp/chksum","w"); 
    line   = 0;
#endif
  }
  //
  ~Checksum(void)
  {
#ifdef TESTING
    if (tmpout)
      fclose(tmpout);
    tmpout = NULL;
#endif
  }
  //
  // Return the checksum so far.
  ULONG ValueOf(void) const
  {
    return m_ucCount1 | (m_ucCount2 << 8);
  }
  //
  // Update the checksum for a data block.
  void Update(const UBYTE *b,ULONG size)
  {
    UWORD sum;
    //
    while(size) {
      sum  = m_ucCount1;
#ifdef TESTING
      if (tmpout) {
        fprintf(tmpout,"%10d\t%02x\n",line++,*b);
      }
#endif
      sum += *b++;
      sum += (sum + 1) >> 8; // sum modulo 255
      m_ucCount1 = UBYTE(sum);
      assert(m_ucCount1 != 0xff);
      
      sum  = m_ucCount2;
      sum += m_ucCount1;
      sum += (sum + 1) >> 8;
      m_ucCount2 = UBYTE(sum);
      assert(m_ucCount2 != 0xff);
      
      size--;
    }
  }
  //
  // Update the checksum for a single byte.
  void Update(UBYTE b)
  {
    UWORD sum;
    //
    sum  = m_ucCount1;
#ifdef TESTING
    if (tmpout) {
      fprintf(tmpout,"%10d\t%02x\n",line++,b);
    }
#endif
    sum += b;
    sum += (sum + 1) >> 8; // sum modulo 255
    m_ucCount1 = UBYTE(sum);
    assert(m_ucCount1 != 0xff);
      
    sum  = m_ucCount2;
    sum += m_ucCount1;
    sum += (sum + 1) >> 8;
    m_ucCount2 = UBYTE(sum);
    assert(m_ucCount2 != 0xff);
  }
};
///

///
#endif
