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
** A subsequent (refinement) scan of a progressive scan.
**
** $Id: refinementscan.cpp,v 1.42 2016/10/28 13:58:53 thor Exp $
**
*/

/// Includes
#include "codestream/refinementscan.hpp"
#include "codestream/tables.hpp"
#include "marker/frame.hpp"
#include "marker/component.hpp"
#include "coding/huffmantemplate.hpp"
#include "coding/huffmancoder.hpp"
#include "coding/huffmandecoder.hpp"
#include "coding/huffmanstatistics.hpp"
#include "coding/quantizedrow.hpp"
#include "codestream/rectanglerequest.hpp"
#include "dct/dct.hpp"
#include "std/assert.hpp"
#include "interface/bitmaphook.hpp"
#include "interface/imagebitmap.hpp"
#include "colortrafo/colortrafo.hpp"
#include "tools/traits.hpp"
#include "control/blockbuffer.hpp"
#include "control/blockbitmaprequester.hpp"
#include "control/blocklineadapter.hpp"
///

/// RefinementScan::RefinementScan
RefinementScan::RefinementScan(class Frame *frame,class Scan *scan,
                               UBYTE start,UBYTE stop,UBYTE lowbit,UBYTE highbit,
                               bool,bool residual)
  : EntropyParser(frame,scan), m_ACBuffer(frame->EnvironOf(),256), m_pBlockCtrl(NULL),
    m_ucScanStart(start), m_ucScanStop(stop), m_ucLowBit(lowbit), m_ucHighBit(highbit),
    m_bResidual(residual)
{
  m_ucCount = scan->ComponentsInScan();

  for(int i = 0;i < 4;i++) {
    m_pACDecoder[i]    = NULL;
    m_pACCoder[i]      = NULL;
    m_pACStatistics[i] = NULL;
  }
  assert(m_ucHighBit == m_ucLowBit + 1);
}
///

/// RefinementScan::~RefinementScan
RefinementScan::~RefinementScan(void)
{
}
///

/// RefinementScan::StartParseScan
void RefinementScan::StartParseScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl)
{ 
  int i;

  //
  // A DC coder for the huffman part is not required.
  for(i = 0;i < m_ucCount;i++) {
    if (m_ucScanStop) {
      m_pACDecoder[i]  = m_pScan->ACHuffmanDecoderOf(i);
    } else {
      m_pACDecoder[i]  = NULL; // not required, is DC only.
    }
    m_ulX[i]           = 0;
    m_usSkip[i]        = 0;
  }

  assert(!ctrl->isLineBased());
  m_pBlockCtrl = dynamic_cast<BlockCtrl *>(ctrl);
  m_pBlockCtrl->ResetToStartOfScan(m_pScan);

  m_Stream.OpenForRead(io,chk);
}
///

/// RefinementScan::StartWriteScan
void RefinementScan::StartWriteScan(class ByteStream *io,class Checksum *chk,class BufferCtrl *ctrl)
{ 
  int i;
 
  for(i = 0;i < m_ucCount;i++) { 
    if (m_ucScanStop) {
      m_pACCoder[i]    = m_pScan->ACHuffmanCoderOf(i);
    } else {
      m_pACCoder[i]    = NULL;
    }
    m_pACStatistics[i] = NULL;
    m_ulX[i]           = 0;
    m_usSkip[i]        = 0;
  }
  m_bMeasure = false;
  
  assert(!ctrl->isLineBased());
  m_pBlockCtrl = dynamic_cast<BlockCtrl *>(ctrl);
  m_pBlockCtrl->ResetToStartOfScan(m_pScan);

  EntropyParser::StartWriteScan(io,chk,ctrl);
  
  m_pScan->WriteMarker(io);
  m_Stream.OpenForWrite(io,chk);
}
///

