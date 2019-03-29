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
** $Id: decoderstream.cpp,v 1.10 2015/03/30 09:39:09 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "decoderstream.hpp"
#include "interface/parameters.hpp"
#include "interface/tagitem.hpp"
///

/// DecoderStream::~DecoderStream
// The destructor of the decoder stream
// gets rid of all the buffer nodes in here.
DecoderStream::~DecoderStream(void)
{
  struct BufferNode *next,*node;

  // Only if we own the lists
  if (m_pParent == NULL) {
    node = m_pBufferList;
    while(node) {
      next = node->NextOf();
      delete node;
      node = next;
    }
  }
}
///

/// DecoderStream::Fill
// Feed the stream with the next available buffer
// or increase the EOF counter if there is no next buffer.
LONG DecoderStream::Fill(void)
{  
  //
  // Do we have a EOF condition? If so, stop reading
  while (!m_bEOF) { 
    struct BufferNode *next;
    //
    // First check whether we do have a node for reading.
    if (m_pCurrent) {
      // Yes, we have had buffer list. We must now switch
      // to the next node and dispose this one, but only if
      // we are the master (parent) node. Otherwise, leave the
      // master alone.
      next = m_pCurrent->NextOf();
      //
      if (m_pParent == NULL) {
        m_pCurrent->Remove(m_pBufferList);
        delete m_pCurrent;
      }
      m_uqCounter += m_pucBufPtr - m_pucBuffer;
      m_pCurrent   = next;
    } else {
      // If not, start from the beginning.
      m_pCurrent = m_pBufferList;
    }
    // Ok, m_pBufferList now either points to the
    // next available BufferNode to read, or is NULL in case
    // there is no such thing. We then have an EOF.
    if (m_pCurrent) {
      m_ulBufSize = m_pCurrent->bn_ulBufSize;
      m_pucBuffer = m_pucBufPtr  = m_pCurrent->bn_pucBuffer;
      m_pucBufEnd = m_pucBuffer  + m_ulBufSize;
      //
      return m_ulBufSize;
    } else {
      // No follow-up node. Return EOF,
      // will also abort the loop.
      m_bEOF      = true;
      m_ulBufSize = 0;
      m_pucBuffer = NULL;
      m_pucBufPtr = NULL;
      m_pucBufEnd = NULL;
    }
  } // of loop on not EOF.
  //
  //
  return 0;
}
///

/// DecoderStream::Append
// Given a byte stream and a size,
// attach as many bytes in the byte stream to the decoder stream.
// The bytes are read from the byte stream and feed into here.
// Returns false on error.
bool DecoderStream::Append(class ByteStream *from,ULONG read_size,ULONG index)
{
  //
  // First, check whether we provide any bytes at all. If not, bail out.
  if (read_size) {
    ULONG size;
    struct BufferNode *bn;
    // Now allocate a new buffer node of the given priority and link it in.
    bn = BufferNode::AddBuffer(m_pEnviron,m_pBufferList,index,read_size);
    // And read the data into the buffer.
    size = from->Read(bn->bn_pucBuffer,read_size);
    if (size != read_size) {
      if (size < read_size) {
        // fill the remaining part with zeros.
        memset(bn->bn_pucBuffer + size,0,read_size - size);
      }
      // Support truncated streams, but warn!
      JPG_WARN(UNEXPECTED_EOF, "DecoderStream::Append",
               "unexpected EOF on pulling encoded data");
      return false;
    }
  }
  return true;
}
///

/// DecoderStream::Append
// The same again, but carring the data over from a second decoder stream.
// The "source" decoder stream is empty afterwards, and its data
// is appended *at the end* of the current decoder stream and not
// sorted it according to the recorded priority.
void DecoderStream::Append(class DecoderStream *from)
{  
  // That's a queue operation.
  BufferNode::AttachQueue(m_pBufferList,from->m_pBufferList);
}
///

/// DecoderStream::PeekWord
// read the next marker segment from the decoder stream without
// advancing the file pointer. Deliver J2K_EOF in case we hit an EOF.
LONG DecoderStream::PeekWord(void)
{    
  LONG byte1,byte2;
  //
  if (!m_bEOF) {
    class DecoderStream temp(this, JPGFLAG_OFFSET_CURRENT); 
    // make a backup of this guy
    // NOTE: This decoder stream does not allocate bytes
    // in any way so we may silently take it from the stack...
    byte1 = temp.Get();
    if (byte1 != EOF) {
      byte2 = temp.Get();
      if (byte2 != EOF) {
        // Ok, pack the marker into a word and deliver.
        return ((byte1<<8) | byte2);
      }
    }
  }
  //
  // Otherwise, it's an EOF.
  return EOF;
}
///

