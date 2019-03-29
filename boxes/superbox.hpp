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
** $Id: superbox.hpp,v 1.5 2014/09/30 08:33:15 thor Exp $
**
*/

#ifndef BOXES_SUPERBOX_HPP
#define BOXES_SUPERBOX_HPP

/// Includes
#include "interface/types.hpp"
#include "tools/environment.hpp"
#include "boxes/box.hpp"
///

/// Forwards
class NameSpace;
///

/// class SuperBox
// This is the base class for all superboxes.
// It decodes its contents as sub-boxes.
class SuperBox : public Box {
  //
  // The list of sub-boxes of this superbox. This is a singly linked list
  // of boxes queued by their m_pNext pointer.
  class Box *m_pSubBoxes;
  //
  // Parse the contents of the superbox as sub-boxes. This is implemented here
  // because the contents is already structured at the box-level. It
  // creates boxes from the box types, but leaves the actual box-parsing to the
  // correspondig super-box implementation.
  virtual bool ParseBoxContent(class ByteStream *stream,UQUAD boxsize);
  //
  // Write the super box content, namely all the boxes, into the output stream.
  // This also calls the superbox implementation to perform the operation.
  virtual bool CreateBoxContent(class MemoryStream *target);
  //
  // To be implemented by derived classes: Create a box for this superbox
  // in the given box list.
  virtual class Box *CreateBox(class Box *&boxlist,ULONG tbox) = 0;
  // 
  // Inform the superbox that the box is now created. Does nothing by default,
  // can be overloaded to sort the new box in.
  virtual void AcknowledgeBox(class Box *,ULONG)
  {
    // by default, nothing happens here.
  }
  //
protected:
  //
  // Register this box as primary namespace.
  void RegisterNameSpace(class NameSpace *names);
  //
public:
  //
  SuperBox(class Environ *env,class Box *&boxlist,ULONG boxtype);
  //
  virtual ~SuperBox(void);
  //
  // Create a new box as sub-box of this superbox.
  class Box *CreateBox(ULONG type);
};
///

///
#endif