/// RefinementScan::StartMeasureScan
// Measure scan statistics.
void RefinementScan::StartMeasureScan(class BufferCtrl *ctrl)
{ 
  int i;

  for(i = 0;i < m_ucCount;i++) { 
    m_pACCoder[i]        = NULL;
    if (m_ucScanStop) {
      m_pACStatistics[i] = m_pScan->ACHuffmanStatisticsOf(i);
    } else {
      m_pACStatistics[i] = NULL;
    }
    m_ulX[i]             = 0;
    m_usSkip[i]          = 0;
  }
  m_bMeasure = true;

  assert(!ctrl->isLineBased());
  m_pBlockCtrl = dynamic_cast<BlockCtrl *>(ctrl);
  m_pBlockCtrl->ResetToStartOfScan(m_pScan);

  EntropyParser::StartWriteScan(NULL,NULL,ctrl);

  m_Stream.OpenForWrite(NULL,NULL);
}
///

/// RefinementScan::StartMCURow
// Start a MCU scan. Returns true if there are more rows.
bool RefinementScan::StartMCURow(void)
{
  bool more = m_pBlockCtrl->StartMCUQuantizerRow(m_pScan);

  for(int i = 0;i < m_ucCount;i++) {
    m_ulX[i]   = 0;
  }

  return more;
}
///

/// RefinementScan::Flush
// Flush the remaining bits out to the stream on writing.
void RefinementScan::Flush(bool)
{
  if (m_ucScanStart || m_bResidual) {
    // Progressive, AC band. It looks wierd to code the remaining
    // block skips right here. However, AC bands in spectral selection
    // are always coded in isolated scans, thus only one component
    // per scan and no interleaving. Hence, no problem.
    assert(m_ucCount == 1);
    if (m_usSkip[0]) {
      // Flush out any pending block
      if (m_pACStatistics[0]) { // only count.
        UBYTE symbol = 0;
        while(m_usSkip[0] >= (1L << symbol))
          symbol++;
        m_pACStatistics[0]->Put((symbol - 1) << 4);
        m_usSkip[0] = 0;
      } else {
        CodeBlockSkip(m_pACCoder[0],m_usSkip[0]);
      }
    }
  }
  if (!m_bMeasure)
    m_Stream.Flush();
} 
///

/// RefinementScan::Restart
// Restart the parser at the next restart interval
void RefinementScan::Restart(void)
{
  for(int i = 0; i < m_ucCount;i++) {
    m_usSkip[i]        = 0;
  }

  m_Stream.OpenForRead(m_Stream.ByteStreamOf(),m_Stream.ChecksumOf());
}
///

/// RefinementScan::WriteMCU
// Write a single MCU in this scan. Return true if there are more blocks in this row.
bool RefinementScan::WriteMCU(void)
{ 
  bool more = true;
  int c;

  assert(m_pBlockCtrl);
  
  BeginWriteMCU(m_bMeasure?NULL:m_Stream.ByteStreamOf());

  for(c = 0;c < m_ucCount;c++) {
    class Component *comp           = m_pComponent[c];
    class QuantizedRow *q           = m_pBlockCtrl->CurrentQuantizedRow(comp->IndexOf());
    class HuffmanCoder *ac          = m_pACCoder[c];
    class HuffmanStatistics *acstat = m_pACStatistics[c];
    UWORD &skip                     = m_usSkip[c];
    UBYTE mcux                      = (m_ucCount > 1)?(comp->MCUWidthOf() ):(1);
    UBYTE mcuy                      = (m_ucCount > 1)?(comp->MCUHeightOf()):(1);
    ULONG xmin                      = m_ulX[c];
    ULONG xmax                      = xmin + mcux;
    ULONG x,y; 
    if (xmax >= q->WidthOf()) {
      more     = false;
    }
    for(y = 0;y < mcuy;y++) {
      for(x = xmin;x < xmax;x++) {
        LONG *block,dummy[64];
        if (q && x < q->WidthOf()) {
          block  = q->BlockAt(x)->m_Data;
        } else {
          block  = dummy;
          memset(dummy ,0,sizeof(dummy) );
        }
        if (m_bMeasure) {
          MeasureBlock(block,acstat,skip);
        } else {
          EncodeBlock(block,ac,skip);
        }
      }
      if (q) q = q->NextOf();
    }
    // Done with this component, advance the block.
    m_ulX[c] = xmax;
  }

  return more;
}
///

