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
** The actual downsampling implementation.
**
** $Id: downsampler.cpp,v 1.9 2014/09/30 08:33:18 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "tools/rectangle.hpp"
#include "upsampling/downsamplerbase.hpp"
#include "upsampling/downsampler.hpp"
#include "std/string.hpp"
///

/// Downsampler::Downsampler
template<int sx,int sy>
Downsampler<sx,sy>::Downsampler(class Environ *env,ULONG width,ULONG height)
  : DownsamplerBase(env,sx,sy,width,height)
{
}
///

/// Downsampler::~Downsampler
template<int sx,int sy>
Downsampler<sx,sy>::~Downsampler(void)
{
}
///

/// Downsampler::DownsampleRegion
// The actual downsampling process. To be implemented by the actual
// classes inheriting from this. Coordinates are in the downsampled
// block domain the block indices. Requires an output buffer that
// will keep the downsampled data.
template<int sx,int sy>
void Downsampler<sx,sy>::DownsampleRegion(LONG bx,LONG by,LONG *buffer) const
{
  LONG ofs = (bx * sx) << 3; // first pixel in the buffer.
  LONG yfs = (by * sy) << 3; // first line.
  int lines = 0; // number of lines already managed to be summed up.
  int cnt   = 8; // number of output lines to go.
  struct Line *line = m_pInputBuffer;
  LONG y = m_lY;

  assert(yfs >= m_lY && yfs < m_lY + m_lHeight);

  //
  // Get the line.
  while(y < yfs) {
    line = line->m_pNext;
    y++;
  }
  assert(line);

  do {
    //
    // Start of a new line clear the entire output buffer.
    if (lines == 0)
      buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = buffer[5] = buffer[6] = buffer[7] = 0;
    //
    // Still something in the image?
    if (line) {
      LONG *src = line->m_pData + ofs; // Current input buffer position.
      LONG *bp  = buffer;
      int i = 8; // pixel in the line

      do {
        switch(sx) { // actually this will be unrolled because it is a template
        case 4:
          *bp += src[3];
        case 3:
          *bp += src[2];
        case 2:
          *bp += src[1];
        case 1:
          *bp += src[0];
        }
        src += sx;
        bp++;
      } while(--i);
      //
      // Now continue with the next line if there is one, count the number of lines summed up.
      line   = line->m_pNext;
      lines++;
    }
    //
    // If we're done with this block, or if there are no more lines, normalize this
    // block and continue with the next.
    if (lines >= sy || line == NULL) {
      // Only if there is actually anything in the buffer, otherwise just leave it empty.
      if (lines) {
        WORD norm = lines * sx;
        if (norm > 1) {
          // Normalize the summed pixels.
          buffer[0] /= norm;buffer[1] /= norm;buffer[2] /= norm;buffer[3] /= norm;
          buffer[4] /= norm;buffer[5] /= norm;buffer[6] /= norm;buffer[7] /= norm;
        }
      }
      // Start the next buffer line.
      buffer    += 8;
      cnt--;
      lines      = 0;
    }
  } while(cnt);
}
///


/// Explicit instaciations
template class Downsampler<1,1>;
template class Downsampler<1,2>;
template class Downsampler<1,3>;
template class Downsampler<1,4>;
template class Downsampler<2,1>;
template class Downsampler<2,2>;
template class Downsampler<2,3>;
template class Downsampler<2,4>;
template class Downsampler<3,1>;
template class Downsampler<3,2>;
template class Downsampler<3,3>;
template class Downsampler<3,4>;
template class Downsampler<4,1>;
template class Downsampler<4,2>;
template class Downsampler<4,3>;
template class Downsampler<4,4>;
///