/// DecoderStream::Cleanup
// Prefetch the next buffer. The main reason for this call is to
// release a eaten up buffer, hence to run into the "fill()" 
// the next Get() would have run into anyhow.
void DecoderStream::CleanUp(void)
{
  // If this hit already the EOF and the last buffer
  // node is done, then nothing to do here.
  if (m_pCurrent) {
    UBYTE *realend = m_pucBufEnd;
    // Is all data in this buffer used up? Note that BufEnd of the
    // current node has been modified and shifted to the back by
    // m_ulMovedBytes.
    if (m_pucBufPtr >= realend) {
      // Modify the buffer end such that the next call to this
      // function returns the same value.
      m_pucBufEnd    = m_pucBufPtr;
      //
      // Can we potentially discard this buffer?
      if (m_pParent == NULL && m_pCurrent == m_pBufferList) {
        // Proceed further only if we are the first and only buffer in the list.
        // If we are the only node, then new nodes become the first one,
        // this is identically to the behaivour if we would keep this
        // node since the next fill would discard us.
        m_pBufferList = m_pCurrent->NextOf();
        delete m_pCurrent;
        m_pCurrent    = NULL;
      }
    }
  } 
}
///

/// DecoderStream::BufferedBytes
// Return the amount of buffered data in here.
ULONG DecoderStream::BufferedBytes(void) const
{
  ULONG cnt             = 0;
  struct BufferNode *bn;
  // 
  // Do we have a EOF condition? If so, then that's it.
  if (m_bEOF)
    return 0;
  //
  if ((bn = m_pCurrent)) {
    // This buffer is dirty. Just count the remaining bytes
    // in the output buffer.
    cnt += m_pucBufEnd - m_pucBufPtr;
    bn   = bn->NextOf();
  } else {
    bn   = m_pBufferList;
  }
  //
  // Now add up the size of the data still in the buffer.
  while(bn) {
    cnt += bn->bn_ulBufSize;
    bn   = bn->NextOf();
  }
  //
  return cnt;
}
///

/// DecoderStream::SetFilePointer
// Set the file pointer to the indicated position (read only). This may
// seek within the stream. Note that this implements an absolute 
// seek relative to the start of the file, and in this case to the
// start of the buffer. Works only in case we are a copy of a decoder
// stream and thus the buffer didn't get removed while reading.
void DecoderStream::SetFilePointer(UQUAD newpos)
{
  UQUAD mypos = 0;
  struct BufferNode *bn;
  //
  // This only works if we share the buffer from a parent such
  // that the buffer stays intact.
  assert(m_pParent);
  //
  // Get the initial buffer node.
  bn = m_pBufferList;
  while(bn) {
    // Here mypos is always the absolute file offset of the start
    // of the buffer.
    // Check whether the target position is within this buffer.
    if (newpos >= mypos && newpos < mypos + bn->bn_ulBufSize) {
      ULONG offset = newpos - mypos; // offset within the buffer
      // Is within this buffer. Ok, make this the current buffer
      // and set the buffer pointer accordingly.
      m_pCurrent   = bn;
      m_pucBuffer  = m_pCurrent->bn_pucBuffer;
      m_ulBufSize  = m_pCurrent->bn_ulBufSize;
      m_pucBufPtr  = m_pucBuffer + offset;
      m_pucBufEnd  = m_pucBuffer + m_ulBufSize;
      // Not at EOF, set the file pointer.
      m_uqCounter  = mypos;
      m_bEOF       = false;
      return;
    }
    // Otherwise, advance the file pointer by the buffer size.
    mypos += bn->bn_ulBufSize;
    bn     = bn->NextOf();
  }
  //
  // If we are seeking to the EOF, allow that as well.
  if (mypos == newpos) {
    m_pCurrent   = NULL;
    m_pucBuffer  = NULL;
    m_pucBufPtr  = NULL;
    m_pucBufEnd  = NULL;
    m_uqCounter  = newpos;
    m_bEOF       = true;
    return;
  }
  //
  // Otherwise, generate an error.
  JPG_THROW(OVERFLOW_PARAMETER,"DecoderStream::SetFilePointer","tried to seek beyond EOF");
}
///
