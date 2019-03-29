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
** $Id: blockbuffer.cpp,v 1.20 2016/10/28 13:58:53 thor Exp $
**
*/

/// Includes
#include "control/bitmapctrl.hpp"
#include "control/blockbuffer.hpp"
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
#include "dct/dct.hpp"
#include "colortrafo/colortrafo.hpp"
#include "std/string.hpp"
///

/// BlockBuffer::BlockBuffer
BlockBuffer::BlockBuffer(class Frame *frame)
  : BlockCtrl(frame->EnvironOf()), m_pFrame(frame), m_pulY(NULL), m_pulCurrentY(NULL), 
    m_ppDCT(NULL), 
    m_ppQTop(NULL), m_ppRTop(NULL), 
    m_pppQStream(NULL), m_pppRStream(NULL)
{
  m_ucCount       = frame->DepthOf();
  m_ulPixelWidth  = frame->WidthOf();
  m_ulPixelHeight = frame->HeightOf();
}
///

/// BlockBuffer::~BlockBuffer
BlockBuffer::~BlockBuffer(void)
{
  class QuantizedRow *row;
  UBYTE i;

  if (m_ppDCT) {
    for(i = 0;i < m_ucCount;i++) {
      delete m_ppDCT[i];
    }
    m_pEnviron->FreeMem(m_ppDCT,m_ucCount * sizeof(class DCT *));
  }
  
  if (m_pulY)
    m_pEnviron->FreeMem(m_pulY,m_ucCount * sizeof(ULONG));
  
  if (m_pulCurrentY)
    m_pEnviron->FreeMem(m_pulCurrentY,m_ucCount * sizeof(ULONG));

  if (m_ppQTop) {
    for(i = 0;i < m_ucCount;i++) {
      while((row = m_ppQTop[i])) {
        m_ppQTop[i] = row->NextOf();
        delete row;
      }
    }
    m_pEnviron->FreeMem(m_ppQTop,m_ucCount * sizeof(class QuantizedRow *));
  }

  if (m_ppRTop) {
    for(i = 0;i < m_ucCount;i++) {
      while((row = m_ppRTop[i])) {
        m_ppRTop[i] = row->NextOf();
        delete row;
      }
    }
    m_pEnviron->FreeMem(m_ppRTop,m_ucCount * sizeof(class QuantizedRow *));
  }

  if (m_pppQStream)
    m_pEnviron->FreeMem(m_pppQStream,m_ucCount * sizeof(class QuantizedRow **));

  if (m_pppRStream)
    m_pEnviron->FreeMem(m_pppRStream,m_ucCount * sizeof(class QuantizedRow **));
}
///

/// BlockBuffer::BuildCommon
// Build common structures for encoding and decoding
void BlockBuffer::BuildCommon(void)
{
  if (m_ppDCT == NULL) {
    m_ppDCT = (class DCT **)m_pEnviron->AllocMem(sizeof(class DCT *) * m_ucCount);
    memset(m_ppDCT,0,sizeof(class DCT *) * m_ucCount);
  }

  if (m_pulY == NULL) {
    m_pulY        = (ULONG *)m_pEnviron->AllocMem(sizeof(ULONG) * m_ucCount);
    memset(m_pulY,0,sizeof(ULONG) * m_ucCount);
  }
  
  if (m_pulCurrentY == NULL) {
    m_pulCurrentY = (ULONG *)m_pEnviron->AllocMem(sizeof(ULONG) * m_ucCount);
    memset(m_pulCurrentY,0,sizeof(ULONG) * m_ucCount);
  }

  if (m_ppQTop == NULL) {
    m_ppQTop      = (class QuantizedRow **)m_pEnviron->AllocMem(sizeof(class QuantizedRow *) * 
                                                              m_ucCount);
    memset(m_ppQTop,0,sizeof(class QuantizedRow *) * m_ucCount);
  }

  if (m_ppRTop == NULL) {
    m_ppRTop      = (class QuantizedRow **)m_pEnviron->AllocMem(sizeof(class QuantizedRow *) * 
                                                                m_ucCount);
    memset(m_ppRTop,0,sizeof(class QuantizedRow *) * m_ucCount);
  }

  if (m_pppQStream == NULL) {
    m_pppQStream  = (class QuantizedRow ***)m_pEnviron->AllocMem(sizeof(class QuantizedRow **) * 
                                                                 m_ucCount);
    memset(m_pppQStream,0,m_ucCount * sizeof(class QuantizedRow **));
  }

  if (m_pppRStream == NULL) {
    m_pppRStream  = (class QuantizedRow ***)m_pEnviron->AllocMem(sizeof(class QuantizedRow **) * 
                                                                 m_ucCount);
    memset(m_pppRStream,0,m_ucCount * sizeof(class QuantizedRow **));
  }
}
///

