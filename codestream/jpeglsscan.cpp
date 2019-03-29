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
** A JPEG LS scan. This is the base for all JPEG LS scan types, namely
** separate, line interleaved and sample interleaved.
**
** $Id: jpeglsscan.cpp,v 1.25 2016/10/28 13:58:53 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "io/bitstream.hpp"
#include "codestream/entropyparser.hpp"
#include "codestream/jpeglsscan.hpp"
#include "codestream/tables.hpp"
#include "control/bufferctrl.hpp"
#include "control/linebuffer.hpp"
#include "marker/frame.hpp"
#include "marker/scan.hpp"
#include "marker/component.hpp"
#include "marker/thresholds.hpp"
#include "tools/line.hpp"
///

/// JPEGLSScan::m_lJ Runlength array
// The runlength J array. 
///
/// JPEGLSScan::JPEGLSScan
// Create a new scan. This is only the base type.
JPEGLSScan::JPEGLSScan(class Frame *frame,class Scan *scan,UBYTE near,const UBYTE *mapping,UBYTE point)
  : EntropyParser(frame,scan)
{
  NOREF(near);
  NOREF(mapping);
  NOREF(point);
}
///

/// JPEGLSScan::~JPEGLSScan
JPEGLSScan::~JPEGLSScan(void)
{ 
}
///

/// JPEGLSScan::FindComponentDimensions
// Collect the component information.
void JPEGLSScan::FindComponentDimensions(void)
{
}
///

/// JPEGLSScan::WriteFrameType
// Write the marker that indicates the frame type fitting to this scan.
void JPEGLSScan::WriteFrameType(class ByteStream *io)
{
  io->PutWord(0xfff7); // JPEG LS
}
///

/// JPEGLSScan::StartParseScan 
// Fill in the tables for decoding and decoding parameters in general.
void JPEGLSScan::StartParseScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl)
{
  NOREF(io);
  NOREF(chk);
  NOREF(ctrl);
  JPG_THROW(NOT_IMPLEMENTED,"JPEGLSScan::StartParseScan",
            "JPEG LS not available in your code release, please contact Accusoft for a full version");
}
///

/// JPEGLSScan::StartWriteScan
// Begin writing the scan data
void JPEGLSScan::StartWriteScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl)
{
  NOREF(io);
  NOREF(chk);
  NOREF(ctrl);
  JPG_THROW(NOT_IMPLEMENTED,"JPEGLSScan::StartWriteScan",
            "JPEG LS not available in your code release, please contact Accusoft for a full version");
}
///

/// JPEGLSScan::StartMeasureScan
// Start measuring the statistics. Since JPEG LS is not Huffman based,
// this need not to be implemented.
void JPEGLSScan::StartMeasureScan(class BufferCtrl *)
{
  JPG_THROW(NOT_IMPLEMENTED,"LosslessScan::StartMeasureScan",
            "JPEG LS is not based on Huffman coding and does not require a measurement phase");
}
///

/// JPEGLSScan::StartOptimizeScan
// Start making an optimization run to adjust the coefficients.
void JPEGLSScan::StartOptimizeScan(class BufferCtrl *)
{  
  JPG_THROW(NOT_IMPLEMENTED,"LosslessScan::StartOptimizeScan",
            "JPEG LS is not based on Huffman coding and does not support R/D optimization");
}
///

/// JPEGLSScan::StartMCURow
// Start a MCU scan. Returns true if there are more rows.
bool JPEGLSScan::StartMCURow(void)
{
  return false;
}
///

/// JPEGLSScan::Flush
// Flush the remaining bits out to the stream on writing.
void JPEGLSScan::Flush(bool)
{
}
///

/// JPEGLSScan::Restart
// Restart the parser at the next restart interval
void JPEGLSScan::Restart(void)
{
}
///

/// JPEGLSScan::BeginReadMCU
// Scanning for a restart marker is here a bit more tricky due to the
// presence of bitstuffing - the stuffed zero-bit need to be removed
// (and thus the byte containing it) before scanning for the restart
// marker.
bool JPEGLSScan::BeginReadMCU(class ByteStream *io)
{
  return EntropyParser::BeginReadMCU(io);
}
///

/// JPEGLSScan::OptimizeBlock
// Make an R/D optimization for the given scan by potentially pushing
// coefficients into other bins. 
void JPEGLSScan::OptimizeBlock(LONG, LONG, UBYTE ,double ,
                               class DCT *,LONG [64])
{
  JPG_THROW(NOT_IMPLEMENTED,"JPEGLSScan::OptimizeBlock",
            "Rate-distortion optimization is not available for line-based coding modes");
}
///

/// JPEGLSScan::OptimizeDC
// Make an R/D optimization of the DC scan. This includes all DC blocks in
// total, not just a single block. This is because the coefficients are not
// coded independently.
void JPEGLSScan::OptimizeDC(void)
{ 
  JPG_THROW(NOT_IMPLEMENTED,"JPEGLSScan::OptimizeDC",
            "Rate-distortion optimization is not available for line-based coding modes");
}
///
