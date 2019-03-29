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
 * Member functions of the bitmap hook class
 * 
 * $Id: bitmaphook.cpp,v 1.10 2015/03/16 08:55:33 thor Exp $
 *
 */

/// Includes
#include "interface/types.hpp"
#include "interface/bitmaphook.hpp"
#include "interface/hooks.hpp"
#include "interface/tagitem.hpp"
#include "interface/parameters.hpp"
#include "marker/component.hpp"
#include "tools/environment.hpp"
#include "tools/traits.hpp"
#include "std/assert.hpp"
///

/// BitMapHook::BitMapHook
BitMapHook::BitMapHook(const struct JPG_TagItem *tags)
  : m_pHook(NULL), m_pLDRHook(NULL), m_pAlphaHook(NULL)
{
  // Fill in use useful defaults. This really depends on the user
  // As the following tags are all optional.
  //
  m_DefaultImageLayout.ibm_pData           = NULL;
  m_DefaultImageLayout.ibm_ulWidth         = 0;
  m_DefaultImageLayout.ibm_ulHeight        = 0;
  m_DefaultImageLayout.ibm_lBytesPerRow    = 0;
  m_DefaultImageLayout.ibm_cBytesPerPixel  = 0;
  m_DefaultImageLayout.ibm_ucPixelType     = 0;
  m_DefaultImageLayout.ibm_pUserData       = NULL;
  //
  InitHookTags(m_BitmapTags);
  InitHookTags(m_LDRTags);

  if (tags)
    ParseTags(tags); // Parse off the user tags.
}
///

/// BitMapHook::ParseTags
// Parse off the user parameters to fill in the default image bitmap layout.
void BitMapHook::ParseTags(const struct JPG_TagItem *tag)
{  
  assert(tag);
  //
  // Override the default image layout
  while (tag) {
    switch (tag->ti_Tag) {
    case JPGTAG_BIO_MEMORY:
      m_DefaultImageLayout.ibm_pData           = tag->ti_Data.ti_pPtr;
      break;
    case JPGTAG_BIO_WIDTH:
      m_DefaultImageLayout.ibm_ulWidth         = tag->ti_Data.ti_lData;
      break;
    case JPGTAG_BIO_HEIGHT:
      m_DefaultImageLayout.ibm_ulHeight        = tag->ti_Data.ti_lData;
      break;
    case JPGTAG_BIO_BYTESPERROW:
      m_DefaultImageLayout.ibm_lBytesPerRow    = tag->ti_Data.ti_lData;
      break;
    case JPGTAG_BIO_BYTESPERPIXEL:
      m_DefaultImageLayout.ibm_cBytesPerPixel  = (UBYTE) tag->ti_Data.ti_lData;
      break;
    case JPGTAG_BIO_PIXELTYPE:
      m_DefaultImageLayout.ibm_ucPixelType     = (UBYTE) tag->ti_Data.ti_lData;
      break;
    case JPGTAG_BIO_USERDATA:
      m_DefaultImageLayout.ibm_pUserData       = tag->ti_Data.ti_pPtr;
      break;
    case JPGTAG_BIH_HOOK:
      m_pHook                                  = (struct JPG_Hook *)tag->ti_Data.ti_pPtr;
      break;
    case JPGTAG_BIH_LDRHOOK:
      m_pLDRHook                               = (struct JPG_Hook *)tag->ti_Data.ti_pPtr;
      break;
    case JPGTAG_BIH_ALPHAHOOK:
      m_pAlphaHook                             = (struct JPG_Hook *)tag->ti_Data.ti_pPtr;
      break;
    }
    tag = tag->NextTagItem();
  }
}
///