/// RefinementScan::ParseMCU
// Parse a single MCU in this scan. Return true if there are more blocks in this row.
bool RefinementScan::ParseMCU(void)
{
  bool more = true;
  int c;

  assert(m_pBlockCtrl);

  bool valid = BeginReadMCU(m_Stream.ByteStreamOf());
  
  for(c = 0;c < m_ucCount;c++) {
    class Component *comp    = m_pComponent[c];
    class QuantizedRow *q    = m_pBlockCtrl->CurrentQuantizedRow(comp->IndexOf());
    class HuffmanDecoder *ac = m_pACDecoder[c];
    UWORD &skip              = m_usSkip[c];
    UBYTE mcux               = (m_ucCount > 1)?(comp->MCUWidthOf() ):(1);
    UBYTE mcuy               = (m_ucCount > 1)?(comp->MCUHeightOf()):(1);
    ULONG xmin               = m_ulX[c];
    ULONG xmax               = xmin + mcux;
    ULONG x,y;
    if (xmax >= q->WidthOf()) {
      more     = false;
    }
    for(y = 0;y < mcuy;y++) {
      for(x = xmin;x < xmax;x++) {
        LONG *block,dummy[64];
        if (q && x < q->WidthOf()) {
          block  = q->BlockAt(x)->m_Data;
        } else {
          block  = dummy;
        }
        if (valid) {
          DecodeBlock(block,ac,skip);
        } 
        // Do not modify the data in here otherwise, keep the data unrefined...
        // actually, all further refinement scans should better be skipped as the
        // data is likely nonsense anyhow.
      }
      if (q) q = q->NextOf();
    }
    // Done with this component, advance the block.
    m_ulX[c] = xmax;
  }

  return more;
}
///

/// RefinementScan::MeasureBlock
// Make a block statistics measurement on the source data.
void RefinementScan::MeasureBlock(const LONG *block,
                                  class HuffmanStatistics *ac,
                                  UWORD &skip)
{ 
  bool relevant = false;
  // DC coding does not undergo huffman coding and hence
  // does not change anything.
  // AC coding
  if (m_ucScanStop || m_bResidual) {
    UBYTE run = 0;
    int k = m_ucScanStart;

    //
    // Must be separate from the DC coding.
    assert(m_ucScanStart || m_bResidual);
    
    do {
      LONG prev,data = block[DCT::ScanOrder[k]]; 
      // Check whether this is refinement coding or a a new coefficient.
      // Refinement codes are not huffman coded and hence not included
      // here.
      prev = (data >= 0)?(data >> m_ucHighBit):(-((-data) >> m_ucHighBit));
      if (prev == 0) {
        // Not refinement coding. Huffman codes are used.
        data = (data >= 0)?(data >> m_ucLowBit):(-((-data) >> m_ucLowBit));
        if (data == 0) {
          run++;
        } else {
          // Code (or compute the length of) any missing zero run.
          if (skip) {
            UBYTE sksymbol = 0;
            while(skip >= (1L << sksymbol))
              sksymbol++;
            ac->Put((sksymbol - 1) << 4);
            skip = 0;
          }
          // First ensure that the run is at most 15, the largest cathegory.
          while(run > 15) {
            ac->Put(0xf0); // r = 15 and s = 0
            run -= 16;
          }
          ac->Put(1 | (run << 4));
          // Refinement data is not Huffman coded, ignore.
          // Run is over.
          run      = 0;
          // significant coefficients would have been coded here.
          relevant = false;
        }
      } else {
        relevant = true; // some relevant coefficients have been skipped
      }
    } while(++k <= m_ucScanStop);
    
    // Is there still an open run? If so, code an EOB.
    if (run || relevant) {
      skip++;
      if (skip == MAX_WORD) {
        ac->Put(0xe0); // symbol for maximum length
        skip = 0;
      }
    }
  }
}
///


