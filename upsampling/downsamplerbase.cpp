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
** $Id: downsamplerbase.cpp,v 1.11 2014/09/30 08:33:18 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "tools/rectangle.hpp"
#include "upsampling/downsamplerbase.hpp"
#include "upsampling/downsampler.hpp"
#include "std/string.hpp"
///

/// DownsamplerBase::DownsamplerBase
DownsamplerBase::DownsamplerBase(class Environ *env,int sx,int sy,ULONG width,ULONG height)
  : JKeeper(env), m_ulWidth(width), m_lTotalLines(height), m_lY(0), m_lHeight(0),
    m_ucSubX(sx), m_ucSubY(sy), m_pInputBuffer(NULL), m_pLastRow(NULL), m_pFree(NULL)
{
}
///

/// DownsamplerBase::~DownsamplerBase
DownsamplerBase::~DownsamplerBase(void)
{ 
  struct Line *row;

  while((row = m_pInputBuffer)) {
    m_pInputBuffer = row->m_pNext;
    if (row->m_pData)
      m_pEnviron->FreeMem(row->m_pData,(m_ulWidth + (m_ucSubX << 3)) * sizeof(LONG));
    delete row;
  } 

  while((row = m_pFree)) {
    m_pFree = row->m_pNext;
    m_pEnviron->FreeMem(row->m_pData,(m_ulWidth + (m_ucSubX << 3)) * sizeof(LONG));
    delete row;
  }
}
///

/// DownsamplerBase::SetBufferedRegion
// Define the region to be buffered, clipping off what has been applied
// here before. This extends the internal buffer to hold at least
// the regions here.
void DownsamplerBase::SetBufferedRegion(const RectAngle<LONG> &region)
{
  //
  // Does just the same right now...
  ExtendBufferedRegion(region);
}
///

/// DownsamplerBase::ExtendBufferedRegion
// Make the buffered region larger to include at least the given rectangle.
// The rectangle is given in image/canvas coordinates.
void DownsamplerBase::ExtendBufferedRegion(const RectAngle<LONG> &region)
{ 
  // Create all lines between the current last line, m_lY+m_lHeight-1 and
  // the last line of the rectangle, region.ra_MaxY
  while(m_lY + m_lHeight < region.ra_MaxY + 1) {
    struct Line *qrow,*alloc = NULL;
    //
    // Get a new pixel row, either from the buffered
    // rows or from the heap.
    if (m_pFree) {
      qrow          = m_pFree;
      m_pFree       = qrow->m_pNext;
      qrow->m_pNext = NULL;
    } else {
      alloc         = new(m_pEnviron) struct Line;
      qrow          = alloc;
    }
    //
    // 
    if (m_pLastRow) {
      m_pLastRow->m_pNext = qrow;
      m_pLastRow = qrow;
    } else {
      assert(m_pInputBuffer == NULL);
      m_pLastRow = m_pInputBuffer = qrow;
    }
    //
    // Allocate the memory for it.
    if (alloc) {
      alloc->m_pData = (LONG *)m_pEnviron->AllocMem((m_ulWidth + (m_ucSubX << 3)) * sizeof(LONG));
    }
    m_lHeight++;
  }
}
///

/// DownsamplerBase::DefineRegion
// Define the region to contain the given data, copy it to the line buffers
// for later downsampling. Coordinates are in 8x8 blocks.
void DownsamplerBase::DefineRegion(LONG x,LONG y,const LONG *data)
{
  struct Line *line = m_pInputBuffer;
  LONG topy = y << 3;
  LONG yf   = m_lY;
  LONG ofs  = x << 3;
  LONG cnt  = 8;
  LONG ovl  = (m_ucSubX << 3) - 1; // number of pixels extended to the right.

  assert(topy >= m_lY && topy < m_lY + m_lHeight);

  while(yf < topy) {
    line = line->m_pNext;
    yf++;
  }

  assert(line);
  
  if (ofs + 8 >= LONG(m_ulWidth)) {
    do {
      LONG *dst = line->m_pData;
      UBYTE i;
      // Overlab at the boundary, extend to the right to keep the downsampling simple
      memcpy(dst + ofs,data,8 * sizeof(LONG));
      for(i = 0; i < ovl;i++) {
        // Mirror-extend. Actually, any type of extension is suitable as long as the
        // mean is sensible.
        dst[m_ulWidth + i] = dst[(m_ulWidth > i)?(m_ulWidth - 1 - i):0]; 
      }
      line  = line->m_pNext;
      data += 8;
    } while(--cnt && line);
  } else {
    do {
      memcpy(line->m_pData + ofs,data,8 * sizeof(LONG));
      line  = line->m_pNext;
      data += 8;
    } while(--cnt && line);
  }
}
///

