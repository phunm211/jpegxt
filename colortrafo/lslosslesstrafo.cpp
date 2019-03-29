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
** This file provides the transformation from RGB to YCbCr
**
** $Id: lslosslesstrafo.cpp,v 1.21 2014/09/30 08:33:16 thor Exp $
**
*/

/// Includes
#include "colortrafo/lslosslesstrafo.hpp"
#include "tools/traits.hpp"
#include "std/string.hpp"
#include "marker/lscolortrafo.hpp"
#include "marker/frame.hpp"
#include "marker/component.hpp"
///

/// LSLosslessTrafo::LSLosslessTrafo
template<typename external,int count>
LSLosslessTrafo<external,count>::LSLosslessTrafo(class Environ *env,LONG dcshift,LONG max,
                                                 LONG rdcshift,LONG rmax,LONG outshift,LONG outmax)
  : ColorTrafo(env,dcshift,max,rdcshift,rmax,outshift,outmax)
{
}
///

/// LSLosslessTrafo::~LSLosslessTrafo
template<typename external,int count>
LSLosslessTrafo<external,count>::~LSLosslessTrafo(void)
{
}
///

/// LSLosslessTrafo::InstallMarker
// Install the transformation from an LSColorTrafo marker, one of
// the JPEG LS extensions markers.
template<typename external,int count>
void LSLosslessTrafo<external,count>::InstallMarker(const class LSColorTrafo *marker,
                                                    const class Frame *frame)
{
  int i,j;
  assert(count == marker->DepthOf()); // Should be done correctly on construction.

  m_lMaxTrans  = marker->MaxTransOf();
  m_lNear      = marker->NearOf();
  m_lModulo    = m_lMaxTrans + 1;
  m_lOffset    = (m_lMaxTrans + 1) >> 1;

  memset(m_ucInverse,MAX_UBYTE,sizeof(m_ucInverse));

  for(i = 0;i < count;i++) {
    m_ucRightShift[i] = marker->RightShiftOf()[i];
    m_bCentered[i]    = marker->CenteredFlagsOf()[i];
    m_ucInternal[i]   = frame->FindComponent(marker->LabelsOf()[i])->IndexOf();
    if (m_ucInternal[i] >= count)
      JPG_THROW(OVERFLOW_PARAMETER,"LSLosslessTrafo::InstallMarker",
                "cannot handle more than four components in the JPEG LS part 2 color transformation");
    if (m_ucInverse[m_ucInternal[i]] != MAX_UBYTE)
      JPG_THROW(INVALID_PARAMETER,"LSLosslessTrafo::InstallMarker",
                "invalid JPEG LS color transformation - a component is used more than once");
    m_ucInverse[m_ucInternal[i]] = i;
    for(j = 0;j < count-1;j++) {
      m_usMatrix[i][j] = marker->MatrixOf()[j + i * (count - 1)];
    }
  }
}
///

