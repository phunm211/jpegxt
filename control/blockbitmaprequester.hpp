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
** $Id: blockbitmaprequester.hpp,v 1.29 2016/10/28 13:58:53 thor Exp $
**
*/

#ifndef CONTROL_BLOCKBITMAPREQUESTER_HPP
#define CONTROL_BLOCKBITMAPREQUESTER_HPP

/// Includes
#include "control/bitmapctrl.hpp"
#include "control/blockbuffer.hpp"
///

/// Forwards
class DCT;
class UpsamplerBase;
class DownsamplerBase;
class ColorTrafo;
class QuantizedRow;
class ResidualBlockHelper;
class DeRinger;
///

/// class BlockBitmapRequester
// This class pulls blocks from the frame and reconstructs from those
// quantized block lines or encodes from them.
class BlockBitmapRequester : public BlockBuffer, public BitmapCtrl {
  //
  class Environ             *m_pEnviron;
  class Frame               *m_pFrame;
  //
  // Dimensions of the frame.
  ULONG                      m_ulPixelWidth;
  ULONG                      m_ulPixelHeight;
  //
  // Number of components in the frame.
  UBYTE                      m_ucCount;
  //
  // Number of lines already in the input buffer on encoding.
  ULONG                     *m_pulReadyLines;
  //
  // Temporary for decoding how many MCUs are ready on the next
  // iteration.can be pulled next.
  ULONG                      m_ulMaxMCU;
  // 
  // Downsampling operator.
  class DownsamplerBase    **m_ppDownsampler;
  //
  // The downsampler for the residual image.
  class DownsamplerBase    **m_ppResidualDownsampler;
  //
  // And the inverse, if required.
  class UpsamplerBase      **m_ppUpsampler;
  //
  // The upsampler for the residual image.
  class UpsamplerBase      **m_ppResidualUpsampler;
  //
  // The original image buffered in a dummy 1,1 downsampler
  class DownsamplerBase    **m_ppOriginalImage;
  //
  // Temporary bitmaps
  struct ImageBitMap       **m_ppTempIBM;
  //
  struct ImageBitMap       **m_ppOriginalIBM;
  //
  // Temporary data pointers for the residual computation.
  LONG                     **m_ppQTemp;
  LONG                     **m_ppRTemp;
  //
  // Temporary output buffer for the residual
  LONG                     **m_ppDTemp;
  //
  // The output color buffer.
  LONG                      *m_plResidualColorBuffer;
  //
  // The buffer for the original data.
  LONG                      *m_plOriginalColorBuffer;
  //
  // Current position in reconstruction or encoding,
  // going through the color transformation.
  // On decoding, the line in here has the Y-coordinate 
  // in m_ulReadyLines.
  class QuantizedRow      ***m_pppQImage;
  //
  // Current position for the residual image.
  class QuantizedRow      ***m_pppRImage;
  //
  // A helper class that encodes the residual.
  class ResidualBlockHelper *m_pResidualHelper;
  //
  // Deblocking filter (if any)
  class DeRinger           **m_ppDeRinger;
  //
  // True if subsampling is required.
  bool                       m_bSubsampling;
  //
  // True if this is an openloop encoder, i.e. we do not
  // use the reconstructed DCT samples.
  bool                       m_bOpenLoop;
  //
  // If this is true, the post-DCT R/D optimizer is on.
  bool                       m_bOptimize;
  //
  // If this is true, run the deblocking filter as well.
  bool                       m_bDeRing;
  //
  // Build common structures for encoding and decoding
  void BuildCommon(void);
  //
  // Create the next row of the image such that m_pppImage[i] is valid.
  class QuantizedRow *BuildImageRow(class QuantizedRow **qrow,class Frame *frame,int i);
  //
  // Forward the state machine for the quantized rows by one image-8-block line
  void AdvanceQRows(void);
  //
  // Compute the residual data and move that into the R-output buffers.
  void AdvanceRRows(const RectAngle<LONG> &region,class ColorTrafo *ctrafo);
  //
  // Get the source data from the source image(s)
  // and place them into the downsampler and the original
  // image buffer.
  void PullSourceData(const RectAngle<LONG> &region,class ColorTrafo *ctrafo);
  //
  // The encoding procedure without subsampling, which is the much simpler case.
  void EncodeUnsampled(const RectAngle<LONG> &region,class ColorTrafo *ctrafo);
  //
  // Reconstruct a region not using any subsampling.
  void ReconstructUnsampled(const struct RectangleRequest *rr,const RectAngle<LONG> &region,
                            ULONG maxmcu,class ColorTrafo *ctrafo);
  //
  // Pull the quantized data into the upsampler if there is one.
  void PullQData(const struct RectangleRequest *rr,const RectAngle<LONG> &region);
  //
  // Get the residual data and potentially move it into the
  // residual upsampler
  void PullRData(const struct RectangleRequest *rr,const RectAngle<LONG> &region);
  //
  // Generate the final output of the reconstructed data.
  void PushReconstructedData(const struct RectangleRequest *rr,const RectAngle<LONG> &region,
                             ULONG maxmcu,class ColorTrafo *ctrafo);
  //
public:
  //
  BlockBitmapRequester(class Frame *frame);
  //
  virtual ~BlockBitmapRequester(void); 
  //
  class Environ *EnvironOf(void) const
  {
    return m_pEnviron;
  }
  //
  // First time usage: Collect all the information for encoding.
  // May throw on out of memory situations
  virtual void PrepareForEncoding(void);
  //
  // First time usage: Collect all the information for decoding.
  // May throw on out of memory situations.
  virtual void PrepareForDecoding(void);
  //
  // Return the color transformer responsible for this scan.
  class ColorTrafo *ColorTrafoOf(bool encoding);
  //
  // First step of a region encoder: Find the region that can be pulled in the next step,
  // from a rectangle request. This potentially shrinks the rectangle, which should be
  // initialized to the full image.
  virtual void CropEncodingRegion(RectAngle<LONG> &region,const struct RectangleRequest *rr);
  //
  // Request user data for encoding for the given region, potentially clip the region to the
  // data available from the user.
  virtual void RequestUserDataForEncoding(class BitMapHook *bmh,RectAngle<LONG> &region,bool alpha);
  //
  // Pull data buffers from the user data bitmap hook
  virtual void RequestUserDataForDecoding(class BitMapHook *bmh,RectAngle<LONG> &region,
                                          const struct RectangleRequest *rr,bool alpha); 
  //
  // Encode a region, push it into the internal buffers and
  // prepare everything for coding.
  virtual void EncodeRegion(const RectAngle<LONG> &region);
  //
  // Reconstruct a block, or part of a block
  virtual void ReconstructRegion(const RectAngle<LONG> &region,const struct RectangleRequest *rr);
  //
  // Return true if the next MCU line is buffered and can be pushed
  // to the encoder.
  virtual bool isNextMCULineReady(void) const;
  //
  // Reset all components on the image side of the control to the
  // start of the image. Required when re-requesting the image
  // for encoding or decoding.
  virtual void ResetToStartOfImage(void); 
  //
  // Return an indicator whether all of the image has been loaded into
  // the image buffer.
  virtual bool isImageComplete(void) const;
  //
  // Return true in case this buffer is organized in lines rather
  // than blocks.
  virtual bool isLineBased(void) const
  {
    return false;
  }  
  //
  // Return the number of lines available for reconstruction from this scan.
  virtual ULONG BufferedLines(const struct RectangleRequest *rr) const
  {
    return BlockBuffer::BufferedLines(rr);
  }
  //
  // Install a block helper.
  void SetBlockHelper(class ResidualBlockHelper *helper);
  //
  // Post the height of the frame in lines. This happens
  // when the DNL marker is processed.
  virtual void PostImageHeight(ULONG lines)
  {
    BitmapCtrl::PostImageHeight(lines);
    BlockBuffer::PostImageHeight(lines);
    
    m_ulPixelHeight = lines;
  }
};
///

///
#endif
