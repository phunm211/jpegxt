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
** $Id: linemerger.cpp,v 1.27 2015/06/03 15:37:25 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "tools/line.hpp"
#include "control/lineadapter.hpp"
#include "control/linemerger.hpp"
#include "marker/frame.hpp"
#include "marker/component.hpp"
#include "codestream/tables.hpp"
#include "colortrafo/colortrafo.hpp"
#include "std/string.hpp"
#define COLOR_BITS ColorTrafo::COLOR_BITS
///

/// LineMerger::LineMerger
// The frame to create the line merger from is the highpass frame as
// its line dimensions are identical to that of the required output.
LineMerger::LineMerger(class Frame *frame,class LineAdapter *low,class LineAdapter *high,
                       bool expandhor,bool expandver)
  : LineAdapter(frame)
{
  NOREF(frame);
  NOREF(low);
  NOREF(high);
  NOREF(expandhor);
  NOREF(expandver);
}
///

/// LineMerger::~LineMerger
LineMerger::~LineMerger(void)
{
}
///

/// LineMerger::BuildCommon
// Second-stage constructor, construct the internal details.
void LineMerger::BuildCommon(void)
{
}
///


/// LineMerger::GetNextLowPassLine
// Fetch the next line from the low-pass and expand it horizontally if required.
///

/// LineMerger::GetNextExpandedLowPassLine
// Fetch a line from the low-pass filter and expand it in horizontal
// or vertical direction. Do not do anything else.
///

/// LineMerger::GetNextLine
// Get the next available line from the output
// buffer on reconstruction. The caller must make
// sure that the buffer is really loaded up to the
// point or the line will be neutral grey.
struct Line *LineMerger::GetNextLine(UBYTE comp)
{
  NOREF(comp);
  return NULL;
}
///

/// LineMerger::ReleaseLine
// Release a line passed out to the user. As the lines always come from the highpass,
// let the highpass release it.
void LineMerger::ReleaseLine(struct Line *line,UBYTE comp)
{
  NOREF(line);
  NOREF(comp);
}
///

/// LineMerger::AllocateLine
// Allocate a new line for encoding.
struct Line *LineMerger::AllocateLine(UBYTE comp)
{
  NOREF(comp);
  return NULL;
}
///

/// LineMerger::PushLine
// Push the next line into the output buffer. If eight lines
// are accumulated (or enough lines up to the end of the image)
// these lines are automatically transfered to the input
// buffer of the block based coding back-end.
void LineMerger::PushLine(struct Line *line,UBYTE comp)
{
  NOREF(line);
  NOREF(comp);
}
///

/// LineMerger::ResetToStartOfImage
// Rewind the image buffer to the start of the image
void LineMerger::ResetToStartOfImage(void)
{
}
///

/// LineMerger::GenerateDifferentialImage
// Generate a differential image by pulling the reconstructed
// image from the low-pass and pushing the differential signal
// into the high-pass.
void LineMerger::GenerateDifferentialImage(void)
{
}
///

/// LineMerger::PostImageHeight
// Post the height of the frame in lines. This happens
// when the DNL marker is processed.
void LineMerger::PostImageHeight(ULONG lines)
{
  LineAdapter::PostImageHeight(lines);

}
///
