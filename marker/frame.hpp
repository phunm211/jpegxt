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
** This class represents a single frame and the frame dimensions.
**
** $Id: frame.hpp,v 1.67 2017/02/21 15:48:21 thor Exp $
**
*/


#ifndef MARKER_FRAME_HPP
#define MARKER_FRAME_HPP

/// Includes
#include "tools/environment.hpp"
#include "marker/scantypes.hpp"
#include "boxes/databox.hpp"
///

/// Forwards
class ByteStream;
class Component;
class Tables;
class Scan;
class BitmapCtrl;
class LineAdapter;
class BufferCtrl;
class ResidualBlockHelper;
class Checksum;
class ChecksumAdapter;
class DCT;
///

/// class Frame
// This class represents a single frame and the frame dimensions.
class Frame : public JKeeper {
  // 
  // The image of this frame
  class Image           *m_pParent;
  //
  // In case this frame is part of a sequence of
  // hierarchical frames, this is the next larger
  // frame required to compose the full image.
  class Frame           *m_pNext;
  //
  // The tables of this frame, i.e. huffman and quantization tables.
  class Tables          *m_pTables;
  //
  // The scan pattern.
  class Scan            *m_pScan;
  //
  // The last scan.
  class Scan            *m_pLast;
  //
  // The currently active scan.
  class Scan            *m_pCurrent;
  //
  // The buffer this frame.
  class BufferCtrl      *m_pImage;
  //
  // Computes the residual data.
  class ResidualBlockHelper *m_pBlockHelper;
  //
  // The type of the frame encoding.
  ScanType               m_Type;
  //
  // width of the image in pixels.
  ULONG                  m_ulWidth; 
  //
  // height of the image in pixels.
  ULONG                  m_ulHeight;
  //
  // Sample precision in bits.
  UBYTE                  m_ucPrecision;
  //
  // Number of components.
  UBYTE                  m_ucDepth;
  //
  // Maximum MCU width and height. This data is required to compute
  // the subsampling factors.
  UBYTE                  m_ucMaxMCUWidth;
  UBYTE                  m_ucMaxMCUHeight;
  //
  // The definition of the components, the component array.
  class Component      **m_ppComponent;
  //
  // Currently active refinement data box.
  class DataBox         *m_pCurrentRefinement;
  //
  // The current adapter for updating the checksum over the
  // encoded data.
  class ChecksumAdapter *m_pAdapter;
  //
  // Indicate the height by the DNL marker?
  bool                   m_bWriteDNL;
  //
  // State flags for parsing. Make the next scan a refinement
  // scan even though there is no more data in the IO stream?
  bool                   m_bBuildRefinement;
  bool                   m_bCreatedRefinement;
  bool                   m_bEndOfFrame;
  bool                   m_bStartedTables;
  //
  // Counts the refinement scans.
  UWORD                  m_usRefinementCount;
  //
  // Compute the largest common denominator of a and b.
  static int gcd(int a,int b)
  {
    while(b) {
      int t  = a % b;
      a  = b;
      b  = t;
    }

    return a;
  }
  //
  // Define a component for writing. Must be called exactly once per component for encoding.
  // idx is the component index (not its label, which is generated automatically), and
  // the component subsampling factors. Must be called after installing precision and depth.
  class Component *DefineComponent(UBYTE idx,UBYTE subx = 1,UBYTE suby = 1);
  //
  // Compute the MCU sizes of the components from the subsampling values
  void ComputeMCUSizes(void);
  //
  // Start parsing a scan. Returns true if the scan start is found and there is another hidden
  // scan. Returns false otherwise.
  bool ScanForScanHeader(class ByteStream *stream);
  //
  // Attach a new scan to the frame, return the scan
  // and make this the current scan.
  class Scan *AttachScan(void);
  //
  // Helper function to create a regular scan from the tags.
  // There are no scan tags here, instead all components are included.
  // If breakup is set, then each component gets its own scan, otherwise
  // groups of four components get into one scan.
  void CreateSequentialScanParameters(bool breakup,ULONG tagoffset,const struct JPG_TagItem *tags);
  //
  // Helper function to create progressive scans. These need to be broken
  // up over several components. A progressive scan cannot contain more
  // than one component if it includes AC parameters.
  void CreateProgressiveScanParameters(bool breakup,ULONG tagoffset,const struct JPG_TagItem *tags,
                                       const struct JPG_TagItem *scantags);
  //
public:
  // This requires a type identifier.
  Frame(class Image *parent,class Tables *tables,ScanType t);
  //
  ~Frame(void);
  //
  // Return the image this frame is part of.
  class Image *ImageOf(void) const
  {
    return m_pParent;
  }
  //
  // Next frame in a sequence of hierarchical frames.
  class Frame *NextOf(void) const
  {
    return m_pNext;
  }
  //
  // Tag on a frame
  void TagOn(class Frame *next)
  {
    assert(m_pNext == NULL);
    m_pNext = next;
  }
  //
  // Set the image the frame data goes into. Required before the
  // user can call StartParse|Write|MeasureScan.
  void SetImageBuffer(class BufferCtrl *img)
  {
    m_pImage = img;
  }
  //
  // Return an indicator whether the end of a frame was reached.
  bool isEndOfFrame(void) const
  {
    return m_bEndOfFrame;
  }
  //
  // Extend the image by a merging process, and install it
  // here.
  void ExtendImageBuffer(class BufferCtrl *img,class Frame *residual);
  //
  // Parse off a frame header
  void ParseMarker(class ByteStream *io);
  //
  // Write the frame header
  void WriteMarker(class ByteStream *io);
  //
  // Find a component by a component identifier. Throws if the component does not exist.
  class Component *FindComponent(UBYTE id) const;
  //
  // Return the width of the frame in pixels.
  ULONG WidthOf(void) const
  {
    return m_ulWidth;
  }
  //
  // Return the height of the frame in pixels, or zero if it is still undefined.
  ULONG HeightOf(void) const
  {
    return m_ulHeight;
  }
  //
  // Return the number of components.
  UBYTE DepthOf(void) const
  {
    return m_ucDepth;
  }
  //
  // Return the precision in bits per sample.
  UBYTE PrecisionOf(void) const
  {
    return m_ucPrecision;
  }
  //
  // Return the precision including the hidden bits.
  UBYTE HiddenPrecisionOf(void) const;
  //
  // Return the point preshift, the adjustment of the
  // input samples by a shift that moves them into the
  // limits of JPEG. This is the R_b value from the specs.
  UBYTE PointPreShiftOf(void) const;
  //
  // Define default scan parameters. Returns the scan for further refinement if required.
  // tagoffset is an offset added to the tags - used to read from the residual scan types
  // rather the regular ones if this is a residual frame.
  class Scan *InstallDefaultParameters(ULONG width,ULONG height,UBYTE depth,UBYTE precision,
                                       bool writednl,
                                       const UBYTE *subx,const UBYTE *suby,
                                       ULONG tagoffset,
                                       const struct JPG_TagItem *tags);
  //
  // Start parsing a single scan. Could also create a checksum
  // in case the APP markers come late.
  class Scan *StartParseScan(class ByteStream *io,class Checksum *chk);
  //
  // Start writing a single scan. Scan parameters must have been installed before.
  class Scan *StartWriteScan(class ByteStream *io,class Checksum *chk);
  //
  // Start a measurement scan that can be added upfront to optimize the huffman
  // coder
  class Scan *StartMeasureScan(void);
  //
  // Start an optimization scan for the R/D optimizer.
  class Scan *StartOptimizeScan(void);
  //
  // End parsing the current scan.
  void EndParseScan(void);
  //
  // End writing the current scan
  void EndWriteScan(void);
  //
  // Return the scan.
  class Scan *FirstScanOf(void) const
  {
    return m_pScan;
  }
  //
  // Return the currently active scan.
  class Scan *CurrentScanOf(void) const
  {
    return m_pCurrent;
  }
  //
  // Advance the current frame to the next one,
  // returns it if there is a next one, NULL
  // otherwise if all scans are written.
  class Scan *NextScan(void);
  //
  // Reset the scan to the first in the image
  void ResetToFirstScan(void)
  {
    m_pCurrent = m_pScan;
  }
  //
  // The scan type of this frame, or rather the frame type.
  ScanType ScanTypeOf(void) const
  {
    return m_Type;
  }
  //
  // Return the settings tables of this frame.
  class Tables *TablesOf(void) const
  {
    return m_pTables;
  }
  //
  // Return the i'th component. Note that the argument
  // is here the component in the order they are defined
  // in the frame, not in the scan. The argument is not
  // a component ID but its relative index
  class Component *ComponentOf(UBYTE idx) const
  {
    assert(idx < m_ucDepth);
    return m_ppComponent[idx];
  }
  //
  // Write the marker that identifies this type of frame, and all the scans within it.
  void WriteFrameType(class ByteStream *io) const;
  //
  // Parse off the EOI marker at the end of the image. Return false
  // if there are no more scans in the file, true otherwise.
  bool ParseTrailer(class ByteStream *io);
  //
  // Build the line adapter fitting to the frame type.
  class LineAdapter *BuildLineAdapter(void);
  //
  // Build the image buffer type fitting to the frame type.
  class BitmapCtrl *BuildImageBuffer(void);
  //
  // Write the scan trailer of this frame. This is only the
  // DNL marker if it is enabled.
  void WriteTrailer(class ByteStream *io);
  //
  // Complete the current refinement scan if there is one.
  void CompleteRefimentScan(class ByteStream *io);
  //
  // Define the image size if it is not yet known here. This is
  // called whenever the DNL marker is parsed in.
  void PostImageHeight(ULONG height);
  //
  // Optimize a single DCT block through all scans of this frame for
  // ideal R/D performance.
  void OptimizeDCTBlock(LONG bx,LONG by,UBYTE compidx,class DCT *dct,LONG block[64]);
  //
};
///

///
#endif
