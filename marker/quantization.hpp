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
** This class represents the quantization tables.
**
** $Id: quantization.hpp,v 1.15 2016/10/28 13:58:54 thor Exp $
**
*/

#ifndef MARKER_QUANTIZATION_HPP
#define MARKER_QUANTIZATION_HPP

/// Includes
#include "tools/environment.hpp"
///

/// Forwards
class ByteStream;
class QuantizationTable;
///

/// class Quantization
// This class describes the quantization tables for lossless JPEG coding.
class Quantization : public JKeeper {
  //
  // The actual quantization tables. This marker can hold up to four of them.
  class QuantizationTable *m_pTables[4];
  //
public:
  Quantization(class Environ *env);
  //
  ~Quantization(void);
  //
  // Write the DQT marker to the stream.
  void WriteMarker(class ByteStream *io);
  //
  // Parse off the contents of the DQT marker
  void ParseMarker(class ByteStream *io);
  //
  // Initialize the quantization table to the standard example
  // tables for quality q, q=0...100
  // If "addresidual" is set, additional quantization tables for 
  // residual coding are added into the legacy quantization matrix.
  // If "foresidual" is set, the quantization table is for the residual
  // codestream, using the hdrquality parameter (with known ldr parameters)
  // but injected into the residual codestream.
  // If "rct" is set, the residual color transformation is the RCT which
  // creates one additional bit of precision for lossless. In lossy modes,
  // this bit can be stripped off.
  // The table selector argument specifies which of the build-in
  // quantization table to use. CUSTOM is then a pointer to a custom
  // table if the table selector is custom.
  void InitDefaultTables(UBYTE quality,UBYTE hdrquality,bool colortrafo,
                         bool addresidual,bool forresidual,bool rct,
                         LONG tableselector,
                         const LONG customluma[64],
                         const LONG customchroma[64]);
  //
  class QuantizationTable *QuantizationTable(UBYTE idx) const
  {
    assert(idx < 4);
    return m_pTables[idx];
  }
};
///

///
#endif