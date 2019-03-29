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
 * Definition of a rectangle.
 * 
 * $Id: rectangle.hpp,v 1.7 2014/09/30 08:33:18 thor Exp $
 *
 * This defines a rectangular array of pixels in some domain.
 * It is just a structure, not a class, and only used as a
 * tiny helper structure that I'm going to use here and there.
 *
 */

#ifndef TOOLS_RECTANGLE_HPP
#define TOOLS_RECTANGLE_HPP

/// Includes
#include "interface/types.hpp"
///

/// Design
/** Design
******************************************************************
** struct RectAngle                                             **
** Super Class: none                                            **
** Sub Classes: none                                            **
** Friends:                                                     **
******************************************************************

Defines simply a rectangular pair of coordinates within whatever.
Typically of either WORD or LONG type.

It provides functions that clips one rectangle to the interiour
of another, or checks whether a rectangle is empty or not.

Rectangle coordinates are understood to be always inclusive,
for the minimum as well as for the maximum coordinates. Keep
care of this when computing the size of a rectangle "in pixels".

* */
///

/// Rectangle templated structure
// This structure defines a rectangle in the pixel domain.
// it should be templated to either LONG or WORD.
template <class T> struct RectAngle {
  T ra_MinX,ra_MinY;   // Minimum coordinate, inclusively
  T ra_MaxX,ra_MaxY;   // Maximum coordinate, inclusively
  // Check whether a rectangle is empty.
  bool IsEmpty(void) const
  {
    return (((ra_MinX > ra_MaxX) || (ra_MinY > ra_MaxY))?true:false);
  }
  //
  // ClipRect: Clip a rectangle into a (supposed to be) larger
  void ClipRect(const RectAngle<T> &bounds)
  {
    if (ra_MinX < bounds.ra_MinX)
      ra_MinX = bounds.ra_MinX;
    
    if (ra_MinY < bounds.ra_MinY)
      ra_MinY = bounds.ra_MinY;
    
    if (ra_MaxX > bounds.ra_MaxX)
      ra_MaxX = bounds.ra_MaxX;
    
    if (ra_MaxY > bounds.ra_MaxY)
      ra_MaxY = bounds.ra_MaxY;
  }
  //
  // Return the width of the rectangle in pixels.
  T WidthOf(void) const
  {
    return ra_MaxX - ra_MinX + 1;
  }
  // Return the height of the rectangle in pixels
  T HeightOf(void) const
  {
    return ra_MaxY - ra_MinY + 1;
  }
  // EnlargeOver: Enlarge a rectangle such that it covers the 
  // passed rectangle. This is somehow the reverse of the above.
  void EnlargeOver(const RectAngle<T> &within)
  {
    if (ra_MinX > within.ra_MinX)
      ra_MinX = within.ra_MinX;

    if (ra_MinY > within.ra_MinY)
      ra_MinY = within.ra_MinY;

    if (ra_MaxX < within.ra_MaxX)
      ra_MaxX = within.ra_MaxX;

    if (ra_MaxY < within.ra_MaxY)
      ra_MaxY = within.ra_MaxY;
  }
  //
  // Move a rectangle by a given amount
  void MoveRect(LONG deltax,LONG deltay)
  {
    ra_MinX  -= deltax;
    ra_MaxX  -= deltax;
    ra_MinY  -= deltay;
    ra_MaxY  -= deltay;
  }
  //
  // Equality operator between rectangles.
  bool operator==(const RectAngle<T> &cmp) const
  {
    if (ra_MinX != cmp.ra_MinX)
      return false;
    if (ra_MinY != cmp.ra_MinY)
      return false;
    if (ra_MaxX != cmp.ra_MaxX)
      return false;
    if (ra_MaxY != cmp.ra_MaxY)
      return false;
    return true;
  } 
  //
  // Inequality between rectangles.
  bool operator!=(const RectAngle<T> &cmp) const 
  {
    if (ra_MinX != cmp.ra_MinX)
      return true;
    if (ra_MinY != cmp.ra_MinY)
      return true;
    if (ra_MaxX != cmp.ra_MaxX)
      return true;
    if (ra_MaxY != cmp.ra_MaxY)
      return true;
    return false;
  }
  // 
  // Check whether this rectangle is covered by the 
  // given rectangle completely. Returns true if so.
  bool IsCoveredBy(const RectAngle<T> &cmp) const
  {
    if (ra_MinX >= cmp.ra_MinX &&
        ra_MinY >= cmp.ra_MinY &&
        ra_MaxX <= cmp.ra_MaxX &&
        ra_MaxY <= cmp.ra_MaxY)
      return true;
    return false;
  }
  //
  // Check whether this rectangle intersects with another
  // rectangle. Returns true if so.
  bool Intersects(const RectAngle<T> &cmp) const
  {
    if (ra_MinX > cmp.ra_MaxX ||
        ra_MaxX < cmp.ra_MinX ||
        ra_MinY > cmp.ra_MaxY ||
        ra_MaxY < cmp.ra_MinY)
      return false;
    return true;
  }
  //
  // Check whether a given point is contained in the rectangle.
  bool Contains(T x,T y) const
  {
    if (x >= ra_MinX && x <= ra_MaxX &&
        y >= ra_MinY && y <= ra_MaxY)
      return true;
    return false;
  }
};
///

///
#endif
