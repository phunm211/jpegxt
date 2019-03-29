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
** This file provides the trival transformation from RGB to RGB
**
** $Id: trivialtrafo.cpp,v 1.17 2014/09/30 08:33:16 thor Exp $
**
*/

/// Includes
#include "colortrafo/trivialtrafo.hpp"
#include "tools/traits.hpp"
#include "std/string.hpp"
///

/// TrivialTrafo::TrivialTrafo
template<typename internal,typename external,int count>
TrivialTrafo<internal,external,count>::TrivialTrafo(class Environ *env,LONG dcshift,LONG max)
  : ColorTrafo(env,dcshift,max,dcshift,max,dcshift,max)
{
}
///

/// TrivialTrafo::~TrivialTrafo
template<typename internal,typename external,int count>
TrivialTrafo<internal,external,count>::~TrivialTrafo(void)
{
}
///

/// TrivialTrafo::RGB2YCbCr
// Transform a block from RGB to YCbCr. Input are the three image bitmaps
// already clipped to the rectangle to transform, the coordinate rectangle to use
// and the level shift.
template<typename internal,typename external,int count>
void TrivialTrafo<internal,external,count>::RGB2YCbCr(const RectAngle<LONG> &r,const struct ImageBitMap *const *source,
                                                      Buffer target)
{
  LONG x,y;
  LONG xmin   = r.ra_MinX & 7;
  LONG ymin   = r.ra_MinY & 7;
  LONG xmax   = r.ra_MaxX & 7;
  LONG ymax   = r.ra_MaxY & 7;

  if (xmax < 7 || ymax < 7 || xmin > 0 || ymin > 0) {
    switch(count) {
    case 3:
      memset(target[2],0,sizeof(Block));
      memset(target[1],0,sizeof(Block));
    case 1:
      memset(target[0],0,sizeof(Block));
    }
  }
  
  for(x = 1; x < count;x++) {
    if (source[0]->ibm_ucPixelType != source[x]->ibm_ucPixelType) {
      JPG_THROW(INVALID_PARAMETER,"TrivialTrafo::RGB2YCbCr",
                "pixel types of all three components in a RGB to RGB conversion must be identical");
    }
  }

  {
    const external *rptr,*gptr,*bptr;
    switch(count) {
    case 3:
      bptr = (const external *)(source[2]->ibm_pData);
      gptr = (const external *)(source[1]->ibm_pData);
    case 1:
      rptr = (const external *)(source[0]->ibm_pData);
    }
    for(y = ymin;y <= ymax;y++) {
      internal *ydst,*cbdst,*crdst;
      const external *r,*g,*b;
      switch(count) {
      case 3:
        crdst   = (internal *)target[2] + xmin + (y << 3);
        b       = bptr;
        cbdst   = (internal *)target[1] + xmin + (y << 3);
        g       = gptr;
      case 1:
        ydst    = (internal *)target[0] + xmin + (y << 3);
        r       = rptr;
      }
      for(x = xmin;x <= xmax;x++) { 
        switch(count) {
        case 3:
          *crdst = *b;
          assert(TypeTrait<external>::isFloat || *crdst <= m_lMax);
          crdst++;
          b  = (const external *)((const UBYTE *)(b) + source[2]->ibm_cBytesPerPixel);
          *cbdst = *g;
          assert(TypeTrait<external>::isFloat || *cbdst <= m_lMax);
          cbdst++;
          g  = (const external *)((const UBYTE *)(g) + source[1]->ibm_cBytesPerPixel);
        case 1:
          *ydst  = *r;
          assert(TypeTrait<external>::isFloat || *ydst <= m_lMax);
          ydst++;
          r  = (const external *)((const UBYTE *)(r) + source[0]->ibm_cBytesPerPixel);
        }
      }
      switch(count) {
      case 3:
        bptr  = (const external *)((const UBYTE *)(bptr) + source[2]->ibm_lBytesPerRow);
        gptr  = (const external *)((const UBYTE *)(gptr) + source[1]->ibm_lBytesPerRow);
      case 1:
        rptr  = (const external *)((const UBYTE *)(rptr) + source[0]->ibm_lBytesPerRow);
      }
    }
  }
}
///

