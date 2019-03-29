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
** This class keeps the namespaces together and finds boxes according
** to the priorities defined in the standard.
**
** $Id: namespace.hpp,v 1.4 2014/11/11 09:49:12 thor Exp $
**
*/

#ifndef BOXES_NAMESPACE_HPP
#define BOXES_NAMESPACE_HPP

/// Includes
#include "interface/types.hpp"
#include "tools/environment.hpp"
#include "boxes/parametrictonemappingbox.hpp"
///

/// Forwards
class Box;
class MatrixBox;
class ToneMapperBox;
///

/// class NameSpace
// This class keeps the namespaces together and finds boxes according
// to the priorities defined in the standard.
class NameSpace : public JKeeper {
  //
  // Pointers to box lists for searching objects.
  // These objects are not maintained here, the list is
  // just a list pointer.
  //
  // The primary search path for objects: This is the box list of the merging spec list.
  class Box **m_ppPrimaryList;
  //
  // The secondary lookup target. This is the global name space.
  class Box **m_ppSecondaryList;
  //
public:
  NameSpace(class Environ *env)
  : JKeeper(env), m_ppPrimaryList(NULL), m_ppSecondaryList(NULL)
  { }
  //
  ~NameSpace(void)
  { }
  //
  // Define the primary lookup namespace.
  void DefinePrimaryLookup(class Box **boxlist)
  {
    assert(m_ppPrimaryList == NULL);
    m_ppPrimaryList = boxlist;
  }
  //
  // Define the secondary lookup namespace.
  void DefineSecondaryLookup(class Box **boxlist)
  {
    assert(m_ppSecondaryList == NULL);
    m_ppSecondaryList = boxlist;
  }
  //
  // Find the tone mapping box of the given table index, or NULL
  // if this box is missing.
  class ToneMapperBox *FindNonlinearity(UBYTE tabidx) const;
  //
  // Find the transformation matrix of the given matrix index.
  class MatrixBox *FindMatrix(UBYTE idx) const;
  //
  // Check whether there is a duplicate nonlinearity of the given Id.
  bool isUniqueNonlinearity(UBYTE tabidx) const;
  //
  // Check whether there is a duplicate matrix of the given Id.
  bool isUniqueMatrix(UBYTE idx) const;
  //
  // Find a parametric curve box with the given parameters, or return NULL if such a box
  // does not yet exist.
  const class ParametricToneMappingBox *FindNonlinearity(ParametricToneMappingBox::CurveType type,
                                                         UBYTE rounding_mode,
                                                         FLOAT p1 = 0.0,FLOAT p2 = 0.0,
                                                         FLOAT p3 = 0.0,FLOAT p4 = 0.0) const;
  //
  // Allocate an ID for a nonlinarity.
  UBYTE AllocateNonlinearityID(void) const;
  //
  // Allocate an ID for a matrix
  UBYTE AllocateMatrixID(void) const;
};
///

///
#endif