/// BlockBuffer::ResetStreamToStartOfScan
// Make sure to reset the block control to the
// start of the scan for the indicated components in the scan, 
// required after collecting the statistics for this scan.
void BlockBuffer::ResetToStartOfScan(class Scan *scan)
{ 
  if (scan) {
    UBYTE ccnt = scan->ComponentsInScan();
    
    for(UBYTE i = 0;i < ccnt;i++) {
      class Component *comp = scan->ComponentOf(i);
      UBYTE idx             = comp->IndexOf(); 
      if (m_ppDCT[idx] == NULL)
        m_ppDCT[idx]        = m_pFrame->TablesOf()->BuildDCT(comp,m_ucCount,
                                                             m_pFrame->HiddenPrecisionOf());
      m_pulY[idx]           = 0;
      m_pulCurrentY[idx]    = 0;
      m_pppQStream[idx]     = NULL;
      m_pppRStream[idx]     = NULL;
    }
  } else {
    // All components.
    for(UBYTE idx = 0;idx < m_ucCount;idx++) { 
      class Component *comp = m_pFrame->ComponentOf(idx);
      if (m_ppDCT[idx] == NULL)
        m_ppDCT[idx]        = m_pFrame->TablesOf()->BuildDCT(comp,m_ucCount,
                                                             m_pFrame->HiddenPrecisionOf());
      m_pulY[idx]           = 0;
      m_pulCurrentY[idx]    = 0;
      m_pppQStream[idx]     = NULL;
      m_pppRStream[idx]     = NULL;
    }
  }
}
///

/// BlockBuffer::StartMCUQuantizerRow
// Start a MCU scan by initializing the quantized rows for this row
// in this scan.
bool BlockBuffer::StartMCUQuantizerRow(class Scan *scan)
{
  bool more  = true;
  UBYTE ccnt = scan->ComponentsInScan();
  
  for(UBYTE i = 0;i < ccnt;i++) {
    ULONG y,ymin,ymax,width,height;
    class QuantizedRow **last;
    class Component *comp = scan->ComponentOf(i);
    UBYTE mcuheight = (ccnt > 1)?(comp->MCUHeightOf()):(1);
    UBYTE subx      = comp->SubXOf();
    UBYTE suby      = comp->SubYOf();
    UBYTE idx       = comp->IndexOf();
    last            = m_pppQStream[idx];
    width           = (m_ulPixelWidth  + subx - 1) / subx;
    height          = (m_ulPixelHeight + suby - 1) / suby;
    ymin            = m_pulY[idx];
    ymax            = ymin + (mcuheight << 3);

    if (m_ulPixelHeight > 0 && ymax > height)
      ymax = height;

    if (ymin < ymax) {
      m_pulCurrentY[idx] = m_pulY[idx];
  
      //
      // Skip all the lines in the MCU
      if (last) {
        while(mcuheight) {
          assert(*last);
          last = &((*last)->NextOf());
          mcuheight--;
        }
      } else {
        last = &m_ppQTop[idx];
      }

      for(y = ymin;y < ymax;y+=8) {
        if (*last == NULL) {
          *last = new(m_pEnviron) class QuantizedRow(m_pEnviron);
        }
        (*last)->AllocateRow(width);
        if (y == ymin)
          m_pppQStream[idx] = last;
        last = &((*last)->NextOf());
      }
    } else {
      more = false;
    }    
    m_pulY[idx] = ymax;
  }

  return more;
}
///

/// BlockBuffer::BufferedLines
// Return the number of lines available for reconstruction from this scan.
ULONG BlockBuffer::BufferedLines(const struct RectangleRequest *rr) const
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
/// BlockBuffer::StartMCUResidualRow
// Start a MCU scan by initializing the residuals for this row.
bool BlockBuffer::StartMCUResidualRow(class Scan *scan)
{
  bool more  = true;
  UBYTE ccnt = scan->ComponentsInScan();

  for(UBYTE j = 0;j < ccnt;j++) {
    ULONG y,ymin,ymax,width,height;
    class QuantizedRow **last;
    class Component *comp = scan->ComponentOf(j);
    UBYTE i         = comp->IndexOf();
    UBYTE mcuheight = (ccnt > 1)?(comp->MCUHeightOf()):(1);
    UBYTE subx      = comp->SubXOf();
    UBYTE suby      = comp->SubYOf(); 
    last            = m_pppRStream[i];
    width           = (m_ulPixelWidth  + subx - 1) / subx;
    height          = (m_ulPixelHeight + suby - 1) / suby;
    ymin            = m_pulY[i];
    ymax            = ymin + (mcuheight << 3);  
    
    if (m_ulPixelHeight > 0 && ymax > height)
      ymax = height;

    if (ymin < ymax) {
      m_pulCurrentY[i] = m_pulY[i];

      if (last) {
        while(mcuheight) {
          assert(*last);
          last = &((*last)->NextOf());
          mcuheight--;
        }
      } else {
        last = &m_ppRTop[i];
      }

      for(y = ymin;y < ymax;y+=8) {
        if (*last == NULL) {
          *last = new(m_pEnviron) class QuantizedRow(m_pEnviron);
        }
        (*last)->AllocateRow(width);
        if (y == ymin)
          m_pppRStream[i] = last;
        last = &((*last)->NextOf());
      }
    } else {
      more = false;
    }
    m_pulY[i] = ymax;
  }

  return more;
}
///

