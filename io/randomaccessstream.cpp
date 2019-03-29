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
** A random access IO stream that allows forwards and backwards
** seeking. This is an abstraction of the known "IOHook".
**
** $Id: randomaccessstream.cpp,v 1.8 2014/09/30 08:33:17 thor Exp $
**
*/

/// Includes
#include "randomaccessstream.hpp"
///

/// RandomAccessStream::PeekMarker
// Peek the next word in the stream, deliver the marker without
// advancing the file pointer. Deliver EOF in case we run into
// the end of the stream.
LONG RandomAccessStream::PeekWord(void)
{  
  LONG byte1,byte2;
  //
  // First, read the first and the second byte
  byte1 = Get();
  if (byte1 != EOF) {
    byte2 = Get();
    if (byte2 != EOF) {
      // Here we are in a mess... We must un-get two characters whereas
      // the base class only guarantees one.
      // We can always un-do one single get. So do it now.
      LastUnDo();
      // Ok, now check whether we can undo the first get as well.
      if (m_pucBufPtr>m_pucBuffer) {
        // Yes, we can.
        LastUnDo();
      } else {
        // Otherwise, we're in a mess. We allocated the buffer one
        // byte larger than necessary, so we can move the buffer
        // contents up by one byte and place the first byte at the
        // beginning of the buffer. *Yuck*
        memmove(m_pucBuffer+1,m_pucBuffer,m_pucBufEnd - m_pucBuffer);
        m_pucBuffer[0] = (UBYTE) byte1;
        m_pucBufEnd++;
        m_uqCounter--;
      }
      // Deliver the result.
      return ((byte1<<8) | byte2);
    }
    // Unput the first character at least
    // and hack it into the buffer.
    assert(m_pucBuffer);
    m_pucBufPtr    = m_pucBuffer;
    m_pucBuffer[0] = byte1;
    m_pucBufEnd    = m_pucBuffer + 1;
    m_uqCounter--;
  }
  // We are in the EOF case either.
  return EOF;
}
///