/// LSLosslessTrafo::RGB2YCbCr
// Transform a block from RGB to YCbCr. Input are the three image bitmaps
// already clipped to the rectangle to transform, the coordinate rectangle to use
// and the level shift.
template<typename external,int count>
void LSLosslessTrafo<external,count>::RGB2YCbCr(const RectAngle<LONG> &r,const struct ImageBitMap *const *source,
                                                Buffer target)
{
  LONG x,y;
  LONG xmin   = r.ra_MinX & 7;
  LONG ymin   = r.ra_MinY & 7;
  LONG xmax   = r.ra_MaxX & 7;
  LONG ymax   = r.ra_MaxY & 7;

  assert(m_lMax == m_lOutMax);
  
  if (xmax < 7 || ymax < 7 || xmin > 0 || ymin > 0) {
    switch(count) {
    case 4:
      memset(target[3],0,sizeof(Block));
    case 3:
      memset(target[2],0,sizeof(Block));
      memset(target[1],0,sizeof(Block));
      memset(target[0],0,sizeof(Block));
    }
  }

  for(x = 1;x < count;x++) {
    if (source[0]->ibm_ucPixelType != source[x]->ibm_ucPixelType) {
      JPG_THROW(INVALID_PARAMETER,"LSLosslessTrafo::RGB2YCbCr",
                "pixel types of all three components in a RGB to YCbCr conversion must be identical");
    }
  }

  {
    const external *rptr,*gptr,*bptr,*aptr;
    switch(count) {
    case 4:
      aptr = (const external *)(source[3]->ibm_pData);
    case 3:
      rptr = (const external *)(source[0]->ibm_pData);
      gptr = (const external *)(source[1]->ibm_pData);
      bptr = (const external *)(source[2]->ibm_pData);
    }
    for(y = ymin;y <= ymax;y++) {
      LONG in[4],dst[4],*inp[4];
      const external *r,*g,*b,*a;
      switch(count) {
      case 4:
        inp[3]  = target[3] + xmin + (y << 3);
        a       = aptr;
      case 3:
        inp[0]  = target[0] + xmin + (y << 3);
        inp[1]  = target[1] + xmin + (y << 3);
        inp[2]  = target[2] + xmin + (y << 3);
        r       = rptr;
        g       = gptr;
        b       = bptr;
      }
      for(x = xmin;x <= xmax;x++) { 
        // Step one: Pick up the sources.
        switch(count) {
        case 4:
          dst[m_ucInternal[3]] = *a;
          assert(dst[m_ucInternal[3]] <= m_lMax);
          a  = (const external *)((const UBYTE *)(a) + source[3]->ibm_cBytesPerPixel);
        case 3:
          dst[m_ucInternal[0]] = *r;
          assert(dst[m_ucInternal[0]] <= m_lMax);
          r  = (const external *)((const UBYTE *)(r) + source[0]->ibm_cBytesPerPixel);
          //
          dst[m_ucInternal[1]] = *g;
          assert(dst[m_ucInternal[1]] <= m_lMax);
          g  = (const external *)((const UBYTE *)(g) + source[1]->ibm_cBytesPerPixel);
          //
          dst[m_ucInternal[2]] = *b;
          assert(dst[m_ucInternal[2]] <= m_lMax);
          b  = (const external *)((const UBYTE *)(b) + source[2]->ibm_cBytesPerPixel);
        }
        // Step one-and-a-half: Unfortunately, the way how the decoder is specified allows
        // even when maxtrans is *larger* than the range of the input samples, an underrun
        // below zero. The transformation should have rather checked whether the sample
        // value is below -near instead of just checking below 0. Unfortunately, it does
        // not, so we have to take care about that no overrun or underrun can happen at the
        // decoder. To avoid this, clip the input range of the samples so that the l^infty
        // error plus the value cannot underrun at the decoder. Bummer!
        if (m_lNear > 0) {
          switch(count) {
          case 4:
            if (dst[3] < m_lNear)               dst[3] = m_lNear;
            if (dst[3] > m_lMaxTrans - m_lNear) dst[3] = m_lMaxTrans - m_lNear;
          case 3:
            if (dst[2] < m_lNear)               dst[2] = m_lNear;
            if (dst[2] > m_lMaxTrans - m_lNear) dst[2] = m_lMaxTrans - m_lNear;
            if (dst[1] < m_lNear)               dst[1] = m_lNear;
            if (dst[1] > m_lMaxTrans - m_lNear) dst[1] = m_lMaxTrans - m_lNear;
            if (dst[0] < m_lNear)               dst[0] = m_lNear;
            if (dst[0] > m_lMaxTrans - m_lNear) dst[0] = m_lMaxTrans - m_lNear;
          }
        }
        // Step two: Transform with the matrix using the lifting steps of the
        // backwards transformation.
        switch(count) {
        case 4:
          in[3]   = m_usMatrix[3][0] * dst[0] + m_usMatrix[3][1] * dst[1] + m_usMatrix[3][2] * dst[2];
          in[3] >>= m_ucRightShift[3];
          if (m_bCentered[3]) {
            in[3] = dst[3] + in[3];
            if (in[3] < 0)          in[3] += m_lModulo;
            if (in[3] >= m_lModulo) in[3] -= m_lModulo;
          } else {
            in[3] = dst[3] - in[3];
            if (in[3] < -m_lOffset) in[3] += m_lModulo;
            if (in[3] >= m_lOffset) in[3] -= m_lModulo;
          }
          //
          in[2]   = m_usMatrix[2][0] * dst[0] + m_usMatrix[2][1] * dst[1] + m_usMatrix[2][2] *  in[3];
          in[2] >>= m_ucRightShift[2];
          if (m_bCentered[2]) {
            in[2] = dst[2] + in[2];
            if (in[2] < 0)          in[2] += m_lModulo;
            if (in[2] >= m_lModulo) in[2] -= m_lModulo;
          } else {
            in[2] = dst[2] - in[2];
            if (in[2] < -m_lOffset) in[2] += m_lModulo;
            if (in[2] >= m_lOffset) in[2] -= m_lModulo;
          }
          //
          in[1]   = m_usMatrix[1][0] * dst[0] + m_usMatrix[1][1] *  in[2] + m_usMatrix[1][2] *  in[3];
          in[1] >>= m_ucRightShift[1];
          if (m_bCentered[1]) {
            in[1] = dst[1] + in[1];
            if (in[1] < 0)          in[1] += m_lModulo;
            if (in[1] >= m_lModulo) in[1] -= m_lModulo;
          } else {
            in[1] = dst[1] - in[1];
            if (in[1] < -m_lOffset) in[1] += m_lModulo;
            if (in[1] >= m_lOffset) in[1] -= m_lModulo;
          }
          //
          in[0]   = m_usMatrix[0][0] *  in[1] + m_usMatrix[0][1] *  in[2] + m_usMatrix[0][2] *  in[3];
          in[0] >>= m_ucRightShift[0];
          if (m_bCentered[0]) {
            in[0] = dst[0] + in[0];
            if (in[0] < 0)          in[0] += m_lModulo;
            if (in[0] >= m_lModulo) in[0] -= m_lModulo;
          } else {
            in[0] = dst[0] - in[0];
            if (in[0] < -m_lOffset) in[0] += m_lModulo;
            if (in[0] >= m_lOffset) in[0] -= m_lModulo;
          }
          break;
        case 3:
          in[2]   = m_usMatrix[2][0] * dst[0] + m_usMatrix[2][1] * dst[1];
          in[2] >>= m_ucRightShift[2];
          if (m_bCentered[2]) {
            in[2] = dst[2] + in[2];
            if (in[2] < 0)          in[2] += m_lModulo;
            if (in[2] >= m_lModulo) in[2] -= m_lModulo;
          } else {
            in[2] = dst[2] - in[2];
            if (in[2] < -m_lOffset) in[2] += m_lModulo;
            if (in[2] >= m_lOffset) in[2] -= m_lModulo;
          }
          //
          in[1]   = m_usMatrix[1][0] * dst[0] + m_usMatrix[1][1] *  in[2];
          in[1] >>= m_ucRightShift[1];
          if (m_bCentered[1]) {
            in[1] = dst[1] + in[1];
            if (in[1] < 0)          in[1] += m_lModulo;
            if (in[1] >= m_lModulo) in[1] -= m_lModulo;
          } else {
            in[1] = dst[1] - in[1];
            if (in[1] < -m_lOffset) in[1] += m_lModulo;
            if (in[1] >= m_lOffset) in[1] -= m_lModulo;
          }
          //
          in[0]   = m_usMatrix[0][0] *  in[1] + m_usMatrix[0][1] *  in[2];
          in[0] >>= m_ucRightShift[0];
          if (m_bCentered[0]) {
            in[0] = dst[0] + in[0];
            if (in[0] < 0)          in[0] += m_lModulo;
            if (in[0] >= m_lModulo) in[0] -= m_lModulo;
          } else {
            in[0] = dst[0] - in[0];
            if (in[0] < -m_lOffset) in[0] += m_lModulo;
            if (in[0] >= m_lOffset) in[0] -= m_lModulo;
          }
          break;
        }
        //
        // Center and clip to the output range.
        switch(count) {
        case 4:
          if (!m_bCentered[3]) in[3] += m_lOffset;
          if (in[3] < 0)       in[3]  = 0;
          if (in[3] > m_lMax)  in[3]  = m_lMax;
        case 3:
          if (!m_bCentered[2]) in[2] += m_lOffset;
          if (in[2] < 0)       in[2]  = 0;
          if (in[2] > m_lMax)  in[2]  = m_lMax;
          if (!m_bCentered[1]) in[1] += m_lOffset;
          if (in[1] < 0)       in[1]  = 0;
          if (in[1] > m_lMax)  in[1]  = m_lMax;
          if (!m_bCentered[0]) in[0] += m_lOffset;
          if (in[0] < 0)       in[0]  = 0;
          if (in[0] > m_lMax)  in[0]  = m_lMax;
        }
        //
        // Write to the output, potentially center.
        switch(count) {
        case 4:
          *inp[m_ucInverse[3]]++ = in[3];
        case 3:
          *inp[m_ucInverse[0]]++ = in[0];
          *inp[m_ucInverse[1]]++ = in[1];
          *inp[m_ucInverse[2]]++ = in[2];
        }
      }
      switch(count) {
      case 4:
        aptr  = (const external *)((const UBYTE *)(aptr) + source[3]->ibm_lBytesPerRow);
      case 3:
        rptr  = (const external *)((const UBYTE *)(rptr) + source[0]->ibm_lBytesPerRow);
        gptr  = (const external *)((const UBYTE *)(gptr) + source[1]->ibm_lBytesPerRow);
        bptr  = (const external *)((const UBYTE *)(bptr) + source[2]->ibm_lBytesPerRow);
      }
    }
  }
}
///

