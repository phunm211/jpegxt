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
** This class updates a checksum from bytes read or written over an
** arbitrary IO stream. It links an IO stream with a checksum.
**
** $Id: checksumadapter.cpp,v 1.7 2014/09/30 08:33:17 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "io/bytestream.hpp"
#include "io/checksumadapter.hpp"
#include "tools/checksum.hpp"
///

/// ChecksumAdapter::ChecksumAdapter
// Construct a checksum adapter from a bytestream and a checksum
// This does not use its own buffer, but rather steals the buffer
// from the parent bytestream.
ChecksumAdapter::ChecksumAdapter(class ByteStream *parent,class Checksum *sum,bool writing)
 : ByteStream(parent->EnvironOf()), 
   m_pChecksum(sum), m_pStream(parent), m_bWriting(writing)
{ 
  m_pucBuffer = parent->m_pucBufPtr;
  m_pucBufPtr = parent->m_pucBufPtr;
  m_pucBufEnd = parent->m_pucBufEnd;
  m_ulBufSize = m_pucBufEnd - m_pucBufPtr;
  m_uqCounter = parent->m_uqCounter + (m_pucBufPtr - parent->m_pucBuffer);
}
///

/// ChecksumAdapter::~ChecksumAdapter
// The destructor, updates the checksum for all data
// read over the stream.
ChecksumAdapter::~ChecksumAdapter(void)
{
  if (m_bWriting == false) {
    // Data in the buffer not yet read needs to be checksummed.
    assert(m_pucBufPtr >= m_pStream->m_pucBufPtr);
    m_pChecksum->Update(m_pStream->m_pucBufPtr,m_pucBufPtr - m_pStream->m_pucBufPtr); 
    // Re-align the streams so the parent is back in sync.
    m_pStream->m_pucBufPtr = m_pucBufPtr;
  } else {
    // Check whether there is any data in here that is not yet checksummed.
    assert(m_pucBufPtr == m_pStream->m_pucBufPtr);
  }
}
///

/// ChecksumAdapter::Flush
// Flush out all data buffered here. This requires to take the checksum over
// the buffered data, then adjusting the buffer pointers of the parent stream.
void ChecksumAdapter::Flush(void)
{
  assert(m_bWriting);
  // The data from the parent stream buffer pointer to our buffer pointer is not
  // yet included in the checksum. Thus, adjust the checksum for the missing
  // data, then fush the parent stream, and fixup our buffer pointers.
  assert(m_pucBufPtr >= m_pStream->m_pucBufPtr);
  m_pChecksum->Update(m_pStream->m_pucBufPtr,m_pucBufPtr - m_pStream->m_pucBufPtr);
  //
  // Re-align the streams.
  m_pStream->m_pucBufPtr = m_pucBufPtr;
  //
  // If the parent buffer overruns, flush it there.
  if (m_pStream->m_pucBufPtr >= m_pStream->m_pucBufEnd) {
    m_pStream->Flush();
    //
    // Re-adjust our data.
    m_ulBufSize = m_pStream->m_ulBufSize;
    m_pucBuffer = m_pStream->m_pucBuffer;
    m_pucBufPtr = m_pStream->m_pucBufPtr;
    m_pucBufEnd = m_pStream->m_pucBufEnd;
    m_uqCounter = m_pStream->m_uqCounter;
  }
}
///

/// ChecksumAdapter::Close
// Complete the current checksum computation, close the stream.
void ChecksumAdapter::Close(void)
{
  // The data from the parent stream buffer pointer to our buffer pointer is not
  // yet included in the checksum. Thus, adjust the checksum for the missing
  // data, then fush the parent stream, and fixup our buffer pointers.
  assert(m_pucBufPtr >= m_pStream->m_pucBufPtr);
  m_pChecksum->Update(m_pStream->m_pucBufPtr,m_pucBufPtr - m_pStream->m_pucBufPtr);
  //
  // Re-align the streams.
  m_pStream->m_pucBufPtr = m_pucBufPtr;
}
///

/// ChecksumAdapter::Fill
// Re-fill the internal buffer when reading data.
// This adjusts the checksum for all the data read so far,
// then re-fills the data from the parent stream.
LONG ChecksumAdapter::Fill(void)
{
  LONG newdata;
  //
  assert(!m_bWriting);
  //
  // Data from the parent buffer pointer to the current buffer pointer is
  // not yet checksumed.
  assert(m_pucBufPtr >= m_pStream->m_pucBufPtr);
  m_pChecksum->Update(m_pStream->m_pucBufPtr,m_pucBufPtr - m_pStream->m_pucBufPtr);
  //
  // Do we need to refill the stream?
  if (m_pucBufPtr >= m_pStream->m_pucBufEnd) {
    // Re-align the streams so the parent is back in sync.
    m_pStream->m_pucBufPtr = m_pucBufPtr;
    // Yes, this stream is empty.
    newdata     = m_pStream->Fill();
    //
    // Realign the streams.
    m_ulBufSize = m_pStream->m_ulBufSize;
    m_pucBuffer = m_pStream->m_pucBuffer;
    m_pucBufPtr = m_pStream->m_pucBufPtr;
    m_pucBufEnd = m_pStream->m_pucBufEnd;
    m_uqCounter = m_pStream->m_uqCounter;
  } else {
    // Not yet. The amount of data made available is the amount of bytes in the buffer
    newdata     = m_pucBufEnd - m_pucBufPtr;
    //
    // Re-align the streams.
    m_pStream->m_pucBufPtr = m_pucBufPtr;
  }
  
  return newdata;
}
///

/// ChecksumAdapter::PeekWord
// Peek the next word in the stream, deliver the marker without
// advancing the file pointer. Deliver EOF in case we run into
// the end of the stream.
LONG ChecksumAdapter::PeekWord(void)
{
  LONG marker;
  //
  // Peeking does not update the checksum since bytes are not
  // removed from the stream.
  assert(!m_bWriting);
  //
  // The catch is, though, that just calling PeekWord from the
  // original stream will ignore the bytes read from this
  // stream, thus call "fill" first to re-align them. This will also
  // compute the checksum over the data that is already there.
  Fill();
  //
  // Then peek-ahead now that both streams point to the same position.
  marker = m_pStream->PeekWord();

  // As a result of this peek-around, the main stream might have
  // changed its state. So resync.
  m_ulBufSize = m_pStream->m_ulBufSize;
  m_pucBuffer = m_pStream->m_pucBuffer;
  m_pucBufPtr = m_pStream->m_pucBufPtr;
  m_pucBufEnd = m_pStream->m_pucBufEnd;
  m_uqCounter = m_pStream->m_uqCounter;
  
  return marker;
}
///

/// ChecksumAdapter::Query
// read stream buffer status. Also to be overloaded.
LONG ChecksumAdapter::Query(void)
{
  return m_pStream->Query();
}
///
