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
** This class is responsible for parsing the huffman specific part of the
** DHT marker and generating the corresponding decoder classes.
**
** $Id: huffmantemplate.hpp,v 1.18 2014/09/30 08:33:16 thor Exp $
**
*/

#ifndef CODING_HUFFMANTEMPLATE_HPP
#define CODING_HUFFMANTEMPLATE_HPP

/// Includes
#include "tools/environment.hpp"
#include "marker/scantypes.hpp"
///

/// Defines
//#define COLLECT_STATISTICS 1
///

/// Forwards
class HuffmanDecoder;
class HuffmanCoder;
class HuffmanStatistics;
///

/// class HuffmanTemplate
class HuffmanTemplate : public JKeeper {
  //
  // Number of huffman codes of length L_i.
  UBYTE                    m_ucLengths[16];
  //
  // Total number of codewords.
  ULONG                    m_ulCodewords;
  //
  // Array containing the huffman values for each code, ordered by code lengths.
  UBYTE                   *m_pucValues;
  //
  // The huffman encoder for this level.
  class HuffmanCoder      *m_pEncoder;
  //
  // The huffman decoder for this level.
  class HuffmanDecoder    *m_pDecoder;
  //
  // The huffman statistics collector, used to optimize the
  // huffman encoder.
  class HuffmanStatistics *m_pStatistics;
  //
#ifdef COLLECT_STATISTICS
  // The AC/DC switch. True for AC
  bool                     m_bAC;
  //
  // The chroma/luma switch. True for chroma.
  bool                     m_bChroma;
  //
#endif
  //
  // Reset the huffman table for an alphabet with N entries.
  void ResetEntries(ULONG count);
  //
  // Build the huffman encoder given the template data.
  void BuildEncoder(void);
  //
  // Build the huffman decoder given the template data.
  void BuildDecoder(void);
  //
  // Build the huffman statistics.
  void BuildStatistics(bool fordc);
  //
public:
  HuffmanTemplate(class Environ *env);
  //
  ~HuffmanTemplate(void);
  //
  // Parse the huffman specific part of the DHT table.
  void ParseMarker(class ByteStream *io);
  //
  // Write the huffman table stored here.
  void WriteMarker(class ByteStream *io);
  //
  // Return the space required to write this part of the marker
  UWORD MarkerOverhead(void) const;
  //
  // Default huffman code initializations. These methods just install
  // the default huffman tables found in the standard, other tables
  // might work as well, or should even perform better.
  //
  // Install the default Luminance DC default table.
  void InitDCLuminanceDefault(ScanType type,UBYTE depth,UBYTE hidden);
  //
  // Install the default Chrominance DC table.
  void InitDCChrominanceDefault(ScanType type,UBYTE depth,UBYTE hidden);
  //
  // Install the default Luminance AC default table.
  void InitACLuminanceDefault(ScanType type,UBYTE depth,UBYTE hidden);
  //
  // Install the default Chrominance AC table.
  void InitACChrominanceDefault(ScanType type,UBYTE depth,UBYTE hidden);
  //
  // Use the collected statistics to build an optimized
  // huffman table.
  void AdjustToStatistics(void);
  //
  // Return the decoder (chain).
  class HuffmanDecoder *DecoderOf(void)
  {
    if (m_pDecoder == NULL)
      BuildDecoder();
    return m_pDecoder;
  }
  //
  // Return the encoder.
  class HuffmanCoder *EncoderOf(void)
  {
    if (m_pEncoder == NULL)
      BuildEncoder();
    return m_pEncoder;
  }
  //
  // Return the statistics class.
  class HuffmanStatistics *StatisticsOf(bool fordc)
  {
    if (m_pStatistics == NULL)
      BuildStatistics(fordc);
    return m_pStatistics;
  }
};
///

///
#endif

  
