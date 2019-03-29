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
** Represents all data in a single scan, and hence is the SOS marker.
**
** $Id: scan.hpp,v 1.65 2016/10/28 13:58:54 thor Exp $
**
*/


#ifndef MARKER_SCAN_HPP
#define MARKER_SCAN_HPP

/// Includes
#include "tools/environment.hpp"
#include "tools/rectangle.hpp"
#include "interface/imagebitmap.hpp"
#include "marker/scantypes.hpp"
#include "std/assert.hpp"
///

/// Forwards
class ByteStream;
class Component;
class Frame;
class Tables;
class QuantizedRow;
struct ImageBitMap;
class BitMapHook;
struct RectangleRequest;
class UpsamplerBase;
class DownsamplerBase;
class BitmapCtrl;
class BufferCtrl;
class LineAdapter;
class EntropyParser;
class Checksum;
///

/// class Scan
// This class implements the scan header.
class Scan : public JKeeper {
  //
  // Next scan in line, potentially covering more
  // components.
  class Scan            *m_pNext;
  //
  // Frame this scan is part of.
  class Frame           *m_pFrame;
  //
  // The codestream parser that interprets the entropy coded
  // data. Not done here.
  class EntropyParser   *m_pParser;
  // 
  // Scans may have private AC coding tables that adapt
  // to the statistics of the components within. If so,
  // such tables are here. These are not used on decoding
  // where tables come from the global "tables".
  //
  // The huffman table.
  class HuffmanTable    *m_pHuffman;
  //
  // The AC table.
  class ACTable         *m_pConditioner;
  //
  // Number of the components in the scan.
  UBYTE                  m_ucCount;
  //
  // Components selected for the scan, there are as many as
  // indicated above.
  UBYTE                  m_ucComponent[4];
  //
  // The DC coding table selector.
  UBYTE                  m_ucDCTable[4];
  //
  // The AC coding table selector.
  UBYTE                  m_ucACTable[4];
  //
  // Spectral coding selector, start of scan.
  // Also the NEAR value for JPEG-LS
  UBYTE                  m_ucScanStart;
  //
  // Spectral coding selector, end of scan.
  // Also the interleaving value for JPEG-LS.
  UBYTE                  m_ucScanStop;
  //
  // Start approximation high bit position.
  UBYTE                  m_ucHighBit;
  //
  // End of approximation low bit position.
  // Also the point transformation.
  UBYTE                  m_ucLowBit;
  //
  // Number of hidden bits not included in the low bit count.
  UBYTE                  m_ucHiddenBits;
  //
  // Set if this scan is a hidden scan and goes into a 
  // side channel.
  bool                   m_bHidden;
  //
  // Mapping table selector for JPEG_LS
  UBYTE                  m_ucMappingTable[4];
  //
  // Component pointers
  class Component       *m_pComponent[4];
  // 
  // Create a suitable parser given the scan type as indicated in the
  // header and the contents of the marker. The parser is kept
  // here as it is local to the scan.
  void CreateParser(void);
  //
public:
  //
  Scan(class Frame *frame);
  //
  virtual ~Scan(void);
  //
  // Flush the remaining bits out to the stream on writing.
  void Flush(void);
  //
  // Return the next scan found here.
  class Scan *NextOf(void) const
  {
    return m_pNext;
  }
  //
  // Tag on a next scan to this scan.
  void TagOn(class Scan *next)
  {
    assert(m_pNext == NULL);
    m_pNext = next;
  }
  //
  // Return the i'th component of the scan.
  class Component *ComponentOf(UBYTE i);
  //
  // Return the number of the components in the scan.
  UBYTE ComponentsInScan(void) const
  {
    return m_ucCount;
  }
  //
  // Check whether this scan is in a side channel and hidden
  // in an extra box included in an APP11 marker.
  bool isHidden(void) const
  {
    return m_bHidden;
  }
  //
  // Find the DC huffman table of the indicated index.
  class HuffmanTemplate *FindDCHuffmanTable(UBYTE idx) const;
  //
  // Find the AC huffman table of the indicated index.
  class HuffmanTemplate *FindACHuffmanTable(UBYTE idx) const;
  //
  // Find the AC conditioner table for the indicated index
  // and the DC band.
  class ACTemplate *FindDCConditioner(UBYTE idx) const;
  //
  // The same for the AC band.
  class ACTemplate *FindACConditioner(UBYTE idx) const;
  //
  // Find the thresholds of the JPEG LS scan.
  class Thresholds *FindThresholds(void) const;
  //
  // Write the scan type marker at the beginning of the
  // file.
  void WriteFrameType(class ByteStream *io);
  //
  // Parse the marker contents. The scan type comes from
  // the frame type.
  void ParseMarker(class ByteStream *io);
  //
  // Parse the marker contents where the scan type
  // comes from an additional parameter.
  void ParseMarker(class ByteStream *io,ScanType type);
  //
  // Write the marker to the stream. Note that this should
  // be called indirectly by the implementing interface of
  // the entropy parser and not called here from toplevel.
  void WriteMarker(class ByteStream *io);
  //
  // Install the defaults for a given scan type 
  // containing the given number of components.
  // The tag offset is added to the tag to offset them for the
  // residual coding tags.
  void InstallDefaults(UBYTE depth,ULONG tagoffset,const struct JPG_TagItem *tags);
  //
  // Make this scan a hidden refinement scan starting at the indicated
  // bit position in the indicated component label. If start and stop are
  // both zero to indicate a DC scan, all components are included and comp
  // may be NULL.
  void MakeHiddenRefinementScan(UBYTE bitposition,class Component *comp,UBYTE sstart,UBYTE sstop);
  //
  // Parse off a hidden refinement scan from the given position.
  void StartParseHiddenRefinementScan(class ByteStream *io,class BufferCtrl *ctrl);
  //
  // Fill in the decoding tables required.
  void StartParseScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl); 
  //
  // Fill in the encoding tables.
  void StartWriteScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl); 
  //
  // Start making a measurement run to optimize the
  // huffman tables.
  void StartMeasureScan(class BufferCtrl *ctrl);
  //
  // Start a rate/distortion optimization for scan on the given buffer.
  void StartOptimizeScan(class BufferCtrl *ctrl);
  //
  // Start a MCU scan.
  bool StartMCURow(void);  
  //
  // Parse a single MCU in this scan.
  bool ParseMCU(void);
  //
  // Write a single MCU in this scan.
  bool WriteMCU(void);
  //
  // Return the huffman decoder of the DC value for the
  // indicated component.
  class HuffmanDecoder *DCHuffmanDecoderOf(UBYTE idx) const;
  //
  // Return the huffman decoder of the DC value for the
  // indicated component.
  class HuffmanDecoder *ACHuffmanDecoderOf(UBYTE idx) const;
  //
  // Find the Huffman decoder of the indicated index.
  class HuffmanCoder *DCHuffmanCoderOf(UBYTE idx) const;
  //
  // Find the Huffman decoder of the indicated index.
  class HuffmanCoder *ACHuffmanCoderOf(UBYTE idx) const;
  //
  // Find the Huffman decoder of the indicated index.
  class HuffmanStatistics *DCHuffmanStatisticsOf(UBYTE idx) const;
  //
  // Find the Huffman decoder of the indicated index.
  class HuffmanStatistics *ACHuffmanStatisticsOf(UBYTE idx) const;
  //
  // Find the arithmetic coding conditioner table for the indicated
  // component and the DC band.
  class ACTemplate *DCConditionerOf(UBYTE idx) const;
  //
  // The same for the AC band.
  class ACTemplate *ACConditionerOf(UBYTE idx) const;
  //
  // Return the DC table of the conditioner.
  UBYTE DCTableIndexOf(UBYTE idx) const
  {
    assert(idx < 4);
    
    return m_ucDCTable[idx];
  } 
  //
  // Return the AC table of the conditioner.
  UBYTE ACTableIndexOf(UBYTE idx) const
  {
    assert(idx < 4);
    
    return m_ucACTable[idx];
  }
  //
  // Optimize the given DCT block for ideal rate-distortion performance. The
  // input parameters are the component this applies to, the critical R/D slope,
  // the original transformed but unquantized DCT data and the quantized DCT
  // block.
  void OptimizeDCTBlock(LONG bx,LONG by,UBYTE compidx,DOUBLE lambda,
                        class DCT *dct,LONG quantized[64]);
  //
  // Run a joint optimization of the R/D performance of all DC coefficients
  // within this scan. This requires a separate joint efford as DC coefficients
  // are encoded dependently.
  void OptimizeDC(void);
};
///

///
#endif
