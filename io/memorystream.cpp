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
 * An implementation of the ByteStream class that reads/writes bytes
 * to a "ram disk".
 *
 * $Id: memorystream.cpp,v 1.11 2014/09/30 08:33:17 thor Exp $
 *
 */

/// Includes
#include "tools/environment.hpp"
#include "io/memorystream.hpp"
#include "interface/parameters.hpp"
#include "interface/tagitem.hpp"
#include "std/stddef.hpp"
///

/// MemoryStream::Fill
LONG MemoryStream::Fill(void)
{
  //
  // This is definitely nonsense...
  assert(m_pParent);
  assert(m_pucBufPtr <= m_pucBufEnd);
  //
  // Check whether we are currently reading from the last node of
  // the parent. If so, check whether this node got additional data
  // since the last time we refetched the buffer from there.
  if (m_pCurrent == m_pLast) {
    UBYTE *last;
    // We also enter here in case the parent had no current node when
    // this memory stream got constructed. If this happens, then
    // try to fetch the last buffer node from the parent now.
    if (m_pCurrent == NULL) {
      // If the parent has still no buffer, this is an EOF condition.
      // Otherwise, start reading at the beginning of the parent buffer
      // list since that was the current as well as the beginning position
      // at the time the memorystream got created.
      m_pCurrent = m_pParent->m_pBufferList;
      if (m_pCurrent == NULL)
        return 0;
      //
      // Start from the first byte of the parernt buffer.
      m_pLast      = m_pCurrent;
      m_pucBufPtr  = m_pCurrent->bn_pucBuffer;
    }
    //
    // Reading from the node that used to be the last node in the
    // buffer list of the parent the last time the buffer got filled.
    // Find the last valid byte in the parent buffer.
    if (m_pLast->bn_pNext) {
      // Is no longer the last of the parent. The full buffer is ours.
      last = m_pLast->bn_pucBuffer + m_ulBufSize;
    } else {
      // Only up to the EOF position.
      last = m_pParent->m_pucBufPtr;
    }
    //
    // Remove the bytes we have read already.
    m_pucBufEnd  = last;
    // Adjust the buffer to point to the current buffer start,
    // which is the old end.
    m_pucBuffer  = m_pucBufPtr;
  }
  //
  // Check whether the above was already sufficient to provide data.
  // If not, go to the next buffer as the current one is now out of
  // data. If we are at the EOF of the parent, this is also correct
  // and just finds the next pointer empty.
  if (m_pucBufPtr >= m_pucBufEnd) {
    struct BufferNode *next = m_pCurrent->bn_pNext;
    //
    // Get the next node, if there is one.
    if (next) {
      m_pCurrent     = next;
      m_pLast        = m_pParent->m_pLast;
      // 
      // Adjust the buffer pointer to point to the start of
      // this new buffer.
      m_pucBuffer    = m_pCurrent->bn_pucBuffer;
      m_pucBufPtr    = m_pucBuffer;
      //
      // And compute the number of bytes in here. Again, this
      // depends on whether this is the last node in the parent
      // that is currently getting filled, or just one in the middle.
      if (m_pCurrent == m_pLast) {
        // The last one. Read only up to the EOF of the parent.
        m_pucBufEnd  = m_pParent->m_pucBufPtr;
      } else {
        // An intemediate one, all the buffer size is for us.
        m_pucBufEnd  = m_pucBuffer + m_ulBufSize;
      }
    }
  }
  //
  assert(m_pucBufPtr <= m_pucBufEnd);
  //
  // The number of buffered bytes might still be zero, but that's then an EOF.
  // Increment the counter.
  m_uqCounter += m_pucBufEnd - m_pucBufPtr;
  return m_pucBufEnd - m_pucBufPtr;
}
///

/// MemoryStream::MemoryStream clone constructor
// Note that we set the buffer list to NUL such as if this
// guy is destructed, we keep the buffers of the parent.
MemoryStream::MemoryStream(class Environ *env, class MemoryStream *parent,LONG mode)
  :  ByteStream(env), m_pBufferList(NULL)
{
  ReOpenFrom(parent,mode);
}
///

