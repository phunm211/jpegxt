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
** $Id: linebitmaprequester.cpp,v 1.31 2015/06/03 15:37:24 thor Exp $
**
*/

/// Includes
#include "control/bitmapctrl.hpp"
#include "control/linebitmaprequester.hpp"
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

/// LineBitmapRequester::LineBitmapRequester
LineBitmapRequester::LineBitmapRequester(class Frame *frame)
  : LineBuffer(frame), BitmapCtrl(frame), m_pEnviron(frame->EnvironOf()), m_pFrame(frame),
    m_pulReadyLines(NULL),
    m_ppDownsampler(NULL), m_ppUpsampler(NULL), m_ppTempIBM(NULL), 
    m_pppImage(NULL), m_bSubsampling(false)
{ 
  m_ucCount       = frame->DepthOf();
  m_ulPixelWidth  = frame->WidthOf();
  m_ulPixelHeight = frame->HeightOf();
}
///

/// LineBitmapRequester::~LineBitmapRequester
LineBitmapRequester::~LineBitmapRequester(void)
{
  UBYTE i;

  if (m_ppDownsampler) {
    for(i = 0;i < m_ucCount;i++) {
      delete m_ppDownsampler[i];
    }
    m_pEnviron->FreeMem(m_ppDownsampler,m_ucCount * sizeof(class DownsamplerBase *));
  }

  if (m_ppUpsampler) {
    for(i = 0;i < m_ucCount;i++) {
      delete m_ppUpsampler[i];
    }
    m_pEnviron->FreeMem(m_ppUpsampler,m_ucCount * sizeof(class UpsamplerBase *));
  }

  if (m_ppTempIBM) {
    for(i = 0;i < m_ucCount;i++) {
      delete m_ppTempIBM[i];
    }
    m_pEnviron->FreeMem(m_ppTempIBM,m_ucCount * sizeof(struct ImageBitMap *));
  }
  
  if (m_pulReadyLines)
    m_pEnviron->FreeMem(m_pulReadyLines,m_ucCount * sizeof(ULONG));

  if (m_pppImage)
    m_pEnviron->FreeMem(m_pppImage,m_ucCount * sizeof(struct Line **));
}
///

/// LineBitmapRequester::BuildCommon
// Build common structures for encoding and decoding
void LineBitmapRequester::BuildCommon(void)
{
  UBYTE i;

  BitmapCtrl::BuildCommon();
  LineBuffer::BuildCommon();

  if (m_ppTempIBM == NULL) {
    m_ppTempIBM = (struct ImageBitMap **)m_pEnviron->AllocMem(sizeof(struct ImageBitMap **) * m_ucCount);
    memset(m_ppTempIBM,0,sizeof(struct ImageBitMap *) * m_ucCount);
  }

  if (m_pulReadyLines == NULL) {
    m_pulReadyLines = (ULONG *)m_pEnviron->AllocMem(sizeof(ULONG) * m_ucCount);
    memset(m_pulReadyLines,0,sizeof(ULONG) * m_ucCount);
  }

  if (m_pppImage == NULL) {
    m_pppImage    = (struct Line ***)m_pEnviron->AllocMem(sizeof(struct Line **) * m_ucCount);
    for(i = 0;i < m_ucCount;i++) {
       m_pppImage[i]         = m_ppTop + i;
    }
  }

  for(i = 0;i < m_ucCount;i++) {
    if (m_ppTempIBM[i] == NULL)
      m_ppTempIBM[i]      = new(m_pEnviron) struct ImageBitMap();
  }
}
///

/// LineBitmapRequester::PrepareForEncoding
// First time usage: Collect all the information
void LineBitmapRequester::PrepareForEncoding(void)
{  
  UBYTE i;
  
  BuildCommon();

  if (m_ppDownsampler == NULL) {
    m_ppDownsampler = (class DownsamplerBase **)m_pEnviron->AllocMem(sizeof(class DownsamplerBase *) * m_ucCount);
    memset(m_ppDownsampler,0,sizeof(class DownsamplerBase *) * m_ucCount);
    
    for(i = 0;i < m_ucCount;i++) {
      class Component *comp = m_pFrame->ComponentOf(i);
      UBYTE sx = comp->SubXOf();
      UBYTE sy = comp->SubYOf();
      
      if (sx > 1 || sy > 1) {
        m_ppDownsampler[i] = DownsamplerBase::CreateDownsampler(m_pEnviron,sx,sy,
                                                                m_ulPixelWidth,m_ulPixelHeight);
        m_bSubsampling     = true;
      }
    }
  }
}
///

