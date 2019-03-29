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
** This file provides a simple TMO that provides fine quality and natural
** look in most cases. It is mostly a global Reinhard operator:
** Erik Reinhard and Kate Devlin. Dynamic Range Reduction Inspired by
** Photoreceptor Physiology.  IEEE Transactions on Visualization and
** Computer Graphics (2004).
**
** This is the default TMO for profile C.
**
** $Id: defaulttmoc.cpp,v 1.4 2015/01/20 21:51:49 thor Exp $
**
*/

/// Includes
#include "cmd/defaulttmoc.hpp"
#include "cmd/iohelpers.hpp"
#include "std/stdio.hpp"
#include "std/math.hpp"
///

/// BuildToneMapping_C
// Make a simple attempt to find a reasonable tone mapping from HDR to LDR.
// This is by no means ideal, but seem to work well in most cases.
// The algorithm used here is a simplified version of the exrpptm tone mapper,
// found in the following paper:
// Erik Reinhard and Kate Devlin. Dynamic Range Reduction Inspired by
// Photoreceptor Physiology.  IEEE Transactions on Visualization and
// Computer Graphics (2004).
void BuildToneMapping_C(FILE *in,int w,int h,int depth,int count,UWORD tonemapping[65536],
                        bool flt,bool bigendian,bool xyz,int hiddenbits)
{
  long pos    = ftell(in);
  int x,y,i;
  int maxin   = 256 << hiddenbits;
  double max  = (1 << depth) - 1;
  double lav  = 0.0;
  double llav = 0.0;
  double minl = HUGE_VAL;
  double maxl =-HUGE_VAL;
  double miny = HUGE_VAL;
  double maxy =-HUGE_VAL;
  double m;
  long cnt = 0;

  for(y = 0;y < h;y++) {
    for(x = 0;x < w;x++) {
      int r,g,b;
      double y;

      ReadRGBTriple(in,r,g,b,y,depth,count,flt,bigendian,xyz);

      if (y > 0.0) {
        double logy = log(y);
        lav  += y;
        llav += logy;
        if (logy < minl)
          minl = logy;
        if (logy > maxl)
          maxl = logy;
        if (y < miny)
          miny = y;
        if (y > maxy)
          maxy = y;
        cnt++;
      }
    }
  }

  lav  /= cnt;
  llav /= cnt;
  if (maxl <= minl) {
    m   = 0.3;
  } else {
    double k = (maxl - llav) / (maxl - minl);
    if (k > 0.0) {
      m  = 0.3 + 0.7 * pow(k,1.4);
    } else {
      m  = 0.3;
    }
  }

  fseek(in,pos,SEEK_SET);

  for(i = 0;i < maxin;i++) {
    if (flt) {
      double out = i / double(maxin);
      double in  = pow(pow(lav,m) * out / (1.0 - out),2.2);
      if (in < 0.0) in = 0.0;
      
      tonemapping[i] = DoubleToHalf(in);
    } else {
      double out = i / double(maxin);
      double in  = max * (miny + (maxy - miny) * pow(lav,m) * out / (1.0 - out));
      if (in < 0.0) in = 0.0;
      if (in > max) in = max;
      
      tonemapping[i] = in;
    }
  }
}
///
