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
** This class is the abstract base class for all boxes, the generic
** extension mechanism for 10918-1. Boxes are used consistently for all
** types of extended data.
**
** $Id: box.hpp,v 1.17 2015/03/13 15:14:06 thor Exp $
**
*/

#ifndef BOXES_BOX_HPP
#define BOXES_BOX_HPP

/// Includes
#include "interface/types.hpp"
#include "tools/environment.hpp"
///

/// Defines
// Create a box ID from an ASCII code.
#define MAKE_ID(a,b,c,d) (((a) << 24) | ((b) << 16) | ((c) << 8) | ((d) << 0))
///

/// Forwards
class DecoderStream;
class MemoryStream;
class ByteStream;
class SuperBox;
class NameSpace;
class Tables;
///

/// class Box
// This is the base class for all types of extended data,
// using the generic APP11 marker to fit its data into the
// syntax of 10918-1.
class Box : public JKeeper {
  friend class SuperBox;
  //
  // Boxes are queued in the table class, this is the next
  // box in the list.
  class Box           *m_pNext;
  //
  // The box type, the four-character identifyer.
  ULONG                m_ulBoxType;
  //
  // The size of the box. The standard currently only allows box sizes up
  // to 4GB, though be prepared that this will grow beyond this limit
  // using the same mechanism as in J2K.
  UQUAD                m_uqBoxSize;
  //
  // Number of bytes already parsed off in the decoder stream. As soon
  // as this equals the indicated box size, the second level parsing
  // of the box contents may begin.
  UQUAD                m_uqParsedBytes;
  //
  // The enumerator that disambuigates between boxes of the same box
  // type.
  UWORD                m_usEnumerator;
  //
  // The stream that keeps the still unparsed data until the box is complete.
  class DecoderStream *m_pInputStream;
  //
  // Output stream when generating the box at the encoder side.
  class MemoryStream  *m_pOutputStream;
  //
  // Second level parsing stage: This is called from the first level
  // parser as soon as the data is complete. Must be implemented
  // by the concrete box. Returns true in case the contents is
  // parsed and the stream can go away.
  virtual bool ParseBoxContent(class ByteStream *stream,UQUAD boxsize) = 0;
  //
  // Second level creation stage: Write the box content into a temporary stream
  // from which the application markers can be created.
  // Returns whether the box content is already complete and the stream
  // can go away.
  virtual bool CreateBoxContent(class MemoryStream *target) = 0;
  //
  // Write the box content into a superbox. This does not require an enumerator.
  void WriteBoxContent(class ByteStream *target);
  //
protected:
  //
  // Create the input stream we can parse from, or return it.
  class DecoderStream *InputStreamOf(void);
  //
  // Create the output stream we can write into, or return it.
  class MemoryStream *OutputStreamOf(void);
  //
  // Write the box contents to the given stream, potentially breaking it up into
  // several APP11 markers.
  void WriteBoxContent(class ByteStream *target,UWORD enu);
  //
public:
  Box(class Environ *env,class Box *&boxlist,ULONG boxtype);
  //
  virtual ~Box(void);
  //
  // Parse an APP11 extended JPEG marker, find or create the necessary box, and
  // append the data there. This is the first level parsing.
  // As soon as the box is complete, perform second level parsing. Requires the 
  // head of the box list as the box is possibly created.
  // The marker, the marker size and the common identifier are already parsed off.
  // Returns the box as soon as it can be delivered, i.e. all bytes are available.
  static class Box *ParseBoxMarker(class Tables *tables,class Box *&boxlist,class ByteStream *stream,UWORD length);
  //
  // Write all boxes into APP11 markers, breaking them up and creating the
  // enumerators for them. This first calls second level box creation, then writes
  // the data into the output stream.
  static void WriteBoxMarkers(class Box *&boxlist,class ByteStream *target);
  //
  // Type of this box.
  ULONG BoxTypeOf(void) const
  {
    return m_ulBoxType;
  }
  //
  // Return the index of this box within the total list of boxes.
  UWORD EnumeratorOf(void) const
  {
    return m_usEnumerator;
  }
  //
  // Create a box of the given type and return it.
  static class Box *CreateBox(class Tables *tables,class Box *&boxlist,ULONG type);
  //
  // Return the next box in the box chain.
  class Box *NextOf(void) const
  {
    return m_pNext;
  }
};
///

///
#endif