/// DownsamplerBase::RemoveBlocks
// Remove the blocks of the given block line, given in downsampled
// block coordinates.
void DownsamplerBase::RemoveBlocks(ULONG by)
{
  LONG firstkeep = ((by + 1) << 3) * m_ucSubY; // The first line that has to be kept.

  while(m_lY < firstkeep) {
    struct Line *row;
    // The current Y line is no longer required, drop it. If it is there.
    row = m_pInputBuffer;
    if (row) {
      m_pInputBuffer = row->m_pNext;
      if (m_pInputBuffer == NULL) {
        assert(row == m_pLastRow); 
        assert(m_lHeight == 1);
        // it hopefully is as it has no following line
        m_pLastRow = NULL;
      }
      row->m_pNext = m_pFree;
      m_pFree      = row;
      m_lHeight--;
    }
    m_lY++;
  }
}
///

/// DownsamplerBase::GetCollectedBlocks
// Return a rectangle of block coordinates in the downsampled domain
// that is ready for output.
void DownsamplerBase::GetCollectedBlocks(RectAngle<LONG> &rect) const
{
  // Everything in horizontal direction.
  rect.ra_MinX = 0;
  rect.ra_MaxX = (((m_ulWidth + m_ucSubX - 1) / m_ucSubX + 7) >> 3) - 1;
  // In vertical direction, start at the upper edge of the first buffered line,
  // but use the first complete block.
  rect.ra_MinY = ((m_lY / m_ucSubY) + 7) >> 3;
  // Find the first block that is not buffered, remove that block.
  // If we are at the end of the image, just return the last block and
  // complete even if not all lines are ready.
  if (m_lY + m_lHeight >= m_lTotalLines) {
    rect.ra_MaxY = (((m_lTotalLines + m_ucSubY - 1) / m_ucSubY + 7) >> 3) - 1;
  } else {
    rect.ra_MaxY = (((m_lY + m_lHeight) / m_ucSubY) >> 3) - 1;
  }
}
///

/// DownsamplerBase::CreateDownsampler
// Create an upsampler for the given upsampling factors. Currently, only
// factors from 1x1 to 4x4 are supported.
class DownsamplerBase *DownsamplerBase::CreateDownsampler(class Environ *env,int sx,int sy,ULONG width,ULONG height)
{
  switch(sy) {
  case 1:
    switch(sx) {
    case 1:
      return new(env) Downsampler<1,1>(env,width,height);
      break;
    case 2:
      return new(env) Downsampler<2,1>(env,width,height);
      break;
    case 3:
      return new(env) Downsampler<3,1>(env,width,height);
      break;
    case 4:
      return new(env) Downsampler<4,1>(env,width,height);
      break;
    }
    break;
  case 2:
    switch(sx) {
    case 1:
      return new(env) Downsampler<1,2>(env,width,height);
      break;
    case 2:
      return new(env) Downsampler<2,2>(env,width,height);
      break;
    case 3:
      return new(env) Downsampler<3,2>(env,width,height);
      break;
    case 4:
      return new(env) Downsampler<4,2>(env,width,height);
      break;
    }
    break;
  case 3:
    switch(sx) {
    case 1:
      return new(env) Downsampler<1,3>(env,width,height);
      break;
    case 2:
      return new(env) Downsampler<2,3>(env,width,height);
      break;
    case 3:
      return new(env) Downsampler<3,3>(env,width,height);
      break;
    case 4:
      return new(env) Downsampler<4,3>(env,width,height);
      break;
    }
    break;
  case 4:
    switch(sx) {
    case 1:
      return new(env) Downsampler<1,4>(env,width,height);
      break;
    case 2:
      return new(env) Downsampler<2,4>(env,width,height);
      break;
    case 3:
      return new(env) Downsampler<3,4>(env,width,height);
      break;
    case 4:
      return new(env) Downsampler<4,4>(env,width,height);
      break;
    }
    break;
  }

  return NULL;
}
///


