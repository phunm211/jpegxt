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
** PriorityQueue: This is a base object that allows byte indexed
** priority sorted queues, as for example used for the buffers of
** the generic box extension layer.
** 
** $Id: priorityqueue.hpp,v 1.6 2014/09/30 08:33:18 thor Exp $
**
*/

#ifndef TOOLS_PRIORITYQUEUE_HPP
#define TOOLS_PRIORITYQUEUE_HPP

/// Includes
#include "tools/environment.hpp"
///

/// Class PriorityQueue
// This object keeps a priority sorted list of something.
// Must derive from this object to have something useful.
template<class T>
class PriorityQueue : public JObject {
  //
protected:
  // Pointer to the next entry of the queue, or NULL
  // for the last one.
  T                   *m_pNext;
  //
  // Priority of this object
  ULONG                m_ulPrior;
  //
public:
  // Append the priorityQueue to an existing queue.
  PriorityQueue(T *&head,ULONG prior)
    : m_ulPrior(prior)
  {
    T **next = &head;
    //
    // Check whether we have a next entry. If so, insert here if our index is smaller
    // than that of the next item.
    while(*next && (*next)->m_ulPrior <= prior) {
      next = &((*next)->m_pNext);
    }
    // At this point, either next no longer exists, or its prior is larger than ours
    // and we must insert in front of it.
    m_pNext = *next;
    *next   = (T *)this;
  }
  //
  // Return the next element of this list.
  T *NextOf(void) const
  {
    return m_pNext;
  }
  //
  // Return the priority of this object.
  ULONG PriorOf(void) const
  {
    return m_ulPrior;
  }
  //
  // Remove this object from the priority queue given the head.
  void Remove(T *&head) const
  {
    const T *me = (const T *)this;
    T    **prev = &head;
    //
    while(*prev != me) {
      // if prev is NULL now, then the current node is not
      // in this list. How that?
      assert(*prev);
      prev = &((*prev)->m_pNext);
    }
    *prev  = m_pNext;
  }
  //
  // Attach another priority queue at the end of this queue, empty
  // the other queue.
  static void AttachQueue(T *&head,T *&other)
  {
    T **prev = &head;
    // 
    // First find the end of our buffer list.
    while(*prev)
      prev = &((*prev)->m_pNext);
    //
    *prev = other;
    other = NULL;
  }
  //
  // Find the last node of the given priority, or return NULL if there
  // is no such node. Note that the priority must match exactly here.
  static T *FindPriorityTail(T *head,ULONG prior)
  {
    T *next;

    while(head) {
      next = head->m_pNext;
      if (head->m_ulPrior == prior) {
        // A potential candiate for the node. Make this
        // a real candidate if the next node is absent
        // or has a higher priority.
        if (next == NULL || next->m_ulPrior > prior)
          return head;
      } else if (head->m_ulPrior > prior) {
        // Iterated too long, no candidate.
        break;
      }
      head = next;
    }
    return NULL;
  }
};
///

///
#endif


