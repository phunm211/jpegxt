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
** This class contains and maintains the huffman code parsers.
**
** $Id: huffmantable.cpp,v 1.19 2015/09/17 11:20:35 thor Exp $
**
*/

/// Includes
#include "marker/huffmantable.hpp"
#include "io/bytestream.hpp"
#include "coding/huffmantemplate.hpp"
#include "coding/arithmetictemplate.hpp"
#include "std/string.hpp"
///

/// HuffmanTable::HuffmanTable
HuffmanTable::HuffmanTable(class Environ *env)
  : JKeeper(env)
{
  memset(m_pCoder,0,sizeof(m_pCoder));
}
///

/// HuffmanTable::~HuffmanTable
HuffmanTable::~HuffmanTable(void)
{
  int i;

  for(i = 0;i < 8;i++) {
    delete m_pCoder[i];
  }
}
///


/// HuffmanTable::isEmpty
// Check whether the tables are empty or not
// In such a case, do not write the tables.
bool HuffmanTable::isEmpty(void) const
{ 
  int i = 0;

  for(i = 0;i < 8;i++) {
    if (m_pCoder[i]) {
      return false;
    }
  }

  return true;
}
///

/// HuffmanTable::WriteMarker
// Write the currently defined huffman tables back to a stream.
void HuffmanTable::WriteMarker(class ByteStream *io)
{
  int i = 0;
  ULONG len = 2; // marker size itself.

  for(i = 0;i < 8;i++) {
    if (m_pCoder[i]) {
      len += 1; // Th and Tc field.
      len += m_pCoder[i]->MarkerOverhead();
    }
  }

  if (len > MAX_UWORD)
    JPG_THROW(OVERFLOW_PARAMETER,"HuffmanTable::WriteMarker","DHT marker overhead too large, Huffman tables too complex");

  io->PutWord(len);
  
  for(i = 0;i < 8;i++) {
    if (m_pCoder[i]) {
      UBYTE type = 0;
      if (i >= 4)
        type |= 0x10;    // is an AC table then.
      type |= i & 0x03;  // the huffman table identifier.
      io->Put(type);
      m_pCoder[i]->WriteMarker(io);
    }
  }
}
///  

/// HuffmanTable::ParseMarker
// Parse the marker contents of a DHT marker.
void HuffmanTable::ParseMarker(class ByteStream *io)
{
  LONG len = io->GetWord();

  if (len < 2)
    JPG_THROW(MALFORMED_STREAM,"HuffmanTable::ParseMarker","Huffman table length must be at least two bytes long");

  len -= 2; // remove the marker length.

  while(len > 0) {
    LONG  t = io->Get();
    UQUAD p = io->FilePosition();
    UQUAD q;
    
    if (t == ByteStream::EOF)
      JPG_THROW(MALFORMED_STREAM,"HuffmanTable::ParseMarker","Huffman table marker run out of data");
    len--;
    
    if ((t >> 4) > 1) {
      JPG_THROW(MALFORMED_STREAM,"HuffmanTable::ParseMarker","undefined Huffman table type");
      return;
    }
    if ((t & 0x0f) > 3) {
      JPG_THROW(MALFORMED_STREAM,"HuffmanTable::ParseMarker",
                "invalid Huffman table destination, must be between 0 and 3");
    }
    t = (t & 0x03) | ((t & 0xf0) >> 2);
    delete m_pCoder[t];m_pCoder[t] = NULL;
    m_pCoder[t] = new(m_pEnviron) HuffmanTemplate(m_pEnviron);
    m_pCoder[t]->ParseMarker(io);
    
    q = io->FilePosition();
    assert(q >= p);
    q -= p;

    if (q > ULONG(len))
      JPG_THROW(MALFORMED_STREAM,"HuffmanTable::ParseMarker","huffman table size corrupt");
    len -= q;
  }

  if (len)
    JPG_THROW(MALFORMED_STREAM,"HuffmanTable::ParseMarker","huffman table size is corrupt");
}
///

/// HuffmanTable::AdjustToStatistics
// Adjust all coders in here to the statistics collected before, i.e.
// find optimal codes.
void HuffmanTable::AdjustToStatistics(void)
{
  for(int i = 0;i < 8;i++) {
    if (m_pCoder[i])
      m_pCoder[i]->AdjustToStatistics();
  }
}
///

/// HuffmanTable::DCTemplateOf
// Get the template for the indicated DC table or NULL if it doesn't exist.
class HuffmanTemplate *HuffmanTable::DCTemplateOf(UBYTE idx,ScanType type,UBYTE depth,UBYTE hidden)
{
  assert(m_pCoder && idx < 4);
  
  if (m_pCoder[idx] == NULL) {
    m_pCoder[idx] = new(m_pEnviron) class HuffmanTemplate(m_pEnviron);
    // Provide a default that seems sensible. Everything else requires
    // measurement.
    if (idx == 0) {
      m_pCoder[idx]->InitDCLuminanceDefault(type,depth,hidden);
    } else {
      m_pCoder[idx]->InitDCChrominanceDefault(type,depth,hidden);
    }
  }
  
  return m_pCoder[idx];
}
///

/// HuffmanTable::ACTemplateOf
// Get the template for the indicated AC table or NULL if it doesn't exist.
class HuffmanTemplate *HuffmanTable::ACTemplateOf(UBYTE idx,ScanType type,UBYTE depth,UBYTE hidden)
{
  assert(m_pCoder && idx < 4);

  idx += 4;
  
  if (m_pCoder[idx] == NULL) {
    m_pCoder[idx] = new(m_pEnviron) class HuffmanTemplate(m_pEnviron);
    // Provide a default that seems sensible. Everything else requires
    // measurement.
    if (idx == 4) {
      m_pCoder[idx]->InitACLuminanceDefault(type,depth,hidden);
    } else {
      m_pCoder[idx]->InitACChrominanceDefault(type,depth,hidden);
    }
  }
  
  return m_pCoder[idx];
}
///
