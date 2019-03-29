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
 * $Id: imagebitmap.cpp,v 1.8 2014/09/30 08:33:17 thor Exp $
 *
 *
 */

/// Includes
#include "interface/imagebitmap.hpp"
#include "tools/traits.hpp"
#include "std/stddef.hpp"
///

/// ImageBitMap::ExtractBitMap
//
void ImageBitMap::ExtractBitMap(const struct ImageBitMap *source,const RectAngle<LONG> &rect)
{
  LONG xshift,yshift;
  ULONG width  = 0,height  = 0;
  ULONG iwidth = 0,iheight = 0;

  // just copy the bitmap data over, make modifications
  // later. Not required if source and destination are identically.
  if (this != source)
    *this = *source;

  // compute the left offset of the new bitmap within the
  // old bitmap. Negative rectangle coordiates are clipped
  // away.
  xshift = (rect.ra_MinX<0)?(0):(rect.ra_MinX);
  yshift = (rect.ra_MinY<0)?(0):(rect.ra_MinY);

  // compute the dimension of the remaining
  // rectangle if we clip away the overlapping
  // edges to the left
  if (xshift <= rect.ra_MaxX) {
    width  = rect.ra_MaxX - xshift + 1;
    if (source->ibm_ulWidth > ULONG(xshift))
      iwidth = source->ibm_ulWidth - xshift;
    else
      iwidth = 0; // clipped off completely
  }

  if (yshift <= rect.ra_MaxY) {
    height  = rect.ra_MaxY - yshift + 1;
    if (source->ibm_ulHeight > ULONG(yshift))
      iheight = source->ibm_ulHeight - yshift;
    else
      iheight = 0; // clipped off completely
  }
  
  // clip this again to the width of the parent bitmap 
  // to make sure that we do not write past the right or
  // bottom edge.

  if (width > iwidth)
    width = iwidth;

  if (height > iheight)
    height = iheight;

  // check whether the source bitmap is *blank*, i.e. has
  // a NULL data pointer. If so, make the child blank as well
  // and null out width and height for consistency
  if (source->ibm_ucPixelType && width && height) {
    ibm_pData = ((UBYTE *)source->ibm_pData) + 
      xshift * ptrdiff_t(source->ibm_cBytesPerPixel) +
      yshift * ptrdiff_t(source->ibm_lBytesPerRow);

  } else {
    ibm_pData       = NULL;
    // ibm_ucPixelType = 0;  // FIX: Leave the pixel type valid such that further
    // requests can still find that data can be extracted from here, though the
    // result will always be blank.
    // Special rule: If the source bitmap is indicated as blank, do not
    // define constraints concerning its dimensions.
    if (source->ibm_ucPixelType == 0) {
      width  = MAX_LONG;
      height = MAX_LONG;
    }
  }
  //
  ibm_ulWidth  = width;  // the component transformer needs width
  ibm_ulHeight = height; // and height to be initialized even for blank BMs.
}
///

/// ImageBitMap::SubsampleFrom
// Extract by subsampling data from the given source bitmap with the subsampling
// factors as indicated. The offsets define the position within the subsampled pixel
// and shall be between 0..subsampling-1. Length, and modulo values get adjusted 
// accordingly.
// IMPORTANT! The caller must guarantee that the subsampling values are all positive
// and not zero.
void ImageBitMap::SubsampleFrom(const struct ImageBitMap *src,UBYTE subx,UBYTE suby,UBYTE xoffset,UBYTE yoffset)
{
  // For the following, note that the user IBM's are anchored at the canvas offset (0,0) and not at the
  // tile or requested rectangle.
  if (src->ibm_ulWidth > xoffset) {
    ibm_ulWidth         = 1 + (src->ibm_ulWidth  - xoffset - 1) / subx;
  } else {
    ibm_ulWidth         = 0;
  }
  if (src->ibm_ulHeight > yoffset) {    
    ibm_ulHeight        = 1 + (src->ibm_ulHeight - yoffset - 1) / suby;
  } else {
    ibm_ulHeight        = 0;
  }
  if (src->ibm_ucPixelType && ibm_ulWidth > 0 && ibm_ulHeight > 0) {
    ibm_pData         = ((UBYTE *)(src->ibm_pData))
      + xoffset * ptrdiff_t(src->ibm_cBytesPerPixel) + yoffset * ptrdiff_t(src->ibm_lBytesPerRow);
  } else {
    ibm_pData         = NULL;
  }
  ibm_cBytesPerPixel  = src->ibm_cBytesPerPixel * subx;
  ibm_lBytesPerRow    = src->ibm_lBytesPerRow   * suby;
  ibm_ucPixelType     = src->ibm_ucPixelType;
}
///

/// ImageBitMap::DefineBitMap
// Define a bitmap to describe an image of the given dimension at the given rectangle.
void ImageBitMap::DefineBitMap(APTR buffer,UBYTE type,const RectAngle<LONG> &rect)
{
  UBYTE pixelsize    = type & CTYP_SIZE_MASK;
  // Initialize the internal bitmap (that the traditional color transformer maps to) to
  // the internal buffer of the client data. Note that libjpeg always
  // anchores the user bitmaps at zero,zero, so shift it.
  ibm_ulWidth        = rect.ra_MaxX + 1; // note that rect is always inclusive
  ibm_ulHeight       = rect.ra_MaxY + 1;
  // Fixpoint types.
  ibm_ucPixelType    = type;
  ibm_cBytesPerPixel = pixelsize;
  ibm_lBytesPerRow   = rect.WidthOf() * pixelsize;
  ibm_pData          = ((UBYTE *)buffer) - rect.ra_MinX * pixelsize - rect.ra_MinY * ibm_lBytesPerRow;
}
///

/// ImageBitMap::DefineBitMap
// Define a bitmap of the given dimension with the given modulo value.
void ImageBitMap::DefineBitMap(APTR buffer,UBYTE type,const RectAngle<LONG> &rect,ULONG samplesperrow)
{ 
  UBYTE pixelsize    = type & CTYP_SIZE_MASK;
  // Initialize the internal bitmap (that the traditional color transformer maps to) to
  // the internal buffer of the client data. Note that libjpeg always
  // anchores the user bitmaps at zero,zero, so shift it.
  ibm_ulWidth        = rect.ra_MaxX + 1; // note that rect is always inclusive
  ibm_ulHeight       = rect.ra_MaxY + 1;
  // Fixpoint types.
  ibm_ucPixelType    = type;
  ibm_cBytesPerPixel = pixelsize;
  ibm_lBytesPerRow   = samplesperrow * pixelsize;
  ibm_pData          = ((UBYTE *)buffer) - rect.ra_MinX * pixelsize - rect.ra_MinY * ibm_lBytesPerRow;
}
///

