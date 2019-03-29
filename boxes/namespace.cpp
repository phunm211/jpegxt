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
** $Id: namespace.cpp,v 1.4 2014/11/11 09:49:11 thor Exp $
**
*/

/// Includes
#include "interface/types.hpp"
#include "tools/environment.hpp"
#include "boxes/namespace.hpp"
#include "boxes/matrixbox.hpp"
#include "boxes/tonemapperbox.hpp"
#include "boxes/mergingspecbox.hpp"
///

/// NameSpace::FindNonlinearity
// Find the tone mapping box of the given table index, or NULL
// if this box is missing.
class ToneMapperBox *NameSpace::FindNonlinearity(UBYTE tabidx) const
{
  class Box *box;

  if (m_ppPrimaryList) {
    box = *m_ppPrimaryList;
    while(box) {
      class ToneMapperBox *tmo = dynamic_cast<ToneMapperBox *>(box);
      if (tmo) {
        if (tmo->TableDestinationOf() == tabidx)
          return tmo;
      }
      box = box->NextOf();
    }
  }

  if (m_ppSecondaryList) {
    box = *m_ppSecondaryList;
    while(box) {
      class ToneMapperBox *tmo = dynamic_cast<ToneMapperBox *>(box);
      if (tmo) {
        if (tmo->TableDestinationOf() == tabidx)
          return tmo;
      }
      box = box->NextOf();
    }
  }

  return NULL;
}
///

/// NameSpace::FindMatrix
// Find the transformation matrix of the given matrix index.
class MatrixBox *NameSpace::FindMatrix(UBYTE idx) const
{
  class Box *box;

  if (m_ppPrimaryList) {
    box = *m_ppPrimaryList;
    while(box) {
      class MatrixBox *matrix = dynamic_cast<MatrixBox *>(box);
      if (matrix) {
        if (matrix->IdOf() == idx)
          return matrix;
      }
      box = box->NextOf();
    }
  }

  if (m_ppSecondaryList) {
    box = *m_ppSecondaryList;
    while(box) {
      class MatrixBox *matrix = dynamic_cast<MatrixBox *>(box);
      if (matrix) {
        if (matrix->IdOf() == idx)
          return matrix;
      }
      box = box->NextOf();
    }
  }

  return NULL;
}
///

/// NameSpace::AllocateNonlinearityID
// Allocate an ID for a nonlinarity.
UBYTE NameSpace::AllocateNonlinearityID(void) const
{
  UBYTE idx = 0;

  if (m_ppPrimaryList) {
    const class Box *box = *m_ppPrimaryList;
    while(box) {
      const class ToneMapperBox *tmo = dynamic_cast<const ToneMapperBox *>(box);
      if (tmo) {
        if (tmo->TableDestinationOf() >= idx)
          idx = tmo->TableDestinationOf() + 1;
      }
      box = box->NextOf();
    }
  }

  if (m_ppSecondaryList) {
    const class Box *box = *m_ppSecondaryList;
    while(box) {
      const class ToneMapperBox *tmo = dynamic_cast<const ToneMapperBox *>(box);
      if (tmo) {
        if (tmo->TableDestinationOf() >= idx)
          idx = tmo->TableDestinationOf() + 1;
      }
      box = box->NextOf();
    }
  }

  if (idx > 15)
    JPG_THROW(OVERFLOW_PARAMETER,"NameSpace::AllocateNonlinearityID",
              "cannot create more than 16 nonlinear point transformations");
  
  return idx;
}
///

