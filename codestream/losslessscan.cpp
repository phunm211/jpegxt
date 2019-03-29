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
** Represents the scan including the scan header.
**
** $Id: losslessscan.cpp,v 1.50 2015/03/25 08:45:43 thor Exp $
**
*/

/// Includes
#include "codestream/losslessscan.hpp"
#include "io/bytestream.hpp"
#include "control/linebuffer.hpp"
#include "control/linebitmaprequester.hpp"
#include "control/lineadapter.hpp"
#include "marker/frame.hpp"
#include "marker/scan.hpp"
#include "marker/component.hpp"
#include "codestream/tables.hpp"
#include "io/bitstream.hpp"
#include "coding/huffmantemplate.hpp"
#include "coding/huffmancoder.hpp"
#include "coding/huffmandecoder.hpp"
#include "coding/huffmanstatistics.hpp"
#include "codestream/tables.hpp"
#include "codestream/predictorbase.hpp"
#include "tools/line.hpp"
///

/// LosslessScan::LosslessScan
LosslessScan::LosslessScan(class Frame *frame,class Scan *scan,UBYTE predictor,UBYTE lowbit,bool differential)
  : PredictiveScan(frame,scan,predictor,lowbit,differential)
{ 
}
///

/// LosslessScan::~LosslessScan
LosslessScan::~LosslessScan(void)
{
}
///

/// LosslessScan::WriteFrameType
// Write the marker that indicates the frame type fitting to this scan.
void LosslessScan::WriteFrameType(class ByteStream *io)
{
  NOREF(io);
}
///

/// LosslessScan::StartParseScan
void LosslessScan::StartParseScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl)
{
  NOREF(io);
  NOREF(chk);
  NOREF(ctrl);
  JPG_THROW(NOT_IMPLEMENTED,"LosslessScan::StartParseScan",
            "Lossless JPEG not available in your code release, please contact Accusoft for a full version");
}
///

/// LosslessScan::StartWriteScan
void LosslessScan::StartWriteScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl)
{
  NOREF(io);
  NOREF(chk);
  NOREF(ctrl);
  JPG_THROW(NOT_IMPLEMENTED,"LosslessScan::StartWriteScan",
            "Lossless JPEG not available in your code release, please contact Accusoft for a full version");
}
///

/// LosslessScan::StartMeasureScan
void LosslessScan::StartMeasureScan(class BufferCtrl *ctrl)
{
  NOREF(ctrl);
}
///

/// LosslessScan::WriteMCU
// Write a single MCU in this scan. Actually, this is not quite true,
// as we write an entire group of eight lines of pixels, as a MCU is
// here a group of pixels. But it is more practical this way.
bool LosslessScan::WriteMCU(void)
{
  return false;
}
///

/// LosslessScan::WriteMCU
// The actual MCU-writer, write a single group of pixels to the stream,
// or measure their statistics.
void LosslessScan::WriteMCU(struct Line **prev,struct Line **top)
{
  NOREF(prev);
  NOREF(top);
}
///

/// LosslessScan::MeasureMCU
// The actual MCU-writer, write a single group of pixels to the stream,
// or measure their statistics. This here only measures the statistics
// to design an optimal Huffman table
void LosslessScan::MeasureMCU(struct Line **prev,struct Line **top)
{
  NOREF(prev);
  NOREF(top);
}
///

/// LosslessScan::ParseMCU
// This is actually the true MCU-parser, not the interface that reads
// a full line.
void LosslessScan::ParseMCU(struct Line **prev,struct Line **top)
{ 
  NOREF(prev);
  NOREF(top);
}
///

/// LosslessScan::ParseMCU
// Parse a single MCU in this scan. Actually, this is not quite true,
// as we write an entire group of eight lines of pixels, as a MCU is
// here a group of pixels. But it is more practical this way.
bool LosslessScan::ParseMCU(void)
{
  return false; // no further blocks here.
}
///

/// LosslessScan::StartMCURow
// Start a MCU scan. Returns true if there are more rows.
bool LosslessScan::StartMCURow(void)
{
  return false;
}
///

/// LosslessScan::Flush
// Flush the remaining bits out to the stream on writing.
void LosslessScan::Flush(bool)
{  
}
///

/// LosslessScan::Restart
// Restart the parser at the next restart interval
void LosslessScan::Restart(void)
{ 
}
///
