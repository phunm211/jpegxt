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
** This class contains and maintains the huffman code parsers.
**
** $Id: huffmantable.hpp,v 1.16 2015/09/17 11:20:35 thor Exp $
**
*/

#ifndef MARKER_HUFFMANTABLE_HPP
#define MARKER_HUFFMANTABLE_HPP

/// Includes
#include "tools/environment.hpp"
#include "marker/scantypes.hpp"
///

/// Forwards
class ByteStream;
class DecderTemplate;
///

/// HuffmanTable
class HuffmanTable : public JKeeper {
  //
  // Table specification. 4 DC and 4 AC tables.
  class HuffmanTemplate *m_pCoder[8];
  //
public:
  HuffmanTable(class Environ *env);
  //
  ~HuffmanTable(void);
  //
  // Check whether the tables are empty or not
  // In such a case, do not write the tables.
  bool isEmpty(void) const;
  //
  // Write the marker contents to a DHT marker.
  void WriteMarker(class ByteStream *io);
  //
  // Parse the marker contents of a DHT marker.
  void ParseMarker(class ByteStream *io);
  //
  // Get the template for the indicated DC table or NULL if it doesn't exist.
  class HuffmanTemplate *DCTemplateOf(UBYTE idx,ScanType type,UBYTE depth,UBYTE hidden);
  //
  // Get the template for the indicated AC table or NULL if it doesn't exist.
  class HuffmanTemplate *ACTemplateOf(UBYTE idx,ScanType type,UBYTE depth,UBYTE hidden);
  //
  // Adjust all coders in here to the statistics collected before, i.e.
  // find optimal codes.
  void AdjustToStatistics(void);
};
///

///
#endif