/// RefinementScan::CodeBlockSkip
// Code any run of zero blocks here. This is only valid in
// the progressive mode.
void RefinementScan::CodeBlockSkip(class HuffmanCoder *ac,UWORD &skip)
{  
  if (skip) {
    UBYTE symbol = 0;
    do {
      symbol++;
      if (skip < (1L << symbol)) {
        symbol--;
        assert(symbol <= 14);
        ac->Put(&m_Stream,symbol << 4);
        if (symbol)
          m_Stream.Put(symbol,skip);
        skip = 0;
        break;
      }
    } while(true);
    //
    // Code any remaining AC refinement data.
    {
      LONG data;
      class MemoryStream readback(m_pEnviron,&m_ACBuffer,JPGFLAG_OFFSET_BEGINNING);
      //
      while((data = readback.Get()) != ByteStream::EOF) {
        // Only the magnitude counts.
        m_Stream.Put<1>(data);
      }
      m_ACBuffer.Clean();
    }
  }
}
///

/// RefinementScan::EncodeBlock
// Encode a single huffman block
void RefinementScan::EncodeBlock(const LONG *block,class HuffmanCoder *ac,UWORD &skip)
{
  UBYTE refinement[64],*br = refinement; // runlength refinement buffer.
  //
  // DC coding
  if (m_ucScanStart == 0 && m_bResidual == false) {
    // Symbols are not DPCM encoded, but the remaining bits are simply encoded 
    // in raw.
    m_Stream.Put<1>(block[0] >> m_ucLowBit);
  }
  
  // AC coding
  if (m_ucScanStop || m_bResidual) {
    UBYTE run = 0,group = 0;
    int k = m_ucScanStart;

    assert(m_ucScanStart || m_bResidual); // AC must be coded separately from DC

    do {
      LONG data = block[DCT::ScanOrder[k]];
      LONG prev; // Data in the previous scan, might be zero or non-zero
      // Implement the point transformation. This is here a division, not
      // a shift (rounding is different for negative numbers).
      prev = (data >= 0)?(data >> m_ucHighBit):(-((-data) >> m_ucHighBit));
      data = (data >= 0)?(data >> m_ucLowBit):(-((-data) >> m_ucLowBit));
      if (prev) {
        // This is a coefficient which was nonzero in the scan before and
        // hence only undergoes refinement coding. It is skipped for the
        // purpose of runlength coding. Interestingly, the refinement
        // coding is defined in a somewhat wierd way where the "correction"
        // bits are coded behind a run, but not necessarily behind the
        // "nearest" run. Instead, correction bits go beyond either the first
        // coefficient that becomes significant, or beyond the first run
        // that crosses a runlength of 16, unless that run is at the
        // end of the block. To avoid going through the block twice to
        // detect the latter condition, we need to keep track within
        // which block of run-16 runs a correction bit goes.
        // The original flowchart from G-7 would now flush out the remaining
        // runs that cross the 16-run boundary. Instead, we just increment
        // the group counter to keep track of them and reset the run to
        // what the described flowchart would have done.
        group += (run >> 4) << 1; // number of 16-wrap-arounds
        run   &= 0x0f; // remaining runs.
        //
        // Buffer the correction bit, plus the group where it belongs.
        *br++ = (data & 0x01) | group;
      } else {
        if (data == 0) {
          run++;
        } else {
          UBYTE *b = refinement;
          UBYTE  g = 0;
          // Are there any skipped blocks we still need to code? Since this
          // block is none of them. If so, this includes also all buffered
          // refinement bits that are part of the EOB pattern.
          if (skip)
            CodeBlockSkip(ac,skip);
          //
          // Run groups. These are complete groups of runs of 16 or more
          // zeros uninterrupted by refinement bits. First the runs are
          // encoded, and then the correction bits that belong to the
          // corresponding group. The last (current) group belongs
          // to the current run, but that is only coded with ZRL as
          // long as it is a run longer than 16. The rest goes then
          // into the combined symbol/run code that codes the 
          // coefficient that just became significant. If any significant
          // coefficients became refined during the last run-group,
          // they are coded as part of the >15 run, and not as part
          // of the symbol - which is exactly what G-7 says about this.
          //
          // First flush out data that would have been flushed before
          // in previous groups.
          while(g < group) {
            ac->Put(&m_Stream,0xf0); // ZRL, 16-run.
            // All correction bits that belong to the current group.
            while(b < br && (((*b^g) & (~0x01)) == 0)) {
              m_Stream.Put<1>(*b++);
            }
            // A full group, equivalent to a full run of 16.
            g   += 2;
          }
          // Now to the final group we are currently part of. This is
          // part of the "regular" coding and follows again precisely
          // flowchart G-7.
          assert(g == group);
          //
          // Now all remaining ZRL-runs. These should be all in the current
          // and final group. Note that these bits go into the run, and not
          // into the final residual run that is combined with the symbol.
          while(run > 15) {
            ac->Put(&m_Stream,0xf0); // ZRL, 16-run.
            while(b < br) {
              assert(((*b^g) & (~0x01)) == 0);
              m_Stream.Put<1>(*b++);
            }
            run -= 16;
          }
          // Now we have a non-zero coefficient that just became non-zero.
          // Since we're coding bitplanes, the coefficent can now only be +1 or -1.
          // Since we store the magnitude, it is +1.
          ac->Put(&m_Stream,1 | (run << 4));
          // Store the sign of the coefficient. Zero for negative.
          m_Stream.Put<1>((data >= 0)?1:0);
          // And send the last refinement bits ("correction bits") that are
          // part of the run in front of the coefficient. If there are any.
          // If the last run was longer than 16, there aren't.
          while(b < br) {
            assert(((*b^g) & (~0x01)) == 0);
            m_Stream.Put<1>(*b++);
          }
          //
          // All correction bits coded, run is over.
          br    = refinement;
          group = 0;
          run   = 0;
        }
      }
    } while(++k <= m_ucScanStop);

    //
    // Check whether any coefficients where skipped at the end of the block.
    // This happens either if run > 0, i.e. insignificant coefficients were
    // skipped, or if there is something in the buffer, i.e. significant
    // coefficients were skipped.
    if (run || br > refinement) {
      // If this is part of the (isolated) AC scan of the progressive JPEG,
      // check whether we could potentially accumulate this into a run of
      // zero blocks.
      skip++;
      // Append to the buffered bits. Which group the correction bits belong
      // to does here not matter anymore as they become all part of the EOB
      // coding and are then handled by the next block, or at the end of the
      // scan.
      m_ACBuffer.Write(refinement,br - refinement);
      if (skip == MAX_WORD) {
        // avoid an overflow, code now.
        CodeBlockSkip(ac,skip);
      }
    }
  }
}
///