/// LineBitmapRequester::PrepareForDecoding
// First time usage: Collect all the information
void LineBitmapRequester::PrepareForDecoding(void)
{  
  UBYTE i;
  
  BuildCommon();

  if (m_ppUpsampler == NULL) {
    m_ppUpsampler = (class UpsamplerBase **)m_pEnviron->AllocMem(sizeof(class UpsamplerBase *) * m_ucCount);
    memset(m_ppUpsampler,0,sizeof(class Upsampler *) * m_ucCount);
    
    for(i = 0;i < m_ucCount;i++) {
      class Component *comp = m_pFrame->ComponentOf(i);
      UBYTE sx = comp->SubXOf();
      UBYTE sy = comp->SubYOf();
      
      if (sx > 1 || sy > 1) {
        m_ppUpsampler[i] = UpsamplerBase::CreateUpsampler(m_pEnviron,sx,sy,
                                                          m_ulPixelWidth,m_ulPixelHeight);
        m_bSubsampling   = true;
      }
    }
  }
}
///

/// LineBitmapRequester::ResetToStartOfImage
// Reset all components on the image side of the control to the
// start of the image. Required when re-requesting the image
// for encoding or decoding.
void LineBitmapRequester::ResetToStartOfImage(void)
{
  for(UBYTE i = 0;i < m_ucCount;i++) {
    m_pppImage[i]      = &m_ppTop[i];
    m_pulReadyLines[i] = 0;
  }
}
///

/// LineBitmapRequester::ColorTrafoOf
// Return the color transformer responsible for this scan.
class ColorTrafo *LineBitmapRequester::ColorTrafoOf(bool encoding)
{
  return m_pFrame->TablesOf()->ColorTrafoOf(m_pFrame,NULL,PixelTypeOf(),encoding);
}
///

/// LineBitmapRequester::Start8Lines
// Get the next block of eight lines of the image
struct Line *LineBitmapRequester::Start8Lines(UBYTE c)
{
  if (*m_pppImage[c] == NULL) {
    struct Line **target = m_pppImage[c];
    int cnt = 8;
    do {
      *target = new(m_pEnviron) struct Line;
      (*target)->m_pData = (LONG *)m_pEnviron->AllocMem(m_pulWidth[c] * sizeof(LONG));
      target  = &((*target)->m_pNext);
    } while(--cnt);
  }
  return *m_pppImage[c];
}
///

/// LineBitmapRequester::Next8Lines
// Advance the image line pointer by the next eight lines
// which is here a "pseudo"-MCU block.
void LineBitmapRequester::Next8Lines(UBYTE c)
{
  int cnt = 8;
  do {
    struct Line *row = *m_pppImage[c];
    assert(row);
    m_pppImage[c] = &(row->m_pNext);
  } while(--cnt && *m_pppImage[c]);
}
///

/// LineBitmapRequester::CropEncodingRegion
// First step of a region encoder: Find the region that can be pulled in the next step,
// from a rectangle request. This potentially shrinks the rectangle, which should be
// initialized to the full image.
void LineBitmapRequester::CropEncodingRegion(RectAngle<LONG> &region,const struct RectangleRequest *)
{
  int i;  

  ClipToImage(region);

  // Find the region to request.
  for(i = 0;i < m_ucCount;i++) {
    if (m_pulReadyLines[i] < ULONG(region.ra_MinY))
      region.ra_MinY = m_pulReadyLines[i];
  } 
}
///

/// LineBitmapRequester::RequestUserDataForEncoding
// Request user data for encoding for the given region, potentially clip the region to the
// data available from the user.
void LineBitmapRequester::RequestUserDataForEncoding(class BitMapHook *bmh,RectAngle<LONG> &region,bool alpha)
{ 
  int i;

  m_ulMaxMCU = MAX_ULONG;
  
  for(i = 0;i < m_ucCount;i++) {
    ULONG max;
    //
    // Components are always requested completely on encoding.
    RequestUserData(bmh,region,i,alpha);
    // All components must have the same sample precision here.
    max = (m_ppBitmap[i]->ibm_ulHeight - 1) >> 3;
    if (max < m_ulMaxMCU)
      m_ulMaxMCU = max; 
    if (LONG(m_ppBitmap[i]->ibm_ulHeight) - 1 < region.ra_MaxY)
      region.ra_MaxY = m_ppBitmap[i]->ibm_ulHeight - 1;
  }
}
///

