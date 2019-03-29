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
** $Id: checksumadapter.hpp,v 1.6 2014/09/30 08:33:17 thor Exp $
**
*/

#ifndef IO_CHECKSUMADAPTER_HPP
#define IO_CHECKSUMADAPTER_HPP

/// Includes
#include "tools/environment.hpp"
#include "io/bytestream.hpp"
///

/// Class ChecksumAdapter
// This class updates a checksum from bytes read or written over an
// arbitrary IO stream. It links an IO stream with a checksum.
class ChecksumAdapter : public ByteStream {
  //
  // The checksum that is updated by this stream.
  class Checksum   *m_pChecksum;
  //
  // The stream that does the real job.
  class ByteStream *m_pStream;
  //
  // A flag whether write-only or read-only is attempted.
  // This is actually only for debugging purposes.
  bool              m_bWriting;
  //
  // Fill up the buffer and flush it.
  // these two have to be replaced by the corresponding
  // member functions of the inherited classses
  virtual LONG Fill(void);
  //
  // Flush the IO buffer. This must be defined by the instances of
  // this class.
  virtual void Flush(void);
  //
public:
  ChecksumAdapter(class ByteStream *parent,class Checksum *sum,bool writing);
  //
  // The destructor. This updates the checksum. It cannot flush the buffer
  // on writing since this may throw.
  ~ChecksumAdapter(void);
  //
  // Peek the next word in the stream, deliver the marker without
  // advancing the file pointer. Deliver EOF in case we run into
  // the end of the stream.
  virtual LONG PeekWord(void);
  //
  // read stream buffer status. Also to be overloaded.
  virtual LONG Query(void);
  //
  // On reading & writing, flush the checksum and prepare to go.
  void Close(void);
  //
  // Return the checksum we are updating.
  class Checksum *ChecksumOf(void) const
  {
    return m_pChecksum;
  }
};
///

///
#endif


  
