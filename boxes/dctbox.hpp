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
** This class represents multiple boxes that all contain specifications
** on the DCT process. These boxes are only used by part-8.
**
** $Id: dctbox.hpp,v 1.3 2014/09/30 08:33:14 thor Exp $
**
*/

#ifndef BOXES_DCTBOX_HPP
#define BOXES_DCTBOX_HPP

/// Includes
#include "boxes/box.hpp"
///

/// class DCTBox
// This class represents multiple boxes that all specify
// the DCT operations.
class DCTBox : public Box {
  //
  // The type of the DCT to use.
  UBYTE m_ucDCTType;
  //
  // Enable or disable noise shaping. Only if the DCT is disabled.
  bool  m_bNoiseShaping;
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
public:
  enum {
    Base_Type        = MAKE_ID('L','D','C','T'), // base DCT
    Residual_Type    = MAKE_ID('R','D','C','T')  // residual DCT
  };
  //
  // Possible DCT types.
  enum DCTType {
    FDCT   = 0, // the fixpoint version
    IDCT   = 2, // the integer version
    Bypass = 3  // the DCT bypass
  };
  //
  // Create a DCT box. This also requires a type since there are
  // multiple boxes that all share the same syntax.
  DCTBox(class Environ *env,class Box *&boxlist,ULONG type)
    : Box(env,boxlist,type), m_ucDCTType(FDCT), m_bNoiseShaping(false)
  { 
  }
  //
  virtual ~DCTBox(void)
  { }
  //
  // Return the type of the DCT to be used.
  DCTType DCTTypeOf(void) const
  {
    return DCTType(m_ucDCTType);
  }
  //
  // Return a flag indicating whether noise shaping is enabled or disabled.
  bool isNoiseShapingEnabled(void) const
  {
    return m_bNoiseShaping;
  }
  //
  // Define the DCT operation
  void DefineDCT(DCTType t)
  {
    assert(t == FDCT || t == IDCT || t == Bypass);

    m_ucDCTType = t;
  }
  //
  // Enable or disable the noise shaping.
  void DefineNoiseShaping(bool onoff)
  {
    m_bNoiseShaping = onoff;
  }
};
///

///
#endif

    
