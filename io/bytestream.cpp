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
 * Base class for all IO support functions, the abstract ByteStream
 * class.
 *
 * $Id: bytestream.cpp,v 1.8 2014/09/30 08:33:17 thor Exp $
 *
 */

/// Includes
#include "bytestream.hpp"
#include "interface/parameters.hpp"
#include "interface/tagitem.hpp"
#include "tools/debug.hpp"
///

/// ByteStream::Read
LONG ByteStream::Read(UBYTE *buffer,ULONG size)
{
  ULONG bytesread = 0;
  ULONG avail     = m_pucBufEnd - m_pucBufPtr; // number of available bytes

  while (size >= avail) { // more bytes to read than in the buffer

    assert(m_pucBufPtr <= m_pucBufEnd);
    
    if (avail) {
      memcpy(buffer,m_pucBufPtr,avail); // copy all data over
      m_pucBufPtr  += avail;         // required for correct fill
      bytesread    += avail;
      buffer       += avail;
      size         -= avail;
    }

    // Now fill up the buffer again if we have to.
    if (size == 0 || Fill() == 0) { // read zero bytes -> EOF. Don't loop forever
      return bytesread;
    }
    //
    // Update the number of available bytes in the buffer.
    avail = m_pucBufEnd - m_pucBufPtr;
  }

  // only a partial read from the buffer
  // now is size <= avail, guaranteed.
  if (size) {
    memcpy(buffer,m_pucBufPtr,size);
    m_pucBufPtr  += size;
    // buffer    += size;  // not needed
    // size      -= size;  // ditto
    bytesread    += size;
  }

  return bytesread;
}
///

/// ByteStream::Write
LONG ByteStream::Write(const UBYTE *buffer,ULONG size)
{
  ULONG byteswritten = 0;
  ULONG avail = m_pucBufEnd - m_pucBufPtr;

  while(size > avail) { // write more bytes than fit into the buffer? 
   
    assert(m_pucBufPtr <= m_pucBufEnd);
    
    // For JPIPs BinDefStream: Must not flush if the data fits, thus ">" and not ">=".
    if (avail) {
      memcpy(m_pucBufPtr,buffer,avail);  // copy the data over
      m_pucBufPtr  += avail;         // required for correct flush
      byteswritten += avail;
      buffer       += avail;
      size         -= avail;
    }
    // Now write the buffer (or allocate one, at least)
    Flush();
    //
    // Update the number of available bytes in the buffer.
    avail        = m_pucBufEnd - m_pucBufPtr;
  }

  // size is now smaller than m_ulBufBytes, guaranteed!
  if (size) {
    memcpy(m_pucBufPtr,buffer,size);
    m_pucBufPtr  += size;
    // buffer    += size;  // not needed
    // size      -= size;  // ditto
    byteswritten += size;
  }

  return byteswritten;
}
///

/// ByteStream::SkipToMarker
// Seek forwards to one of the supplied marker segments, but
// do not pull the marker segment itself.
// This method is required for error-resilliance features, namely
// to resynchronize.
// Returns the detected marker, or EOF.
LONG ByteStream::SkipToMarker(UWORD marker1,UWORD marker2,
                              UWORD marker3,UWORD marker4,
                              UWORD marker5)
{
  LONG byte;

  for(;;) {
    // Read bytes until we detect an 0xff which the marker has to start
    // with. Or an EOF, obviously.
    byte = Get();
    if (byte == EOF)
      return EOF; // We run out of data, *sigh*

    if (byte == 0xff) {
      // A possible marker segment? If so, put the 0xff back and
      // check for the available marker now.
      LastUnDo();
      // And now seek for the marker.
      byte = PeekWord();
      if ((byte == marker1) || (byte == marker2) || 
          (byte == marker3) || (byte == marker4) ||
          (byte == marker5))
        return byte;
      //
      // otherwise, not the marker we seek for. Skip, and don't forget
      // to pull the 0xff we put back.
      Get();
    }
  }
}
///

/// ByteStream::Push
LONG ByteStream::Push(class ByteStream *out,ULONG size)
{
  ULONG bytesread = 0;
  ULONG avail = m_pucBufEnd - m_pucBufPtr;

  while (size >= avail) { // more bytes to read than in the buffer
   
    assert(m_pucBufPtr <= m_pucBufEnd);
    
    if (avail) {
      out->Write(m_pucBufPtr,avail);    // write all data out
      m_pucBufPtr  += avail;           // required for correct fill
      bytesread    += avail;
      size         -= avail;
    }

    // Now fill up the buffer again.
    if (Fill() == 0) { // read zero bytes -> EOF. Don't loop forever
      return bytesread;
    } 
    //
    // Update the number of available bytes in the buffer.
    avail        = m_pucBufEnd - m_pucBufPtr;
  }

  // only a partial read from the buffer
  // now is size <= m_ulBufBytes, guaranteed.
  if (size) {
    out->Write(m_pucBufPtr,size);
    m_pucBufPtr  += size;
    // size      -= size;  // not needed
    bytesread    += size;
  }

  return bytesread;
}
///

/// ByteStream::Get
#if CHECK_LEVEL > 0
LONG ByteStream::Get(void)                          // read a single byte (not inlined)
{
 
  if (m_pucBufPtr >= m_pucBufEnd) {
    if (Fill() == 0)                    // Found EOF
      return EOF;
  }
  assert(m_pucBufPtr < m_pucBufEnd);
  
  return *m_pucBufPtr++;
}
#endif
///

/// ByteStream::SkipBytes
// Skip over bytes, ignore their contribution
void ByteStream::SkipBytes(ULONG offset)
{   
  ULONG avail = m_pucBufEnd - m_pucBufPtr;
  //
  // Cannot seek backwards here, a decoderstream disposes data
  // as soon as it is read (maybe).
  do {
    ULONG bufbytes;
    //
    assert(m_pucBufPtr <= m_pucBufEnd);
    //
    // If we have no buffer or the buffer is empty, try to seek over
    // or refill the buffer.
    if (avail == 0) {
      if (Fill() == 0 && offset) {
        // If this happens, and there's still something to skip,
        // then something's wrong because we should never seek 
        // over all data. If we do, the stream is most likely corrupt.
        JPG_THROW(UNEXPECTED_EOF,"ByteStream::SkipBytes","unexpectedly hit the end of the stream while skipping bytes");
      }
      // Update the number of available bytes.
      avail = m_pucBufEnd - m_pucBufPtr;
    }
    //
    // Abort now. The above condition is still taken since it
    // disposes unnecessary data early.
    if (offset == 0)
      return;
    //
    // Now some bytes should be available.
    assert(m_pucBufPtr < m_pucBufEnd);
    //
    // Check how many bytes we can skip.
    bufbytes = avail;
    if (bufbytes > offset)
      bufbytes = offset;
    //
    // Now skip the indicated number of bytes
    offset       -= bufbytes;
    m_pucBufPtr  += bufbytes;
    avail        -= bufbytes;
    //
    assert(m_pucBufPtr <= m_pucBufEnd);
  } while(true);
}
///