/// LineBitmapRequester::EncodeRegion
// Encode a region without downsampling but color transformation
void LineBitmapRequester::EncodeRegion(const RectAngle<LONG> &region)
{
  class ColorTrafo *ctrafo = ColorTrafoOf(true);
  int i;
  
  if (m_bSubsampling) { 
    RectAngle<LONG> r;
    ULONG minx   = region.ra_MinX >> 3;
    ULONG maxx   = region.ra_MaxX >> 3;
    ULONG miny   = region.ra_MinY >> 3;
    ULONG maxy   = region.ra_MaxY >> 3;
    ULONG x,y;

    // First part: Collect the data from
    // the user and push it into the color transformer buffer.
    // For that first build the downsampler.
    for(i = 0;i < m_ucCount;i++) {
      if (m_ppDownsampler[i]) {
        m_ppDownsampler[i]->SetBufferedRegion(region);
      }
    }
    
    for(y = miny,r.ra_MinY = region.ra_MinY;y <= maxy;y++,r.ra_MinY = r.ra_MaxY + 1) {
      r.ra_MaxY = (r.ra_MinY & -8) + 7;
      if (r.ra_MaxY > region.ra_MaxY)
        r.ra_MaxY = region.ra_MaxY;
        
      for(x = minx,r.ra_MinX = region.ra_MinX;x <= maxx;x++,r.ra_MinX = r.ra_MaxX + 1) {
        r.ra_MaxX = (r.ra_MinX & -8) + 7;
        if (r.ra_MaxX > region.ra_MaxX)
          r.ra_MaxX = region.ra_MaxX;
        
        for(i = 0;i < m_ucCount;i++) {
          // Collect the source data.
          ExtractBitmap(m_ppTempIBM[i],r,i);
        }
        
        //
        // Run the color transformer.
        ctrafo->RGB2YCbCr(r,m_ppTempIBM,m_ppCTemp);
        
        // Now push the transformed data into either the downsampler, 
        // or the forward DCT block row.
        for(i = 0;i < m_ucCount;i++) {
          if (m_ppDownsampler[i]) {
            // Just collect the data in the downsampler for the time
            // being. Will be taken care of as soon as it is complete.
            m_ppDownsampler[i]->DefineRegion(x,y,m_ppCTemp[i]);
          } else { 
            DefineRegion(x,Start8Lines(i),m_ppCTemp[i],i);
          }
        }
      }
      //
      // Advance the quantized rows for the non-subsampled components,
      // downsampled components will be advanced later.
      for(i = 0;i < m_ucCount;i++) {
        m_pulReadyLines[i]    += 8; // somehwere in the buffer.
        if (m_ppDownsampler[i] == NULL) {
          Next8Lines(i);
        } else {
          LONG bx,by;
          RectAngle<LONG> blocks;
          // Collect the downsampled blocks and push that into the DCT.
          m_ppDownsampler[i]->GetCollectedBlocks(blocks);
          for(by = blocks.ra_MinY;by <= blocks.ra_MaxY;by++) {
            struct Line *row = Start8Lines(i);
            for(bx = blocks.ra_MinX;bx <= blocks.ra_MaxX;bx++) {
              LONG src[64]; // temporary buffer, the DCT requires a 8x8 block
              m_ppDownsampler[i]->DownsampleRegion(bx,by,src);
              DefineRegion(bx,row,src,i);
            }
            m_ppDownsampler[i]->RemoveBlocks(by);
            Next8Lines(i);
          }
        }
      }
    }
  } else { // No downsampling required, residual coding possible.
    RectAngle<LONG> r;
    ULONG minx   = region.ra_MinX >> 3;
    ULONG maxx   = region.ra_MaxX >> 3;
    ULONG miny   = region.ra_MinY >> 3;
    ULONG maxy   = region.ra_MaxY >> 3;
    ULONG x,y;

    for(y = miny,r.ra_MinY = region.ra_MinY;y <= maxy;y++,r.ra_MinY = r.ra_MaxY + 1) {
      r.ra_MaxY = (r.ra_MinY & -8) + 7;
      if (r.ra_MaxY > region.ra_MaxY)
        r.ra_MaxY = region.ra_MaxY;
        
      for(x = minx,r.ra_MinX = region.ra_MinX;x <= maxx;x++,r.ra_MinX = r.ra_MaxX + 1) {
        r.ra_MaxX = (r.ra_MinX & -8) + 7;
        if (r.ra_MaxX > region.ra_MaxX)
          r.ra_MaxX = region.ra_MaxX;

        for(i = 0;i < m_ucCount;i++) {      
          ExtractBitmap(m_ppTempIBM[i],r,i);
        }
        
        ctrafo->RGB2YCbCr(r,m_ppTempIBM,m_ppCTemp);

        for(i = 0;i < m_ucCount;i++) {
          DefineRegion(x,Start8Lines(i),m_ppCTemp[i],i);
        }
      }
      for(i = 0;i < m_ucCount;i++) {
        Next8Lines(i);
        m_pulReadyLines[i]   += 8;
      }
    }
  }
}
///
 
