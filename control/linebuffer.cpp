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
** This class pulls blocks from the frame and reconstructs from those
** quantized block lines or encodes from them.
**
** $Id: linebuffer.cpp,v 1.13 2015/06/03 15:37:24 thor Exp $
**
*/

/// Includes
#include "control/bitmapctrl.hpp"
#include "control/linebuffer.hpp"
#include "control/residualblockhelper.hpp"
#include "interface/imagebitmap.hpp"
#include "upsampling/upsamplerbase.hpp"
#include "upsampling/downsamplerbase.hpp"
#include "coding/quantizedrow.hpp"
#include "codestream/tables.hpp"
#include "codestream/rectanglerequest.hpp"
#include "marker/frame.hpp"
#include "marker/scan.hpp"
#include "marker/component.hpp"
#include "tools/line.hpp"
#include "dct/dct.hpp"
#include "colortrafo/colortrafo.hpp"
#include "std/string.hpp"
///

/// LineBuffer::LineBuffer
LineBuffer::LineBuffer(class Frame *frame)
  : JKeeper(frame->EnvironOf()), m_pFrame(frame), m_pulY(NULL), m_pulCurrentY(NULL),
    m_pulWidth(NULL), m_pulEnd(NULL),
    m_ppTop(NULL), m_pppCurrent(NULL), m_ppPrev(NULL)
{
  m_ucCount       = m_pFrame->DepthOf(); 
  m_ulPixelWidth  = frame->WidthOf();
  m_ulPixelHeight = frame->HeightOf();
}
///

/// LineBuffer::~LineBuffer
LineBuffer::~LineBuffer(void)
{
  struct Line *row;
  UBYTE i;
  
  if (m_pulY)
    m_pEnviron->FreeMem(m_pulY,m_ucCount * sizeof(ULONG));

  if (m_pulCurrentY)
    m_pEnviron->FreeMem(m_pulCurrentY,m_ucCount * sizeof(ULONG));

  if (m_ppTop) {
    for(i = 0;i < m_ucCount;i++) {
      while((row = m_ppTop[i])) {
        m_ppTop[i] = row->m_pNext;
        if (row->m_pData)
          m_pEnviron->FreeMem(row->m_pData,m_pulWidth[i] * sizeof(LONG));
        delete row;
      }
    }
    m_pEnviron->FreeMem(m_ppTop,m_ucCount * sizeof(struct Line *));
  }

  if (m_pppCurrent)
    m_pEnviron->FreeMem(m_pppCurrent,m_ucCount * sizeof(struct Line **));

  if (m_ppPrev)
    m_pEnviron->FreeMem(m_ppPrev,m_ucCount * sizeof(struct Line *));

  if (m_pulWidth)
    m_pEnviron->FreeMem(m_pulWidth,m_ucCount * sizeof(ULONG));

  if (m_pulEnd)
    m_pEnviron->FreeMem(m_pulEnd,m_ucCount * sizeof(ULONG));
}
///

/// LineBuffer::BuildCommon
// Build common structures for encoding and decoding
void LineBuffer::BuildCommon(void)
{
  UBYTE i;

  if (m_pulY == NULL) {
    m_pulY        = (ULONG *)m_pEnviron->AllocMem(sizeof(ULONG) * m_ucCount);
    memset(m_pulY,0,sizeof(ULONG) * m_ucCount);
  }
  
  if (m_pulCurrentY == NULL) {
    m_pulCurrentY = (ULONG *)m_pEnviron->AllocMem(sizeof(ULONG) * m_ucCount);
    memset(m_pulCurrentY,0,sizeof(ULONG) * m_ucCount);
  }
  
  if (m_pulWidth == NULL) {
    UBYTE i;
    assert(m_pulEnd == NULL);
    m_pulWidth = (ULONG *)m_pEnviron->AllocMem(sizeof(ULONG) * m_ucCount);
    m_pulEnd   = (ULONG *)m_pEnviron->AllocMem(sizeof(ULONG) * m_ucCount);
    for(i = 0;i < m_ucCount;i++) {
      class Component *comp = m_pFrame->ComponentOf(i);
      UBYTE subx            = comp->SubXOf();
      UBYTE mcuw            = comp->MCUWidthOf();
      m_pulWidth[i]         = ((m_ulPixelWidth  + subx - 1) / subx + 7 + mcuw) & (-8);
      m_pulEnd[i]           = ((m_ulPixelWidth  + subx - 1) / subx); 
      // where the line ends, nominally.
    }
  } else {
    assert(m_pulEnd);
  }

  if (m_ppTop == NULL) {
    m_ppTop      = (struct Line **)m_pEnviron->AllocMem(sizeof(struct Line *) * m_ucCount);
    memset(m_ppTop,0,sizeof(struct Line *) * m_ucCount);
  }

  if (m_pppCurrent == NULL) {
    m_pppCurrent  = (struct Line ***)m_pEnviron->AllocMem(sizeof(struct Line **) * m_ucCount);
    for(i = 0;i < m_ucCount;i++) {
      m_pppCurrent[i]       = m_ppTop + i;
    }
  }

  if (m_ppPrev == NULL) {
    m_ppPrev      = (struct Line **) m_pEnviron->AllocMem(sizeof(struct Line *) * m_ucCount);
    memset(m_ppPrev,0,m_ucCount * sizeof(struct Line *));
  }
}
///

