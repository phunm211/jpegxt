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
** This class represents a single component.
**
** $Id: component.hpp,v 1.17 2014/09/30 08:33:17 thor Exp $
**
*/

#ifndef MARKER_COMPONENT_HPP
#define MARKER_COMPONENT_HPP

/// Includes
#include "tools/environment.hpp"
#include "tools/rectangle.hpp"
///

/// Forwards
class ByteStream;
///

/// class Component
// This class represents a single component
class Component : public JKeeper {
  //
  // The component number from zero up.
  UBYTE m_ucIndex;
  //
  // The component identifier
  UBYTE m_ucID;
  //
  // The horizontal subsampling factor.
  UBYTE m_ucMCUWidth;
  //
  // The vertical subsampling factor.
  UBYTE m_ucMCUHeight;
  //
  // Subsampling factors in X and Y direction.
  UBYTE m_ucSubX,m_ucSubY;
  //
  // The quantization table index to use for quantization.
  UBYTE m_ucQuantTable;
  //
  // The bit precision of this component.
  UBYTE m_ucPrecision;
  //
public:
  Component(class Environ *env,UBYTE idx,UBYTE prec,UBYTE subx = 1,UBYTE suby = 1);
  //
  ~Component(void);
  //
  // Parse off parts of the frame marker.
  void ParseMarker(class ByteStream *io);
  //
  // Write the contents of the component to the marker.
  void WriteMarker(class ByteStream *io);
  //
  // Compute the subsampling factors for this component. 
  // Requires the maximum MCU size.
  void SetSubsampling(UBYTE maxwidth,UBYTE maxheight)
  {
    if (maxwidth % m_ucMCUWidth != 0 || maxheight % m_ucMCUHeight) {
      JPG_THROW(NOT_IMPLEMENTED,"Component::SetSubsampling",
                "non-integer subsampling factors are not supported by this implementation, sorry");
    }
    m_ucSubX = maxwidth  / m_ucMCUWidth;
    m_ucSubY = maxheight / m_ucMCUHeight;
  }
  //
  // Compute the MCU dimensions from the subsampling factors and
  // the smallest common multiple of all subsampling factors.
  void SetMCUSize(UBYTE maxwidth,UBYTE maxheight)
  {
    m_ucMCUWidth  = maxwidth  / m_ucSubX;
    m_ucMCUHeight = maxheight / m_ucSubY;
  }
  //
  // Return the component ID. This is arbitrary and
  // only required to identify components.
  UBYTE IDOf(void) const
  {
    return m_ucID;
  }
  //
  // Return the component index. Counts from zero up.
  UBYTE IndexOf(void) const
  {
    return m_ucIndex;
  }
  //
  // Return the MCU width in blocks.
  UBYTE MCUWidthOf(void) const
  {
    return m_ucMCUWidth;
  }
  //
  // Return the MCU height in blocks
  UBYTE MCUHeightOf(void) const
  {
    return m_ucMCUHeight;
  }
  //
  // Return the subsampling in X dimension.
  UBYTE SubXOf(void) const
  {
    return m_ucSubX;
  }
  //
  // Return the subsampling in Y dimension.
  UBYTE SubYOf(void) const
  {
    return m_ucSubY;
  }
  //
  // Return the quantizer responsible for this component.
  UBYTE QuantizerOf(void) const
  {
    return m_ucQuantTable;
  }
  //
  // Bit precision of the component.
  UBYTE PrecisionOf(void) const
  {
    return m_ucPrecision;
  }
  //
  // Install the component label.
  void SetComponentID(UBYTE id)
  {
    m_ucID = id;
  }
  //
  // Install the component quantizer index.
  void SetQuantizer(UBYTE quant)
  {
    m_ucQuantTable = quant;
  }
};
///

///
#endif
