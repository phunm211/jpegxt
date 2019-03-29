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
** This class represents the interface for parsing the
** entropy coded data in JPEG as part of a single scan.
**
** $Id: entropyparser.cpp,v 1.21 2014/10/08 12:05:02 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "marker/scan.hpp"
#include "marker/frame.hpp"
#include "codestream/tables.hpp"
#include "codestream/entropyparser.hpp"
#include "io/bytestream.hpp"
///

/// EntropyParser::EntropyParser
EntropyParser::EntropyParser(class Frame *frame,class Scan *scan)
  : JKeeper(scan->EnvironOf()), m_pScan(scan), m_pFrame(frame)
{
  m_ucCount = scan->ComponentsInScan();

  // The residual scan uses all components here, not just for, but
  // it does not require the component count either.
  for(UBYTE i = 0;i < m_ucCount && i < 4;i++) {
    m_pComponent[i] = scan->ComponentOf(i);
  }

  m_usRestartInterval   = m_pFrame->TablesOf()->RestartIntervalOf();
  m_usNextRestartMarker = 0xffd0;
  m_usMCUsToGo          = m_usRestartInterval;
  m_bSegmentIsValid     = true;
  m_bScanForDNL         = (m_pFrame->HeightOf() == 0)?true:false;
  m_bDNLFound           = false;
}
///

/// EntropyParser::StartWriteScan
// Write the marker to the stream.
void EntropyParser::StartWriteScan(class ByteStream *,class Checksum *,class BufferCtrl *)
{
  // Reset the restart marker count.
  m_usRestartInterval   = m_pFrame->TablesOf()->RestartIntervalOf();
  m_usNextRestartMarker = 0xffd0;
  m_usMCUsToGo          = m_usRestartInterval;
}
///

/// EntropyParser::~EntropyParser
EntropyParser::~EntropyParser(void)
{
}
///

/// EntropyParser::WriteRestartMarker
// Flush the scan statistics, write the restart marker,
// reset the MCU counter.
void EntropyParser::WriteRestartMarker(class ByteStream *io)
{
  Flush(false);
  if (io) {
    io->PutWord(m_usNextRestartMarker);
    m_usNextRestartMarker = (m_usNextRestartMarker + 1) & 0xfff7;
  }
  m_usMCUsToGo          = m_usRestartInterval;
}
///

/// EntropyParser::ParseRestartMarker
// Parse the restart marker or resync at the restart marker.
void EntropyParser::ParseRestartMarker(class ByteStream *io)
{
  LONG dt = io->PeekWord();
  
  while(dt == 0xffff) {
    // Found a filler byte. Skip over and try again.
    io->Get();
    dt = io->PeekWord();
  }
  
  if (dt == 0xffdc && m_bScanForDNL) {
    ParseDNLMarker(io);
  } else if (dt == m_usNextRestartMarker) {
    // Everything worked fine! Continue going after removing the marker.
    io->GetWord();
    Restart();
    m_usNextRestartMarker = (m_usNextRestartMarker + 1) & 0xfff7;
    m_usMCUsToGo          = m_usRestartInterval;
    m_bSegmentIsValid     = true;
  } else {
    JPG_WARN(MALFORMED_STREAM,"EntropyParser::ParseRestartMarker",
             "entropy coder is out of sync, trying to advance to the next marker");
    // As said...
    //
    do {
      dt = io->Get();
      if (dt == ByteStream::EOF) {
        // Outch, run completely out of data.
        JPG_THROW(UNEXPECTED_EOF,"EntropyParser::ParseRestartMarker",
                  "run into end of file while trying to resync the entropy parser");
        //
        // Code never goes here...
        return;
      } else if (dt == 0xff) {
        // Could be a marker.
        io->LastUnDo();
        dt = io->PeekWord();
        // Depends now on the marker.
        if (dt >= 0xffd0 && dt < 0xffd8) {
          // Is a restart marker. If this is the correct one, just leave,
          // the entropy coder was behind and we are then again up at the
          // correct index.
          if (dt == m_usNextRestartMarker) {
            io->GetWord();
            Restart();
            m_usNextRestartMarker = (m_usNextRestartMarker + 1) & 0xfff7;
            m_usMCUsToGo          = m_usRestartInterval;
            m_bSegmentIsValid     = true;
            return;
          } else if (((dt - m_usNextRestartMarker) & 0x07) >= 4) {
            // Here dt is *likely* behind, i.e. we need to skip more
            // data to advance to the correct restart marker.
            io->GetWord();
            // Remove the marker and keep going.
          } else {
            // Here dt is likely ahead, that is, the entropy decoder
            // should better skip the next entropy coded segment
            // completely and then should re-enter to re-examine whether
            // the marker fits. Keep the marker in the stream, then, but
            // do not continue to decode.
            m_bSegmentIsValid     = false;
            m_usNextRestartMarker = (m_usNextRestartMarker + 1) & 0xfff7;
            m_usMCUsToGo          = m_usRestartInterval;
            // Do not run into a restart as this may pull bytes.
            return;
          }
        } else if (dt >= 0xffc0 && dt < 0xfff0) {
          // Is apparently some other marker, i.e. we are at the end of
          // the segment. Continue skipping until the end is reached and
          // the parser run out of fun...
          m_bSegmentIsValid     = false;
          m_usNextRestartMarker = (m_usNextRestartMarker + 1) & 0xfff7;
          m_usMCUsToGo          = m_usRestartInterval;
          // Do not run into a restart as this may pull bytes.
          return;
        } else {
          // Some garbadge data, or a 0xff00. Just eat it up, and continue
          // scanning. Note that a single Get is used here to eventually
          // skip over a "fill byte".
          io->Get();
        }
      }
    } while(true);
  }
}
///

/// EntropyParser::ParseDNLMarker
// Parse the DNL marker, update the frame height. If
// the result is true, the marker has been found.
bool EntropyParser::ParseDNLMarker(class ByteStream *io)
{
  LONG dt;

  if (m_bDNLFound)
    return true;
  
  dt = io->PeekWord();

  while(dt == 0xffff) {
    // A filler byte followed by the marker (hopefully). Skip the
    // filler and try again.
    io->Get();
    dt = io->PeekWord();
  }

  if (dt == 0xffdc) {
    dt = io->GetWord();
    dt = io->GetWord();
    if (dt != 4)
      JPG_THROW(MALFORMED_STREAM,"EntropyParser::ParseDNLMarker",
                "DNL marker size is out of range, must be exactly four bytes long");
    
    dt = io->GetWord();
    if (dt == ByteStream::EOF)
      JPG_THROW(UNEXPECTED_EOF,"EntropyParser::ParseDNLMarker",
                "stream is truncated, could not read the DNL marker");
    if (dt == 0)
      JPG_THROW(MALFORMED_STREAM,"EntropyParser::ParseDNLMarker",
                "frame height as indicated by the DNL marker is corrupt, must be > 0");
    
    m_pFrame->PostImageHeight(dt);

    m_bDNLFound = true;
    return true;
  } else {
    return false;
  }
}
///

/// EntropyParser::FractionalColorBitsOf
// Return the number of fractional bits due to the color
// transformation.
UBYTE EntropyParser::FractionalColorBitsOf(void) const
{
  return m_pFrame->TablesOf()->FractionalColorBitsOf(m_pFrame->DepthOf());
}
///
