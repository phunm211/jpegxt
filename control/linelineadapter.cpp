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
** This class adapts to a line buffer in a way that allows the user
** to pull out (or push in) individual lines, thus not too much is to
** do here. Again, this adapts to the upsampling process of the
** hierarchical mode.
**
** $Id: linelineadapter.cpp,v 1.11 2015/06/03 15:37:25 thor Exp $
**
*/

/// Includes
#include "control/linelineadapter.hpp"
#include "marker/frame.hpp"
#include "marker/component.hpp"
#include "codestream/tables.hpp"
#include "tools/line.hpp"
#include "std/string.hpp"
///

/// LineLineAdapter::LineLineAdapter
LineLineAdapter::LineLineAdapter(class Frame *frame)
  : LineBuffer(frame), LineAdapter(frame), m_pEnviron(frame->EnvironOf()), m_pFrame(frame),
    m_pppImage(NULL), m_pulReadyLines(NULL), m_pulLinesPerComponent(NULL)
{
  m_ucCount = m_pFrame->DepthOf();
}
///

/// LineLineAdapter::~LineLineAdapter
LineLineAdapter::~LineLineAdapter(void)
{
  // The buffered image is in the line buffer itself,
  // so do not bother here to transmit.

  if (m_pulReadyLines)
    m_pEnviron->FreeMem(m_pulReadyLines,m_ucCount * sizeof(ULONG));

  if (m_pppImage)
    m_pEnviron->FreeMem(m_pppImage,m_ucCount * sizeof(struct Line **));

  if (m_pulLinesPerComponent)
    m_pEnviron->FreeMem(m_pulLinesPerComponent,m_ucCount * sizeof(ULONG));
}
///

/// LineLineAdapter::BuildCommon
void LineLineAdapter::BuildCommon(void)
{
  UBYTE i;
  
  LineBuffer::BuildCommon();
  LineAdapter::BuildCommon();

  if (m_pulReadyLines == NULL) {
    m_pulReadyLines = (ULONG *)m_pEnviron->AllocMem(m_ucCount * sizeof(ULONG));
    memset(m_pulReadyLines,0,sizeof(ULONG) * m_ucCount);
  }

  if (m_pppImage == NULL) {
    m_pppImage = (struct Line ***)m_pEnviron->AllocMem(m_ucCount * sizeof(struct Line **));
    for(i = 0;i < m_ucCount;i++) {
      m_pppImage[i]              = m_ppTop  + i;
    }
  }

  if (m_pulLinesPerComponent == NULL) {
    m_pulLinesPerComponent = (ULONG *)m_pEnviron->AllocMem(m_ucCount * sizeof(ULONG));
    for(i = 0;i < m_ucCount;i++) {
      class Component *comp      = m_pFrame->ComponentOf(i);
      UBYTE suby                 = comp->SubYOf();
      m_pulLinesPerComponent[i]  = (m_ulPixelHeight + suby - 1) / suby;
    }
  }
}
///

/// LineLineAdapter::GetNextLine
// Get the next available line from the output
// buffer on reconstruction. The caller must make
// sure that the buffer is really loaded up to the
// point or the line will be neutral grey.
struct Line *LineLineAdapter::GetNextLine(UBYTE comp)
{
  struct Line *line;
  assert(comp < m_ucCount);
  //
  // Get the next line from the image, possibly extend the image
  if (*m_pppImage[comp] == NULL) { 
    line = AllocateLine(comp);
    //
    // Initialize to neutral grey as no data is there.
    memset(line->m_pData,0,sizeof(LONG) * m_pulWidth[comp]);
  } else {
    //
    line = *m_pppImage[comp];
    assert(line);
    m_pppImage[comp]  = &(line->m_pNext);
  }
  
  return line;
}
///

/// LineLineAdapter::AllocateLine
// Allocate the next line for encoding. This line must
// later on then be pushed back into this buffer by
// PushLine below.
struct Line *LineLineAdapter::AllocateLine(UBYTE comp)
{
  struct Line *line;
  
