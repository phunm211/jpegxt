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
** Generic DCT transformation plus quantization class.
** All DCT implementations should derive from this.
**
** $Id: dct.cpp,v 1.9 2016/10/28 13:58:53 thor Exp $
**
*/


/// Includes
#include "dct/dct.hpp"

/// DCT::ScanOrder
// The scan order.
#define P(x,y) ((x) + ((y) << 3))
const int DCT::ScanOrder[64] = {
    P(0,0),
    P(1,0),P(0,1),
    P(0,2),P(1,1),P(2,0),
    P(3,0),P(2,1),P(1,2),P(0,3),
    P(0,4),P(1,3),P(2,2),P(3,1),P(4,0),
    P(5,0),P(4,1),P(3,2),P(2,3),P(1,4),P(0,5),
    P(0,6),P(1,5),P(2,4),P(3,3),P(4,2),P(5,1),P(6,0),
    P(7,0),P(6,1),P(5,2),P(4,3),P(3,4),P(2,5),P(1,6),P(0,7),
    P(1,7),P(2,6),P(3,5),P(4,4),P(5,3),P(6,2),P(7,1),
    P(7,2),P(6,3),P(5,4),P(4,5),P(3,6),P(2,7),
    P(3,7),P(4,6),P(5,5),P(6,4),P(7,3),
    P(7,4),P(6,5),P(5,6),P(4,7),
    P(5,7),P(6,6),P(7,5),
    P(7,6),P(6,7),
    P(7,7)
};
#undef P
///

