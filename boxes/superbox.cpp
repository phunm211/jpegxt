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
** This class provides the necessary mechanisms for superbox parsing.
** It describes a box that contains sub-boxes and parses the sub-boxes.
**
** $Id: superbox.cpp,v 1.6 2014/09/30 08:33:15 thor Exp $
**
*/

/// Includes
#include "interface/types.hpp"
#include "interface/parameters.hpp"
#include "tools/environment.hpp"
#include "boxes/box.hpp"
#include "boxes/superbox.hpp"
#include "boxes/namespace.hpp"
#include "io/memorystream.hpp"
#include "io/decoderstream.hpp"
///

/// SuperBox::SuperBox
SuperBox::SuperBox(class Environ *env,class Box *&boxlist,ULONG boxtype)
      : Box(env,boxlist,boxtype), m_pSubBoxes(NULL)
{
}
///

/// SuperBox::~SuperBox
SuperBox::~SuperBox(void)
{
  class Box *box;

  // Destroy the subboxes recursively.
  while((box = m_pSubBoxes)) {
    m_pSubBoxes = box->NextOf();
    delete box;
  }
}
///

/// SuperBox::RegisterNameSpace
// Register this box as primary namespace.
void SuperBox::RegisterNameSpace(class NameSpace *names)
{
  names->DefinePrimaryLookup(&m_pSubBoxes);
}
///