  assert(comp < m_ucCount);

  // Get the next line from the image, possibly extend the image
  if (*m_pppImage[comp] == NULL) { 
    // Line is not yet there, create it.
    line = new(m_pEnviron) struct Line;
    *m_pppImage[comp] = line;
    line->m_pData     = (LONG *)m_pEnviron->AllocMem((m_pulWidth[comp] * sizeof(LONG)));
  }
  //
  line = *m_pppImage[comp];
  assert(line);
  m_pppImage[comp] = &(line->m_pNext);

  return line;
}
///

/// LineLineAdapter::PushLine
// Push the next line into the output buffer.
void LineLineAdapter::PushLine(struct Line *line,UBYTE comp)
{ 

  // There is really not much to do here...
  assert(comp < m_ucCount);
  assert(line);
  assert(m_pulReadyLines[comp] < m_pulLinesPerComponent[comp]);
  NOREF(line);
  m_pulReadyLines[comp]++;
}
///

/// LineLineAdapter::ResetToStartOfImage
// Reset all components on the image side of the control to the
// start of the image. Required when re-requesting the image
// for encoding or decoding.
void LineLineAdapter::ResetToStartOfImage(void)
{ 
  for(UBYTE i = 0;i < m_ucCount;i++) {
    m_pppImage[i]      = &m_ppTop[i];
    m_pulReadyLines[i] = 0;
  }
}
///

/// LineLineAdapter::isNextMCULineReady
// Return true if the next MCU line is buffered and can be pushed
// to the encoder. Note that the data here is *not* subsampled.
bool LineLineAdapter::isNextMCULineReady(void) const
{
  int i;
  
  for(i = 0;i < m_ucCount;i++) {
    if (m_pulReadyLines[i] < m_ulPixelHeight) { // There is still data to encode
      class Component *comp = m_pFrame->ComponentOf(i);
      ULONG codedlines      = m_pulCurrentY[i];
      // codedlines + comp->SubYOf() << 3 * comp->MCUHeightOf() is the number of
      // lines that must be buffered to encode the next MCU
      if (m_pulReadyLines[i] < codedlines + 8 * comp->MCUHeightOf())
        return false;
    }
  }
  
  return true;
}
///

/// LineLineAdapter::isImageComplete
// Return an indicator whether all of the image has been loaded into
// the image buffer.
bool LineLineAdapter::isImageComplete(void) const
{
  for(UBYTE i = 0;i < m_ucCount;i++) {
    if (m_pulReadyLines[i] < m_pulLinesPerComponent[i])
      return false;
  }
  return true;
}
///

/// LineLineAdapter::BufferedLines
// Returns the number of lines buffered for the given component.
// Note that subsampling expansion has not yet taken place here,
// this is to be done top-level.
ULONG LineLineAdapter::BufferedLines(UBYTE i) const
{
class Component *comp = m_pFrame->ComponentOf(i);
  ULONG curline = m_pulCurrentY[i] + (comp->MCUHeightOf() << 3);
  if (curline >= m_ulPixelHeight) { // end of image
    curline = m_ulPixelHeight;
  }
  return curline;
}
///

/// LineLineAdapter::PostImageHeight
// Post the height of the frame in lines. This happens
// when the DNL marker is processed.
void LineLineAdapter::PostImageHeight(ULONG lines)
{
  LineBuffer::PostImageHeight(lines);
  LineAdapter::PostImageHeight(lines);

  assert(m_pulLinesPerComponent);

  for(UBYTE i = 0;i < m_ucCount;i++) {
    class Component *comp      = m_pFrame->ComponentOf(i);
    UBYTE suby                 = comp->SubYOf();
    m_pulLinesPerComponent[i]  = (m_ulPixelHeight + suby - 1) / suby;
  }
}
///
