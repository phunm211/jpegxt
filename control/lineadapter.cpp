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
** This is a purely abstract interface class for an interface that
** is sufficient for the line merger to pull lines out of a frame,
** or another line merger.
**
** $Id: lineadapter.cpp,v 1.15 2015/06/03 15:37:24 thor Exp $
**
*/


/// Includes
#include "tools/environment.hpp"
#include "control/lineadapter.hpp"
#include "marker/frame.hpp"
#include "marker/component.hpp"
#include "std/string.hpp"
///

/// LineAdapter::LineAdapter
LineAdapter::LineAdapter(class Frame *frame)
  : BufferCtrl(frame->EnvironOf()), m_pFrame(frame), m_pulPixelsPerLine(NULL), m_ppFree(NULL)
{
  m_ucCount = frame->DepthOf();
}
///

/// LineAdapter::~LineAdapter
LineAdapter::~LineAdapter(void)
{
  struct Line *line;
  UBYTE i;
  
  if (m_ppFree) {
    assert(m_pulPixelsPerLine);
    for(i = 0;i < m_ucCount;i++) {
      while ((line = m_ppFree[i])) {
        m_ppFree[i] = line->m_pNext;
        if (line->m_pData) m_pEnviron->FreeMem(line->m_pData,m_pulPixelsPerLine[i] * sizeof(LONG));
        delete line;
      }
    }
    m_pEnviron->FreeMem(m_ppFree,m_ucCount * sizeof(struct Line *));
  }

  if (m_pulPixelsPerLine)
    m_pEnviron->FreeMem(m_pulPixelsPerLine,m_ucCount * sizeof(ULONG));
}
///

/// LineAdapter::BuildCommon
void LineAdapter::BuildCommon(void)
{ 
  ULONG width = m_pFrame->WidthOf();
  UBYTE i;
  
  if (m_pulPixelsPerLine == NULL) {
    m_pulPixelsPerLine = (ULONG *)m_pEnviron->AllocMem(m_ucCount * sizeof(ULONG));
    for(i = 0;i < m_ucCount;i++) {
      class Component *comp      = m_pFrame->ComponentOf(i);
      UBYTE subx                 = comp->SubXOf();
      m_pulPixelsPerLine[i]      = ((((width + subx - 1) / subx) + 7) & -8) + 2;
    }
  }
  
  if (m_ppFree == NULL) {
    m_ppFree = (struct Line **)m_pEnviron->AllocMem(m_ucCount * sizeof(struct Line *));
    memset(m_ppFree,0,sizeof(struct Line *) * m_ucCount);
  } 
}
///

/// LineAdapter::AllocLine
// Create a new line for component comp
struct Line *LineAdapter::AllocLine(UBYTE comp)
{
  struct Line *line;

  if ((line = m_ppFree[comp])) {
    m_ppFree[comp] = line->m_pNext;
    line->m_pNext  = NULL;
#if CHECK_LEVEL > 0
    assert(line->m_pOwner == NULL);
    line->m_pOwner = this;
#endif
    return line;
  } else {
    line = new(m_pEnviron) struct Line;
    // Temporarily link into the free list to prevent a memory leak in case the 
    // allocation throws.
    line->m_pNext  = m_ppFree[comp];
    m_ppFree[comp] = line;
    line->m_pData     = (LONG *)m_pEnviron->AllocMem(m_pulPixelsPerLine[comp] * sizeof(LONG));
    m_ppFree[comp] = line->m_pNext;
    line->m_pNext  = NULL;
#if CHECK_LEVEL > 0
    line->m_pOwner = this;
#endif
    return line;
  }
}
///

/// LineAdapter::FreeLine
// Release a line again, just put it onto the free list for recylcing.
void LineAdapter::FreeLine(struct Line *line,UBYTE comp)
{
  if (line) {
#if CHECK_LEVEL > 0
    assert(line->m_pOwner == this);
    line->m_pOwner = NULL;
#endif
    line->m_pNext = m_ppFree[comp];
    m_ppFree[comp] = line;
  }
}
///

