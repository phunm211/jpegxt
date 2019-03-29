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
 * Definition of how to request a given rectangle for display,
 * for load or for checking for a necessary update.
 * 
 * $Id: rectanglerequest.hpp,v 1.10 2015/03/11 16:02:42 thor Exp $
 *
 */

#ifndef CODESTREAM_RECTANGLEREQUEST_HPP
#define CODESTREAM_RECTANGLEREQUEST_HPP

/// Includes
#include "interface/types.hpp"
#include "tools/rectangle.hpp"
#include "tools/environment.hpp"
#include "std/string.hpp"
///

/// Forward references
struct JPG_TagItem;
class Image;
///

/// Design
/** Design
******************************************************************
** struct RectangleRequest                                      **
** Super Class: none                                            **
** Sub Classes: none                                            **
** Friends:                                                     **
******************************************************************

Defines a rectangular image domain that is requested from the
user to be loaded or to be previewed. Hence, the purpose of
this helper class is to pass parameters about the user request
information down the jpeg stream.

This structure is constructed by the decoder, and sent down
to the band class which will generate the requests for each
code block. On its way, more and more components gets parsed
and get interpreted.
* */
///

/// RectangleRequest
// This structure defines the sub-image requested by DisplayRectangle()
// and similar functions. It is consistently used to define an image
// area and as such forwarded to the canvas and all its sub-structures
struct RectangleRequest : public JObject, private Explicit { 
  //
  struct RectangleRequest *rr_pNext;
  RectAngle<LONG>          rr_Request;
  UWORD                    rr_usFirstComponent; // starting component
  UWORD                    rr_usLastComponent;  // inclusive end component
  BYTE                     rr_cPriority;        // order of rectangles
  bool                     rr_bIncludeAlpha;    // include the alpha channel in the request
  //
  RectangleRequest(void)
    : rr_pNext(NULL)
  { }
  //
  // Copy constructor.
  RectangleRequest(const struct RectangleRequest &req)
    : Explicit()
  {
    // Not nice, but this is really faster and simpler
    memcpy(this,&req,sizeof(struct RectangleRequest));
    // Not linked in any way if this is new.
    rr_pNext = NULL;
  }
  //
  // Assignment operator.
  RectangleRequest &operator=(const struct RectangleRequest &req)
  { 
    // Not nice, but this is really faster and simpler
    memcpy(this,&req,sizeof(struct RectangleRequest));
    // Not linked in any way if this is new.
    rr_pNext = NULL;
    //
    return *this;
  }
  //
  //
  // Queues the request in the rectangle request structure.
  void ParseTags(const struct JPG_TagItem *tags,const class Image *image);
  //
  // Check whether this request contains the argument as sub-request, i.e.
  // whether requesting this request first and then the sub-request as
  // argument does nothing.
  bool Contains(const struct RectangleRequest *sub) const;
  // 
  // Check whether this rectangle intersects with another
  // rectangle. Returns true if so.
  bool Intersects(const RectAngle<LONG> &cmp) const
  {
    return rr_Request.Intersects(cmp);
  }
  //
  // Returns the next rectangle in a singly linked list
  struct RectangleRequest *NextOf(void) const 
  {
    return rr_pNext;
  }
  //
  // Enqueue a rectangle into a list according to its priority.
  void Enqueue(struct RectangleRequest *&first);
  //
};
///

///
#endif
