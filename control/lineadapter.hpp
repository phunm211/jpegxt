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
** This is a purely abstract interface class for an interface that
** is sufficient for the line merger to pull lines out of a frame,
** or another line merger.
**
** $Id: lineadapter.hpp,v 1.17 2014/09/30 12:38:49 thor Exp $
**
*/

#ifndef CONTROL_LINEADAPTER_HPP
#define CONTROL_LINEADAPTER_HPP

/// Includes
#include "tools/environment.hpp"
#include "tools/line.hpp"
#include "control/bufferctrl.hpp"
///

/// Class LineAdapter
// This is a purely abstract interface class for an interface that
// is sufficient for the line merger to pull lines out of a frame,
// or another line merger.
class LineAdapter : public BufferCtrl {
  //
  // The frame this is part of.
  class Frame   *m_pFrame;
  //
  // Size of the lines in allocated elements 
  ULONG         *m_pulPixelsPerLine;
  //
  // The linked list of lines ready for recycling.
  struct Line  **m_ppFree;
  //
protected:
  //
  // Number of components handled here.
  UBYTE          m_ucCount;
  //
  // Create the line adapter
  LineAdapter(class Frame *frame);
  //
  // Build the substructures after construction.
  void BuildCommon(void);
  //
  // Create a new line for the indicated component.
  struct Line *AllocLine(UBYTE comp);
  //
  // Dispose a line of the indicated component.
  void FreeLine(struct Line *line,UBYTE comp);
  //
public:
  //
  // Dispose it again.
  virtual ~LineAdapter(void);
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
  // Return the frame this is belongs to. This is always the larger
  // high-pass frame.
  class Frame *FrameOf(void) const
  {
    return m_pFrame;
  }
  //
  // Get the next available line from the output
  // buffer on reconstruction. The caller must make
  // sure that the buffer is really loaded up to the
  // point or the line will be neutral grey.
  virtual struct Line *GetNextLine(UBYTE comp) = 0;
  //
  // Release the line as soon as it is no longer required - this
  // step goes after GetNextLine on the client.
  virtual void ReleaseLine(struct Line *line,UBYTE comp) = 0;
  //
  // Allocate the next line for encoding. This line must
  // later on then be pushed back into this buffer by
  // PushLine below.
  virtual struct Line *AllocateLine(UBYTE comp) = 0;
  //
  // Push the next line into the output buffer. If eight lines
  // are accumulated (or enough lines up to the end of the image)
  // these lines are automatically transfered to the input
  // buffer of the block based coding back-end.
  virtual void PushLine(struct Line *line,UBYTE comp) = 0;
  //
  // In case an allocated line shall be destroyed, call
  // this instead of ReleaseLine. The allocation strategy on
  // encoding and decoding might be different, and this is
  // the encoding release.
  virtual void DropLine(struct Line *line,UBYTE comp) = 0;
  // 
  // Reset all components on the image side of the control to the
  // start of the image. Required when re-requesting the image
  // for encoding or decoding.
  virtual void ResetToStartOfImage(void) = 0;
  //
  // Return an indicator whether all of the image has been loaded into
  // the image buffer.
  virtual bool isImageComplete(void) const = 0; 
  //
  // Return true if the next MCU line is buffered and can be pushed
  // to the encoder.
  virtual bool isNextMCULineReady(void) const = 0;
  //
  // Return the number of lines available for reconstruction from this scan.
  virtual ULONG BufferedLines(UBYTE comp) const = 0;
  //  
  // Post the height of the frame in lines. This happens
  // when the DNL marker is processed.
  virtual void PostImageHeight(ULONG)
  {
    // Nothing to do here.
  }
  //
  // In case the high-pass has a DC offset in its data, deliver it here.
  virtual LONG DCOffsetOf(void) const = 0;
  //
  // In case the high-pass is supposed to be a lossless process such that
  // we require exact differentials, return true.
  virtual bool isLossless(void) const = 0;
};
///

///
#endif
