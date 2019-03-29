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
** This class computes, prepares or includes residual data for block
** based processing. It abstracts parts of the residual coding
** process.
**
** $Id: residualblockhelper.hpp,v 1.33 2016/10/28 13:58:53 thor Exp $
**
*/


#ifndef CONTROL_RESIDUALBLOCKHELPER_HPP
#define CONTROL_RESIDUALBLOCKHELPER_HPP

/// Includes
#include "tools/environment.hpp"
#include "tools/rectangle.hpp"
#include "marker/frame.hpp"
///

/// Forwards
struct ImageBitMap;
class QuantizationTable;
class DCT;
///

/// Class ResidualBlockHelper
// This class computes, prepares or includes residual data for block
// based processing. It abstracts parts of the residual coding
// process.
class ResidualBlockHelper : public JKeeper {
  //
  // The frame that contains the image.
  class Frame         *m_pFrame;
  //
  // The residual frame that contains the image.
  class Frame         *m_pResidualFrame;
  //
  // Number of components in the frame, number of components we handle.
  UBYTE                m_ucCount;
  //
  // The DCT for the components, with quantization filled in
  // In case no DCT is run, this is left as NULL.
  class DCT           *m_pDCT[4];
  //
  // The quantization values for luma and chroma. There is only
  // one per component - this is in case the DCT is turned off.
  UWORD                m_usQuantization[4];
  //
  // Noise shaping parameters, one per parameter.
  bool                 m_bNoiseShaping[4];
  //
  // Maximum error for noise masking. Keep at zero.
  UBYTE                m_ucMaxError;
  //
  // Do we have the quantization values?
  bool                 m_bHaveQuantizers;
  //
  // Color transformer buffer
  LONG                 m_ColorBuffer[4][64];
  LONG                *m_pBuffer[4];
  //
  // Find the quantization table for residual component i (index, not label).
  class QuantizationTable *FindQuantizationFor(UBYTE i) const;
  //
  // Find the DCT for the given component, when DCT is enabled. 
  class DCT *FindDCTFor(UBYTE i) const;
  //
  // Allocate the temporary buffers to hold the residuals and their bitmaps.
  // Only required during encoding.
  void AllocateBuffers(void);
  //
public:
  //
  // Construct the helper from the frame and its residual version.
  ResidualBlockHelper(class Frame *frame,class Frame *residualframe);
  //
  ~ResidualBlockHelper(void);
  //
  // Dequantize the already decoded residual (possibly taking the decoded
  // image as predictor) and return it, ready for the color transformation.
  void DequantizeResidual(const LONG *legacy,LONG *target,const LONG *residual,UBYTE i);
  //
  // Quantize the residuals of a block given the DCT data
  void QuantizeResidual(const LONG *legacy,LONG *residual,UBYTE i,LONG bx,LONG by);
  //
  // Return the frame this is part of which is extended by a residual
  class Frame *FrameOf(void) const
  {
    return m_pFrame;
  }
  //
  // Return the residual frame this is part of and which extends the above
  // frame by residuals.
  class Frame *ResidualFrameOf(void) const
  {
    return m_pResidualFrame;
  }
};
///

///
#endif