/// SuperBox::ParseBoxContent
// Parse the contents of the superbox as sub-boxes. This is implemented here
// because the contents is already structured at the box-level. It
// creates boxes from the box types, but leaves the actual box-parsing to the
// correspondig super-box implementation.
bool SuperBox::ParseBoxContent(class ByteStream *stream,UQUAD boxsize)
{
  //
  // Superboxes may be empty.
  while(boxsize > 0) {
    class Box *box;
    LONG  lo,hi;
    ULONG lbox;
    ULONG tbox;
    ULONG overhead = 0;
    UQUAD xlbox;
    // At least the LBox and the TBox fields must be present.
    if (boxsize < 4 + 4)
      JPG_THROW(MALFORMED_STREAM,"SuperBox::ParseBoxContent","found incomplete box header within a superbox");
    //
    // Parse off lbox.
    hi   = stream->GetWord();
    lo   = stream->GetWord();
    if (lo == ByteStream::EOF)
      JPG_THROW(UNEXPECTED_EOF,"SuperBox::ParseBoxContent","run into an EOF while parsing a box header in a superbox");
    lbox = (hi << 16) | lo;
    //
    // Parse off tbox.
    hi   = stream->GetWord();
    lo   = stream->GetWord();
    if (lo == ByteStream::EOF)
      JPG_THROW(UNEXPECTED_EOF,"SuperBox::ParseBoxContent","run into an EOF while parsing a box header in a superbox");
    tbox = (hi << 16) | lo;
    //
    // Check whether we need an xlbox field. This happens if lbox is one.
    if (lbox == 1) {
      LONG l1,l2,l3,l4;
      if (boxsize < 4 + 4 + 8)
        JPG_THROW(MALFORMED_STREAM,"SuperBox::ParseBoxContent","found incomplete box header within a superbox");
      //
      // Read XLBox.
      l1 = stream->GetWord();
      l2 = stream->GetWord();
      l3 = stream->GetWord();
      l4 = stream->GetWord();
      if (l4 == ByteStream::EOF)
        JPG_THROW(UNEXPECTED_EOF,"SuperBox::ParseBoxContent","run into an EOF while parsing a box header in a superbox");
      //
      xlbox = (UQUAD(l1) << 48) | (UQUAD(l2) << 32) | (UQUAD(l3) << 16) | (UQUAD(l4) << 0);
      //
      // Check for consisteny. The xlbox size needs to include tbox,lbox and xlbox.
      if (xlbox < 4 + 4 + 8)
        JPG_THROW(MALFORMED_STREAM,"SuperBox::ParseBoxContent","box size within super box is inconsistent and too short");
      overhead = 4 + 4 + 8;
    } else if (lbox == 0) {
      // This is actually not part of the standard. It could mean (and it does mean, in J2K) that the
      // box extends to the end of the superbox, or the end of the EOF.
      JPG_THROW(MALFORMED_STREAM,"SuperBox::ParseBoxContent","found a box size of zero within a superbox");
    } else if (lbox < 4 + 4) {
      // The box is too short and does not include its own size fields.
      JPG_THROW(MALFORMED_STREAM,"SuperBox::ParseBoxContent","box size within super box is inconsistent and too short");
    } else {
      // This is a regularly sized box. Use xlbox for the box size from this point on.
      xlbox    = lbox;
      overhead = 4 + 4;
    }
    //
    // Check whether there are enough bytes left for the body of this box.
    if (boxsize < xlbox)
      JPG_THROW(MALFORMED_STREAM,"SuperBox::ParseBoxContent",
                "incomplete super box, super box does not provide enough data for body of sub-box");
    //
    // Create a new sub-box of this box.
    box      = CreateBox(tbox);
    if (box == NULL) {
      // Done parsing this box, remove the bytes.
      boxsize -= xlbox;
      // Nobody interested in this box. Just skip the body bytes.
      xlbox   -= overhead;
      while(xlbox > 0) {
        UWORD skip;
        if (xlbox > MAX_UWORD) {
          skip = MAX_UWORD;
        } else {
          skip = UWORD(xlbox);
        }
        stream->SkipBytes(skip);
        xlbox -= skip;
      }
      //
      //
    } else {
      // Note that the box size does not need to be set here. It is private to the
      // box and is already known. It is a temporary to the parser.
      //
      // Parse now the body of the box. It is complete by definition since the
      // superbox is complete.
      if (box->ParseBoxContent(stream,xlbox - overhead)) {
        // Done. Reduce the number of available bytes.
        boxsize -= xlbox;
        // Inform the superbox that the box is now created.
        AcknowledgeBox(box,tbox);
      } else if (xlbox - overhead <= MAX_ULONG) {
        // Push this into the decoder stream of the box and
        // let the box do its parsing when it feels like it.
        box->InputStreamOf()->Append(stream,xlbox - overhead,0);
        boxsize -= xlbox;
      } else {
        // Input is too long. I don't want to buffer >4GB.
        JPG_THROW(OVERFLOW_PARAMETER,"SuperBox::ParseBoxContent",
                  "sub-box of a superbox is too long (>4GB) for buffering");
      }
    }
    // Done with this box. Continue until all bytes done.
  }
  //
  // Done parsing. If there is a buffer, be gone with it.
  return true;
}
///

/// SuperBox::CreateBox
// Create a new box within the box list of this superbox, or return
// NULL in case the box type is irrelevant.
class Box *SuperBox::CreateBox(ULONG tbox)
{
  return CreateBox(m_pSubBoxes,tbox);
}
///

/// SuperBox::CreateBoxContent
// Write the super box content, namely all the boxes, into the output stream.
// This also calls the superbox implementation to perform the operation.
bool SuperBox::CreateBoxContent(class MemoryStream *target)
{
  class Box *box = m_pSubBoxes;
  bool done      = true;
  // Contents of the subboxes now go into the memory output stream
  // for buffering and determining their size. This is computed last.
  while(box) {
    // Create the content of the sub-box, buffer all its data.
    if (box->CreateBoxContent(box->OutputStreamOf())) {
      // Buffering done. Determine the size of the output stream.
      // This writes without an enumerator because we are in a superbox.
      box->WriteBoxContent(target);
    } else {
      // This is a sub-box which does not yet know its size. We cannot do that within
      // super boxes.
      done = false;
      assert(!"found subbox of a superbox that is not yet complete - this cannot work");
    }
    //
    // Advance to the next box.
    box = box->NextOf();
  }
  //
  return done;
}
///

  
