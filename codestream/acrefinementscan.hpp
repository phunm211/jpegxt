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
** Represents the scan including the scan header for the
** arithmetic coding procedure for follow-up refinement
** scans.
**
** $Id: acrefinementscan.hpp,v 1.25 2016/10/28 13:58:53 thor Exp $
**
*/

#ifndef CODESTREAM_ACREFINEMENTSCAN_HPP
#define CODESTREAM_ACREFINEMENTSCAN_HPP

/// Includes
#include "tools/environment.hpp"
#include "coding/qmcoder.hpp"
#include "coding/quantizedrow.hpp"
#include "codestream/entropyparser.hpp"
///

/// Forwards
class Tables;
class ByteStream;
class DCT;
class Frame;
struct RectangleRequest;
class BufferCtrl;
class BlockCtrl;
///

/// class ACRefinementScan
class ACRefinementScan : public EntropyParser {
  //
  //
  // Flush the remaining bits out to the stream on writing.
  virtual void Flush(bool final);
  //
  // Restart the parser at the next restart interval
  virtual void Restart(void);
  //
private:
  //
  // Write the marker that indicates the frame type fitting to this scan.
  virtual void WriteFrameType(class ByteStream *io);
  //
  //
public:
  // Create an AC coded refinement scan. The differential flag is always
  // ignored, so is the residual flag.
  ACRefinementScan(class Frame *frame,class Scan *scan,UBYTE start,UBYTE stop,
                   UBYTE lowbit,UBYTE highbit,
                   bool differential = false,bool residual = false);
  //
  ~ACRefinementScan(void);
  // 
  // Fill in the tables for decoding and decoding parameters in general.
  virtual void StartParseScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl);
  //
  // Write the default tables for encoding 
  virtual void StartWriteScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl);
  //
  // Measure scan statistics. Not implemented here since it is not
  // required. The AC coder is adaptive.
  virtual void StartMeasureScan(class BufferCtrl *ctrl);
  //
  // Start making an optimization run to adjust the coefficients.
  virtual void StartOptimizeScan(class BufferCtrl *ctrl);
  //
  // Start a MCU scan. Returns true if there are more rows. False otherwise.
  virtual bool StartMCURow(void);
  //
  // Parse a single MCU in this scan. Return true if there are more
  // MCUs in this row.
  virtual bool ParseMCU(void);  
  //
  // Write a single MCU in this scan.
  virtual bool WriteMCU(void);  
  //
  // Make an R/D optimization for the given scan by potentially pushing
  // coefficients into other bins. 
  virtual void OptimizeBlock(LONG bx,LONG by,UBYTE component,double critical,
                             class DCT *dct,LONG quantized[64]);
  //
  // Make an R/D optimization of the DC scan. This includes all DC blocks in
  // total, not just a single block. This is because the coefficients are not
  // coded independently.
  virtual void OptimizeDC(void);
};
///


///
#endif
