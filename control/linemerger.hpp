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
** This class merges the two sources of a differential frame together,
** expanding its non-differential source.
**
** $Id: linemerger.hpp,v 1.23 2015/03/25 08:45:43 thor Exp $
**
*/

#ifndef CONTROL_LINEMERGER_HPP
#define CONTROL_LINEMERGER_HPP

/// Includes
#include "tools/environment.hpp"
#include "tools/line.hpp"
#include "control/lineadapter.hpp"
///

/// Class LineMerger
// This class merges the two sources of a differential frame together,
// expanding its non-differential source.
class LineMerger : public LineAdapter {
  //
  //
public:
  // The frame to create the line merger from is the highpass frame as
  // its line dimensions are identical to that of the required output.
  LineMerger(class Frame *frame,class LineAdapter *low,class LineAdapter *high,
             bool expandh,bool expandv);
  //
  virtual ~LineMerger(void);
  //
  // Second-stage constructor, construct the internal details.
  void BuildCommon(void);
  // 
  // First time usage: Collect all the information for encoding.
  // May throw on out of memory situations
  virtual void PrepareForEncoding(void)
  {
  }
  //
  // First time usage: Collect all the information for decoding.
  // May throw on out of memory situations.
  virtual void PrepareForDecoding(void)
  {
  }
  //
  // Return the next smaller scale adapter if there is any, or
  // NULL otherwise.
  virtual class LineAdapter *LowPassOf(void) const
  {
    return NULL;
  }
  //
  // The high-pass end if there is one, or NULL.
  virtual class LineAdapter *HighPassOf(void) const
  {
    return NULL;
  }
  //
  // Check whether a horizontal expansion is performed here.
  bool isHorizontallyExpanding(void) const
  {
    return false;
  }
  //
  // Check whether a vertical expansion is performed here.
  bool isVerticallyExpanding(void) const
  {
    return false;
  }
  //
  // Get the next available line from the output
  // buffer on reconstruction. The caller must make
  // sure that the buffer is really loaded up to the
  // point or the line will be neutral grey.
  virtual struct Line *GetNextLine(UBYTE comp);
  //
  // Release the line as soon as it is no longer required - this
  // step goes after GetNextLine on the client.
  virtual void ReleaseLine(struct Line *line,UBYTE comp);
  //
  // Allocate the next line for encoding. This line must
  // later on then be pushed back into this buffer by
  // PushLine below.
  virtual struct Line *AllocateLine(UBYTE comp);
  //
  // Push the next line into the output buffer. If eight lines
  // are accumulated (or enough lines up to the end of the image)
  // these lines are automatically transfered to the input
  // buffer of the block based coding back-end.
  virtual void PushLine(struct Line *line,UBYTE comp);
  //
  // In case an allocated line shall be destroyed, call
  // this instead of ReleaseLine. The allocation strategy on
  // encoding and decoding might be different, and this is
  // the encoding release.
  virtual void DropLine(struct Line *line,UBYTE comp)
  {
    FreeLine(line,comp);
  }
  //  
  // Reset all components on the image side of the control to the
  // start of the image. Required when re-requesting the image
  // for encoding or decoding.
  virtual void ResetToStartOfImage(void); 
  //
  // Generate a differential image by pulling the reconstructed
  // image from the low-pass and pushing the differential signal
  // into the high-pass.
  void GenerateDifferentialImage(void); 
  //
  // Return an indicator whether all of the image has been loaded into
  // the image buffer.
  virtual bool isImageComplete(void) const
  {
    return true;
  } 
  //
  // Return true if the next MCU line is buffered and can be pushed
  // to the encoder.
  virtual bool isNextMCULineReady(void) const
  {
    return false;
  } 
  //
  // Return the number of lines available for reconstruction from this scan.
  virtual ULONG BufferedLines(UBYTE comp) const
  {
    NOREF(comp);
    return 0;
  }
  //
  // This does not really make any difference as this object is not used
  // directly by the back-end.
  virtual bool isLineBased(void) const
  {
    return true;
  }
  //  
  // Post the height of the frame in lines. This happens
  // when the DNL marker is processed.
  virtual void PostImageHeight(ULONG lines);
  //
  // In case the high-pass has a DC offset in its data, deliver it here.
  virtual LONG DCOffsetOf(void) const
  {
    return 0; // none.
  }
  //
  // In case the high-pass is supposed to be a lossless process such that
  // we require exact differentials, return true.
  virtual bool isLossless(void) const
  {
    return false;
  }
};
///

///
#endif