/// LineBuffer::ResetStreamToStartOfScan
// Make sure to reset the block control to the
// start of the scan for the indicated components in the scan, 
// required after collecting the statistics for this scan.
void LineBuffer::ResetToStartOfScan(class Scan *scan)
{ 
  UBYTE ccnt = scan->ComponentsInScan();
  
  for(UBYTE i = 0;i < ccnt;i++) {
    class Component *comp = scan->ComponentOf(i);
    UBYTE idx             = comp->IndexOf();
    m_pulY[idx]           = 0;
    m_pulCurrentY[idx]    = 0;
    m_pppCurrent[idx]     = &m_ppTop[idx];
    m_ppPrev[idx]         = NULL;
  }
}
///

/// LineBuffer::StartMCUQuantizerRow
// Start a MCU scan by initializing the quantized rows for this row
// in this scan.
bool LineBuffer::StartMCUQuantizerRow(class Scan *scan)
{
  bool more  = true;
  UBYTE ccnt = scan->ComponentsInScan();
  
  for(UBYTE i = 0;i < ccnt;i++) {
    ULONG y,ymin,ymax,height;
    struct Line **last;
    class Component *comp = scan->ComponentOf(i);
    UBYTE idx       = comp->IndexOf();
    UBYTE mcuheight = (ccnt > 1)?(comp->MCUHeightOf() << 3):(8);
    UBYTE suby      = comp->SubYOf();
    last            = m_pppCurrent[idx];
    height          = (m_ulPixelHeight + suby - 1) / suby;
    ymin            = m_pulY[idx];
    ymax            = ymin + mcuheight; // Always allocated in groups of eight lines.

    if (m_ulPixelHeight > 0 && ymax > height)
      ymax = height;

    if (ymin < ymax) {
      //
      // Advance to the end of the current block row.
      while (*last && m_pulCurrentY[idx] < m_pulY[idx]) {
        m_ppPrev[idx]   = *last;       // last line buffered is previous line of next block.
        last = &((*last)->m_pNext);
        m_pulCurrentY[idx]++;
      }

      for(y = ymin;y < ymax;y++) {
        if (*last == NULL) {
          *last = new(m_pEnviron) struct Line;
        }
        if ((*last)->m_pData == NULL)
          (*last)->m_pData = (LONG *)m_pEnviron->AllocMem(m_pulWidth[idx] * sizeof(LONG));
        if (y == ymin)
          m_pppCurrent[idx] = last;
        last = &((*last)->m_pNext);
      }
    } else {
      more = false;
    }    
    m_pulY[idx] = ymax;
  }

  return more;
}
///

/// LineBuffer::StartMCUResidualRow
// The same for a row of residuals.
bool LineBuffer::StartMCUResidualRow(void)
{
  JPG_THROW(NOT_IMPLEMENTED,"LineBuffer::StartMCUResidualRow",
            "residual coding not implemented (and not necessary) for line based processes");

  return false; // code never goes here.
}
///

/// LineBuffer::BufferedLines
// Return the number of lines available for reconstruction from this scan.
ULONG LineBuffer::BufferedLines(const struct RectangleRequest *rr) const
{
  int i;
  ULONG maxlines = m_ulPixelHeight;

  for(i = rr->rr_usFirstComponent;i <= rr->rr_usLastComponent;i++) {
    class Component *comp = m_pFrame->ComponentOf(i);
    ULONG curline = comp->SubYOf() * (m_pulCurrentY[i] + (comp->MCUHeightOf() << 3));
    if (curline >= m_ulPixelHeight) { // end of image
      curline = m_ulPixelHeight;
    } else if (curline > 0 && comp->SubYOf() > 1) { // need one extra pixel at the end for subsampling expansion
      curline  = (curline - comp->SubYOf()) & (-8); // one additional subsampled line, actually,
      // and as we reconstruct always multiples of eight, round down again.
    }
    if (curline < maxlines)
      maxlines = curline;
  }

  return maxlines;
}
///

/// LineBuffer::DefineRegion
// Define a single 8x8 block starting at the x offset and the given
// line, taking the input 8x8 buffer.
void LineBuffer::DefineRegion(LONG x,struct Line *line,const LONG *buffer,UBYTE comp)
{
  int cnt = 8;
  
  assert(comp < m_ucCount);

  x <<= 3;
  
  if (ULONG(x) + 8 >= m_pulEnd[comp]) { 
    // End of line is affected, duplicate pixels there
    do {
      LONG *dst = line->m_pData + x;
      LONG *end = line->m_pData + m_pulWidth[comp];
      LONG *lst = line->m_pData + m_pulEnd[comp] - 1; // last valid pixel

      memcpy(dst,buffer,8 * sizeof(LONG));

      // Duplicate pixel over the edge.
      dst = lst + 1;
      while(dst < end) {
        *dst++ = *lst;
      }
      buffer += 8;
      line  = line->m_pNext;
    } while(line && --cnt); 
  } else {
    do {
      memcpy(line->m_pData + x,buffer,8 * sizeof(LONG));
      buffer += 8;
      line  = line->m_pNext; // Duplicate at the end of the image.
    } while(line && --cnt);
  }
}
///

/// LineBuffer::FetchRegion
// Define a single 8x8 block starting at the x offset and the given
// line, taking the input 8x8 buffer.
void LineBuffer::FetchRegion(LONG x,const struct Line *line,LONG *buffer)
{
  int cnt = 8;
  do {
    memcpy(buffer,line->m_pData + (x << 3),8 * sizeof(LONG));
    buffer += 8;
    line = line->m_pNext;
  } while(line && --cnt);
}
///
