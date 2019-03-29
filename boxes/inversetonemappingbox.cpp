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
** $Id: inversetonemappingbox.cpp,v 1.23 2015/06/27 09:52:29 thor Exp $
**
*/

/// Includes
#include "inversetonemappingbox.hpp"
#include "io/bytestream.hpp"
#include "io/memorystream.hpp"
#include "io/decoderstream.hpp"
#include "std/stdlib.hpp"
///

/// InverseToneMappingBox::~InverseToneMappingBox
InverseToneMappingBox::~InverseToneMappingBox(void)
{
  if (m_plTable)
    m_pEnviron->FreeMem(m_plTable,m_ulTableEntries * sizeof(LONG));

  if (m_plInverseMapping)
    m_pEnviron->FreeMem(m_plInverseMapping,(1UL << (8 + m_ucResidualBits)) * sizeof(LONG));
}
///

/// InverseToneMappingBox::ParseBoxContent
// Second level parsing stage: This is called from the first level
// parser as soon as the data is complete. Must be implemented
// by the concrete box.
bool InverseToneMappingBox::ParseBoxContent(class ByteStream *stream,UQUAD boxsize)
{
  LONG v;
  LONG entries;
  LONG *dt;
  
  if (boxsize > (MAX_UWORD + 1) * 2 + 1)
    JPG_THROW(MALFORMED_STREAM,"InverseToneMappingBox::ParseBoxContent",
              "Malformed JPEG stream, inverse tone mapping box is too large");

  // Size - 1 must be divisible by two - this is the number of entries - and a power of two.
  if ((boxsize & 1) == 0 || boxsize < 256 * 2)
    JPG_THROW(MALFORMED_STREAM,"InverseToneMappingBox::ParseBoxContent",
              "Malformed JPEG stream, number of table entries in the inverse tone mapping box is invalid");

  v                = stream->Get();
  m_ucTableIndex   = v >> 4;
  m_ucResidualBits = v & 0x0f;

  entries = (boxsize - 1) >> 1; // cannot overflow.
  //
  // Is this a power of two?
  if (entries & (entries - 1))
    JPG_THROW(MALFORMED_STREAM,"InverseToneMappingBox::ParseBoxContent",
              "Malformed JPEG stream, number of table entries in the inverse tone mapping box must be a power of two");

  assert(m_plTable == NULL);

  m_ulTableEntries = entries;
  dt = m_plTable   = (LONG *)(m_pEnviron->AllocMem(entries * sizeof(LONG)));
  
  if (m_ucResidualBits <= 8) {
    while(entries) {
      *dt++ = stream->GetWord();
      entries--;
    }
  } else { 
    while(entries) {
      LONG hi = stream->GetWord();
      LONG lo = stream->GetWord();
      *dt++   = (hi << 16) | lo;
      entries--;
    }
  }
  
  return true; // is parsed off
}
///

/// InverseToneMappingBox::CreateBoxContent
// Second level creation stage: Write the box content into a temporary stream
// from which the application markers can be created.
// Returns the buffer where the data is in - the box may use its own buffer.
bool InverseToneMappingBox::CreateBoxContent(class MemoryStream *target)
{
  LONG *dt      = m_plTable;
  ULONG entries = m_ulTableEntries;

  assert(m_plTable);

  target->Put((m_ucTableIndex << 4) | m_ucResidualBits);

  if (m_ucResidualBits <= 8) {
    while(entries) {
      target->PutWord(*dt++);
      entries--;
    }
  } else { 
    while(entries) {
      LONG e = *dt++;
      target->PutWord(WORD(e >> 16));
      target->PutWord(WORD(e >>  0));
      entries--;
    }
  }
  
  return true;
}
///

