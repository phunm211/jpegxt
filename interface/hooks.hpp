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
 * Customizable hooks.
 * 
 * $Id: hooks.hpp,v 1.8 2014/09/30 08:33:17 thor Exp $
 *
 * Hooks provide a standard method to supply call-out functions.
 * They are initialized by the client application and called by
 * the library. They pass data in terms of tag items.
 * They also pass client private data.
 *
 * This is part of the external interface of the jpeg
 * and visible to the outher world.
 */

#ifndef HOOKS_HPP
#define HOOKS_HPP

/// Includes
#include "jpgtypes.hpp"
#include "tagitem.hpp"
///

///
#ifndef JPG_EXPORT
#define JPG_EXPORT
#endif
///

/// Design
/** Design
******************************************************************
** struct J2k_Hook                                              **
** Super Class: none                                            **
** Sub Classes: none                                            **
** Friends:                                                     **
******************************************************************

The hook structure defines a generic call-back hook. This allows
the user to "hook" into the library and to get called on certain
events. The j2k lib uses this structure for all its call-back
functions.

A hook defines first an entry point to be called. This callback
function takes a pointer to the hook structure and a tag list
as arguments. Tag lists are discussed in a separate file and
provide enough flexibility to extend options.

The callback function can be one of two kinds, it may either
return a LONG (as a result code) or a generic pointer. The
library uses currently both types, see the documentation which
hook uses which.

Furthermore, hooks provide a "sub entry" point. This is not
used by the library at all, but you may use it. A typical
application would be to let the main entry point point to a
small "stub" function which loads registers, base pointers etc.
and let the "sub entry" point to the real thing.

Last but not least, a hook contains a user data field you may
use for whatever you feel like. Typically, this would be either
a base register, or a pointer to the corresponding base object
the hook is part of. 

In a C++ environment, a hook entry would consist of a static
member function of your object, which would forward hook calls
to the non-static methods of the object.
* */
///

/// Hook structure
// this is a function as it is called from the library
// It is called with the hook as one parameter such that the 
// client is able to extract its private data. 
// It also comes with a tag item parameter to pass data.

struct JPG_EXPORT JPG_Hook {
  // The callbacks.
#ifdef __cplusplus  
  typedef JPG_LONG (LongHookFunction)(struct JPG_Hook *, struct JPG_TagItem *tag);
  typedef JPG_APTR (APtrHookFunction)(struct JPG_Hook *, struct JPG_TagItem *tag);
#endif
  //
  union JPG_EXPORT HookCallOut {
    JPG_LONG (*hk_pLongEntry)(struct JPG_Hook *, struct JPG_TagItem *tag);      // the main entry point.
    JPG_APTR (*hk_pAPtrEntry)(struct JPG_Hook *, struct JPG_TagItem *tag);
    //
#ifdef __cplusplus
    // Constructors
    HookCallOut(LongHookFunction *hook)
      : hk_pLongEntry(hook)
    { }
    //
    HookCallOut(APtrHookFunction *hook)
      : hk_pAPtrEntry(hook)
    { }
    //
    HookCallOut(void)
      : hk_pLongEntry(0)
    { }
#endif
    //
  }                   hk_Entry,hk_SubEntry;
  //
  // hk_SubEntry
  // can be used by the application to forward the request
  // and to load the private data below in whatever register it needs.
  // Is otherwise not used by the application.
  //
  JPG_APTR hk_pData;  // for private use of the client.
  //
#ifdef __cplusplus
  // Constructors
  JPG_Hook(LongHookFunction *hook = 0, JPG_APTR data = 0)
    : hk_Entry(hook), hk_pData(data)
  { }
  //  
  JPG_Hook(APtrHookFunction *hook, JPG_APTR data = 0)
    : hk_Entry(hook), hk_pData(data)
  { }
  //
  //
  JPG_LONG CallLong(struct JPG_TagItem *tag)
  {
    return (*hk_Entry.hk_pLongEntry)(this,tag);
  }
  //
  JPG_APTR CallAPtr(struct JPG_TagItem *tag)
  {
    return (*hk_Entry.hk_pAPtrEntry)(this,tag);
  }
#endif
  //
};
///

///
#endif
