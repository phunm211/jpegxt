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
** This class parses the markers and holds the decoder together.
**
** $Id: encoder.hpp,v 1.25 2015/03/24 09:45:31 thor Exp $
**
*/

#ifndef CODESTREAM_ENCODER_HPP
#define CODESTREAM_ENCODER_HPP

/// Include
#include "interface/types.hpp"
#include "tools/environment.hpp"
#include "marker/scantypes.hpp"
///

/// Forwards
class ByteStream;
class Quantization;
class HuffmanTable;
class Frame;
class Tables;
class Scan;
class Image;
struct JPG_TagItem;
///

/// class Encoder
// The default encoder. Nothing fancy, just
// uses the default tables and the default quantization
// settings. More to come.
class Encoder : public JKeeper {
  //
  // The image containing all the data
  class Image        *m_pImage;
  //
  // Create from a set of parameters the proper scan type.
  // This fills in the scan type of the base image and the residual image,
  // the number of refinement scans in the LDR and HDR domain, the
  // frame precision (excluding hidden/refinement bits) in the base and extension layer
  // and the number of additional precision bits R_b in the spatial domain.
  void FindScanTypes(const struct JPG_TagItem *tags,LONG frametype,UBYTE defaultdepth,
                     ScanType &scantype,ScanType &restype,
                     UBYTE &hiddenbits,UBYTE &riddenbits,
                     UBYTE &ldrprecision,UBYTE &hdrprecision,
                     UBYTE &rangebits) const;
  //
public:
  Encoder(class Environ *env);
  //
  ~Encoder(void);
  //
  // Create an image from the layout specified in the tags. See interface/parameters
  // for the available tags.
  class Image *CreateImage(const struct JPG_TagItem *tags);
};
///

///
#endif
