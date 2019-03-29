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
** This box defines the parameters for the LDR/residual merging process.
**
** $Id: mergingspecbox.hpp,v 1.32 2015/03/13 15:14:07 thor Exp $
**
*/

#ifndef BOXES_MERGINGSPECBOX_HPP
#define BOXES_MERGINGSPECBOX_HPP

/// Includes
#include "boxes/superbox.hpp"
#include "boxes/dctbox.hpp"
#include "boxes/alphabox.hpp"
#include "std/string.hpp"
///

/// Forwards
class ParametricToneMappingBox;
class OutputConversionBox;
class NonlinearTrafoBox;
class ColorTrafoBox;
class DCTBox;
class RefinementSpecBox;
class MatrixBox;
class NameSpace;
class Tables;
///

/// class MergingSpecBox
// This box defines the parameters for the LDR/residual merging process,
// it is a superbox that contains all the parameters as sub-boxes.
class MergingSpecBox : public SuperBox {
  //
  // The namespace for all boxes. This allocates IDs and finds boxes.
  class NameSpace                *m_pNameSpace;
  //
  // Global boxes for both the legacy and the residual process.
  class RefinementSpecBox        *m_pRefinementSpec;
  class OutputConversionBox      *m_pOutputConversion;
  //
  // Boxes in the base/legacy (L-) path.
  class DCTBox                   *m_pBaseDCT;
  class ColorTrafoBox            *m_pBaseTransformation;     // L-Trafo
  class NonlinearTrafoBox        *m_pBaseNonlinearity;       // L-tables
  class ColorTrafoBox            *m_pColorTransformation;    // C-trafo
  class NonlinearTrafoBox        *m_p2ndBaseNonlinearity;    // L2-tables
  //
  // Boxes in the residual (R-) path
  class DCTBox                   *m_pResidualDCT;
  class NonlinearTrafoBox        *m_pResidualNonlinearity;             // Q-tables
  class ColorTrafoBox            *m_pResidualTransformation;           // R-trafo
  class NonlinearTrafoBox        *m_pIntermediateResidualNonlinearity; // R-tables
  class ColorTrafoBox            *m_pResidualColorTransformation;      // D-trafo
  class NonlinearTrafoBox        *m_p2ndResidualNonlinearity;          // R2-tables
  //
  // Boxes in the diagonal (S-) path
  class ColorTrafoBox            *m_pPrescalingTransformation;         // P-trafo
  class NonlinearTrafoBox        *m_pPrescalingNonlinearity;           // S-table
  class NonlinearTrafoBox        *m_pPostscalingNonlinearity;          // S2-table
  //
  // The alpha box in case we are the alpha merging spec box.
  class AlphaBox                 *m_pAlphaMode;
  //
  // Create a sub-box of this superbox, which is then parsed off.
  // To be implemented by derived classes: Create a box for this superbox
  // in the given box list.
  virtual class Box *CreateBox(class Box *&boxlist,ULONG tbox);
  //
  // Inform the superbox that the box is now created. Does nothing by default,
  // can be overloaded to sort the new box in.
  virtual void AcknowledgeBox(class Box *box,ULONG tbox);
  //
  // All other parts of the box parsing/creation logic are already defined in
  // the superbox.
  //
  // Find the next free linear matrix Id or throw if there is none.
  UBYTE AllocateMatrixID(void) const;
  //
  // Find the next free non-linear lookup/curve ID or throw if there is none.
  UBYTE AllocateNLTID(void) const;
  //
public:
  //
  enum {
    SpecType  = MAKE_ID('S','P','E','C'),
    AlphaType = MAKE_ID('A','S','P','C') 
  };
  //
  // Linear decorrelation types.
  enum DecorrelationType {
    Zero      = 0, // internal use only: If absent.
    Identity  = 1,
    YCbCr     = 2,
    JPEG_LS   = 3, // also internal use only
    RCT       = 4,
    FreeForm  = 5, // and above
    Undefined = MAX_UBYTE
  };
  //
  MergingSpecBox(class Tables *tables,class Box *&boxlist,ULONG boxtype);
  // 
  virtual ~MergingSpecBox(void);
  //
  // Define the number of hidden refinement bits in the legacy stream.
  void DefineHiddenBits(UBYTE hiddenbits);
  //
  // Return the number of hidden DCT bits.
  UBYTE HiddenBitsOf(void) const;
  //
  // Define the number of hidden bits in the residual stream
  void DefineHiddenResidualBits(UBYTE hiddenbits);
  //
  // Return the number of hidden residual bits.
  UBYTE HiddenResidualBitsOf(void) const;
  //
  // Define the additional number of bits in the spatial domain.
  // These are the number of bits on top of the bits in the legacy
  // domain, made available by other means. The total bit precision
  // of the image is r_b + 8.
  void DefineResidualBits(UBYTE residualbits);
  //
  // Return the additional number of bits in the spatial domain.
  UBYTE ResidualBitsOf(void) const;
  //
  // Define the LUT for the component comp in the L-Tables.
  void DefineLTable(UBYTE component,UBYTE tableidx);
  //
  // Return the LUT table index for the L-table for the given component
  // and the type of how to apply it. Return MAX_UBYTE in case the
  // base transformation is not used.
  UBYTE LTableIndexOf(UBYTE component) const;
  //
  // Define the L-multi-component decorrelation transformation.
  // This is the LDR decorrelation transformation.
  void DefineLTransformation(DecorrelationType method);
  //
  // Return the L transformation type. Returns MAX_UBYTE if undefined
  // because the box is not present.
  DecorrelationType LTransformationOf(void) const;
  //
  // Define the LUT for the component comp in the Q-Tables.
  // This is in the QPTS box.
  void DefineQTable(UBYTE component,UBYTE tableidx);
  //
  // Return the LUT table index for the Q-table for the given component
  // or return MAX_UBYTE in case the table is not defined.
  // This is in the QPTS box.
  UBYTE QTableIndexOf(UBYTE component) const;
  //  
  // Define the LUT for the component comp in the R-Tables. This
  // goes into the DPTS box.
  void DefineRTable(UBYTE component,UBYTE tableidx);
  //
  // Return the LUT table index for the R-table for the given component
  // or return MAX_UBYTE in case the table is not defined.
  // This is the contents of the DPTS box.
  UBYTE RTableIndexOf(UBYTE component) const;
  //
  // Define the R-multi-component decorrelation transformation.
  // This the residual decorrelation transformation.
  void DefineRTransformation(DecorrelationType method);
  // 
  // Return the R transformation type or MAX_UBYTE in case it is not
  // defined.
  DecorrelationType RTransformationOf(void) const;
  //
  // Define the C transformation type.
  // This is the color space decorrelation.
  void DefineCTransformation(DecorrelationType method);
  //
  // Return the C transformation type or MAX_UBYTE in case it is not
  // defined.
  DecorrelationType CTransformationOf(void) const;
  //
  // Define the P transformation type. This is the
  // color transformation that, for profile A, computes the luminance
  // from the precursor image.
  void DefinePTransformation(DecorrelationType method);
  //
  // Return the P transformation type or MAX_UYTE in case it is not
  // defined. This is the transformation that computes from the
  // precursor image a luminance value for scaling the chrominance
  // residuals.
  DecorrelationType PTransformationOf(void) const;
  //
  // Define the D transformatin type. This is the color
  // transformation in the residual domain. It is usually the identity,
  // but for profile B, it may be something else.
  void DefineDTransformation(DecorrelationType method);
  //
  // Return the D transformation type or MAX_UBYTE in case it is not
  // defined. This is the transformation in the linear domain of the
  // residual decoding pass that is used by profile B to extend the
  // gammut.
  DecorrelationType DTransformationOf(void) const;
  //
  // Define the curve/entry for the second base transformation.
  void DefineL2Table(UBYTE comp,UBYTE tableidx);
  //
  // Return the table index for the 2nd base transformation table or
  // MAX_UBYTE in case it is not defined.
  UBYTE L2TableIndexOf(UBYTE comp) const;
  //
  // Define the curve/entry for the second residual transformation.
  // This is in the RPTS box.
  void DefineR2Table(UBYTE comp,UBYTE tableidx);
  //
  // Return the table index for the 2nd residual transformation table or
  // MAX_UBYTE in case it is not defined.
  // This is in the RPTS box.
  UBYTE R2TableIndexOf(UBYTE comp) const;
  //
  // Define the DCT process in the L-chain. 
  void DefineLDCTProcess(DCTBox::DCTType dct);
  //
  // Return the DCT process for component i.
  DCTBox::DCTType LDCTProcessOf(void) const;
  //
  // Return the state of the lossless flag.
  bool isLossless(void) const;
  //
  // Define the DCT process for component comp in the R-chain.
  void DefineRDCTProcess(DCTBox::DCTType dct);
  //
  // Return the DCT process in the residual domain.
  DCTBox::DCTType RDCTProcessOf(void) const;
  //
  // Define the noise shaping in the R-channel.
  void DefineNoiseShaping(bool enable_noise_shaping);
  //
  // Check whether the residual data of component i undergoes noise shaping.
  bool isNoiseShapingEnabled(void) const;
  //
  // Set whether the output shall be casted from int to float.
  void DefineOutputConversion(bool convert);
  //
  // Defines the output conversion from a Lookup table index.
  void DefineOutputConversionTable(UBYTE component,UBYTE table);
  //
  // Specify whether the output shall be clipped to range.
  void DefineClipping(bool clipping);
  //
  // Specify whether the process is lossy or not.
  void DefineLossless(bool lossless);
  //
  // Check whether the encoded data uses output conversion from int to IEEE half float.
  bool usesOutputConversion(void) const;
  //
  // Returns the output conversion for the indicated component or MAX_UBYTE in case
  // this table is not used.
  UBYTE OutputConversionLookupOf(UBYTE component) const;
  //
  // Check whether the encoded data uses clipping. Required for int coding.
  bool usesClipping(void) const;
  //
  // Check whether this is (likely) a profile A codec.
  bool isProfileA(void) const;
  //
  // Check whether this is (likely) a profile B codec.
  bool isProfileB(void) const;
  //
  // Defines the nonlinearity for the prescaling transformation. This goes for
  // all components, i.e. there is only a single table.
  void DefinePTable(UBYTE tableidx);
  //
  // Return the table index of the pre-scaling non-linearity or MAX_UBYTE
  // in case this table is not defined.
  UBYTE PTableIndexOf(void) const;
  //
  // Defines the non-linearity for the postscaling transformation. This
  // transformation computes the luminance scale factor for all components
  // after merging the specs.
  void DefineSTable(UBYTE tableidx);
  //
  // Return the table index of the post-scaling non-linearity or MAX_UBYTE
  // in case this table is not defined. This
  // transformation computes the luminance scale factor for all components
  // after merging the specs.
  UBYTE STableIndexOf(void) const;
  //
  // Create an sRGB type nonlinearity and return the table index.
  UBYTE CreatesRGBCurve(UBYTE rounding_mode,FLOAT p1 = 0.04045,FLOAT p2 = 2.4,FLOAT p3 = 0.055);
  //
  // Create a linear ramp (non-)linearity from the points P1 to P2.
  UBYTE CreateLinearRamp(UBYTE rounding_mode,FLOAT p1,FLOAT p2);
  //
  // Create an exponental ramp P3*exp((P2-P1)*x+P1)+P4
  UBYTE CreateExponentialRamp(UBYTE rounding_mode,FLOAT p1,FLOAT p2,FLOAT p3,FLOAT p4);
  //
  // Create a logarithmic map sign(P1)*log((|P1|*x)^P2+P3)+P4
  UBYTE CreateLogarithmicMap(UBYTE rounding_mode,FLOAT p1,FLOAT p2,FLOAT p3,FLOAT p4);
  //
  // Create a power map (p2-p1)*x^p3+p1
  UBYTE CreatePowerMap(UBYTE rounding_mode,FLOAT p1,FLOAT p2,FLOAT p3);
  //
  // Create an identity mapping with the given rounding mode. Note that this is actually scaling.
  UBYTE CreateIdentity(UBYTE rounding_mode);
  //
  // Build a free-form linear transformation that converts between color spaces. 
  // Matrix coefficients start at the given tag-base and will be parsed off.
  // This call will create an appropriate LinearTransformationBox and will return its ID, to be
  // put into the MergingSpecBox.
  DecorrelationType ParseFreeFormTransformation(const struct JPG_TagItem *tags,ULONG tagbase);
  //
  // Build a floating point version of a matrix and place it here. This is similar to the above
  // except that the coefficients are not fixpoint numbers.
  DecorrelationType ParseFreeFormFloatTransformation(const struct JPG_TagItem *tags,ULONG tagbase);
  //
  // Return the current setting of the alpha box if we have one. Return a negative value in case 
  // the box is not defined. Returns the matte color as side information.
  BYTE AlphaModeOf(ULONG &matte_r,ULONG &matte_g,ULONG &matte_b);
  //
  // Define the alpha compositing box box with the given settings and the given matte colors.
  void SetAlphaMode(AlphaBox::Method mode,ULONG r,ULONG g,ULONG b);
};
///

///
#endif
