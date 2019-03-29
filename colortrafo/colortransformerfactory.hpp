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
** This class builds the proper color transformer from the information
** in the MergingSpecBox
**
** $Id: colortransformerfactory.hpp,v 1.22 2015/01/27 18:31:03 thor Exp $
**
*/

#ifndef COLORTRAFO_COLORTRANSFORMERFACTORY_HPP
#define COLORTRAFO_COLORTRANSFORMERFACTORY_HPP

/// Includes
#include "interface/types.hpp"
#include "tools/environment.hpp"
///

/// Forwards
class MergingSpecBox;
class Tables;
class Frame;
class ParametricToneMappingBox;
class IntegerTrafo;
class FloatTrafo;
///

/// class ColorTransformerFactory
// This class builds the proper color transformer from the information
// in the MergingSpecBox
class ColorTransformerFactory : public JKeeper {
  //
  // The color transformation itself - is kept here.
  class ColorTrafo                 *m_pTrafo;
  //
  // Additional coding tables containing the LUTs required
  // for the color transformation.
  class Tables                     *m_pTables;
  //
  // The identity tone mapper that is used for the L-Lut in case no specs are there.
  // This is the identity transformation that preserves the neutral value.
  class Box                        *m_pIdentity0;
  //
  // This is the identity transformation that preserves min and max.
  class Box                        *m_pIdentity1;
  //
  // The zero-mapping for the R-tables in case the scaling type is set to zero.
  class Box                        *m_pZero;
  //
  // Given a LUT index, construct the tone mapping represenging it.
  // The e parameter defines the rounding mode for the default table. 
  // For e=0, the neutral value is preserved, for e=1, the extremes are preserved.
  class ToneMapperBox *FindToneMapping(UBYTE idx,UBYTE e);
  //
  // Build transformations that require only L and R.
  class IntegerTrafo *BuildIntegerTransformation(UBYTE etype,class Frame *frame,class Frame *residualframe,
                                                 class MergingSpecBox *specs,
                                                 UBYTE ocflags,int ltrafo,int rtrafo);
  // 
#if ISO_CODE
  // Build transformations in the floating point transformations A and B.
  class FloatTrafo *BuildFloatTransformation(UBYTE etype,class Frame *frame,class Frame *residualframe,
                                             class MergingSpecBox *specs,bool diagonal,
                                             UBYTE ocflags,int ltrafo,int rtrafo);
#endif
  //
  // Build a transformation using the JPEG-LS color transformation back-end.
  // This works only without a residual (why a residual anyhow?)
  class ColorTrafo *BuildLSTransformation(UBYTE type,
                                          class Frame *frame,class Frame *residualframe,
                                          class MergingSpecBox *,
                                          UBYTE ocflags,int ltrafo,int rtrafo);
  //
  //
  // More helpers to cut down the possibilities a bit.
  template<int count,typename type>
  IntegerTrafo *BuildIntegerTransformationSimple(class Frame *frame,class Frame *residualframe,
                                                       class MergingSpecBox *specs,
                                                       UBYTE ocflags,int ltrafo,int rtrafo);
  //
  template<int count,typename type>
  IntegerTrafo *BuildIntegerTransformationExtensive(class Frame *frame,class Frame *residualframe,
                                                          class MergingSpecBox *specs,
                                                          UBYTE ocflags,int ltrafo,int rtrafo);
  //
  // Install the parameters to fully define a profile C encoder/decoder
  void InstallIntegerParameters(class IntegerTrafo *trafo,
                                class MergingSpecBox *specs,
                                int count,bool encoding,bool residual,
                                UBYTE inbpp,UBYTE outbpp,UBYTE resbpp,UBYTE rbits,
                                MergingSpecBox::DecorrelationType ltrafo,
                                MergingSpecBox::DecorrelationType rtrafo,
                                MergingSpecBox::DecorrelationType ctrafo);
  //
#if ISO_CODE
  // Install all the coding parameters for a profile A encoder or decoder.
  // Get all the parameters from the MergingSpecBox.
  void InstallProfileAParameters(class FloatTrafo *trafo,
                                 class Frame *frame,class Frame *residualframe,
                                 class MergingSpecBox *specs,
                                 int count,bool encoding);
  //
  // Install all the coding parameters for a profile B encoder or decoder.
  // Get all the parameters from the MergingSpecBox.
  void InstallProfileBParameters(class FloatTrafo *trafo,
                                 class Frame *frame,class Frame *residualframe,
                                 class MergingSpecBox *specs,
                                 int count,bool encoding);
#endif
  //
  // Fill in a default matrix from its decorrelation type. This is the fixpoint version.
  void GetStandardMatrix(MergingSpecBox::DecorrelationType dt,LONG matrix[9]);
  //
#if ISO_CODE
  // Fill in a default matrix from its decorrelation type. This is the floating point version.
  void GetStandardMatrix(MergingSpecBox::DecorrelationType dt,FLOAT matrix[9]);
#endif
  //
  // Return the inverse of a standard matrix in fixpoint.
  void GetInverseStandardMatrix(MergingSpecBox::DecorrelationType dt,LONG matrix[9]);
  //
#if ISO_CODE
  // Return the inverse of a standard matrix in floating point.
  void GetInverseStandardMatrix(MergingSpecBox::DecorrelationType dt,FLOAT matrix[9]);
#endif
  //
  //
public:
  // Build a color transformation factory - requires the tables
  // that contain most of the data.
  ColorTransformerFactory(class Tables *tables);
  //
  // Destroy the factory and all that it produced.
  ~ColorTransformerFactory(void);
  //
  // Build a color transformer from the merging specifications
  // passed in. These might be NULL in case there is none and
  // the JPEG stream is non-extended. Also requires the number
  // of components.
  // Residual may be NULL if there is no residual frame.
  // Returns the color transformation class.
  // Note that there is at most one color transformer in the system
  // (there are no tiles here as in JPEG 2000).
  // dctbits is the number of bits "closer to the DCT side", i.e. on decoding, the input bits,
  // spatialbits is the bit precision "closer to the image", i.e. on decoding, the output bits.
  class ColorTrafo *BuildColorTransformer(class Frame *frame,class Frame *residual,
                                          class MergingSpecBox *specs,
                                          UBYTE dctbits,UBYTE spatialbits,
                                          UBYTE external_type,bool encoding);
  //
};
///

///
#endif