/// NameSpace::AllocateMatrixID
// Allocate an ID for a nonlinarity.
UBYTE NameSpace::AllocateMatrixID(void) const
{ 
  UBYTE idx = MergingSpecBox::FreeForm;

  if (m_ppPrimaryList) {
    const class Box *box = *m_ppPrimaryList;
    while(box) {
      const class MatrixBox *matrix = dynamic_cast<const MatrixBox *>(box);
      if (matrix) {
        if (matrix->IdOf() >= idx)
          idx = matrix->IdOf() + 1;
      }
      box = box->NextOf();
    }
  }

  if (m_ppSecondaryList) {
    const class Box *box = *m_ppSecondaryList;
    while(box) {
      const class MatrixBox *matrix = dynamic_cast<const MatrixBox *>(box);
      if (matrix) {
        if (matrix->IdOf() >= idx)
          idx = matrix->IdOf() + 1;
      }
      box = box->NextOf();
    }
  }

  if (idx > 15)
    JPG_THROW(OVERFLOW_PARAMETER,"NameSpace::AllocateNonlinearityID",
              "cannot create more than 11 linear transformations");
  
  return idx;
}
///

/// NameSpace::FindNonlinearity
// Find a parametric curve box with the given parameters, or return NULL if such a box
// does not yet exist.
const class ParametricToneMappingBox *NameSpace::FindNonlinearity(ParametricToneMappingBox::CurveType type,
                                                                  UBYTE e,
                                                                  FLOAT p1,FLOAT p2,FLOAT p3,FLOAT p4) const
{
  const class Box *box;

  if (m_ppPrimaryList) {
    box = *m_ppPrimaryList;
    while(box) {
      const class ParametricToneMappingBox *tmo = dynamic_cast<const ParametricToneMappingBox *>(box);
      if (tmo) {
        if (tmo->CompareCurve(type,e,p1,p2,p3,p4))
          return tmo;
      }
      box = box->NextOf();
    }
  }

  if (m_ppSecondaryList) {
    box = *m_ppSecondaryList;
    while(box) {
      const class ParametricToneMappingBox *tmo = dynamic_cast<const ParametricToneMappingBox *>(box);
      if (tmo) {
        if (tmo->CompareCurve(type,e,p1,p2,p3,p4))
          return tmo;
      }
      box = box->NextOf();
    }
  }

  return NULL;
}
///

/// NameSpace::isUniqueNonlinearity
// Check whether there is a duplicate nonlinearity of the given Id.
bool NameSpace::isUniqueNonlinearity(UBYTE tabidx) const
{
  const class Box *box;
  int count = 0;

  //
  // Note that each of the two sources may not contain a duplicate Id,
  // but the primary source may overload a box in the secondary list.
  if (m_ppPrimaryList) {
    box = *m_ppPrimaryList;
    while(box) {
      const class ToneMapperBox *tmo = dynamic_cast<const ToneMapperBox *>(box);
      if (tmo) {
        if (tmo->TableDestinationOf() == tabidx) {
          count++;
          if (count > 1)
            return false;
        }
      }
      box = box->NextOf();
    }
  }

  count = 0;
  if (m_ppSecondaryList) {
    box = *m_ppSecondaryList;
    while(box) {
      const class ToneMapperBox *tmo = dynamic_cast<const ToneMapperBox *>(box);
      if (tmo) {
        if (tmo->TableDestinationOf() == tabidx) {
          count++;
          if (count > 1)
            return false;
        }
      }
      box = box->NextOf();
    }
  }

  return true;
}
///

/// NameSpace::isUniqueMatrix
// Check whether there is a duplicate matrix of the given Id.
bool NameSpace::isUniqueMatrix(UBYTE idx) const
{  
  const class Box *box;
  int count = 0;

  if (m_ppPrimaryList) {
    box = *m_ppPrimaryList;
    while(box) {
      const class MatrixBox *matrix = dynamic_cast<const MatrixBox *>(box);
      if (matrix) {
        if (matrix->IdOf() == idx) {
          count++;
          if (count > 1)
            return false;
        }
      }
      box = box->NextOf();
    }
  }

  count = 0;
  if (m_ppSecondaryList) {
    box = *m_ppSecondaryList;
    while(box) {
      const class MatrixBox *matrix = dynamic_cast<const MatrixBox *>(box);
      if (matrix) {
        if (matrix->IdOf() == idx) {
          count++;
          if (count > 1)
            return false;
        }
      }
      box = box->NextOf();
    }
  }
  
  return true;
}
///
