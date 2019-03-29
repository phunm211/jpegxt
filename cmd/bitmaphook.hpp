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
** This header provides the interface for the bitmap hook that 
** delivers the bitmap data to the core library.
**
** $Id: bitmaphook.hpp,v 1.5 2016/10/28 13:58:52 thor Exp $
**
*/

#ifndef CMD_BITMAPHOOK
#define CMD_BITMAPHOOK

/// Includes
#include "interface/types.hpp"
#include "std/stdio.hpp"
///

/// Forwards
struct JPG_Hook;
struct JPG_TagItem;
///

/// Administration of bitmap memory.
struct BitmapMemory {
  APTR         bmm_pMemPtr;     // interleaved memory for the HDR image
  APTR         bmm_pLDRMemPtr;  // interleaved memory for the LDR version of the image
  APTR         bmm_pAlphaPtr;   // memory for the alpha channel
  ULONG        bmm_ulWidth;     // width in pixels.
  ULONG        bmm_ulHeight;    // height in pixels; this is only one block in our application.
  UWORD        bmm_usDepth;     // number of components.
  UBYTE        bmm_ucPixelType; // precision etc.
  UBYTE        bmm_ucAlphaType; // pixel type of the alpha channel
  FILE        *bmm_pTarget;     // where to write the data to.
  FILE        *bmm_pSource;     // where the data comes from on reading (encoding)
  FILE        *bmm_pLDRSource;  // if there is a separate source for the LDR image, this is non-NULL.
  FILE        *bmm_pAlphaTarget;// where the alpha (if any) goes to on decoding
  FILE        *bmm_pAlphaSource;// where the alpha data (if any) comes from. There is no dedicated alpha LDR file
  const UWORD *bmm_HDR2LDR;     // the (simple global) tone mapper used for encoding the image.
  bool         bmm_bFloat;      // is true if the input is floating point
  bool         bmm_bAlphaFloat; // is true if the opacity information is floating point
  bool         bmm_bBigEndian;  // is true if the floating point input is big endian
  bool         bmm_bAlphaBigEndian;     // if true, the floating point alpha channel is big endian
  bool         bmm_bNoOutputConversion; // if true, the FLOAT stays float and the half-map is not applied.
  bool         bmm_bNoAlphaOutputConversion; // ditto for alpha
  bool         bmm_bClamp;      // if set, clamp negative values to zero.
  bool         bmm_bAlphaClamp; // if set, alpha values outside [0,1] will be clamped to range
};
///

/// Prototypes
// Pull the LDR data if there is a separate LDR image
extern JPG_LONG LDRBitmapHook(struct JPG_Hook *hook, struct JPG_TagItem *tags);
// Pull the HDR image or push it on decoding.
extern JPG_LONG BitmapHook(struct JPG_Hook *hook, struct JPG_TagItem *tags);
// Pull the opacity information
extern JPG_LONG AlphaHook(struct JPG_Hook *hook, struct JPG_TagItem *tags);
///

///
#endif
