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
 * $Id: bytestream.hpp,v 1.11 2015/03/30 09:39:09 thor Exp $
 *
 */

#ifndef BYTESTREAM_HPP
#define BYTESTREAM_HPP

/// Includes
#include "tools/environment.hpp"
#include "interface/parameters.hpp"
#include "interface/hooks.hpp"
#include "std/string.hpp"
#ifdef EOF
#undef EOF
#endif
///

/// Design
/** Design
******************************************************************
** class ByteStream                                             **
** Super Class: none                                            **
** Sub Classes: IOHook, MemoryStream, DecoderStream, NULStream  **
** Friends:     none                                            **
******************************************************************

The ByteStream is an abstract class that implements byte oriented
input/output functions. This includes reading/writing a block
of data, reading and writing of single bytes and words and some 
status and special operations.

The ByteStream class already implements buffering, and contains
also a file/byte counter. What the corresponding sub classes have
to do is just to implement methods for reading and writing complete
buffers, and to deliver status information.

One special method, PeekWord(), allows to read the next marker
(two bytes) without advancing the file pointer. This is required 
for some of the higher magic of the error resiliance features.
* */
///

/// ByteStream
// The ByteStream class is an abstract data class to read or write groups
// or blocks of bytes from an abstract stream, additionally providing some
// kind of buffering mechanism. It implements non-virtual member functions
// that operate on the buffer only, and some virtual member functions to
// read or write complete buffers.
class ByteStream : public JKeeper {
  // As sad as it sounds, but the segmented stream
  // must grab into the following to steal buffers.
  friend class ChecksumAdapter;
  //
protected:
  ULONG       m_ulBufSize;   // Size of our (internal) IO buffer
  UBYTE      *m_pucBuffer;   // an IO buffer if we have it
  UBYTE      *m_pucBufPtr;   // a pointer to the first valid buffer byte
  UBYTE      *m_pucBufEnd;   // pointer beyond the last valid byte of the buffer.
  UQUAD       m_uqCounter;   // counts output bytes, if possible
  //
  // Note: The counter and the buffers must be maintained by instances of
  // this abstract class.
  //
  // constructors: This just fills in the buffer size and resets the pointers
  //
  ByteStream(class Environ *env, ULONG bufsize = 2048)
    : JKeeper(env),
      m_ulBufSize(bufsize), m_pucBuffer(NULL), m_pucBufPtr(NULL), m_pucBufEnd(NULL), m_uqCounter(0)
  { }
  //
  //
  //
  // fill up the buffer and flush it.
  // these two have to be replaced by the corresponding
  // member functions of the inherited classses
  virtual LONG Fill(void) = 0; 
  // Flush the IO buffer. This must be defined by the instances of
  // this class.
  virtual void Flush(void) = 0;
  //
#if CHECK_LEVEL > 2
  static UBYTE lastbyte;
#endif
  //
public: 
  //
  // A couple of definitions
  enum {
    EOF   = -1 // indication of an EOF condition
  };
  //
  // the destructor is virtual and has to be
  // overloaded.
  virtual ~ByteStream(void)
  { }
  //
  // Some rather standard IO functions, you know what they do.
  // These are not for overloading and non-virtual. All they
  // need is the buffer structure above. 
  LONG Read(UBYTE *buffer,ULONG size);          // read from buffer
  LONG Write(const UBYTE *buffer,ULONG size);   // write to buffer
  // Push the (partial) contents of another bytestream out.
  LONG Push(class ByteStream *out,ULONG size);
  // 
  // read stream buffer status. Also to be overloaded.
  virtual LONG Query(void) = 0;
  //
  // Reset the byte counter. This *MUST* be matched by a flush or a refill
  // or otherwise the result is undesirable.
  void ResetCounter(void)
  {
    m_uqCounter = 0;
  }
  //
  // Peek the next word in the stream, deliver the marker without
  // advancing the file pointer. Deliver EOF in case we run into
  // the end of the stream.
  virtual LONG PeekWord(void) = 0;  
  //
  // Skip over bytes, ignore their contribution. The offset must
  // be positive (or zero).
  virtual void SkipBytes(ULONG offset);
  //
  //
#if CHECK_LEVEL > 0
  LONG Get(void);
#else
  // The following two methods are single byte IO functions,
  // inlined for maximal performance.
  LONG Get(void)                          // read a single byte (inlined)
  {
    if (unlikely(m_pucBufPtr >= m_pucBufEnd)) {
      if (Fill() == 0)                    // Found EOF
        return EOF;
    }
    return *m_pucBufPtr++;
  }
#endif
  //
  // Read a word from the stream. Return EOF if not available.
  LONG GetWord(void)
  {
    LONG in1,in2;
    //
    in1 = Get();
    if (in1 == EOF)
      return EOF;
    in2 = Get();
    if (in2 == EOF)
      return EOF;
    //
    // The FDIS enforces BIG ENDIAN, so do we.
    return (in1 << 8) | in2;
  }
  //
  // Just the same for writing data.
  void Put(UBYTE byte)           // write a single byte (inlined)
  {
    if (unlikely(m_pucBufPtr >= m_pucBufEnd)) {
      Flush();                   // note that this will also allocate a buffer
    }

    *m_pucBufPtr++ = byte;
  }
  //
  // Put a WORD over the stream.
  void PutWord(UWORD word)
  {
    // Use big-endianness, as in the FDIS.
    Put(word >> 8);
    Put(word & 0xff);
  }
  //
  // Return the last byte that has been read from or put into the buffer.
  // If the last byte is not available, return EOF.
  // Note that the ArthDeco/MQ coder requires this behaivour as in this
  // case it can know that the last byte was at least not a 0xff which
  // we removed then already.
  LONG LastByte(void) const
  {
    if (m_pucBufPtr <= m_pucBuffer)
      return EOF;

    return m_pucBufPtr[-1];
  }
  //
  // Return the last byte written/read and un-put/get it.
  UBYTE LastUnDo(void)
  {
    // Actually, this may be valid in case we want to un-do an EOF
    // read. Hence, this is only an error if there was a buffer before.
    assert(m_pucBufPtr == NULL || m_pucBufPtr > m_pucBuffer);
    
    if (m_pucBufPtr > m_pucBuffer) {
      m_pucBufPtr--;
      return *m_pucBufPtr;
    }
    
    return 0; // shut-up g++
  }
  //
  // Return the byte counter = #of bytes read or written
  UQUAD FilePosition(void) const
  {
    if (m_pucBuffer) {
      return m_uqCounter + (m_pucBufPtr - m_pucBuffer);
    } else {
      return m_uqCounter;
    }
  }
  //
  // Seek forwards to one of the supplied marker segments, but
  // do not pull the marker segment itself.
  // This method is required for error-resilliance features, namely
  // to resynchronize.
  // Returns the detected marker, or EOF.
  LONG SkipToMarker(UWORD marker1,UWORD marker2 = 0,
                    UWORD marker3 = 0,UWORD marker4 = 0,
                    UWORD marker5 = 0);
};
///

///
#endif