/// MemoryStream::ReOpenFrom
// Re-open an existing memory stream to read data from the parent starting
// at the indicated node.
void MemoryStream::ReOpenFrom(class MemoryStream *parent,LONG mode)
{
  assert(m_pBufferList == NULL);
  
  m_pParent     = parent;
  m_pCurrent    = NULL;
  m_pucBuffer   = NULL;
  m_pucBufPtr   = NULL;
  m_pucBufEnd   = NULL;
  m_uqCounter   = 0;
  // Keep the size of the memory buffers, copy from the parent.
  m_ulBufSize   = parent->m_ulBufSize;
  //
  if (parent->m_pBufferList) {
    switch (mode) {
    case JPGFLAG_OFFSET_CURRENT:
      m_pCurrent   = parent->m_pLast;     // keep note in front of us
      m_pucBuffer  = parent->m_pucBuffer; // buffer *starts* at current position.
      m_pucBufPtr  = parent->m_pucBufPtr;     
      m_pucBufEnd  = parent->m_pucBufPtr; // nothing in it, because we're at the EOF of the parent.
      m_uqCounter  = parent->m_uqCounter;
      break;
    case JPGFLAG_OFFSET_BEGINNING:
      m_pCurrent   = parent->m_pBufferList;
      m_pucBuffer  = parent->m_pBufferList->bn_pucBuffer;
      m_pucBufPtr  = m_pucBuffer;
      // Compute the size of this buffer. If the current node is the node of the parent
      // it is currently writing to, this is the number of bytes within the buffer of
      // the parent node. Otherwise, the full number of bytes.
      if (m_pCurrent == parent->m_pLast) {
        m_pucBufEnd  = parent->m_pucBufPtr;
      } else {
        m_pucBufEnd  = m_pucBuffer + m_ulBufSize;
      }    
      m_uqCounter    = m_pucBufEnd - m_pucBuffer;
      break;
    default:
      assert(false);
      break;
    }
  }
  // Also keep the last node of the parent so we know
  // whether the current node can be possibly extended beyond
  // its current end because the parent writes additional data.
  m_pLast      = parent->m_pLast;
  //
  assert(m_pucBufPtr <= m_pucBufEnd);
}
///

/// MemoryStream::Flush
void MemoryStream::Flush(void)
{
  struct BufferNode *node;
  UBYTE *buf;

  // This is definitely nonsense...
  // The number of bytes in the buffer must be > 0.
  assert(m_ulBufSize && m_pParent == NULL);
  assert(m_pucBufPtr <= m_pucBufEnd);
  //
  // If this is not the first refill, adjust by the number of buffered
  // bytes.
  if (m_pucBuffer) {
    m_uqCounter      += m_pucBufPtr-m_pucBuffer;  // increment bytes within
  }
  //
  // Get a new buffer node and append at the end of the node.
  node = new(m_pEnviron) BufferNode;
  // Link into the end of the buffer list so nothing is lost.
  if (m_pLast) {
    node->bn_pNext    = m_pLast->bn_pNext;
    m_pLast->bn_pNext = node;  // link us into the list
  } else {
    m_pBufferList     = node;  // otherwise, we are the first entry
  }
  m_pLast             = node;
  // Get now the buffer itself.
  buf                 = (UBYTE *)m_pEnviron->AllocMem(m_ulBufSize);
  node->bn_pucBuffer  = buf;
  //
  // And make this the new buffer.
  m_pucBuffer         = buf;
  m_pucBufPtr         = buf;
  m_pucBufEnd         = buf + m_ulBufSize; // number of available bytes in the buffer 
}
///

/// MemoryStream::~MemoryStream
MemoryStream::~MemoryStream(void)
{
  struct BufferNode *node,*next;

  if (m_pParent == NULL) {
    // In case we do have a parent, then
    // do not touch the buffer as it is not ours.
    if ((node = m_pBufferList)) {
      // release the buffer list
      // the active buffer is part of this list as well
      do {
        next = node->bn_pNext;  // get the next node already 
        m_pEnviron->FreeMem(node->bn_pucBuffer,m_ulBufSize);
        delete node;
      } while((node = next));
    }
  }
}
///

/// MemoryStream::Push
// push the indicated number of bytes over the IOHook.
ULONG MemoryStream::Push(class ByteStream *dest,ULONG total)
{
  ULONG written=0;
  ULONG bytestowrite;
  UBYTE *buf;

#if DEBUG_LEVEL > 2
  static int cnt = 0;
  printf("Writing %d bytes in call %d out:\n",total,cnt++);
#endif

  while(total) {
    if (m_pucBufPtr >= m_pucBufEnd) {
      if (Fill() == 0) {                  // Found EOF
        assert(false);
        return written;
      }
    }
    
    bytestowrite = m_pucBufEnd - m_pucBufPtr;
    if (bytestowrite > total)
      bytestowrite = total;
    buf           = m_pucBufPtr;

    m_pucBufPtr  += bytestowrite;
    bytestowrite  = dest->Write(buf,bytestowrite);

#if DEBUG_LEVEL > 2
    int i;
    for(i = 0;i < bytestowrite;i++) {
      printf("%02x ",buf[i]);
    }
#endif

    total        -= bytestowrite;
    written      += bytestowrite;
  }

#if DEBUG_LEVEL > 2
  printf("\n");
#endif
  return written;
}
///