/// BitMapHook::InitHookTags
// Setup the input parameter tags for the user hook.
void BitMapHook::InitHookTags(struct JPG_TagItem *tags)
{
  // Setup the tags for the bitmap/image data
  tags[0].ti_Tag              = JPGTAG_BIO_ACTION;
  tags[1].ti_Tag              = JPGTAG_BIO_MEMORY;
  tags[2].ti_Tag              = JPGTAG_BIO_WIDTH;
  tags[3].ti_Tag              = JPGTAG_BIO_HEIGHT;
  tags[4].ti_Tag              = JPGTAG_BIO_BYTESPERROW;
  tags[5].ti_Tag              = JPGTAG_BIO_BYTESPERPIXEL;
  tags[6].ti_Tag              = JPGTAG_BIO_PIXELTYPE;
  tags[7].ti_Tag              = JPGTAG_BIO_ROI;
  tags[7].ti_Data.ti_lData    = false;
  tags[8].ti_Tag              = JPGTAG_BIO_COMPONENT;
  tags[9].ti_Tag              = JPGTAG_BIO_USERDATA;
  tags[9].ti_Data.ti_pPtr     = m_DefaultImageLayout.ibm_pUserData;
  tags[10].ti_Tag             = JPGTAG_BIO_MINX;
  tags[11].ti_Tag             = JPGTAG_BIO_MINY;
  tags[12].ti_Tag             = JPGTAG_BIO_MAXX;
  tags[13].ti_Tag             = JPGTAG_BIO_MAXY;
  tags[14].ti_Tag             = JPGTAG_BIO_ALPHA;
  tags[14].ti_Data.ti_lData   = false;
  tags[15].ti_Tag             = JPGTAG_TAG_IGNORE; // was: BIO_SLICE;
  tags[15].ti_Data.ti_lData   = 0;
  tags[16].ti_Tag             = JPGTAG_TAG_IGNORE; // was: JPGTAG_BIO_COLOR;
  tags[17].ti_Tag             = JPGTAG_BIO_PIXEL_MINX;
  tags[18].ti_Tag             = JPGTAG_BIO_PIXEL_MINY;
  tags[19].ti_Tag             = JPGTAG_BIO_PIXEL_MAXX;
  tags[20].ti_Tag             = JPGTAG_BIO_PIXEL_MAXY;
  tags[21].ti_Tag             = JPGTAG_BIO_PIXEL_XORG;
  tags[22].ti_Tag             = JPGTAG_BIO_PIXEL_YORG;
  tags[23].ti_Tag             = JPGTAG_TAG_DONE;
}
///

/// BitMapHook::Request
// Fill the tag items for a request call and make the call.
void BitMapHook::Request(struct JPG_Hook *hook,struct JPG_TagItem *tags,UBYTE pixeltype,
                         const RectAngle<LONG> &rect,struct ImageBitMap *ibm,
                         const class Component *comp,bool alpha)
{
  // Fill in the encoding tags.
  tags[0].ti_Data.ti_lData  = JPGFLAG_BIO_REQUEST;
  tags[1].ti_Data.ti_pPtr   = m_DefaultImageLayout.ibm_pData;
  tags[2].ti_Data.ti_lData  = m_DefaultImageLayout.ibm_ulWidth;
  tags[3].ti_Data.ti_lData  = m_DefaultImageLayout.ibm_ulHeight;
  tags[4].ti_Data.ti_lData  = m_DefaultImageLayout.ibm_lBytesPerRow;
  tags[5].ti_Data.ti_lData  = m_DefaultImageLayout.ibm_cBytesPerPixel;
  tags[6].ti_Data.ti_lData  = pixeltype;
  tags[8].ti_Data.ti_lData  = comp->IndexOf();
  tags[9].ti_Data.ti_pPtr   = m_DefaultImageLayout.ibm_pUserData;
  tags[10].ti_Data.ti_lData = rect.ra_MinX;
  tags[11].ti_Data.ti_lData = rect.ra_MinY;
  tags[12].ti_Data.ti_lData = rect.ra_MaxX;
  tags[13].ti_Data.ti_lData = rect.ra_MaxY;
  tags[14].ti_Data.ti_lData = alpha;
  tags[15].ti_Data.ti_lData = 0;
  tags[16].ti_Data.ti_lData = comp->IndexOf();
  tags[17].ti_Data.ti_lData = (rect.ra_MinX + comp->SubXOf() - 1) / comp->SubXOf();
  tags[18].ti_Data.ti_lData = (rect.ra_MinY + comp->SubYOf() - 1) / comp->SubYOf();
  tags[19].ti_Data.ti_lData = (rect.ra_MaxX + comp->SubXOf() + 1 - 1) / comp->SubXOf() - 1;
  tags[20].ti_Data.ti_lData = (rect.ra_MaxY + comp->SubYOf() + 1 - 1) / comp->SubYOf() - 1;
  tags[21].ti_Data.ti_lData = 0;
  tags[22].ti_Data.ti_lData = 0;
  //
  // Now call the hook if it exists
  if (hook)
    hook->CallLong(tags);

  // and now, finally, scan what we got back
  ibm->ibm_pData           = tags[1].ti_Data.ti_pPtr;
  ibm->ibm_ulWidth         = tags[2].ti_Data.ti_lData;
  ibm->ibm_ulHeight        = tags[3].ti_Data.ti_lData;
  ibm->ibm_lBytesPerRow    = tags[4].ti_Data.ti_lData;
  ibm->ibm_cBytesPerPixel  = tags[5].ti_Data.ti_lData;
  ibm->ibm_ucPixelType     = tags[6].ti_Data.ti_lData;
  ibm->ibm_pUserData       = tags[9].ti_Data.ti_pPtr;
}
///

