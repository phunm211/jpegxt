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
** An implementation of the ByteStream that reads from or writes to
** a static buffer allocated outside of this class.
** $Id: staticstream.hpp,v 1.8 2014/09/30 08:33:17 thor Exp $
**
*/

#ifndef STATICSTREAM_HPP
#define STATICSTREAM_HPP

/// Includes
#include "bytestream.hpp"
///

/// class StaticStream
// This implementation of a ByteStream simply buffers
// the data it gets, but unlike the memory stream the
// data goes into a buffer that is administrated outside
// of this class, and has a finite length. If the
// stream tries to write beyond this buffer, an error
// is generated. If the caller tries to read beyond the
// end of the buffer, an EOF is generated.
class StaticStream : public ByteStream {
  //
public:
  //
  // Constructor: build a Static given a buffer and a buffer size.
  StaticStream(class Environ *env, UBYTE *buffer,ULONG bufsize)
    : ByteStream(env, bufsize)
  { 
    m_pucBuffer = buffer;
    m_pucBufPtr = buffer;
    m_pucBufEnd = buffer + bufsize;
  }
  //
  // Destructor: Get rid of the buffered bytes
  virtual ~StaticStream(void)
  {
  }
  //
  // Implementation of the abstract functions:
  virtual LONG Fill(void)
  {
    return 0; // always an EOF.
  }
  //
  virtual void Flush(void)
  {
    JPG_THROW(OVERFLOW_PARAMETER,"StaticStream::Flush","static memory buffer run over");
  }
  virtual LONG Query(void)
  {
    return 0;  // always success
  }
  //
  // Peek the next word in the stream, deliver the marker without
  // advancing the file pointer. Deliver EOF in case we run into
  // the end of the stream.
  virtual LONG PeekWord(void)
  {
    if (m_pucBufPtr + 1 < m_pucBufEnd) {
      return (m_pucBufPtr[0] << 8) | m_pucBufPtr[1];
    }
    return ByteStream::EOF;
  }
  //
};
///

///
#endif