/// RefinementScan::DecodeBlock
// Decode a single huffman block.
void RefinementScan::DecodeBlock(LONG *block,
                                 class HuffmanDecoder *ac,
                                 UWORD &skip)
{
  if (m_ucScanStart == 0 && m_bResidual == false) {
    UBYTE correction = m_Stream.Get<1>();
    // Simply append the bits from the scan, no further coding.
    block[0] |= correction << m_ucLowBit;
  }

  if (m_ucScanStop || m_bResidual) {
    int k = m_ucScanStart;
    UBYTE run = 0;
    LONG  s   = 0;      // significance available? Positive? Negative?
    //
    assert(m_ucScanStart || m_bResidual); // AC coding must be separate from DC coding.
    //
    if (skip > 0) {
      // The entire block is skipped, decode only the refinement bits.
      run = m_ucScanStop - m_ucScanStart + 1;
      skip--;  // Still blocks to skip
    } else {
      k--;
      goto start; // Not elegant, but efficient.
    }
    //
    do {
      LONG data;
      // Skip coefficients, but for those that were significant,
      // collect refinement bits which we know are available because
      // we are currently parsing off the trailer of either an EOB
      // or of a EOBx.
      if ((data = block[DCT::ScanOrder[k]])) {
        UBYTE correction = m_Stream.Get<1>();
        if (correction) {
          // Correction necessary. The direction depends on the
          // sign. We always correct "away from the origin".
          if (data > 0) {
            block[DCT::ScanOrder[k]] += 1L << m_ucLowBit;
          } else {
            block[DCT::ScanOrder[k]] -= 1L << m_ucLowBit;
          }
        }
      } else if (run) {
        // Still in the run.
        run--;
      } else {
        // Run ended, decode the coefficient that ends the run. If
        // we have a significant coefficient, decode it here. This
        // also covers the case of the last element of a ZRL run.
        // Then s is simply zero.
        block[DCT::ScanOrder[k]] = s << m_ucLowBit; 
        //
        // If this is the last coefficent, then there is nothing more to do
        // on this block.
        if (k == m_ucScanStop)
          break;
        //
        // Get the next run/amplitude pair. This can be either an EOBx symbol
        // for skipping this and the next x blocks, or a run16 symbol to skip
        // the next 16 blocks without any amplitude, or a true run/amplitude
        // pair.
      start:
        UBYTE r,rs;
        rs    = ac->Get(&m_Stream);
        r     = rs >> 4;
        s     = rs & 0x0f;
        if (s == 0) {
          // This is a pure skip without an amplitude pair.
          if (r == 15) {
            run   = r; // No typo, the 16th coefficient is s = 0.
          } else {
            // A progressive EOB run.
            skip  = 1 << r;
            if (r) skip |= m_Stream.Get(r);
            skip--; // this block is included in the count.
            run   = m_ucScanStop - k + 1; // Skip the rest of the block,
            // though not for the refinement bits.
          }
        } else {
          UBYTE sign;
          // A run/amplitude pair. Only +/-1 amplitudes may appear here.
          if (s != 1) {
            JPG_WARN(MALFORMED_STREAM,"RefinementScan::DecodeBlock",
                     "unexpected Huffman symbol in refinement coding, "
                     "must be a +/-1 amplitude");
            // Ok, to recover, do not refine here at all, we are out of sync
            // anyhow. Rather, leave the modification as local as possible
            // and decode as fast as possible so decoding will stop at the 
            // next restart marker.
            run = 0;
            s   = 0;
          } else {
            //
            sign = m_Stream.Get<1>();
            // Get the sign of the coefficient. Zero is for negative.
            if (sign == 0)
              s = -s;
            // Note that we cannot apply the bits itself now. Must skip the coefficients
            // handled here.
            run = r;
          }
        }
      }
    } while(++k <= m_ucScanStop);
  }
}
///

