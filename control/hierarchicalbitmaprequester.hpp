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
** This is the top-level bitmap requester that distributes data to
** image scales on encoding, and collects data from image scales on
** decoding. It also keeps the top-level color transformer and the
** toplevel subsampling expander.
**
** $Id: hierarchicalbitmaprequester.hpp,v 1.15 2016/10/28 13:58:53 thor Exp $
**
*/

#ifndef CONTROL_HIERARCHICALBITMAPREQUESTER_HPP
#define CONTROL_HIERARCHICALBITMAPREQUESTER_HPP

/// Includes
#include "tools/environment.hpp"
#include "control/bitmapctrl.hpp"
///

/// Forwards
class DownsamplerBase;
class UpsamplerBase;
struct ImageBitMap;
class Frame;
class LineAdapter;
///

/// Class HierarchicalBitmapRequester
// This is the top-level bitmap requester that distributes data to
// image scales on encoding, and collects data from image scales on
// decoding. It also keeps the top-level color transformer and the
// toplevel subsampling expander.
class HierarchicalBitmapRequester : public BitmapCtrl {
  //
  //
public:
  // Construct from a frame - the frame is just a "dummy frame"
  // that contains the dimensions, actually a DHP marker segment
  // without any data in it.
  HierarchicalBitmapRequester(class Frame *dimensions);
  //
  ~HierarchicalBitmapRequester(void);
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
  // than blocks. Actually, this matters little as the entropy coding
  // backend talks to the adapters instead.
  virtual bool isLineBased(void) const
  {
    return true;
  }
  // 
  // Return the number of lines available for reconstruction from this scan.
  virtual ULONG BufferedLines(const struct RectangleRequest *rr) const;
  //
  // As soon as a frame is parsed off, or created: Add another scale to the image.
  // The boolean arguments identify whether the reference frame, i.e. what is
  // buffered already from previous frames, will be expanded.
  void AddImageScale(class Frame *frame,bool expandh,bool expandv);
  //
  // After having written the previous image, compute the differential from the downscaled
  // and-re-upscaled version and push it into the next frame, collect the
  // residuals, make this frame ready for encoding, and retrieve the downscaling
  // data.
  void GenerateDifferentialImage(class Frame *target,bool &hexp,bool &vexp); 
  //
  // Post the height of the frame in lines. This happens
  // when the DNL marker is processed.
  virtual void PostImageHeight(ULONG lines);
};
///

///
#endif
