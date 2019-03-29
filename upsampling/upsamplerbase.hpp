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
**
** Base class for all upsamplers, common for all upsampling processes
** and independent of the upsampling factors.
**
** $Id: upsamplerbase.hpp,v 1.16 2014/09/30 08:33:18 thor Exp $
**
*/

#ifndef UPSAMPLING_UPSAMPLERBASE_HPP
#define UPSAMPLING_UPSAMPLERBASE_HPP

/// Includes
#include "tools/environment.hpp"
#include "tools/rectangle.hpp"
#include "tools/line.hpp"
///

/// Class UpsamplerBase
// This class provides the infrastructure for the upsampling process
// common to all implementations regardless of the upsampling factors.
class UpsamplerBase : public JKeeper {
  //
protected:
  //
  // The width of the upsampled data in subsampled pixels.
  ULONG               m_ulWidth;
  //
  // The height of the upsampled data in subsampled lines.
  LONG                m_lTotalLines;
  //
  // The top Y line of the buffer we hold.
  LONG                m_lY;
  //
  // The number of lines buffered here.
  LONG                m_lHeight;
  //
  // Dimensions in pixels.
  ULONG               m_ulPixelWidth;
  ULONG               m_ulPixelHeight;
  //
  // Subsampling factors.
  UBYTE               m_ucSubX;
  //
  UBYTE               m_ucSubY;
  //
  // The reconstructed data itself. This is the input buffer
  // of the upsampling filter.
  struct Line        *m_pInputBuffer;
  //
  // The actual implementations: Filter horizontally from the line into the 8x8 buffer
  template<int>
  static void HorizontalFilterCore(int xmod,LONG *target);
  //
  template<int>
  static void VerticalFilterCore(int ymod,struct Line *top,struct Line *cur,struct Line *bot,
                                 LONG offset,LONG *target);
  //
private:
  //
  // The last row of the buffer.
  struct Line *m_pLastRow;
  //
  // Lines currently not in use.
  struct Line *m_pFree;
  //
public:
  UpsamplerBase(class Environ *env,int sx,int sy,ULONG width,ULONG height);
  //
  virtual ~UpsamplerBase(void);
  //
  // The actual upsampling process. To be implemented by the actual
  // classes inheriting from this.
  virtual void UpsampleRegion(const RectAngle<LONG> &r,LONG *buffer) const = 0;
  //
  // Define the region to be buffered, clipping off what has been buffered
  // here before. Modifies the rectangle to contain only what is needed
  // in addition to what is here already. The rectangle is in block indices.
  void SetBufferedRegion(RectAngle<LONG> &region);
  //
  // Set the buffered region given in image regions, not in
  // block regions. Returns the updated rectangle in blocks
  void SetBufferedImageRegion(RectAngle<LONG> &region);
  //
  // Make the buffered region larger to include at least the given rectangle.
  // The rectangle is given in block indices, not in canvas coordinates.
  void ExtendBufferedRegion(const RectAngle<LONG> &region);
  //
  // Define the region to contain the given data, copy it to the line buffers
  // for later upsampling. Coordinates are in blocks.
  void DefineRegion(LONG bx,LONG by,const LONG *data);
  //
  // Remove the blocks of the given block line, given in image
  // block coordinates.
  void RemoveBlocks(ULONG by);
  //
  // Return a rectangle of block coordinates in the image domain
  // that is ready for output.
  void GetCollectedBlocks(RectAngle<LONG> &rect) const;
  //
  // Create an upsampler for the given upsampling factors. Currently, only
  // factors from 1x1 to 4x4 are supported.
  static class UpsamplerBase *CreateUpsampler(class Environ *env,int sx,int sy,ULONG width,ULONG height);
};
///

///
#endif

