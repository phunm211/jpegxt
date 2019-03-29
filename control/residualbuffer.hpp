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
** This class provides an access path to the residual image in the
** form consistent to the block buffer / buffer control class, except
** that all the regular accesses go to the residual part. It does not
** manage buffers itself, but requires a block bitmap requester as
** base to forward the requests to.
**
** $Id: residualbuffer.hpp,v 1.5 2016/10/28 13:58:53 thor Exp $
**
*/

#ifndef CONTROL_RESIDUALBUFFER_HPP
#define CONTROL_RESIDUALBUFFER_HPP

/// Includes
#include "control/bitmapctrl.hpp"
#include "control/bufferctrl.hpp"
#include "control/blockctrl.hpp"
#include "control/blockbitmaprequester.hpp"
///

/// class ResidualBuffer
// This class provides an access path to the residual image in the
// form consistent to the block buffer / buffer control class, except
// that all the regular accesses go to the residual part. It does not
// manage buffers itself, but requires a block bitmap requester as
// base to forward the requests to.
class ResidualBuffer : public BlockCtrl, public BufferCtrl {
  //
  // Pointer to the actual worker class that has the image.
  class BlockBitmapRequester *m_pParent;
  //
public:
  //
  // Construct the residual image. Requires the parent that keeps the actual buffer.
  ResidualBuffer(class BlockBitmapRequester *parent)
    : BlockCtrl(parent->EnvironOf()), BufferCtrl(parent->EnvironOf()), m_pParent(parent)
  { }
  //
  // Destructor: Nothing to destroy here.
  virtual ~ResidualBuffer(void)
  { }
  //
  //
  // Block buffer interface:
  //
  // Return true in case this buffer is organized in lines rather
  // than blocks.
  virtual bool isLineBased(void) const
  {
    return false;
  }
  //
  // First time usage: Collect all the information for encoding.
  // May throw on out of memory situations
  virtual void PrepareForEncoding(void)
  {
    m_pParent->PrepareForEncoding();
  }
  //
  // First time usage: Collect all the information for decoding.
  // May throw on out of memory situations.
  virtual void PrepareForDecoding(void)
  {
    m_pParent->PrepareForDecoding();
  }
  //
  // Indicate the frame height after the frame has already been
  // started. This call makes the necessary adjustments to handle
  // the DNL marker which appears only after the scan.
  virtual void PostImageHeight(ULONG height)
  {
    m_pParent->PostImageHeight(height);
  }
  //
  //
  // Return the current top MCU quantized line.
  virtual class QuantizedRow *CurrentQuantizedRow(UBYTE comp)
  {
    // On purpose, forward to the residual row.
    return m_pParent->CurrentResidualRow(comp);
  }  
  //
  // Start a MCU scan by initializing the quantized rows for this row
  // in this scan.
  virtual bool StartMCUQuantizerRow(class Scan *scan)
  {
    // On purpose, forward to the start of the residuals, not
    // the legacy data.
    return m_pParent->StartMCUResidualRow(scan);
  }
  //
  // Make sure to reset the block control to the
  // start of the scan for the indicated components in the scan, 
  // required after collecting the statistics for this scan.
  virtual void ResetToStartOfScan(class Scan *scan)
  {
    m_pParent->ResetToStartOfScan(scan);
  }
};
///

///
#endif