/// LineBitmapRequester::RequestUserDataForDecoding
// Pull data buffers from the user data bitmap hook
void LineBitmapRequester::RequestUserDataForDecoding(class BitMapHook *bmh,RectAngle<LONG> &region,
                                                     const struct RectangleRequest *rr,bool alpha)
{ 
  int i;

  m_ulMaxMCU = MAX_ULONG;
  
  for(i = rr->rr_usFirstComponent;i <= rr->rr_usLastComponent;i++) {
    RequestUserData(bmh,region,i,alpha);
    ULONG max = (BitmapOf(i).ibm_ulHeight >> 3) - 1;
    if (max < m_ulMaxMCU)
      m_ulMaxMCU = max;
  }
}
///

/// LineBitmapRequester::ReconstructRegion
// Reconstruct a block, or part of a block
void LineBitmapRequester::ReconstructRegion(const RectAngle<LONG> &region,const struct RectangleRequest *rr)
{
  class ColorTrafo *ctrafo = ColorTrafoOf(false);
  UBYTE i;

  if (m_bSubsampling) { 
    for(i = rr->rr_usFirstComponent;i <= rr->rr_usLastComponent;i++) {
      class Component *comp = m_pFrame->ComponentOf(i);
      UBYTE subx            = comp->SubXOf();
      UBYTE suby            = comp->SubYOf();
      class UpsamplerBase *up;  // upsampler
      LONG bx,by;
      RectAngle<LONG> blocks;
      //
      // Compute the region of blocks
      assert(subx > 0 && suby > 0);
      if ((up = m_ppUpsampler[i])) {
        LONG bwidth           = ((m_ulPixelWidth  + subx - 1) / subx + 7) >> 3;
        LONG bheight          = ((m_ulPixelHeight + suby - 1) / suby + 7) >> 3;
        LONG rx               = (subx > 1)?(1):(0);
        LONG ry               = (suby > 1)?(1):(0);
        // The +/-1 include additional lines required for subsampling expansion
        blocks.ra_MinX        = ((region.ra_MinX / subx - rx) >> 3);
        blocks.ra_MaxX        = ((region.ra_MaxX / subx + rx) >> 3);
        blocks.ra_MinY        = ((region.ra_MinY / suby - ry) >> 3);
        blocks.ra_MaxY        = ((region.ra_MaxY / suby + ry) >> 3);
        // Clip.
        if (blocks.ra_MinX < 0)        blocks.ra_MinX = 0;
        if (blocks.ra_MaxX >= bwidth)  blocks.ra_MaxX = bwidth - 1;
        if (blocks.ra_MinY < 0)        blocks.ra_MinY = 0;
        if (blocks.ra_MaxY >= bheight) blocks.ra_MaxY = bheight - 1;
        up->SetBufferedRegion(blocks); // also removes the rectangle of blocks already buffered.
        //
        for(by = blocks.ra_MinY;by <= blocks.ra_MaxY;by++) {
          for(bx = blocks.ra_MinX;bx <= blocks.ra_MaxX;bx++) {
            LONG dst[64];
            FetchRegion(bx,*m_pppImage[i],dst);
            up->DefineRegion(bx,by,dst);
          }
          Next8Lines(i);
        }
      }
    }
    // Now push blocks into the color transformer from the upsampler.
    {
      RectAngle<LONG> r;
      ULONG minx   = region.ra_MinX >> 3;
      ULONG maxx   = region.ra_MaxX >> 3;
      ULONG miny   = region.ra_MinY >> 3;
      ULONG maxy   = region.ra_MaxY >> 3;
      ULONG x,y;
      
      if (maxy > m_ulMaxMCU)
        maxy = m_ulMaxMCU;

      for(y = miny,r.ra_MinY = region.ra_MinY;y <= maxy;y++,r.ra_MinY = r.ra_MaxY + 1) {
        r.ra_MaxY = (r.ra_MinY & -8) + 7;
        if (r.ra_MaxY > region.ra_MaxY)
          r.ra_MaxY = region.ra_MaxY;
        
        for(x = minx,r.ra_MinX = region.ra_MinX;x <= maxx;x++,r.ra_MinX = r.ra_MaxX + 1) {
          r.ra_MaxX = (r.ra_MinX & -8) + 7;
          if (r.ra_MaxX > region.ra_MaxX)
            r.ra_MaxX = region.ra_MaxX;
          
          for(i = 0;i < m_ucCount;i++) {
            if (i >= rr->rr_usFirstComponent && i <= rr->rr_usLastComponent) {
              ExtractBitmap(m_ppTempIBM[i],r,i);
              if (m_ppUpsampler[i]) {
                // Upsampled case, take from the upsampler, transform
                // into the color buffer.
                m_ppUpsampler[i]->UpsampleRegion(r,m_ppCTemp[i]);
              } else {
                FetchRegion(x,*m_pppImage[i],m_ppCTemp[i]);
              }
            } else {
              // Not requested, zero the buffer.
              memset(m_ppCTemp[i],0,sizeof(LONG) * 64);
            }
          }
          ctrafo->YCbCr2RGB(r,m_ppTempIBM,m_ppCTemp,NULL);
        }
        //
        // Advance the quantized rows for the non-subsampled components,
        // upsampled components have been advanced above.
        for(i = 0;i < m_ucCount;i++) {
          if (m_ppUpsampler[i] == NULL)
            Next8Lines(i);
        }
      }
    }
  } else { // direct case, no upsampling required, residual coding possible.
    RectAngle<LONG> r;
    class ColorTrafo *ctrafo = ColorTrafoOf(false);
    ULONG minx   = region.ra_MinX >> 3;
    ULONG maxx   = region.ra_MaxX >> 3;
    ULONG miny   = region.ra_MinY >> 3;
    ULONG maxy   = region.ra_MaxY >> 3;
    ULONG x,y;
      
    if (maxy > m_ulMaxMCU)
      maxy = m_ulMaxMCU;

    for(y = miny,r.ra_MinY = region.ra_MinY;y <= maxy;y++,r.ra_MinY = r.ra_MaxY + 1) {
      r.ra_MaxY = (r.ra_MinY & -8) + 7;
      if (r.ra_MaxY > region.ra_MaxY)
        r.ra_MaxY = region.ra_MaxY;
        
      for(x = minx,r.ra_MinX = region.ra_MinX;x <= maxx;x++,r.ra_MinX = r.ra_MaxX + 1) {
        r.ra_MaxX = (r.ra_MinX & -8) + 7;
        if (r.ra_MaxX > region.ra_MaxX)
          r.ra_MaxX = region.ra_MaxX;

        for(i = 0;i < m_ucCount;i++) {      
          LONG *dst = m_ppCTemp[i];
          if (i >= rr->rr_usFirstComponent && i <= rr->rr_usLastComponent) {
            ExtractBitmap(m_ppTempIBM[i],r,i);
            FetchRegion(x,*m_pppImage[i],dst);
          } else {
            memset(dst,0,sizeof(LONG) * 64);
          }
        }
        //
        // Perform the color transformation now.
        ctrafo->YCbCr2RGB(r,m_ppTempIBM,m_ppCTemp,NULL);
      } // of loop over x
      //
      // Advance the rows.
      for(i = 0;i < m_ucCount;i++) {
        Next8Lines(i);
      }
    }
  }
}
///

/// LineBitmapRequester::isNextMCULineReady
// Return true if the next MCU line is buffered and can be pushed
// to the encoder.
bool LineBitmapRequester::isNextMCULineReady(void) const
{
  int i;

  for(i = 0;i < m_ucCount;i++) {
    if (m_pulReadyLines[i] < m_ulPixelHeight) { // There is still data to encode
      class Component *comp = m_pFrame->ComponentOf(i);
      ULONG codedlines      = m_pulCurrentY[i] * comp->SubYOf();
      // codedlines + comp->SubYOf() << 3 * comp->MCUHeightOf() is the number of 
      // lines that must be buffered to encode the next MCU
      if (m_pulReadyLines[i] < codedlines + (comp->SubYOf() << 3) * comp->MCUHeightOf())
        return false;
    }
  }

  return true;
}
///

/// LineBitmapRequester::isImageComplete
// Return an indicator whether all of the image has been loaded into
// the image buffer.
bool LineBitmapRequester::isImageComplete(void) const
{
  for(UBYTE i = 0;i < m_ucCount;i++) {
    if (m_pulReadyLines[i] < m_ulPixelHeight)
      return false;
  }
  return true;
}
///
