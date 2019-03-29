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
** Parameter definition and encoding for profile A.
**
** $Id: encodea.hpp,v 1.16 2016/10/28 13:58:52 thor Exp $
**
*/

#ifndef CMD_ENCODEA_HPP
#define CMD_ENCODEA_HPP

/// Includes
#include "interface/types.hpp"
#include "std/stdio.hpp"
#if ISO_CODE
///

/// Prototypes
// Encode an image in profile A, filling in all the parameters the codec needs.
extern void EncodeA(const char *source,const char *ldrsource,const char *target,
                    int quality,int hdrquality,
                    int tabletype,int residualtt,int colortrafo,
                    bool progressive,bool rprogressive,
                    int hiddenbits,int residualhiddenbits,bool optimize,
                    bool openloop,bool deadzone,bool lagrangian,bool dering,
                    bool noclamp,double gamma,bool median,int smooth,
                    const char *sub,const char *resub,
                    const char *alpha,int alphamode,int matte_r,int matte_g,int matte_b,
                    bool alpharesiduals,int alphaquality,int alphahdrquality,
                    int alphatt,int residualalphatt,
                    int ahiddenbits,int ariddenbits,int aresprec,
                    bool aoopenloop,bool adeadzone,bool alagrangian,bool adering,
                    bool aserms,bool abypass);
//
// Provide a useful default for splitting the quality between LDR and HDR.
extern void SplitQualityA(int splitquality,int &quality,int &hdrquality);
///

///
#endif
#endif