/// InverseToneMappingBox::DefineTable
// Define the table from an external source.
void InverseToneMappingBox::DefineTable(UBYTE tableidx,const UWORD *table,ULONG size,UBYTE residualbits)
{
  ULONG i;

  assert(m_plTable == NULL);
  assert((size & (size - 1)) == 0);
  assert(size);

  m_plTable        = (LONG *)m_pEnviron->AllocMem(size * sizeof(LONG));
  m_ulTableEntries = size;
  for(i = 0;i < size;i++)
    m_plTable[i]   = table[i];

  m_ucTableIndex   = tableidx;
  m_ucResidualBits = residualbits;
}
///

/// InverseToneMappingBox::CompareTable
// Check whether the given table is identical to the table stored here, and thus
// no new index is required (to save rate). Returns true if the two are equal.
bool InverseToneMappingBox::CompareTable(const UWORD *table,ULONG size,UBYTE residualbits) const
{
  if (m_ulTableEntries == size && m_ucResidualBits == residualbits && table && m_plTable) {
    ULONG i;
    for(i = 0;i < size;i++) {
      if (m_plTable[i] != table[i])
        return false;
    }
    return true;
  }
  return false;
}
///

/// InverseToneMappingBox::ScaledTableOf
// Return a table that maps inputs in the range 0..2^inputbits-1
// to output bits in the range 0..2^oututbits-1.
const LONG *InverseToneMappingBox::ScaledTableOf(UBYTE inputbits,UBYTE outputbits,UBYTE infract,UBYTE outfract)
{
  // Check whether the table fits. The output bits must fit to the residual bits
  // generated by the LUT.
  if (outputbits + outfract != 8 + m_ucResidualBits)
    JPG_THROW(INVALID_PARAMETER,"InverseToneMappingBox::ScaledTableOf",
              "Codestream is requesting a tone mapping that does not fit to the output bit precision.");
  //
  // Check whether the size of the table fits.
  if (inputbits > 16 || (1UL << inputbits) != m_ulTableEntries)
    JPG_THROW(INVALID_PARAMETER,"InverseToneMappingBox::ScaledTableOf",
              "Codestream is requesting a tone mapping that does not fit to the input bit precision.");

  if (infract != 0)
    JPG_THROW(INVALID_PARAMETER,"InverseToneMappingBox::ScaledTableOf",
              "Codestream is requesting a lookup table in a path that requires fractional bits");

  assert(m_plTable);

  return m_plTable;
}
///

