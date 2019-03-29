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
** This box implements a data container for refinement or residual 
** scans, as hidden in the APP11 markers.
**
** $Id: databox.hpp,v 1.10 2015/03/18 11:54:12 thor Exp $
**
*/

#ifndef BOXES_DATABOX_HPP
#define BOXES_DATABOX_HPP

/// Includes
#include "boxes/box.hpp"
///

/// Forwards
class ByteStream;
///

/// class DataBox
// This box implements a data container for refinement scans, as hidden
// in the APP11 markers.
class DataBox : public Box {
  //
  // Second level parsing stage: This is called from the first level
  // parser as soon as the data is complete. Must be implemented
  // by the concrete box.
  virtual bool ParseBoxContent(class ByteStream *,UQUAD)
  {
    // Actually, this does nothing here. The data just remains in the
    // decoder stream so the actual decoder can grab it when needed.
    return false;
  }
  //
  // Second level creation stage: Write the box content into a temporary stream
  // from which the application markers can be created.
  // Returns the buffer where the data is in - the box may use its own buffer.
  virtual bool CreateBoxContent(class MemoryStream *)
  {
    // This does actually nothing here. Writing the box is triggered
    // when required, but this is not part of the typical box writing logic.
    return false;
  }
  //
public:
  //
  // Data boxes carry plenty of raw data as a stream. Here are
  // their types.
  enum {
    ResidualType                = MAKE_ID('R','E','S','I'), // residual codestream
    RefinementType              = MAKE_ID('F','I','N','E'), // legacy refinement
    ResidualRefinementType      = MAKE_ID('R','F','I','N'), // refinement of the residual codestream
    AlphaType                   = MAKE_ID('A','L','F','A'), // alpha codestream
    AlphaRefinementType         = MAKE_ID('A','F','I','N'), // refinement of the alpha codestream
    AlphaResidualType           = MAKE_ID('A','R','E','S'), // alpha residual codestream
    AlphaResidualRefinementType = MAKE_ID('A','R','R','F')  // Aruff,wuff,wuff.... LeChuck... grrr.... Alpha channel residual refinement box
  };
  //
  // Create a refinement data box.
  DataBox(class Environ *env,class Box *&boxlist,ULONG type)
    : Box(env,boxlist,type)
  { }
  //
  //
  virtual ~DataBox(void)
  { }
  //
  // Return the byte stream buffer where the encoder can drop the data
  // on encoding.
  class ByteStream *EncoderBufferOf(void);
  //
  // Return the stream the decoder will decode from.
  class ByteStream *DecoderBufferOf(void);
  //
  // Flush the buffered data of the box and create the markers.
  // En is an enumerator that disambigutes identical boxes.
  void Flush(class ByteStream *target,UWORD en);
};
///

///
#endif
