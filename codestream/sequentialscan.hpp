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
** A sequential scan, also the first scan of a progressive scan,
** Huffman coded.
**
** $Id: sequentialscan.hpp,v 1.58 2016/10/28 13:58:53 thor Exp $
**
*/

#ifndef CODESTREAM_SEQUENTIALSCAN_HPP
#define CODESTREAM_SEQUENTIALSCAN_HPP

/// Includes
#include "tools/environment.hpp"
#include "marker/scan.hpp"
#include "io/bitstream.hpp"
#include "coding/quantizedrow.hpp"
#include "codestream/entropyparser.hpp"
///

/// Forwards
class HuffmanDecoder;
class HuffmanCoder;
class HuffmanStatistics;
class Tables;
class ByteStream;
class DCT;
class Frame;
struct RectangleRequest;
class BlockBuffer;
class BlockCtrl;
class BufferCtrl;
class LineAdapter;
class BitmapCtrl;
///

/// class SequentialScan
// A sequential scan, also the first scan of a progressive scan,
// Huffman coded.
class SequentialScan : public EntropyParser {
  //
  // Last DC value, required for the DPCM coder.
  LONG                     m_lDC[4];
  //
  // Number of blocks still to skip over.
  // This is only used in the progressive mode.
  UWORD                    m_usSkip[4];
  //
  // The bitstream from which we read the data.
  BitStream<false>         m_Stream;
  //
  // The block control helper that maintains all the request/release
  // logic and the interface to the user.
  class BlockCtrl         *m_pBlockCtrl; 
  //
  // The DC delta values for the DC optimizer.
  LONG                     m_lDCDelta[4];
  //
  // The critical R/D slope (aka \lambda) buffered for the DC optimizer.
  DOUBLE                   m_dCritical[4];
  //
  // Dimensions of the block array.
  ULONG                    m_ulBlockWidth[4];
  ULONG                    m_ulBlockHeight[4];
  //
  // Pointer to the DC buffers. This keeps the DC values for each
  // component to allow later optimization.
  LONG                    *m_plDCBuffer[4];
  // 
  // Scan positions.
  ULONG                    m_ulX[4];
  //
  // The huffman DC tables
  class HuffmanDecoder    *m_pDCDecoder[4];
  //
  // The huffman AC tables
  class HuffmanDecoder    *m_pACDecoder[4];
  //
  // Ditto for the encoder
  class HuffmanCoder      *m_pDCCoder[4];
  class HuffmanCoder      *m_pACCoder[4];
  //
  // Ditto for the statistics collection.
  class HuffmanStatistics *m_pDCStatistics[4];
  class HuffmanStatistics *m_pACStatistics[4];
  // Scan parameters.
  UBYTE                    m_ucScanStart;
  UBYTE                    m_ucScanStop;
  UBYTE                    m_ucLowBit;
  //
  // Measure data or encode?
  bool                     m_bMeasure;
  //
  // Encode a differential scan?
  bool                     m_bDifferential;
  //
  // Encode a residual scan?
  bool                     m_bResidual;
  //
  // Progressive mode?
  bool                     m_bProgressive;
  //
  // Large range DCT mode?
  bool                     m_bLargeRange;
  //
  // Encode a single huffman block
  void EncodeBlock(const LONG *block,
                   class HuffmanCoder *dc,class HuffmanCoder *ac,
                   LONG &prevdc,UWORD &skip);
  //
  // Decode a single huffman block.
  void DecodeBlock(LONG *block,
                   class HuffmanDecoder *dc,class HuffmanDecoder *ac,
                   LONG &prevdc,UWORD &skip);
  //
  // Flush the remaining bits out to the stream on writing.
  virtual void Flush(bool final);
  //
  // Restart the parser at the next restart interval
  virtual void Restart(void);
  //
  // Make a block statistics measurement on the source data.
  void MeasureBlock(const LONG *block,
                    class HuffmanStatistics *dc,class HuffmanStatistics *ac,
                    LONG &prevdc,UWORD &skip);
  //
  // Write the marker that indicates the frame type fitting to this scan.
  virtual void WriteFrameType(class ByteStream *io);
  //
  // Code any run of zero blocks here. This is only valid in
  // the progressive mode.
  void CodeBlockSkip(class HuffmanCoder *ac,UWORD &skip);
  // 
  //
public:
  // Create a sequential scan. The highbit is always ignored as this is
  // a valid setting for progressive only
  SequentialScan(class Frame *frame,class Scan *scan,UBYTE start,UBYTE stop,
                 UBYTE lowbit,UBYTE highbit,
                 bool differential = false,bool residual = false,bool largerange = false);
  //
  ~SequentialScan(void);
  // 
  // Fill in the tables for decoding and decoding parameters in general.
  virtual void StartParseScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl);
  //
  // Write the default tables for encoding 
  virtual void StartWriteScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl);
  //
  // Measure scan statistics.
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
                             class DCT *dct,
                             LONG quantized[64]);
  //
  // Make an R/D optimization of the DC scan. This includes all DC blocks in
  // total, not just a single block. This is because the coefficients are not
  // coded independently.
  virtual void OptimizeDC(void);
};
///


///
#endif