/// LSLosslessTrafo::RGB2Residual
// Compute the residual from the original image and the decoded LDR image, place result in
// the output buffer. This depends rather on the coding model.
template<typename external,int count>
void LSLosslessTrafo<external,count>::RGB2Residual(const RectAngle<LONG> &,const struct ImageBitMap *const *,
                                                   Buffer,Buffer residual)
{
  UBYTE c;
  int i;

  assert(!"JPEG LS coding does not generate residuals");
  
  // Just reset the residual as there is none.
  for(c = 0;c < count;c++) {
    LONG *res = residual[c];
    for(i = 0;i < 64;i++)
      res[i] = m_lRDCShift;
  }
}
///

/// LSLosslessTrafo::YCbCr2RGB
// Inverse transform a block from YCbCr to RGB, incuding a clipping operation and a dc level
// shift.
template<typename external,int count>
void LSLosslessTrafo<external,count>::YCbCr2RGB(const RectAngle<LONG> &r,
                                                const struct ImageBitMap *const *dest,
                                                Buffer source,Buffer)
{ 
  LONG x,y;
  LONG xmin   = r.ra_MinX & 7;
  LONG ymin   = r.ra_MinY & 7;
  LONG xmax   = r.ra_MaxX & 7;
  LONG ymax   = r.ra_MaxY & 7;
  
  assert(m_lMax == m_lOutMax);

  if (m_lMax > TypeTrait<external>::Max) {
    JPG_THROW(OVERFLOW_PARAMETER,"LSLosslessTrafo::YCbCr2RGB",
              "RGB maximum intensity for pixel type does not fit into the type");
  }
  
  for(x = 0;x < count;x++) {
    if (dest[0]->ibm_ucPixelType != dest[x]->ibm_ucPixelType) {
      JPG_THROW(INVALID_PARAMETER,"LSLosslessTrafo::YCbCr2RGB",
                "pixel types of all components in a YCbCr to RGB conversion must be identical");
    }
  }

  {
    external *rptr,*gptr,*bptr,*aptr;
    switch(count) {
    case 4:
      aptr = (external *)(dest[3]->ibm_pData);
    case 3:
      rptr = (external *)(dest[0]->ibm_pData);
      gptr = (external *)(dest[1]->ibm_pData);
      bptr = (external *)(dest[2]->ibm_pData);
    }
    for(y = ymin;y <= ymax;y++) {
      LONG *srcp[4],src[4],out[4];
      external *r,*g,*b,*a;
      switch(count) {
      case 4:
        srcp[3] = source[3] + xmin + (y << 3);
        a       = aptr;
      case 3:
        srcp[0] = source[0] + xmin + (y << 3);  
        srcp[1] = source[1] + xmin + (y << 3);
        srcp[2] = source[2] + xmin + (y << 3);
        r       = rptr;
        g       = gptr;
        b       = bptr;
      }
      for(x = xmin;x <= xmax;x++) {
        // Input clipping and offset shifting. 
        // Clipping is not required for JPEG LS,
        // but for consistency, I include it here.
        switch(count) {
        case 4:
          src[3] = *srcp[m_ucInternal[3]];
          if (!m_bCentered[3]) src[3] -= m_lOffset;
        case 3:
          src[2] = *srcp[m_ucInternal[2]];
          if (!m_bCentered[2]) src[2] -= m_lOffset;

          src[1] = *srcp[m_ucInternal[1]];
          if (!m_bCentered[1]) src[1] -= m_lOffset;

          src[0] = *srcp[m_ucInternal[0]];
          if (!m_bCentered[0]) src[0] -= m_lOffset;
        }
        // Output mapping by the matrix transformation.
        switch(count) {
        case 4:
          out[0]   = m_usMatrix[0][0] * src[1] + m_usMatrix[0][1] * src[2] + m_usMatrix[0][2] * src[3];
          out[0] >>= m_ucRightShift[0];
          out[0]   = (m_bCentered[0])?(src[0] - out[0]):(src[0] + out[0]);
          if (out[0] < 0)          out[0] += m_lModulo;
          if (out[0] >= m_lModulo) out[0] -= m_lModulo;
          //
          out[1]   = m_usMatrix[1][0] * out[0] + m_usMatrix[1][1] * src[2] + m_usMatrix[1][2] * src[3];
          out[1] >>= m_ucRightShift[1];
          out[1]   = (m_bCentered[1])?(src[1] - out[1]):(src[1] + out[1]);
          if (out[1] < 0)          out[1] += m_lModulo;
          if (out[1] >= m_lModulo) out[1] -= m_lModulo;
          //
          out[2]   = m_usMatrix[2][0] * out[0] + m_usMatrix[2][1] * out[1] + m_usMatrix[2][2] * src[3];
          out[2] >>= m_ucRightShift[2];
          out[2]   = (m_bCentered[2])?(src[2] - out[2]):(src[2] + out[2]);
          if (out[2] < 0)          out[2] += m_lModulo;
          if (out[2] >= m_lModulo) out[2] -= m_lModulo;
          //
          out[3]   = m_usMatrix[3][0] * out[0] + m_usMatrix[3][1] * out[1] + m_usMatrix[3][2] * out[2];
          out[3] >>= m_ucRightShift[3];
          out[3]   = (m_bCentered[3])?(src[3] - out[3]):(src[3] + out[3]);
          if (out[3] < 0)          out[3] += m_lModulo;
          if (out[3] >= m_lModulo) out[3] -= m_lModulo;
          break;
        case 3:
          out[0]   = m_usMatrix[0][0] * src[1] + m_usMatrix[0][1] * src[2];
          out[0] >>= m_ucRightShift[0];
          out[0]   = (m_bCentered[0])?(src[0] - out[0]):(src[0] + out[0]);
          if (out[0] < 0)          out[0] += m_lModulo;
          if (out[0] >= m_lModulo) out[0] -= m_lModulo;
          //
          out[1]   = m_usMatrix[1][0] * out[0] + m_usMatrix[1][1] * src[2];
          out[1] >>= m_ucRightShift[1];
          out[1]   = (m_bCentered[1])?(src[1] - out[1]):(src[1] + out[1]);
          if (out[1] < 0)          out[1] += m_lModulo;
          if (out[1] >= m_lModulo) out[1] -= m_lModulo;
          //
          out[2]   = m_usMatrix[2][0] * out[0] + m_usMatrix[2][1] * out[1];
          out[2] >>= m_ucRightShift[2];
          out[2]   = (m_bCentered[2])?(src[2] - out[2]):(src[2] + out[2]);
          if (out[2] < 0)          out[2] += m_lModulo;
          if (out[2] >= m_lModulo) out[2] -= m_lModulo;
          break;
        }
        //
        // Clip to the output range.
        switch(count) {
        case 4:
          if (out[3] < 0)      out[3]  = 0;
          if (out[3] > m_lMax) out[3]  = m_lMax;
        case 3:
          if (out[2] < 0)      out[2]  = 0;
          if (out[2] > m_lMax) out[2]  = m_lMax;
          if (out[1] < 0)      out[1]  = 0;
          if (out[1] > m_lMax) out[1]  = m_lMax;
          if (out[0] < 0)      out[0]  = 0;
          if (out[0] > m_lMax) out[0]  = m_lMax;
        }
        //
        // Finally map by the LUT as we are now back in RGB space.
        switch(count) {
        case 4:
          *a = out[m_ucInverse[3]];
          a  = (external *)((UBYTE *)(a) + dest[3]->ibm_cBytesPerPixel);
          srcp[3]++;
        case 3:
          *r = out[m_ucInverse[0]];
          r  = (external *)((UBYTE *)(r) + dest[0]->ibm_cBytesPerPixel);
          srcp[0]++;
          *g = out[m_ucInverse[1]];
          g  = (external *)((UBYTE *)(g) + dest[1]->ibm_cBytesPerPixel);
          srcp[1]++;
          *b = out[m_ucInverse[2]];
          b  = (external *)((UBYTE *)(b) + dest[2]->ibm_cBytesPerPixel);
          srcp[2]++;
        }
      }
      switch(count) {
      case 4:
        aptr  = (external *)((UBYTE *)(aptr) + dest[3]->ibm_lBytesPerRow);
      case 3:
        rptr  = (external *)((UBYTE *)(rptr) + dest[0]->ibm_lBytesPerRow);
        gptr  = (external *)((UBYTE *)(gptr) + dest[1]->ibm_lBytesPerRow);
        bptr  = (external *)((UBYTE *)(bptr) + dest[2]->ibm_lBytesPerRow);
      }
    }
  }
}
///

/// Explicit instanciations
// Actually, more instanciations would be possible,
// but I don't really care at this time...
template class LSLosslessTrafo<UBYTE,3>;
template class LSLosslessTrafo<UWORD,3>;
///

