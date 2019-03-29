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
** This class collects the huffman coder statistics for optimized huffman
** coding.
**
** $Id: huffmanstatistics.cpp,v 1.17 2014/09/30 08:33:16 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "std/string.hpp"
#include "coding/huffmanstatistics.hpp"
///

/// Defines
// Set the following define to create a universal
// Huffman code that is valid for all sources by
// setting the frequency of all symbols to at least
// one.
//#define COMPLETE_CODESET
///

/// HuffmanStatistics::HuffmanStatistics
#ifdef COMPLETE_CODESET
HuffmanStatistics::HuffmanStatistics(bool dc)
#else
HuffmanStatistics::HuffmanStatistics(bool)
#endif
{
#ifdef COMPLETE_CODESET
  int i,last = 256;
  if (dc)
    last = 16;
  // This should only be set for training purposes
  memset(m_ulCount,0,sizeof(m_ulCount));
  for(i = 0;i < last;i++) {
    m_ulCount[i] = 1;
  }
#else
  memset(m_ulCount,0,sizeof(m_ulCount));
#endif
}
///

/// HuffmanStatistics::CodesizesOf
// Find the number of codesizes of the optimal huffman tree.
// This returns an array of 256 elements, one entry per symbol.
const UBYTE *HuffmanStatistics::CodesizesOf(void)
{
  ULONG freq[257];    // measured but modified statistics.
  ULONG mstt[256];    // modified statistics.
  int   next[257];    // linkage to other symbols in the same branch.
  UBYTE size[257];
  int i;

  memcpy(mstt,m_ulCount,sizeof(mstt));

  do {
    bool valid = true;
    // Copy statistics over from the temporary.
    for(i = 0;i < 256;i++) {
      freq[i] = mstt[i];
      next[i] = -1;
      size[i] = 0;
    }
    freq[256] = 1;
    next[256] = -1;
    size[256] = 0;
    // Merge frequencies. Always move into the least probable symbol, then remove the
    // second highest. As long as there are symbols left.
    do {
      ULONG min1 = MAX_ULONG,min2 = MAX_ULONG;
      int minarg1 = 0,minarg2 = 0; // shut up the compiler. Not used if less than two symbols.
      
      for(i = 0;i <= 256;i++) {
        if (freq[i] > 0) { // Only valid entries
          if (freq[i] < min1) {
            // min1 becomes available, swap to min2?
            assert(min1 <= min2 || min1 == MAX_ULONG);
            min2    = min1;
            minarg2 = minarg1;
            min1    = freq[i];
            minarg1 = i;
          } else if (freq[i] < min2) {
            min2    = freq[i];
            minarg2 = i;
          }
        }
      }
      // If there is only one entry left, exit.
      if (min2 == MAX_ULONG) {
        // If there is only a single symbol, make sure it gets a codesize.
        if (freq[minarg1] == 0)
          freq[minarg1]++;
        break;
      }
      //
      // Merge now, remove the least symbol.
      freq[minarg1] += freq[minarg2];
      freq[minarg2]  = 0;
      //
      // Update the codesize for the subtree at the current position.
      do {
        i = minarg1;
        size[minarg1]++;
        minarg1 = next[minarg1];
      } while(minarg1 >= 0);
      //
      // Merge the two subtrees.
      next[i] = minarg2;
      do {
        size[minarg2]++;
        minarg2 = next[minarg2];
      } while(minarg2 >= 0);
    } while(true);
    //
    // Ok, now check whether all the sizes are at most 16.
    for(i = 0;i < 256;i++) {
      if (size[i] > 16) {
        valid = false;
        break;
      }
      m_ucCodeSize[i] = size[i];
    }
    //
    // If this is a valid assignment, leave.
    if (valid)
      break;
    //
    // Here the optimal huffman code is not JPEG compliant
    // Modify the statistics at the low-frequency end.
    {
      // Find the two minimal *disjoint* code entries.
      ULONG min1 = MAX_ULONG,min2 = MAX_ULONG;
      //
      for(i = 0;i < 256;i++) {
        if (mstt[i] > 0) {
          if (mstt[i] < min1) {
            // min1 becomes available, swap to min2?
            assert(min1 <= min2 || min1 == MAX_ULONG);
            min2    = min1;
            min1    = mstt[i];
          } else if (mstt[i] < min2 && mstt[i] > min1) {
            min2    = mstt[i];
          }
        }
      }
      //
      // Set everything smaller than the second smallest
      // to the second smallest. This will flatten
      // the statistics and hence balance the tree a bit.
      for(i = 0;i < 256;i++) {
        if (mstt[i] > 0 && mstt[i] < min2) {
          mstt[i] = min2;
        }
      }
    }
  } while(true);

  return m_ucCodeSize;
}
///

/// HuffmanStatistics::MergeStatistics
// Merge the counts with the recorded count values in the file.
#ifdef COLLECT_STATISTICS
void HuffmanStatistics::MergeStatistics(FILE *stats)
{
  while(!feof(stats)) {
    int symbol,count;
    if (fscanf(stats,"%d\t%d\n",&symbol,&count) == 2) {
      if (symbol >= 0 && symbol < 256) {
        m_ulCount[symbol] += count;
      }
    }
  }
}
#endif
///

/// HuffmanStatistics::WriteStatistics
// Write the (combined) statistics back to the file.
#ifdef COLLECT_STATISTICS
void HuffmanStatistics::WriteStatistics(FILE *stats)
{
  int symbol;
  
  for(symbol = 0;symbol < 256;symbol++) {
    if (m_ulCount[symbol]) {
      fprintf(stats,"%d\t%d\n",symbol,m_ulCount[symbol]);
    } else {
      fprintf(stats,"%d\t%d\n",symbol,1);
    }
  }
}
#endif
///


