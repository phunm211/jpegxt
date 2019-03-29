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
** A couple of generic TMO related functions: Estimate TMO from LDR and HDR
** image pair, build a gamma mapping.
**
** $Id: tmo.hpp,v 1.7 2015/11/17 15:34:43 thor Exp $
**
*/

#ifndef CMD_TMO_HPP
#define CMD_TMO_HPP

/// Includes
#include "std/stdio.hpp"
#include "interface/types.hpp"
///

/// Prototypes
// Invert a tabulated tone mapping table. To be on the fair side,
// this uses the same algorithm the library uses to build the forwards
// table from the backwards table provided. Otherwise, I could also
// invert numerically the (parametric) table.
extern void InvertTable(UWORD input[65536],UWORD output[65536],UBYTE inbits,UBYTE outbits);
//
// Build an inverse tone mapping from a hdr/ldr image pair
extern void BuildToneMappingFromLDR(FILE *in,FILE *ldrin,int w,int h,int depth,int count,
                                    UWORD ldrtohdr[65536],bool flt,bool bigendian,bool xyz,
                                    int hiddenbits,bool median,bool &fullrange,
                                    int smooth);
// Build an inverse tone mapping from a hdr/ldr image pair, though generate it as
// a floating point table. This requires floating point input.
extern void BuildToneMappingFromLDR(FILE *in,FILE *ldrin,int w,int h,int count,
                                    FLOAT ldrtohdr[256],
                                    bool bigendian,bool median,bool fullrange,
                                    int smooth);
//
// Build three inverse TMOs from a hdr/ldr image pair.
extern void BuildRGBToneMappingFromLDR(FILE *in,FILE *ldrin,int w,int h,int depth,int count,
                                       UWORD red[65536],UWORD green[65536],UWORD blue[65536],
                                       bool flt,bool bigendian,bool xyz,
                                       int hiddenbits,bool median,bool &fullrange,
                                       int smooth);
//
// Build a static gamma mapping to map the HDR to the LDR domain.
extern void BuildGammaMapping(double gamma,double exposure,UWORD ldrtohdr[65536],
                              bool flt,int max,int hiddenbits);
//
// Load an inverse tone mapping table from a file.
extern void LoadLTable(const char *ltable,UWORD ldrtohdr[65536],bool flt,
                       int max,int hiddenbits);
///

///
#endif
