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
** This box keeps all the information for opacity coding, the alpha mode
** and the matte color.
**
** $Id: alphabox.hpp,v 1.1 2015/03/11 16:02:38 thor Exp $
**
*/

#ifndef BOXES_ALPHABOX_HPP
#define BOXES_ALPHABOX_HPP

/// Includes
#include "boxes/box.hpp"
///

/// Forwards
class MemoryStream;
class ByteStream;
///

/// Class AlphaBox
class AlphaBox : public Box {
  //
public:
  enum Method {
    Opaque        = 0,   // no merging with alpha, treat as if alpha is absent
    Regular       = 1,   // regular alpha blending as convex combination of foreground and background
    Premultiplied = 2,   // premultiplied alpha, i.e. alpha multiplication is included in the foreground
    MatteRemoval  = 3    // foreground is merged with constant matte color
  };
  //
private:
  //
  // The Alpha mode recorded here.
  UBYTE m_ucAlphaMode;
  //
  // The matte color for red,green and blue.
  ULONG m_ulMatteRed;
  ULONG m_ulMatteGreen;
  ULONG m_ulMatteBlue;
  //
  // Second level parsing stage: This is called from the first level
  // parser as soon as the data is complete. Must be implemented
  // by the concrete box. Returns true in case the contents is
  // parsed and the stream can go away.
  virtual bool ParseBoxContent(class ByteStream *stream,UQUAD boxsize);
  //
  // Second level creation stage: Write the box content into a temporary stream
  // from which the application markers can be created.
  // Returns whether the box content is already complete and the stream
  // can go away.
  virtual bool CreateBoxContent(class MemoryStream *target);
  //
  //
public:
  enum {
    Type = MAKE_ID('A','M','U','L')
  };
  //
  AlphaBox(class Environ *env,class Box *&boxlist)
    : Box(env,boxlist,Type), m_ucAlphaMode(Regular),
      m_ulMatteRed(0), m_ulMatteGreen(0), m_ulMatteBlue(0)
  { }
  //
  ~AlphaBox(void)
  { }
  //
  // Return the current alpha compositing method.
  Method CompositingMethodOf(void) const
  {
    return Method(m_ucAlphaMode);
  }
  //
  // Set the alpha mode.
  void SetCompositingMethod(Method m)
  {
    UBYTE method = m;
    assert(method <= MatteRemoval);

    m_ucAlphaMode = method;
  }
  //
  // Return the matte color for component n, n = 0,1,2
  ULONG MatteColorOf(UBYTE comp) const
  {
    assert(comp <= 2);

    switch(comp) {
    case 0:
      return m_ulMatteRed;
    case 1:
      return m_ulMatteGreen;
    case 2:
      return m_ulMatteBlue;
    default:
      return 0;
    }
  }
  //
  // Set the matte color of component n, n = 0,1,2
  void SetMatteColor(UBYTE comp,ULONG value)
  {
    assert(comp <= 2);

    switch(comp) {
    case 0:
      m_ulMatteRed = value;
      break;
    case 1:
      m_ulMatteGreen = value;
      break;
    case 2:
      m_ulMatteBlue = value;
      break;
    }
  }
};
///

///
#endif

  
  
