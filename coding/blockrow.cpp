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
** This class represents one row of quantized data of coefficients, i.e. one
** row of 8x8 blocks.
**
** $Id: blockrow.cpp,v 1.7 2014/09/30 08:33:16 thor Exp $
**
*/

/// Includes
#include "coding/blockrow.hpp"
#include "std/string.hpp"
///

/// BlockRow::BlockRow
template<class T>
BlockRow<T>::BlockRow(class Environ *env)
  : JKeeper(env), m_pBlocks(NULL), m_pNext(NULL)
{
}
///

/// BlockRow::~BlockRow
template<class T>
BlockRow<T>::~BlockRow(void)
{
  if (m_pBlocks) {
    m_pEnviron->FreeMem(m_pBlocks,sizeof(struct Block) * m_ulWidth);
  }
}
///

/// BlockRow::AllocateRow
// Allocate a row of data, sufficient to hold the indicated number of cofficients. Note that
// it is still up to the caller to include the subsampling factors.
template<class T>
void BlockRow<T>::AllocateRow(ULONG coefficients)
{
  if (m_pBlocks == NULL) {
    m_ulWidth = (coefficients + 7) >> 3;
    m_pBlocks = (struct Block *)m_pEnviron->AllocMem(sizeof(struct Block) * m_ulWidth);
    memset(m_pBlocks,0,sizeof(struct Block) * m_ulWidth);
  } else {
    assert(m_ulWidth == (coefficients + 7) >> 3);
  }
}
///

/// Explicit template instanciation
template class BlockRow<LONG>;
template class BlockRow<FLOAT>;
///