/// MemoryStream::Append
// Push contents of a different stream into a memory stream, i.e. write
// into the memory stream buffer by using bytes from another bytestream.
void MemoryStream::Append(class ByteStream *in,ULONG bytesize)
{
  while(bytesize) {
    ULONG readsize;
    //
    // Get potentially a new buffer in case this one has no more room.
    if (m_pucBufPtr >= m_pucBufEnd)
      Flush();            
    //
    // Check how many bytes we could place into the current buffer
    readsize = m_pucBufEnd - m_pucBufPtr;
    if (readsize > bytesize)
      readsize = bytesize;
    //
    // Ok, how many bytes have we in the buffer?
    //
    readsize       = in->Read(m_pucBufPtr,readsize);
    if (readsize == 0)
      break;
    
    m_pucBufPtr   += readsize;
    bytesize      -= readsize;
  }
}
///

/// MemoryStream::BufferedBytes
// Return the number of bytes buffered within this memory stream starting
// at the current file position.
ULONG MemoryStream::BufferedBytes(void) const
{
  if (m_pParent) {
    UQUAD bytes;
    //
    assert(m_pucBufPtr <= m_pucBufEnd);
    // The idea here is compute the difference between the
    // file position of the writer and the file position of the reader.
    // The counter includes the number of bytes that have been already
    // placed in the output buffer, even though we haven't read
    // them yet. We just remove the buffers we haven't read yet
    // from the counting: This is the number of bytes in front
    // of the file pointer, i.e. the file position in the
    // reading system. It should be smaller or equal than
    // the file position of the writing end.
    bytes = m_uqCounter - (m_pucBufEnd - m_pucBufPtr);
    //
    // Now compare this with the number of bytes in front of the
    // writing end: This is the number of bytes currently
    // available.
    return ULONG(m_pParent->FilePosition() - bytes);
  } else {
    // The file position directly.
    return FilePosition();
  }
}
///

/// MemoryStream::Clean
// Clean the buffered bytes.
void MemoryStream::Clean(void)
{
  if (m_pParent == NULL) { 
    struct BufferNode *node,*next;
    // In case we do have a parent, then
    // do not touch the buffer as it is not ours.
    m_uqCounter = 0;
    if ((node = m_pBufferList)) {
      // release the buffer list except for the last node.
      // the active buffer is part of this list as well.
      while((next = node->bn_pNext)) {
        m_pEnviron->FreeMem(node->bn_pucBuffer,m_ulBufSize);
        delete node;
        node = next;
      }
      //
      assert(node && node->bn_pNext == NULL);
      // Let the active buffer point to the list.
      m_pBufferList = node;
      m_pLast       = node;
      m_pucBuffer   = node->bn_pucBuffer;
      m_pucBufPtr   = node->bn_pucBuffer;
      m_pucBufEnd   = node->bn_pucBuffer + m_ulBufSize; // number of available bytes in the buffer 
    }
  }
}
///

/// MemoryStream::PeekWord
// Get the next two bytes without removing them from the stream.
LONG MemoryStream::PeekWord(void)
{ 
  const UBYTE *ptr = m_pucBufPtr;
  const UBYTE *end = m_pucBufEnd;

  if (ptr + 2 <= end) {
    return (ptr[0] << 8) | ptr[1]; // the easy case, no buffer re-fill
  } else {
    // The complicated case. Create a temporary memory stream, and pull two bytes.
    class MemoryStream tmp(m_pEnviron);
    tmp.m_pParent    = m_pParent;
    tmp.m_pCurrent   = m_pCurrent;     // keep note in front of us
    tmp.m_pucBuffer  = m_pucBuffer; // buffer *starts* at current position.
    tmp.m_pucBufPtr  = m_pucBufPtr;     
    tmp.m_pucBufEnd  = m_pucBufEnd; // nothing in it, because we're at the EOF of the parent.
    return tmp.GetWord();
  }
}
///
