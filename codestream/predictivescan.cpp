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
** This is the base class for all predictive scan types, it provides the
** services useful to implement them such that the derived classes can
** focus on the actual algorithm.
**
** $Id: predictivescan.cpp,v 1.15 2016/10/28 13:58:53 thor Exp $
**
*/

/// Includes
#include "codestream/predictivescan.hpp"
#include "io/bytestream.hpp"
#include "marker/frame.hpp"
#include "marker/scan.hpp"
#include "marker/component.hpp"
#include "codestream/tables.hpp"
#include "codestream/predictorbase.hpp"
#include "tools/line.hpp"
///

/// PredictiveScan::PredictiveScan
PredictiveScan::PredictiveScan(class Frame *frame,class Scan *scan,UBYTE predictor,UBYTE lowbit,bool differential)
  : EntropyParser(frame,scan)
{ 
  NOREF(predictor);
  NOREF(lowbit);
  NOREF(differential);
}
///

/// PredictiveScan::~PredictiveScan
PredictiveScan::~PredictiveScan(void)
{
}
///

/// PredictiveScan::FindComponentDimensions
// Collect the component information.
void PredictiveScan::FindComponentDimensions(void)
{ 
}
///


/// PredictiveScan::ClearMCU
// Clear the entire MCU
void PredictiveScan::ClearMCU(struct Line **top)
{ 
  NOREF(top);
}
///

/// PredictiveScan::Flush
// Flush at the end of a restart interval
// when writing out code. Reset predictors, check
// for the correctness of the restart alignment.
void PredictiveScan::FlushOnMarker(void)
{
}
///

/// PredictiveScan::Restart
// Restart after reading a full restart interval,
// reset the predictors, check for the correctness
// of the restart interval.
void PredictiveScan::RestartOnMarker(void)
{
}
///

/// PredictiveScan::OptimizeBlock
// Make an R/D optimization for the given scan by potentially pushing
// coefficients into other bins. 
void PredictiveScan::OptimizeBlock(LONG, LONG ,UBYTE ,double ,
                                   class DCT *,LONG [64])
{
  JPG_THROW(NOT_IMPLEMENTED,"PredictiveScan::OptimizeBlock",
            "Rate-distortion optimization is not available for line-based coding modes");
}
///

/// PredictiveScan::OptimizeDC
// Make an R/D optimization for the given scan by potentially pushing
// coefficients into other bins. 
void PredictiveScan::OptimizeDC(void)
{
  JPG_THROW(NOT_IMPLEMENTED,"PredictiveScan::OptimizeDC",
            "Rate-distortion optimization is not available for line-based coding modes");
}
///

/// PredictiveScan::StartOptimizeScan
// Start making an optimization run to adjust the coefficients.
void PredictiveScan::StartOptimizeScan(class BufferCtrl *)
{  
  JPG_THROW(NOT_IMPLEMENTED,"PredictiveScan::StartOptimizeScan",
            "Line-based modes do not support R/D optimization");
}
///
