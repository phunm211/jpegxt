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
 * Defininition of the ImageBitMap structure, used to specify
 * rectangular memory blocks for image representation.
 * 
 * $Id: imagebitmap.hpp,v 1.7 2014/09/30 08:33:17 thor Exp $
 *
 *
 */

#ifndef TOOLS_IMAGEBITMAP_HPP
#define TOOLS_IMAGEBITMAP_HPP

/// Includes
#include "interface/types.hpp"
#include "tools/rectangle.hpp"
#include "tools/environment.hpp"
#include "std/stddef.hpp"
///

/// Design
/** Design
******************************************************************
** struct ImageBitmap                                           **
** Super Class: none                                            **
** Sub Classes: none                                            **
** Friends:                                                     **
******************************************************************

Defines a rectangular array of memory containing image data.

However, this image bitmap is more flexible than a matrix could
be, even though it supports very similar features. 

The first difference is that modulo values are counted in bytes
rather than in elements. This is mainly as convenience for the
client. Furthermore, it does not only provide "vertical"
modulo value (add it to the pixel address to get to the pixel
in the line below), but also a horizontal modulo to be added
to an address to advance by one pixel in horizontal direction.

The idea is here that this allows very easy addressing of
"interleaved" image architectures where red, green and blue
are contained in three (or four, plus alpha channel) continuous
bytes in memory.

A pixel type describes wether the data of a single pixel is
signed, unsigned, byte or word sized. It takes the coefficient 
type indicator as in bandnotation.hpp.

Furthermore, user data can be stored here.

Image bitmaps are the destination object of bitmap hooks. They
create and fill out ImageBitMaps by parsing the result tag list
of the user callback function. The image bitmap then enters
the color transformer for further operation.
* */
///

/// ImageBitMap
// The image bitmap is a small helper structure that indicates
// where to access a bitmap in memory, i.e. where to get the data
// from or where to place it.
struct ImageBitMap : public JObject {
  ULONG   ibm_ulWidth;         // width and height of the data 
  ULONG   ibm_ulHeight;
  BYTE    ibm_cBytesPerPixel;  // byte offset to get from one pixel to the next
  UBYTE   ibm_ucPixelType;     // type identifier as in bandnotation.hpp
  LONG    ibm_lBytesPerRow;    // byte offset to get down from one row to the next. 
  APTR    ibm_pData;           // pointer to the image data
  APTR    ibm_pUserData;       // an ID the client might use for whatever he likes
public:
  // Extract a smaller bitmap from
  // a larger one by giving a rectangle.
  // This will clip correctly to image bitmap coordinates
  // and shift ibm_Data accordingly.
  void ExtractBitMap(const struct ImageBitMap *source,const RectAngle<LONG> &rect);
  //
  // Define a bitmap to describe an image of the given dimension at the given rectangle.
  void DefineBitMap(APTR buffer,UBYTE type,const RectAngle<LONG> &rect);
  //
  // Define a bitmap of the given dimension with the given modulo value.
  void DefineBitMap(APTR buffer,UBYTE type,const RectAngle<LONG> &rect,ULONG samplesperrow);
  //
  // Zero out a bitmap such that it is no longer valid
  void Blank(void) 
  {
    ibm_ulWidth         = 0;
    ibm_ulHeight        = 0;
    ibm_cBytesPerPixel  = 0;
    ibm_lBytesPerRow    = 0;
    ibm_ucPixelType     = 0;
    ibm_pData           = NULL;
  }
  //
  // Return a pointer to the data at a given position.
  APTR At(ULONG x,ULONG y) const
  {
    if (ibm_ucPixelType == 0)
      return NULL; // Blank bitmaps keep blank
   
    assert(x < ibm_ulWidth && y < ibm_ulHeight);
    //
    return (((UBYTE *)(ibm_pData)) + (ptrdiff_t(ibm_cBytesPerPixel) * x) + (ptrdiff_t(ibm_lBytesPerRow) * y));
  }
  //
  // Extract by subsampling data from the given source bitmap with the subsampling
  // factors as indicated. The offsets define the position within the subsampled pixel
  // and shall be between 0..subsampling-1. Length, and modulo values get adjusted 
  // accordingly.
  // IMPORTANT! The caller must guarantee that the subsampling values are all positive
  // and not zero.
  void SubsampleFrom(const struct ImageBitMap *source,
                     UBYTE subx,UBYTE suby,UBYTE offsetx,UBYTE offsety);
  //
  // Advance the line of the data to the next larger Y position. Deliver false in case this
  // is not possible (because we went out of data already).
  bool NextLine(void)
  {
    if (ibm_ulHeight == 0)
      return false;
    ibm_pData = (((UBYTE *)(ibm_pData)) + ibm_lBytesPerRow); 
    if (--ibm_ulHeight == 0)
      return false;
    return true;
  }
  //
  // Initialize the bitmap to have a single object as source or target.
  void AsSinglePoint(ULONG width,ULONG height)
  {  
    ibm_ulWidth         = width;
    ibm_ulHeight        = height;
    ibm_cBytesPerPixel  = 0;
    ibm_lBytesPerRow    = 0;
  }
  // Similar, but from a rectangle.
  void AsSinglePoint(const RectAngle<LONG> &rect)
  {
    ibm_ulWidth         = rect.WidthOf();
    ibm_ulHeight        = rect.HeightOf();
    ibm_cBytesPerPixel  = 0;
    ibm_lBytesPerRow    = 0;
  } 
  //
  // Initialize the bitmap to have a single object as source or target,
  // keep the height unaltered.
  void AsSinglePoint(ULONG width)
  {  
    ibm_ulWidth         = width;
    ibm_cBytesPerPixel  = 0;
    ibm_lBytesPerRow    = 0;
  }
};
///

///
#endif
