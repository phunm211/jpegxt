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
** DecoderStream: Another implementation of the ByteStream class,
** this time used mostly by the decoder. The idea behind this class
** is that it additionally keeps input data in several segments that
** can be sorted according to an index, as used by the enumerator of
** the boxes.
** 
** $Id: decoderstream.hpp,v 1.8 2014/09/30 08:33:17 thor Exp $
**
*/

#ifndef IO_DECODERSTREAM_HPP
#define IO_DECODERSTREAM_HPP

/// Includes
#include "bytestream.hpp"
#include "randomaccessstream.hpp"
#include "tools/priorityqueue.hpp"
///

/// Design
/** Design
******************************************************************
** class DecoderStream                                          **
** Super Class: ByteStream                                      **
** Sub Classes: none                                            **
** Friends:     none                                            **
******************************************************************

This implements a similar RAM disk function like the memory
stream, except that it does not support reading and writing.
The Decoder stream is read-only in the sense that the Put()
methods are not available.

Instead, data enters the decoder stream by means of the Append
functions below in a block-wise fashion.

The major difference between the decoder stream and the memory
stream is that the decoder stream supports data re-ordering.

Each input block that enters the decoder stream is given a 
priority according to which the data is equeued into the
already queued data, and is then read out in this order. 

The decoder stream is used by the decoder (guess what!) to
re-order data as in PPM markers and similar.
* */
///

/// DecoderStream
// A decoder stream is similar to a memory stream, but the
// stream can be filled from the outside with data that
// came into a different byte stream, and the data in the
// stream is sorted by an index.
class DecoderStream : public RandomAccessStream {
  //
  //
  struct BufferNode : public PriorityQueue<BufferNode> {
    UBYTE             *bn_pucBuffer;    // the buffer, keeping the data
    ULONG              bn_ulBufSize;    // size of the buffer in bytes
    //
  private:
    BufferNode(struct BufferNode *&head,ULONG prior,ULONG size)
      : PriorityQueue<BufferNode>(head,prior), bn_ulBufSize(size)
    { 
      // The buffer has been allocated at the end of this structure by the custom allocator.
      bn_pucBuffer = (UBYTE *)(this + 1);
    }
    //
    static void *operator new(size_t size,class Environ *env,ULONG buffersize)
    {
      // Do this via the JObject allocator. This keeps the buffer size fine.
      return JObject::operator new(size + buffersize,env);
    }
    //
  public:
    // Create a new buffer node of the indicated size. The only reason this is here
    // is because the buffer node cannot be constructed on the stack.
    static struct BufferNode *AddBuffer(class Environ *env,struct BufferNode *&head,ULONG prior,ULONG size)
    {
      return new(env,size) struct BufferNode(head,prior,size);
    }
  };
  //
  // The list of buffers buffered here, to be deleted by the master node.
  struct BufferNode   *m_pBufferList;
  //
  // The current read-out position
  struct BufferNode   *m_pCurrent;
  //
  // A pointer to the parent stream in case this stream has been "cloned"
  class DecoderStream *m_pParent;
  //
  // EOF reached?
  bool                 m_bEOF;
  //
public:
  // Constructor
  DecoderStream(class Environ *env)
    : RandomAccessStream(env, 0L), m_pBufferList(NULL), m_pCurrent(NULL),
      m_pParent(NULL), m_bEOF(false)
  { }
  //
  virtual ~DecoderStream(void);
  //
  // Open a decoder stream from another decoder stream,
  // thus open this as a readback.
  // We only support OFFSET_CURRENT as mode right now.
  void ReOpenFrom(class DecoderStream *parent,LONG)
  {
    *this     = *parent;
    m_pParent = parent;
  }
  //
  // Similar to the above, a constructor that clones this stream
  DecoderStream(class DecoderStream *parent,LONG mode) 
    : RandomAccessStream(parent->m_pEnviron)
  {
    assert(mode == JPGFLAG_OFFSET_CURRENT);
    ReOpenFrom(parent,mode);
  }
  //
  // We need our own "fill" routine here.  
  // Remove an FF at the beginning if the last read pulled exactly one
  // EOF. Throw if more than one EOF was pulled. 
  virtual LONG Fill(void);
  //
  // Since decoder streams are never written, there is no flush.
  virtual void Flush(void)
  {
    assert(false);
  }
  //
  virtual LONG Query(void)
  {
    return 0;  // always success
  }
  //
  // Another member function: Given a byte stream and a size,
  // attach as many bytes in the byte stream to the decoder stream.
  // The bytes are read from the byte stream and feed into here.
  // (hence, no longer available to the bytestream for obvious reasons)
  // This also gets an index as parameter as to "sort" incoming buffers
  // by some order. Blocks of equal indices are sorted in FiFo order.
  // Returns false on error.
  bool Append(class ByteStream *from,ULONG size,ULONG priority = 0);
  //
  // The same again, but carring the data over from a second decoder stream.
  // The "source" decoder stream is empty afterwards, and its data
  // is appended *at the end* of the current decoder stream and not
  // sorted it according to the recorded priority.
  void Append(class DecoderStream *from);
  //
  // Peek the next marker in the stream, deliver the marker without
  // advancing the file pointer. Deliver JPG_EOF in case we run into
  // the end of the stream.
  virtual LONG PeekWord(void);
  //
  // Prefetch the next buffer. The main reason for this call is to
  // release a eaten up buffer, hence to run into the "fill()" 
  // the next Get() would have run into anyhow.
  void CleanUp(void);
  //
  // Return the amount of buffered data in here. This is the number
  // of bytes from the current position up to the EOF.
  ULONG BufferedBytes(void) const;
  //
  // SkipBytes skips bytes by either seeking over or, if that should turn
  // out to be impossible, by continuously pulling nonsense out of the
  // buffer. We make it here virtual again since more efficient ways
  // of skipping are possible in case we have random access.
  virtual void SkipBytes(ULONG skip)
  {
    // Though in this specific case, there is no more efficient way...
    ByteStream::SkipBytes(skip);
  }
  //
  // Set the file pointer to the indicated position (read only). This may
  // seek within the stream. Note that this implements an absolute 
  // seek relative to the start of the file, and in this case to the
  // start of the buffer. Works only in case we are a copy of a decoder
  // stream and thus the buffer didn't get removed while reading.
  virtual void SetFilePointer(UQUAD newpos);
};
///

///
#endif
