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
** This class represents the interface for parsing the
** entropy coded data in JPEG as part of a single scan.
**
** $Id: entropyparser.hpp,v 1.21 2016/10/28 13:58:53 thor Exp $
**
*/


#ifndef CODESTREAM_ENTROPYPARSER_HPP
#define CODESTREAM_ENTROPYPARSER_HPP

/// Includes
#include "tools/environment.hpp"
#include "tools/rectangle.hpp"
#include "interface/imagebitmap.hpp"
#include "std/assert.hpp"
///

/// Forwards
class ByteStream;
class Scan;
class Frame;
class BitmapCtrl;
class LineAdapter;
class BufferCtrl;
class Checksum;
///

/// class EntropyParser
// This class represents the interface for parsing the
// entropy coded data in JPEG as part of a single scan.
class EntropyParser : public JKeeper {
  // 
  // The restart interval in MCUs
  UWORD                 m_usRestartInterval;
  //
  // The next restart marker expected or to be written.
  UWORD                 m_usNextRestartMarker;
  //
  // Number of MCUs to be handled before the next MCU
  // is to be written. If this becomes zero before a MCU
  // the restart marker will be written.
  UWORD                 m_usMCUsToGo;
  //
  // Boolean indicator whether the next entropy coded segment
  // up to the next restart marker or SOF/SOS is valid. If true,
  // continue parsing. Otherwise, replace just be zero/grey.
  bool                  m_bSegmentIsValid;
  //
  // Must scan for the DNL marker because the frame does not 
  // know our size.
  bool                  m_bScanForDNL;
  //
  // Set if parsing has come to an halt because DNL has been hit.
  bool                  m_bDNLFound;
  //
  // Flush the entropy coder, write the restart marker and
  // restart the MCU counter.
  void WriteRestartMarker(class ByteStream *io);
  //
  // Parse the restart marker or resync at the restart marker.
  void ParseRestartMarker(class ByteStream *io);
  //
  // Parse the DNL marker, update the frame height. If the
  // return is true, the DNL marker has been found.
  bool ParseDNLMarker(class ByteStream *io);
  //
protected:
  //
  // The scan this is part of.
  class Scan           *m_pScan;
  //
  // The frame we are part of.
  class Frame          *m_pFrame;
  //
  // The components in the scan - at most four.
  class Component      *m_pComponent[4];
   //
  // The number of components we have here.
  UBYTE                 m_ucCount;
  //
  // Create a new parser.
  EntropyParser(class Frame *frame,class Scan *scan);
  //
  // Return the number of fractional bits due to color
  // transformation.
  UBYTE FractionalColorBitsOf(void) const;
  //
  // Start writing a new MCU. Potentially, a restart marker
  // is emitted.
  void BeginWriteMCU(class ByteStream *io)
  {
    if (m_usRestartInterval) {
      if (m_usMCUsToGo == 0) {
        WriteRestartMarker(io);
      }
      m_usMCUsToGo--;
    }
  }
  //
  // Start parsing/reading a MCU. Might expect a restart marker.
  // If so, Restart() (below) is called. Returns true if the next MCU
  // is valid, or false if the parser should replace the next MCU with
  // grey.
  bool BeginReadMCU(class ByteStream *io)
  {   
    if (m_bScanForDNL) {
      if (ParseDNLMarker(io))
        return false;
    }
    if (m_usRestartInterval) {
      if (m_usMCUsToGo == 0) {
        ParseRestartMarker(io);
      }
      m_usMCUsToGo--;
    } 
    return m_bSegmentIsValid;
  }
  //
  // Return if the DNL marker has recently been found.
  bool hasFoundDNL(void) const
  {
    return m_bDNLFound;
  }
  //
public:
  //
  virtual ~EntropyParser(void);
  //
  // Flush the remaining bits out to the stream on writing.
  virtual void Flush(bool final) = 0;
  //
  // Restart the statistics/prediction at the next restart marker on reading.
  virtual void Restart(void) = 0;
  //
  // Return the i'th component of the scan.
  class Component *ComponentOf(UBYTE i)
  {
    assert(i < 4);
    return m_pComponent[i];
  }
  //
  // Return the number of the components in the scan.
  UBYTE ComponentsInScan(void) const
  {
    return m_ucCount;
  }
  //
  // Parse the marker contents.
  virtual void StartParseScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl) = 0;
  //
  // Write the marker to the stream.
  virtual void StartWriteScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl) = 0;
  //
  // Write the marker that indicates the frame type fitting to this scan.
  virtual void WriteFrameType(class ByteStream *io) = 0;
  //
  // Start making a measurement run to optimize the
  // huffman tables.
  virtual void StartMeasureScan(class BufferCtrl *) = 0;
  //
  // Start making an optimization run to adjust the coefficients.
  virtual void StartOptimizeScan(class BufferCtrl *) = 0;
  //
  // Start a MCU scan.
  virtual bool StartMCURow(void) = 0;  
  //
  // Parse a single MCU in this scan.
  virtual bool ParseMCU(void) = 0;
  //
  // Write a single MCU in this scan.
  virtual bool WriteMCU(void) = 0; 
  //
  // Make an R/D optimization for the given scan by potentially pushing
  // coefficients into other bins. This runs an optimization for a single
  // block and requires external control to run over the blocks.
  // component is the component, critical is the critical slope for
  // the R/D optimization of the functional J = \lambda D + R, i.e.
  // this is lambda.
  // Quant are the quantization parameters, i.e. deltas. These are eventually
  // preshifted by "preshift".
  // transformed are the dct-transformed but unquantized data. These are also pre-
  // shifted by "preshift".
  // quantized is the quantized data. These are potentially (and likely) adjusted.
  virtual void OptimizeBlock(LONG bx,LONG by,UBYTE component,double critical,
                             class DCT *dct,LONG quantized[64]) = 0;  
  //
  // Make an R/D optimization of the DC scan. This includes all DC blocks in
  // total, not just a single block. This is because the coefficients are not
  // coded independently.
  virtual void OptimizeDC(void) = 0;
};
///

///
#endif