/// BitMapHook::Release
// Release the tag items for a release call and make the call.
void BitMapHook::Release(struct JPG_Hook *hook,struct JPG_TagItem *tags,UBYTE pixeltype,
                         const RectAngle<LONG> &rect,const struct ImageBitMap *ibm,
                         const class Component *comp,bool alpha)
{
  if (hook) {
    tags[0].ti_Data.ti_lData  = JPGFLAG_BIO_RELEASE;
    tags[1].ti_Data.ti_pPtr   = ibm->ibm_pData;
    tags[2].ti_Data.ti_lData  = ibm->ibm_ulWidth;
    tags[3].ti_Data.ti_lData  = ibm->ibm_ulHeight;
    tags[4].ti_Data.ti_lData  = ibm->ibm_lBytesPerRow;
    tags[5].ti_Data.ti_lData  = ibm->ibm_cBytesPerPixel;
    tags[6].ti_Data.ti_lData  = pixeltype;
    tags[8].ti_Data.ti_lData  = comp->IndexOf();
    tags[9].ti_Data.ti_pPtr   = ibm->ibm_pUserData;
    tags[10].ti_Data.ti_lData = rect.ra_MinX;
    tags[11].ti_Data.ti_lData = rect.ra_MinY;
    tags[12].ti_Data.ti_lData = rect.ra_MaxX;
    tags[13].ti_Data.ti_lData = rect.ra_MaxY;
    tags[14].ti_Data.ti_lData = alpha;
    tags[15].ti_Data.ti_lData = 0;
    tags[16].ti_Data.ti_lData = comp->IndexOf();
    tags[17].ti_Data.ti_lData = (rect.ra_MinX + comp->SubXOf() - 1) / comp->SubXOf();
    tags[18].ti_Data.ti_lData = (rect.ra_MinY + comp->SubYOf() - 1) / comp->SubYOf();
    tags[19].ti_Data.ti_lData = (rect.ra_MaxX + comp->SubXOf() + 1 - 1) / comp->SubXOf() - 1;
    tags[20].ti_Data.ti_lData = (rect.ra_MaxY + comp->SubYOf() + 1 - 1) / comp->SubYOf() - 1;
    tags[21].ti_Data.ti_lData = 0;
    tags[22].ti_Data.ti_lData = 0;
    
    hook->CallLong(tags);
  }
}
///

/// BitMapHook::RequestClientData
// Pass an empty tag list over to the user,
// let the user fill out this tag list and
// fill out the image bitmap from this stuff.
void BitMapHook::RequestClientData(const RectAngle<LONG> &rect,struct ImageBitMap *ibm,
                                   const class Component *comp)
{
  Request(m_pHook,m_BitmapTags,m_DefaultImageLayout.ibm_ucPixelType,rect,ibm,comp,false);
}
///

/// BitMapHook::ReleaseClientData
// Tell the client that we are done with the data
// and release it. The user may use this
// call here to release temporary memory, etc, etc.
void BitMapHook::ReleaseClientData(const RectAngle<LONG> &rect,const struct ImageBitMap *ibm,
                                   const class Component *comp)
{
  Release(m_pHook,m_BitmapTags,m_DefaultImageLayout.ibm_ucPixelType,rect,ibm,comp,false);
}
///

/// BitMapHook::RequestClientAlpha
// Pass an empty tag list over to the user,
// let the user fill out this tag list and
// fill out the image bitmap from this stuff.
void BitMapHook::RequestClientAlpha(const RectAngle<LONG> &rect,struct ImageBitMap *ibm,
                                   const class Component *comp)
{
  Request(m_pAlphaHook,m_BitmapTags,m_DefaultImageLayout.ibm_ucPixelType,rect,ibm,comp,true);
}
///

/// BitMapHook::ReleaseClientAlpha
// Tell the client that we are done with the data
// and release it. The user may use this
// call here to release temporary memory, etc, etc.
void BitMapHook::ReleaseClientAlpha(const RectAngle<LONG> &rect,const struct ImageBitMap *ibm,
                                    const class Component *comp)
{
  Release(m_pAlphaHook,m_BitmapTags,m_DefaultImageLayout.ibm_ucPixelType,rect,ibm,comp,true);
}
///



/// BitMapHook::RequestLDRData
// Retrieve the LDR tone mapped version of the user. This requires that an
// LDR hook function is available, i.e. should only be called if the 
// providesLDRImage() method above returns true.
void BitMapHook::RequestLDRData(const RectAngle<LONG> &rect,struct ImageBitMap *ibm,
                                const class Component *comp)
{
  Request(m_pLDRHook,m_LDRTags,CTYP_UBYTE,rect,ibm,comp,false);
}
///

/// BitMapHook::ReleaseLDRData
// Release the requested LDR data. Requires that an LDR hook is available, i.e.
// providesLDRImage() must have been checked before and must have returned
// true for this to make sense.
void BitMapHook::ReleaseLDRData(const RectAngle<LONG> &rect,const struct ImageBitMap *ibm,
                                const class Component *comp)
{
  Release(m_pLDRHook,m_LDRTags,CTYP_UBYTE,rect,ibm,comp,false);
}
///
