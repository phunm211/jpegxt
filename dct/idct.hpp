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
** Integer DCT operation plus scaled quantization.
**
** $Id: idct.hpp,v 1.20 2016/10/28 13:58:54 thor Exp $
**
*/

#ifndef DCT_IDCT_HPP
#define DCT_IDCT_HPP

/// Includes
#include "tools/environment.hpp"
#include "dct/dct.hpp"
#include "tools/traits.hpp"
///

/// Forwards
class Quantization;
struct ImageBitMap;
///

/// class IDCT
// This class implements the integer based DCT. The template parameter is the number of
// preshifted bits coming in from the color transformer.
template<int preshift,typename T,bool deadzone,bool optimize>
class IDCT : public DCT {
  //
  // Bit assignment
  enum {
    FIX_BITS          = 9,  // fractional bits for fixpoint in the calculation
    // 9 is the maximum for 16bpp input or 12 + 4(color bits)
    // This is because 12+4+3+3+9 = 31
    INTERMEDIATE_BITS = 0,  // fractional bits for representing the intermediate result
    // (none required)
    QUANTIZER_BITS    = 30  // bits for representing the quantizer
  };
  //
  // The (inverse) quantization tables, i.e. multipliers.
  LONG  m_plInvQuant[64];
  //
  // The quantizer tables.
  LONG  m_plQuant[64];
  //
  // Local buffer for the scaled unquantized data. This allows an R/D optimization.
  LONG m_lTransform[64];
  //
  // Quantize a floating point number with a multiplier, round correctly.
  // Must remove FIX_BITS + INTER_BITS + 3
  inline LONG Quantize(LONG n,LONG qnt,int band)
  {
    if (optimize) {
      // If the optimization is on, also store the raw unqantized data.
      // It just has to be scaled correctly.
      // The preshift is not removed since it is part of the quantization
      // settings to remove it.
      m_lTransform[band] = n >> (FIX_BITS + INTERMEDIATE_BITS + 3);
    }
    
    if (deadzone == false || band == 0) {
      return (n * QUAD(qnt) + (ULONG(-n) >> TypeTrait<LONG>::SignBit) + 
              (QUAD(1) << (FIX_BITS + INTERMEDIATE_BITS + QUANTIZER_BITS + preshift + 3 - 1)))
        >> (FIX_BITS + INTERMEDIATE_BITS + QUANTIZER_BITS + preshift + 3);
    } else {
      QUAD m = n >> TypeTrait<LONG>::SignBit;
      QUAD o = m << (FIX_BITS + INTERMEDIATE_BITS + QUANTIZER_BITS + preshift + 3 - 2);
      return (n * QUAD(qnt) + ((~o) & m) +
              (QUAD(3) << (FIX_BITS + INTERMEDIATE_BITS + QUANTIZER_BITS + preshift + 3 - 3)))
        >> (FIX_BITS + INTERMEDIATE_BITS + QUANTIZER_BITS + preshift + 3);
    }
  }
  //
public:
  IDCT(class Environ *env);
  //
  ~IDCT(void);
  //
  // Use the quantization table defined here, scale them to the needs of the DCT and scale them
  // to the right size.
  virtual void DefineQuant(class QuantizationTable *table);
  //
  // Run the DCT on a 8x8 block on the input data, giving the output table.
  virtual void TransformBlock(const LONG *source,LONG *target,LONG dcoffset);
  //
  // Run the inverse DCT on an 8x8 block reconstructing the data.
  virtual void InverseTransformBlock(LONG *target,const LONG *source,LONG dcoffset);
  //
  // Estimate a critical slope (lambda) from the unquantized data.
  // Or to be precise, estimate lambda/delta^2, the constant in front of
  // delta^2.
  virtual DOUBLE EstimateCriticalSlope(void);
  //
  // Return (in case optimization is enabled) a pointer to the unquantized
  // but DCT transformed data. The data is potentially preshifted.
  virtual const LONG *TransformedBlockOf(void) const
  {
    assert(optimize);
    return m_lTransform;
  } 
  //
  // Return (in case optimization is enabled) a pointer to the effective
  // quantization step sizes.
  virtual const LONG *BucketSizes(void) const
  {
    return m_plQuant;
  } 
  //
  // The prescaling of the DCT. This is the number of bits the input data
  // is upshifted compared to the regular input.
  virtual int PreshiftOf(void) const
  {
    return preshift;
  }
};
///

///
#endif
