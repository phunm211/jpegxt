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
** $Id: aclosslessscan.cpp,v 1.41 2015/03/25 08:45:42 thor Exp $
**
*/

/// Includes
#include "codestream/aclosslessscan.hpp"
#include "io/bytestream.hpp"
#include "control/linebuffer.hpp"
#include "control/linebitmaprequester.hpp"
#include "control/lineadapter.hpp"
#include "marker/frame.hpp"
#include "marker/scan.hpp"
#include "marker/component.hpp"
#include "codestream/tables.hpp"
#include "coding/qmcoder.hpp"
#include "coding/actemplate.hpp"
#include "codestream/tables.hpp"
#include "codestream/predictorbase.hpp"
#include "tools/line.hpp"
#include "std/string.hpp"
///

/// ACLosslessScan::ACLosslessScan
ACLosslessScan::ACLosslessScan(class Frame *frame,class Scan *scan,UBYTE predictor,UBYTE lowbit,bool differential)
  : PredictiveScan(frame,scan,predictor,lowbit,differential)
{ 
}
///

/// ACLosslessScan::~ACLosslessScan
ACLosslessScan::~ACLosslessScan(void)
{
}
///

/// ACLosslessScan::WriteFrameType
// Write the marker that indicates the frame type fitting to this scan.
void ACLosslessScan::WriteFrameType(class ByteStream *io)
{
  NOREF(io);
}
///

/// ACLosslessScan::FindComponentDimensions
// Common setup for encoding and decoding.
///

/// ACLosslessScan::StartParseScan
void ACLosslessScan::StartParseScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl)
{
  NOREF(io);
  NOREF(chk);
  NOREF(ctrl);
  JPG_THROW(NOT_IMPLEMENTED,"ACLosslessScan::StartParseScan",
            "JPEG lossless not available your code release, please contact Accusoft for a full version");
}
///

/// ACLosslessScan::StartWriteScan
void ACLosslessScan::StartWriteScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl)
{
  NOREF(io);
  NOREF(chk);
  NOREF(ctrl);
  JPG_THROW(NOT_IMPLEMENTED,"ACLosslessScan::StartWriteScan",
            "JPEG lossless not available your code release, please contact Accusoft for a full version");
}
///

/// ACLosslessScan::StartMeasureScan
void ACLosslessScan::StartMeasureScan(class BufferCtrl *)
{
  JPG_THROW(NOT_IMPLEMENTED,"ACLosslessScan::StartMeasureScan",
            "arithmetic coding is always adaptive and does not require a measurement phase");
}
///

/// ACLosslessScan::WriteMCU
// This is actually the true MCU-writer, not the interface that reads
// a full line.
void ACLosslessScan::WriteMCU(struct Line **prev,struct Line **top)
{ 
  NOREF(prev);
  NOREF(top);
}
///

/// ACLosslessScan::ParseMCU
// The actual MCU-parser, write a single group of pixels to the stream,
// or measure their statistics.
void ACLosslessScan::ParseMCU(struct Line **prev,struct Line **top)
{ 
  NOREF(prev);
  NOREF(top);
}
///

/// ACLosslessScan::WriteMCU
// Write a single MCU in this scan. Actually, this is not quite true,
// as we write an entire group of eight lines of pixels, as a MCU is
// here a group of pixels. But it is more practical this way.
bool ACLosslessScan::WriteMCU(void)
{
  return false;
}
///

/// ACLosslessScan::ParseMCU
// Parse a single MCU in this scan. Actually, this is not quite true,
// as we write an entire group of eight lines of pixels, as a MCU is
// here a group of pixels. But it is more practical this way.
bool ACLosslessScan::ParseMCU(void)
{
  return false; // no further blocks here.
}
///

/// ACLosslessScan::StartMCURow
// Start a MCU scan. Returns true if there are more rows.
bool ACLosslessScan::StartMCURow(void)
{
  return false;
}
///

/// ACLosslessScan::Flush
void ACLosslessScan::Flush(bool)
{
}
///

/// ACLosslessScan::Restart
// Restart the parser at the next restart interval
void ACLosslessScan::Restart(void)
{ 
}
///
