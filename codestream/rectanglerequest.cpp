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
 * $Id: rectanglerequest.cpp,v 1.13 2015/03/11 16:02:42 thor Exp $
 *
 */

/// Includes
#include "interface/types.hpp"
#include "tools/environment.hpp"
#include "interface/tagitem.hpp"
#include "codestream/image.hpp"
#include "rectanglerequest.hpp"
#include "interface/parameters.hpp"
///

/// RectangleRequest::ParseTags
// Queues the request in the rectangle request structure, return a
// boolean indicator that is true when the full canvas has been
// requested.
void RectangleRequest::ParseTags(const struct JPG_TagItem *tags,const class Image *image)
{
  class Environ *m_pEnviron = image->EnvironOf(); // Faked for the exceptions
  LONG coord;
  //
  rr_Request.ra_MinX    = 0;
  rr_Request.ra_MinY    = 0;
  rr_Request.ra_MaxX    = image->WidthOf()-1;
  rr_Request.ra_MaxY    = image->HeightOf();
  if (rr_Request.ra_MaxY == 0) {
    rr_Request.ra_MaxY = MAX_LONG; // Height is not yet defined
  } else {
    rr_Request.ra_MaxY--;
  }
  rr_usFirstComponent   = 0;
  rr_usLastComponent    = image->DepthOf() - 1;
  //
  /*
  rr_usUpToLayer        = MAX_UWORD; // exclusive bound
  rr_ucThumbSize        = 0;
  */
  rr_cPriority          = 0;
  /*
  rr_ucUpsampling       = 0;
  */
  rr_bIncludeAlpha      = true;
  //
  // Changed a bit the reaction on coordinates: We no longer throw
  // errors, but rather clip into the valid coordinates. This goes
  // consistent with the handling of the resolution and component
  // coordinates.
  //
  while(tags) {
    coord = tags->ti_Data.ti_lData;
    switch (tags->ti_Tag) { 
    case JPGTAG_DECODER_MINX:
      if (coord < 0)
        JPG_THROW(OVERFLOW_PARAMETER,"RectangleRequest::ParseFromTagList",
                  "Rectangle MinX underflow, must be >= 0");
      if (coord > rr_Request.ra_MinX) {
        rr_Request.ra_MinX = coord;
      }
      break;
    case JPGTAG_DECODER_MINY:
      if (coord < 0)
        JPG_THROW(OVERFLOW_PARAMETER,"RectangleRequest::ParseFromTagList",
                  "Rectangle MinY underflow, must be >= 0");
      if (coord > rr_Request.ra_MinY) {
        rr_Request.ra_MinY = coord;
      }
      break;
    case JPGTAG_DECODER_MAXX:
      if (coord < 0)
        JPG_THROW(OVERFLOW_PARAMETER,"RectangleRequest::ParseFromTagList",
                  "Rectangle MaxX underflow, must be >= 0");
      if (coord < rr_Request.ra_MaxX) {
        rr_Request.ra_MaxX = coord;
      }
      break;
    case JPGTAG_DECODER_MAXY:
      if (coord < 0)
        JPG_THROW(OVERFLOW_PARAMETER,"RectangleRequest::ParseFromTagList",
                  "Rectangle MaxY underflow, must be >= 0");
      if (coord < rr_Request.ra_MaxY) {
        rr_Request.ra_MaxY = coord;
      }
      break;
    case JPGTAG_DECODER_MINCOMPONENT:
      if (coord < 0 || coord > MAX_UWORD)
        JPG_THROW(OVERFLOW_PARAMETER,"RectangleRequest::ParseFromTagList",
                  "MinComponent overflow, must be >= 0 && < 65536");
      if (UWORD(coord) > rr_usFirstComponent) {
        rr_usFirstComponent = coord;
      }
      break;
    case JPGTAG_DECODER_MAXCOMPONENT:
      if (coord < 0 || coord > MAX_UWORD)
        JPG_THROW(OVERFLOW_PARAMETER,"RectangleRequest::ParseFromTagList",
                  "MaxComponent overflow, must be >= 0 && < 65536");
      if (UWORD(coord) < rr_usLastComponent) {
        rr_usLastComponent = coord;
      }
      break;
    case JPGTAG_DECODER_INCLUDE_ALPHA:
      rr_bIncludeAlpha = (coord != 0)?true:false;
      break;
    }
    tags = tags->NextTagItem();
  }
  //
  // Make a consistency check for the rectangle. Otherwise, the decoder
  // will fall over...
  if (rr_Request.IsEmpty())
    JPG_THROW(INVALID_PARAMETER,"RectangleRequest::ParseFromTagList",
              "the requested rectangle is empty");
}
///

/// RectangleRequest::Contains
// Check whether this request contains the argument as sub-request, i.e.
// whether requesting this request first and then the sub-request as
// argument does nothing.
bool RectangleRequest::Contains(const struct RectangleRequest *sub) const
{

  // First, check whether the rectangle of sub is contained
  // in our rectangle.
  if (sub->rr_Request.ra_MinX < rr_Request.ra_MinX)
    return false;

  if (sub->rr_Request.ra_MinY < rr_Request.ra_MinY)
    return false;

  if (sub->rr_Request.ra_MaxX > rr_Request.ra_MaxX)
    return false;

  if (sub->rr_Request.ra_MaxY > rr_Request.ra_MaxY)
    return false;

  //
  // Then check the same for the component interval
  if (sub->rr_usFirstComponent < rr_usFirstComponent)
    return false;
  
  if (sub->rr_usLastComponent < rr_usLastComponent)
    return false;

  /*
  ** currently disabled, doesn't make much sense
  ** here...
  // Layers must be equal. If sub would request less layers
  // than the larger, the difference would be visible. Same
  // if the sub rectangle requests a deeper layer.
  if (sub->rr_usUpToLayer != rr_usUpToLayer)
    return false;

  // If the thumnail size of the sub request is smaller than
  // that of the supposed to be parent, then the sub request
  // requests more code blocks than my rectangle and we must
  // issue it.
  if (sub->rr_ucThumbSize < rr_ucThumbSize)
    return false;

  if (sub->rr_ucZThumbSize < rr_ucZThumbSize)
    return false;
  **
  **
  */

  /*
  // If the sub-request checks for dirty but the parent does
  // not, the sub request is not superfluous either.
  if (sub->rr_ucUpsampling == rr_ucUpsampling)
    return false;
  */

  // Otherwise, the sub request is truely superfluous
  // and need not to be issued again.
  return true;

}
///

/// RectangleRequest::Enqueue
// Enqueue a (this) rectangle into a list according to its priority.
void RectangleRequest::Enqueue(struct RectangleRequest *&first)
{
  struct RectangleRequest **current = &first;

  for(;;) {
    if (*current == NULL || (*current)->rr_cPriority < rr_cPriority) {
      // Enqueue in front of "*current".
      if (*current && (*current)->Contains(this)) {
        // This request is just superfluous. Dispose.
        delete this;
      } else {
        rr_pNext = *current;
        *current = this;
      }
      return;
    }
    current = &((*current)->rr_pNext);
  }
}
///

