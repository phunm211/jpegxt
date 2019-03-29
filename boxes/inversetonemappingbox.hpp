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
** This box keeps an inverse tone mapping curve, as required for the
** R and L transformations.
**
** $Id: inversetonemappingbox.hpp,v 1.16 2014/09/30 08:33:15 thor Exp $
**
*/

#ifndef BOXES_INVERSETONEMAPPINGBOX_HPP
#define BOXES_INVERSETONEMAPPINGBOX_HPP

/// Includes
#include "boxes/box.hpp"
#include "boxes/tonemapperbox.hpp"
///

/// class InverseToneMappingBox
class InverseToneMappingBox : public ToneMapperBox {
  //
  // The table itself, indexed by the decoded sample value.
  LONG  *m_plTable;
  // 
  // Inverse (encoding) tone mapping curve, if available.
  LONG  *m_plInverseMapping;
  // 
  // Number of additional residual bits bits. Actually, this is only used to
  // test whether the table fits to the data (since it is always
  // unscaled). This is called R_d in the standard.
  UBYTE  m_ucResidualBits;
  //
  // Second level parsing stage: This is called from the first level
  // parser as soon as the data is complete. Must be implemented
  // by the concrete box.
  virtual bool ParseBoxContent(class ByteStream *stream,UQUAD boxsize);
  //
  // Second level creation stage: Write the box content into a temporary stream
  // from which the application markers can be created.
  virtual bool CreateBoxContent(class MemoryStream *target);
  //
public:
  enum {
    Type = MAKE_ID('T','O','N','E')
  };
  //
  InverseToneMappingBox(class Environ *env,class Box *&boxlist)
    : ToneMapperBox(env,boxlist,Type), m_plTable(NULL), m_plInverseMapping(NULL)
  { }
  //
  virtual ~InverseToneMappingBox(void);
  //
  //
  // Return the size of the table in entries.
  ULONG EntriesOf(void) const
  {
    return m_ulTableEntries;
  }
  //
  // Return the table itself.
  const LONG *TableOf(void) const
  {
    return m_plTable;
  }
  //
  // Define the table from an external source.
  void DefineTable(UBYTE tableidx,const UWORD *table,ULONG size,UBYTE residualbits);
  //
  // Check whether the given table is identical to the table stored here, and thus
  // no new index is required (to save rate). Returns true if the two are equal.
  bool CompareTable(const UWORD *table,ULONG size,UBYTE residualbits) const;
  //
  // Return a table that maps inputs in the range 0..2^inputbits-1
  // to output bits in the range 0..2^oututbits-1.
  virtual const LONG *ScaledTableOf(UBYTE inputbits,UBYTE outputbits,UBYTE infract,UBYTE outfract);
  //
  // This is the floating point version of the above. It returns floating point sample
  // values instead of integer sample values.
  virtual const FLOAT *FloatTableOf(UBYTE,UBYTE,UBYTE,UBYTE)
  {
    // Currently, no floating point workflow for the integer tables.
    return NULL;
  }
  //
  // Return the inverse of the table, where the first argument is the number
  // of bits in the DCT domain (the output bits) and the second argument is
  // the number of bits in the spatial (image) domain, i.e. the argument
  // order is identical to that of the backwards table generated above.
  virtual const LONG *InverseScaledTableOf(UBYTE dctbits,UBYTE spatialbits,UBYTE dctfract,UBYTE spatialfract);
};
///

///
#endif
