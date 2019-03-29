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
** $Id: acsequentialscan.cpp,v 1.51 2016/10/28 13:58:53 thor Exp $
**
*/

/// Includes
#include "codestream/acsequentialscan.hpp"
#include "codestream/tables.hpp"
#include "marker/frame.hpp"
#include "marker/scan.hpp"
#include "marker/component.hpp"
#include "coding/quantizedrow.hpp"
#include "codestream/rectanglerequest.hpp"
#include "dct/dct.hpp"
#include "std/assert.hpp"
#include "interface/bitmaphook.hpp"
#include "interface/imagebitmap.hpp"
#include "colortrafo/colortrafo.hpp"
#include "tools/traits.hpp"
#include "control/blockbitmaprequester.hpp"
#include "control/blockbuffer.hpp"
#include "control/blocklineadapter.hpp"
#include "coding/actemplate.hpp"
#include "marker/actable.hpp"
///

/// ACSequentialScan::ACSequentialScan
ACSequentialScan::ACSequentialScan(class Frame *frame,class Scan *scan,
                                   UBYTE start,UBYTE stop,UBYTE lowbit,UBYTE,
                                   bool differential,bool residual,bool large)
  : EntropyParser(frame,scan)
{
  NOREF(start);
  NOREF(stop);
  NOREF(lowbit);
  NOREF(differential);
  NOREF(residual);
  NOREF(large);
}
///

/// ACSequentialScan::~ACSequentialScan
ACSequentialScan::~ACSequentialScan(void)
{
}
///

/// ACSequentialScan::StartParseScan
void ACSequentialScan::StartParseScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl)
{ 
  NOREF(io);
  NOREF(chk);
  NOREF(ctrl);
  JPG_THROW(NOT_IMPLEMENTED,"ACSequentialScan::StartParseScan",
            "Lossless JPEG not available in your code release, please contact Accusoft for a full version");
}
///

/// ACSequentialScan::StartWriteScan
void ACSequentialScan::StartWriteScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl)
{ 
  NOREF(io);
  NOREF(chk);
  NOREF(ctrl);
  JPG_THROW(NOT_IMPLEMENTED,"ACSequentialScan::StartWriteScan",
            "Lossless JPEG not available in your code release, please contact Accusoft for a full version");
}
///

/// ACSequentialScan::StartMeasureScan
// Measure scan statistics.
void ACSequentialScan::StartMeasureScan(class BufferCtrl *)
{ 
  //
  // This is not required.
  JPG_THROW(NOT_IMPLEMENTED,"ACSequentialScan::StartMeasureScan",
            "arithmetic coding is always adaptive and does not require "
            "to measure the statistics");
}
///

/// ACSequentialScan::StartMCURow
// Start a MCU scan. Returns true if there are more rows.
bool ACSequentialScan::StartMCURow(void)
{
  return false;
}
///

/// ACSequentialScan::WriteMCU
// Write a single MCU in this scan. Return true if there are more blocks in this row.
bool ACSequentialScan::WriteMCU(void)
{ 
  return false;
}
///

/// ACSequentialScan::Restart
// Restart the parser at the next restart interval
void ACSequentialScan::Restart(void)
{
}
///

/// ACSequentialScan::ParseMCU
// Parse a single MCU in this scan. Return true if there are more blocks in this row.
bool ACSequentialScan::ParseMCU(void)
{
  return false;
}
///

/// ACSequentialScan::Classify
// Find the DC context class depending on the previous DC and
// the values of L and U given in the conditioner.
///

/// ACSequentialScan::EncodeBlock
// Encode a single block
///

/// ACSequentialScan::DecodeBlock
// Decode a single block.
///

/// ACSequentialScan::WriteFrameType
// Write the marker that indicates the frame type fitting to this scan.
void ACSequentialScan::WriteFrameType(class ByteStream *io)
{
  NOREF(io);
}
///

/// ACSequentialScan::Flush
// Flush the remaining bits out to the stream on writing.
void ACSequentialScan::Flush(bool)
{
}
///

/// ACSequentialScan::OptimizeBlock
// Make an R/D optimization for the given scan by potentially pushing
// coefficients into other bins. 
void ACSequentialScan::OptimizeBlock(LONG, LONG, UBYTE ,double ,
                                     class DCT *,LONG [64])
{
  JPG_THROW(NOT_IMPLEMENTED,"ACSequentialScan::OptimizeBlock",
            "Rate-distortion optimization is not implemented for arithmetic coding");
}
///

/// ACSequentialScan::OptimizeDC
// Make an R/D optimization for the given scan by potentially pushing
// coefficients into other bins. 
void ACSequentialScan::OptimizeDC(void)
{
  JPG_THROW(NOT_IMPLEMENTED,"ACSequentialScan::OptimizeDC",
            "Rate-distortion optimization is not implemented for arithmetic coding");
}
///

/// ACSequentialScan::StartOptimizeScan
// Start making an optimization run to adjust the coefficients.
void ACSequentialScan::StartOptimizeScan(class BufferCtrl *)
{  
  JPG_THROW(NOT_IMPLEMENTED,"ACSequentialScan::StartOptimizeScan",
            "Rate-distortion optimization is not implemented for arithmetic coding");
}
///