/// RefinementScan::WriteFrameType
// Write the marker that indicates the frame type fitting to this scan.
void RefinementScan::WriteFrameType(class ByteStream *io)
{
  // is progressive.
  if (m_bResidual) {
    io->PutWord(0xffb2);
  } else {
    io->PutWord(0xffc2);
  }
}
///

/// RefinementScan::OptimizeBlock
// Make an R/D optimization for the given scan by potentially pushing
// coefficients into other bins. 
void RefinementScan::OptimizeBlock(LONG,LONG,UBYTE,double,class DCT *,LONG [64])
{
  JPG_THROW(NOT_IMPLEMENTED,"RefinementScan::OptimizeBlock",
            "soft-threshold quantizer not implemented in this code version");
}
///

/// RefinementScan::OptimizeDC
// Make an R/D optimization of the DC scan. This includes all DC blocks in
// total, not just a single block. This is because the coefficients are not
// coded independently.
void RefinementScan::OptimizeDC(void)
{
  // There is really nothing to optimize here as the bitrate is constant, no
  // matter what the data actually is.
}
///

/// RefinementScan::StartOptimizeScan
// Start making an optimization run to adjust the coefficients.
void RefinementScan::StartOptimizeScan(class BufferCtrl *)
{  
  // Ditto.
}
///