/// TrivialTrafo::YCbCr2RGB
// Inverse transform a block from YCbCr to RGB, incuding a clipping operation and a dc level
// shift.
template<typename internal,typename external,int count>
void TrivialTrafo<internal,external,count>::YCbCr2RGB(const RectAngle<LONG> &r,const struct ImageBitMap *const *dest,
                                                      Buffer source,Buffer)
{ 
  LONG x,y;
  LONG xmin   = r.ra_MinX & 7;
  LONG ymin   = r.ra_MinY & 7;
  LONG xmax   = r.ra_MaxX & 7;
  LONG ymax   = r.ra_MaxY & 7;
  
  if (TypeTrait<external>::isFloat == false && m_lMax > TypeTrait<external>::Max) {
    JPG_THROW(OVERFLOW_PARAMETER,"TrivialTrafo::YCbCr2RGB",
              "RGB maximum intensity for pixel type does not fit into the type");
  }
  
  for(x = 1;x < count;x++) {
    if (dest[0]->ibm_ucPixelType != dest[x]->ibm_ucPixelType) {
      JPG_THROW(INVALID_PARAMETER,"TrivialTrafo::YCbCr2RGB",
                "pixel types of all three components in a RGB to RGB conversion must be identical");
    }
  }

  {
    external *rptr,*gptr,*bptr;
    switch(count) {
    case 3:
      bptr = (external *)(dest[2]->ibm_pData);
      gptr = (external *)(dest[1]->ibm_pData);
    case 1:
      rptr = (external *)(dest[0]->ibm_pData);
    }
    for(y = ymin;y <= ymax;y++) {
      internal *ysrc,*cbsrc,*crsrc;
      external *r,*g,*b;
      
      switch(count) {
      case 3:
        crsrc  = (internal *)source[2]   + xmin + (y << 3);
        b      = bptr;
        cbsrc  = (internal *)source[1]   + xmin + (y << 3);
        g      = gptr;
      case 1:
        ysrc   = (internal *)source[0]   + xmin + (y << 3);
        r      = rptr;
      }
      for(x = xmin;x <= xmax;x++) {
        internal rv,gv,bv;

        switch(count) {
        case 3:
          bv = *crsrc++; 
          if (!TypeTrait<external>::isFloat) {
            if (bv < 0)      bv = 0;
            if (bv > m_lMax) bv = m_lMax;
          }
          *b = bv;
          b  = (external *)((UBYTE *)(b) + dest[2]->ibm_cBytesPerPixel);
          gv = *cbsrc++;
          if (!TypeTrait<external>::isFloat) {
            if (gv < 0)      gv = 0;
            if (gv > m_lMax) gv = m_lMax;
          }
          *g = gv;
          g  = (external *)((UBYTE *)(g) + dest[1]->ibm_cBytesPerPixel);
        case 1:
          rv = *ysrc++;
          if (!TypeTrait<external>::isFloat) {
            if (rv < 0)      rv = 0;
            if (rv > m_lMax) rv = m_lMax;
          }
          *r = rv;
          r  = (external *)((UBYTE *)(r) + dest[0]->ibm_cBytesPerPixel);
        }
      }
      switch(count) {
      case 3:
        bptr  = (external *)((UBYTE *)(bptr) + dest[2]->ibm_lBytesPerRow);
        gptr  = (external *)((UBYTE *)(gptr) + dest[1]->ibm_lBytesPerRow);
      case 1:
        rptr  = (external *)((UBYTE *)(rptr) + dest[0]->ibm_lBytesPerRow);
      }
    }
  }
}
///

/// Explicit instanciations
template class TrivialTrafo<LONG,UBYTE,1>;
template class TrivialTrafo<LONG,UWORD,1>;
template class TrivialTrafo<LONG,LONG,1>;
template class TrivialTrafo<LONG,UBYTE,3>;
template class TrivialTrafo<LONG,UWORD,3>;
template class TrivialTrafo<LONG,LONG,3>;
template class TrivialTrafo<FLOAT,FLOAT,1>;
template class TrivialTrafo<FLOAT,FLOAT,3>;
///

