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
** This class defines the arbitrary color transformation defined
** in JPEG-LS part-2. It is - in a sense - a special case of the 
** JPEG 2000 part-2 reversible color transformation.
**
** $Id: lscolortrafo.hpp,v 1.7 2014/09/30 08:33:17 thor Exp $
**
*/

#ifndef MARKER_LSCOLORTRAFO_HPP
#define MARKER_LSCOLORTRAFO_HPP

/// Includes
#include "tools/environment.hpp"
///

/// Forwards
class ByteStream;
///

/// class LSColorTrafo
// This class defines the arbitrary color transformation defined
// in JPEG-LS part-2. It is - in a sense - a special case of the 
// JPEG 2000 part-2 reversible color transformation.
class LSColorTrafo : public JKeeper {
  //
  // Number of components that are transformed here:
  UBYTE  m_ucDepth;
  //
  // Near value, if known.
  UWORD  m_usNear;
  //
  // The maximum value of the transformed components.
  UWORD  m_usMaxTrans;
  //
  // Labels of the input components. An array of m_ucDepth
  // indices.
  UBYTE *m_pucInputLabels;
  //
  // The division/shift to be applied after the linear transformation.
  UBYTE *m_pucShift;
  //
  // The center flags which identify whether the components are zero-
  // centered or centered mid-way.
  bool  *m_pbCentered;
  //
  // The transformation matrix as m_ucDepth^2 matrix of multipliers.
  // Interestingly, the specs say these are unsigned. Hmmm?
  // The fast coordinate is here over the input components for
  // reconstrution.
  UWORD *m_pusMatrix;
  //
public:
  LSColorTrafo(class Environ *env);
  //
  ~LSColorTrafo(void);
  //
  // Write the marker contents to a LSE marker.
  void WriteMarker(class ByteStream *io);
  //
  // Parse the marker contents of a LSE marker.
  // marker length and ID are already parsed off.
  void ParseMarker(class ByteStream *io,UWORD len);
  //
  // Return the maximum sample value.
  UWORD MaxTransOf(void) const
  {
    return m_usMaxTrans;
  }
  // 
  // The number of components this transformation
  // handles.
  UBYTE DepthOf(void) const
  {
    return m_ucDepth;
  }
  //
  // Return the labels of the components. Note that
  // this are not component indices!
  const UBYTE *LabelsOf(void) const
  {
    return m_pucInputLabels;
  }
  //
  // Return the right-shift that is applied before the final
  // modulo addition/subtraction.
  const UBYTE *RightShiftOf(void) const
  {
    return m_pucShift;
  }
  //
  // Return the array of the centered flags.
  const bool *CenteredFlagsOf(void) const
  {
    return m_pbCentered;
  }
  //
  // Return the transformation matrix as a pointer to
  // Depth^2 entries, the fast direction iterates over
  // the input components.
  const UWORD *MatrixOf(void) const
  {
    return m_pusMatrix;
  }
  //
  // Return the near value - not stored in the marker,
  // only recorded on encoding. This is the per-component
  // l-infinity error bound for JPEG-LS.
  UWORD NearOf(void) const
  {
    return m_usNear;
  }
  //
  // Install the defaults for a given sample count. This
  // installs the example pseudo-RCT given in the specs.
  void InstallDefaults(UBYTE bpp,UBYTE near);
};
///

///
#endif