/// InverseToneMappingBox::InverseScaledTableOf
// Return the inverse of the table, where the first argument is the number
// of bits in the DCT domain (the output bits) and the second argument is
// the number of bits in the spatial (image) domain, i.e. the argument
// order is identical to that of the backwards table generated above.
const LONG *InverseToneMappingBox::InverseScaledTableOf(UBYTE dctbits,UBYTE spatialbits,UBYTE dctfract,UBYTE spatialfract)
{
  // Check whether the table fits. The output bits must fit to the residual bits
  // generated by the LUT.
  if (spatialbits + spatialfract != 8 + m_ucResidualBits)
    JPG_THROW(INVALID_PARAMETER,"InverseToneMappingBox::InverseScaledTableOf",
              "Codestream is requesting a tone mapping that does not fit to the output bit precision.");

  // Check whether the size of the table fits.
  if (dctbits > 16 || (1UL << dctbits) != m_ulTableEntries)
    JPG_THROW(INVALID_PARAMETER,"InverseToneMappingBox::InverseScaledTableOf",
              "Codestream is requesting a tone mapping that does not fit to the input bit precision.");
  
  if (dctfract != 0)
    JPG_THROW(INVALID_PARAMETER,"InverseToneMappingBox::InverseScaledTableOf",
              "Codestream is requesting a lookup table in a path that requires fractional bits");

  assert(m_plTable);

  if (m_plInverseMapping == NULL) {
    LONG j,lastj,lastanchor;
    LONG last,current,mid;
    LONG outmax = (1L << (spatialbits + spatialfract)) - 1;
    LONG inmax  = (1L << (dctbits     + dctfract))     - 1;
    bool lastfilled;
    
    m_plInverseMapping = (LONG *)m_pEnviron->AllocMem((1 << (spatialbits + spatialfract)) * sizeof(LONG));
    // Nnot guaranteed that the mapping is surjective onto the output
    // range. There is nothing that says how to handle this case. We just define
    // "undefined" outputs to zero, and try our best to continue the missing parts
    // continously along the output range. 
    memset(m_plInverseMapping,0,(1 << (spatialbits + spatialfract)) * sizeof(LONG));
    //
    // Loop over positive coefficients.
    lastj      = inmax;
    lastanchor = inmax;
    lastfilled = false;
    j          = inmax;
    last       = outmax;
    //
    // Go from max to zero. This direction is intentional.
    do {
      // Get the next possible output for the given input.
      // Swap back to get the proper endianness.
      current = m_plTable[j];
      // If the function jumps, fill in half the values with the old
      // the other half with the new values. The output is never
      // swapped here, otherwise the table would grow out of range
      // too easily.
      if (current == last) {
        // Found a "flat" area, i.e. had the same external value for 
        // similar internal values. If so, fill in the midpoint 
        // into the table. If lastanchor + j overflows, then our
        // tables are far too huge in first place.
        m_plInverseMapping[last] = (lastanchor + j) >> 1;
        lastfilled               = true;
      } else {
        // Found a "steep" part of the output curve. 
        // If the function jumps, fill in half the values with the old
        // the other half with the new values. The output is never
        // swapped here, otherwise the table would grow out of range
        // too easily.
      if (last > current) {
        mid = ((current + last + 1) >> 1) - 1;
      } else {
        mid = ((current + last - 1) >> 1) - 1;
      }
      while(last != mid) {
        if (lastfilled == false) // Do not overwrite the flat area from the last time.
          m_plInverseMapping[last] = lastj;
        if (last > mid)    last--;
        else               last++;
        lastfilled = false;
      }
      while(last != current) {
        if (lastfilled == false) // Do not overwrite the flat area from the last time.
          m_plInverseMapping[last]  = j;
        if (last > current) last--;
        else                last++;
        lastfilled = false;
      }
      lastanchor = j;
      }
      lastj = j;
      last  = current;
    } while(j--); // This includes the case j = 0 in the inner loop
    // Now we could have the situation that "lastfilled" is still false,
    // thus lut[last] is not yet filled. j is now -1 and thus not
    // usable, lastj == 0, and there is no further point to extrapolate
    // to. Thus, set to the exact end-point.
    if (lastfilled == false || lastj == 0) /* make the zero exactly reproducable */
      m_plInverseMapping[last] = lastj;
    //
    // Fixup the ends of the table. If the start or the end of the LUT have a very low slope,
    // we will find jumps in the table that are likely undesired. Fix them up here to avoid
    // artefacts in the image.
    if (outmax > 4) {
      LONG i1,i2,i3;
      LONG d1,d2;
      //
      i1 = m_plInverseMapping[0];
      i2 = m_plInverseMapping[1];
      i3 = m_plInverseMapping[2];
      //
      d1 = (i1 > i2)?(i1 - i2):(i2 - i1);
      d2 = (i3 > i2)?(i3 - i2):(i2 - i3);
      //
      // If the first jump is too large, clip it.
      if (d1 > 2 * d2)
        m_plInverseMapping[0] = 2 * i2 - i3;
      //
      // Now at the other end. Note that max is inclusive.
      i1 = m_plInverseMapping[outmax];
      i2 = m_plInverseMapping[outmax - 1];
      i3 = m_plInverseMapping[outmax - 2];
      //
      d1 = (i1 > i2)?(i1 - i2):(i2 - i1);
      d2 = (i3 > i2)?(i3 - i2):(i2 - i3);
      //
      if (d1 > 2 * d2)
        m_plInverseMapping[outmax] = 2 * i2 - i3;
    }
  }
  //
  return m_plInverseMapping;
}
///
