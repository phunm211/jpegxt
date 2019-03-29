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
** This file provides the transformation from RGB to YCbCr for
** the integer coding modes. Floating point coding modes (profiles A and B)
** are handled by the floattrafo.
**
** $Id: colortrafo.hpp,v 1.37 2014/12/30 13:29:35 thor Exp $
**
*/

#ifndef COLORTRAFO_COLORTRAFO_HPP
#define COLORTRAFO_COLORTRAFO_HPP

/// Includes
#include "tools/environment.hpp"
#include "tools/rectangle.hpp"
#include "interface/imagebitmap.hpp"
#include "std/string.hpp"
///

/// Forwards
class ParametricToneMappingBox;
///

/// Class ColorTrafo
// The base class for all decorrelation transformation
class ColorTrafo : public JKeeper {
  //
public:
  // Number of bits preshifted for the color-transformed channels
  enum {
    COLOR_BITS = 4,
    FIX_BITS   = 13
  }; 
  //
  // Flags for the various color transformations.
  enum OutputFlags {
    ClampFlag  = 1,   // Clamp to range (instead of wrap-around)
    Float      = 32,  // set in case the output should be converted to IEEE float
    Extended   = 64,  // should always be set unless no merging spec box is there.
    Residual   = 128  // set if there is a residual
  };
  //
  // A buffer consists of four pointer to 8x8 blocks of data, ordered
  // in R,G,B,alpha or Y,Cb,Cr,Alpha
  typedef LONG *Buffer[4];
  typedef LONG Block[64];
  //
protected:
  //
  // DC-Shift in the legacy domain before applying the LUT.
  LONG  m_lDCShift;
  //
  // Maximum value in the legacy domain before applying the LUT.
  LONG  m_lMax;
  //
  // DC-Shift in the residual domain before applying the LUT
  LONG  m_lRDCShift;
  //
  // Maximum value in the residual domain before applying the LUT.
  LONG  m_lRMax;
  // 
  // DC-shift in the spatial domain.
  LONG m_lOutDCShift;
  //
  // Maximum value in the output (spatial, image) domain.
  LONG  m_lOutMax;
  //
public:
  //
  // Construct a color transformer. Arguments are dcshift and maximum in the legacy domain,
  // dcshift and maximum in the residual domain, both before appyling L and R lut, plus
  // maximum in the image domain.
  ColorTrafo(class Environ *env,LONG dcshift,LONG max,LONG rdcshift,LONG rmax,LONG outshift,LONG outmax)
    : JKeeper(env), m_lDCShift(dcshift), m_lMax(max), 
      m_lRDCShift(rdcshift), m_lRMax(rmax), m_lOutDCShift(outshift), m_lOutMax(outmax)
  { }
  //
  virtual ~ColorTrafo(void)
  { }
  //
  // Transform a block from RGB to YCbCr. Input are the three image bitmaps
  // already clipped to the rectangle to transform, the coordinate rectangle to use
  // and the level shift. This call computes a LDR image from the given input data
  // and moves that into the target buffer. Shift and max values are the clamping
  // of the LDR data.
  virtual void RGB2YCbCr(const RectAngle<LONG> &r,const struct ImageBitMap *const *source,
                         Buffer target) = 0;
  //
  // In case the user already provided a tone-mapped version of the image, this call already
  // takes the LDR version of the image, performs no tone-mapping but only a color
  // decorrelation transformation and injects it as LDR image.
  virtual void LDRRGB2YCbCr(const RectAngle<LONG> &r,const struct ImageBitMap *const *source,
                         Buffer target) = 0;
  //
  // Buffer the original data unaltered. Required for residual coding, for some modes of
  // it at least.
  virtual void RGB2RGB(const RectAngle<LONG> &r,const struct ImageBitMap *const *source,
                       Buffer target) = 0;
  //
  // Compute the residual from the original image and the decoded LDR image, place result in
  // the output buffer. This depends rather on the coding model.
  virtual void RGB2Residual(const RectAngle<LONG> &r,const struct ImageBitMap *const *source,
                            Buffer reconstructed,Buffer residual) = 0;
  //
  // Inverse transform a block from YCbCr to RGB, incuding a clipping operation and a dc level
  // shift.
  virtual void YCbCr2RGB(const RectAngle<LONG> &r,const struct ImageBitMap *const *dest,
                         Buffer source,Buffer residual) = 0;
  //
  // Return the external pixel type of this trafo.
  virtual UBYTE PixelTypeOf(void) const = 0;
};
///

///
#endif
