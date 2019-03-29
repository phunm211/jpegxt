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
** This class contains the marker that defines the JPEG LS thresholds.
**
** $Id: thresholds.hpp,v 1.9 2014/09/30 08:33:17 thor Exp $
**
*/

#ifndef MARKER_THRESHOLDS_HPP
#define MARKER_THRESHOLDS_HPP

/// Includes
#include "tools/environment.hpp"
///

/// Forwards
class ByteStream;
///

/// Thresholds
class Thresholds : public JKeeper {
  //
  // Parameters of the JPEG LS coder as defined by C.2.4.1.1
  //
  // Maximum sample value.
  UWORD m_usMaxVal;
  //
  // Bucket threshold 1
  UWORD m_usT1;
  //
  // Bucket threshold 2
  UWORD m_usT2;
  //
  // Bucket threshold 3
  UWORD m_usT3;
  //
  // The statistics reset value.
  UWORD m_usReset;
  //
public:
  Thresholds(class Environ *env);
  //
  ~Thresholds(void);
  //
  // Write the marker contents to a LSE marker.
  void WriteMarker(class ByteStream *io);
  //
  // Parse the marker contents of a LSE marker.
  // marker length and ID are already parsed off.
  void ParseMarker(class ByteStream *io,UWORD len);
  //
  // Return the maximum sample value.
  UWORD MaxValOf(void) const
  {
    return m_usMaxVal;
  }
  //
  // Return the T1 value.
  UWORD T1Of(void) const
  {
    return m_usT1;
  }
  //
  // Return the T2 value.
  UWORD T2Of(void) const
  {
    return m_usT2;
  }
  //
  // Return the T3 value.
  UWORD T3Of(void) const
  {
    return m_usT3;
  }
  //
  // Return the Reset interval.
  UWORD ResetOf(void) const
  {
    return m_usReset;
  }
  //
  // Install the defaults for a given bits per pixel value
  // and the quality/near value.
  void InstallDefaults(UBYTE bpp,UWORD near);
};
///

///
#endif
