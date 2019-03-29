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
** Generic DCT transformation plus quantization class.
** All DCT implementations should derive from this.
**
** $Id: dct.hpp,v 1.12 2016/10/28 13:58:53 thor Exp $
**
*/

#ifndef DCT_DCT_HPP
#define DCT_DCT_HPP

/// Includes
#include "tools/environment.hpp"
///

/// Forwards
class Quantization;
struct ImageBitMap;
class QuantizationTable;
class HuffmanCoder;
///

/// class DCT
// This class is the base class for all DCT implementations.
class DCT : public JKeeper {
  //
public: 
  //
  // The scan order within a block.
  static const int ScanOrder[64];
  //
  DCT(class Environ *env)
    : JKeeper(env)
  { }
  //
  virtual ~DCT(void)
  { }
  //
  // Use the quantization table defined here, scale them to the needs of the DCT and scale them
  // to the right size.
  virtual void DefineQuant(class QuantizationTable *table) = 0;
  //
  // Run the DCT on a 8x8 block on the input data, giving the output table.
  virtual void TransformBlock(const LONG *source,LONG *target,LONG dcoffset) = 0;
  //
  // Run the inverse DCT on an 8x8 block reconstructing the data.
  virtual void InverseTransformBlock(LONG *target,const LONG *source,LONG dcoffset) = 0;
  //
  // Estimate a critical slope (lambda) from the unquantized data.
  // Or to be precise, estimate lambda/delta^2, the constant in front of
  // delta^2.
  virtual DOUBLE EstimateCriticalSlope(void) = 0;
  //
  // Return (in case optimization is enabled) a pointer to the unquantized
  // but DCT transformed data. The data is potentially preshifted.
  virtual const LONG *TransformedBlockOf(void) const = 0;
  //
  // Return (in case optimization is enabled) a pointer to the effective
  // quantization step sizes.
  virtual const LONG *BucketSizes(void) const = 0;
  //
  // The prescaling of the DCT. This is the number of bits the input data
  // is upshifted compared to the regular input.
  virtual int PreshiftOf(void) const = 0;
};
///

///
#endif
