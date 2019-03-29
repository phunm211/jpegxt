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
** This box keeps a linear transformation that can be used either as L
** or C transformation. Unlike the lineartransformation box, the entries
** of the matrix defined here are 32-bit FLOATs.
**
** $Id: floattransformationbox.hpp,v 1.5 2014/09/30 08:33:15 thor Exp $
**
*/

#ifndef BOXES_FLOATTRANSFORMATIONBOX_HPP
#define BOXES_FLOATTRANSFORMATIONBOX_HPP

/// Includes
#include "std/string.hpp"
#include "std/assert.hpp"
#include "boxes/box.hpp"
#include "boxes/matrixbox.hpp"
///

/// class FloatTransformationBox
// This box keeps a linear transformation that can be used either as L
// or C transformation.
class FloatTransformationBox : public MatrixBox {
  //
  // The linear transformation coefficients as floating point.
  FLOAT m_fMatrix[9];
  //
  // The inverse matrix if it exists.
  FLOAT m_fInverse[9];
  //
  // Compute the inverse matrix and put it into m_lInverse. If it does not
  // exist, throw.
  void InvertMatrix(void);
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
    Type = MAKE_ID('F','T','R','X')
  };
  //
  // The constructor.
  FloatTransformationBox(class Environ *env,class Box *&boxlist)
    : MatrixBox(env,boxlist,Type)
  { }
  //
  // Define a matrix from an ID and a coefficient list.
  void DefineMatrix(UBYTE id,const FLOAT *matrix)
  {
    m_ucID = id;
    memcpy(m_fMatrix,matrix,sizeof(m_fMatrix));
  }
  //
  virtual ~FloatTransformationBox(void)
  { }
  //
  // Return the matrix transformation of this box. The result is a pointer
  // to nine floating point constants.
  const FLOAT *MatrixOf(void) const
  {
    return m_fMatrix;
  }
  //
  // Return the inverse of the matrix. Throws if it does not exist.
  const FLOAT *InverseMatrixOf(void)
  {
    if (!m_bInverseValid)
      InvertMatrix();

    assert(m_bInverseValid);
    
    return m_fInverse;
  }
};
///


///
#endif
