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
** This class adapts to a line buffer in a way that allows the user
** to pull out (or push in) individual lines, thus not too much is to
** do here. Again, this adapts to the upsampling process of the
** hierarchical mode.
**
** $Id: linelineadapter.hpp,v 1.9 2014/09/30 12:38:49 thor Exp $
**
*/

#ifndef CONTROL_LINELINEADAPTER_HPP
#define CONTROL_LINELINEADAPTER_HPP

/// Includes
#include "control/bitmapctrl.hpp"
#include "control/linebuffer.hpp"
#include "control/lineadapter.hpp"
///

/// Forwards
struct Line;
class ByteStream;
///

/// class LineLineAdapter
// This class adapts to a line buffer in a way that allows the user
// to pull out (or push in) individual lines, thus not too much is to
// do here. Again, this adapts to the upsampling process of the
// hierarchical mode.
class LineLineAdapter : public LineBuffer, public LineAdapter {
  //
  class Environ        *m_pEnviron;
  //
  class Frame          *m_pFrame;
  //
  // Next line to be delivered.
  struct Line        ***m_pppImage;
  //
  // The number of lines already pushed into the image.
  ULONG                *m_pulReadyLines;
  //
  // The number of lines allocated per component.
  ULONG                *m_pulLinesPerComponent;
  //
  // Number of components adminstrated here. This is always the
  // full number of components in a frame as the hierachical process
  // is not limited to a single scan.
  UBYTE                 m_ucCount; 
  //
  // The block buffer "Buffered lines" does not return a useful value
  // as it expands subsampling.
  virtual ULONG BufferedLines(const RectangleRequest*) const
  {
    JPG_THROW(NOT_IMPLEMENTED,"LineLineAdapter::BufferedLines",NULL);
    return 0;
  }
  //
  // Allocate all the buffers.
  void BuildCommon(void);
  //
public:
  //
  LineLineAdapter(class Frame *frame);
  //
  virtual ~LineLineAdapter(void); 
  // 
  // First time usage: Collect all the information for encoding.
  // May throw on out of memory situations
  virtual void PrepareForEncoding(void)
  {
    BuildCommon();
  }
  //
  // First time usage: Collect all the information for decoding.
  // May throw on out of memory situations.
  virtual void PrepareForDecoding(void)
  {
    BuildCommon();
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
  virtual void ReleaseLine(struct Line *,UBYTE)
  {
    // There is really nothing to do as the line is part of
    // the image.
  }
  //
  // Allocate the next line for encoding. This line must
  // later on then be pushed back into this buffer by
  // PushLine below.
  virtual struct Line *AllocateLine(UBYTE comp);
  //
  // In case an allocated line shall be destroyed, call
  // this instead of ReleaseLine. The allocation strategy on
  // encoding and decoding might be different, and this is
  // the encoding release.
  virtual void DropLine(struct Line *,UBYTE)
  {
    // Really nothing to do as the line is part of the image
    // anyhow and will be released.
  }
  //
  // Push the next line into the output buffer. If eight lines
  // are accumulated (or enough lines up to the end of the image)
  // these lines are automatically transfered to the input
  // buffer of the block based coding back-end.
  virtual void PushLine(struct Line *line,UBYTE comp);
  //
  // Reset all components on the image side of the control to the
  // start of the image. Required when re-requesting the image
  // for encoding or decoding.
  virtual void ResetToStartOfImage(void); 
  //
  // Return true if the next MCU line is buffered and can be pushed
  // to the encoder. Note that the data here is *not* subsampled.
  virtual bool isNextMCULineReady(void) const;
  //
  // Return an indicator whether all of the image has been loaded into
  // the image buffer.
  virtual bool isImageComplete(void) const; 
  //
  // Returns the number of lines buffered for the given component.
  // Note that subsampling expansion has not yet taken place here,
  // this is to be done top-level.
  ULONG BufferedLines(UBYTE comp) const;
  //
  // Return true in case this buffer is organized in lines rather
  // than blocks.
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
    return true;
  }
};
///

///
#endif
