/* Copyright (c) 2012, Chris Winter <wintercni@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __LIBCORE_DLIST_H__
#define __LIBCORE_DLIST_H__

#if __cplusplus
extern "C" {
#endif

#include <libcore/types.h>

/* Opaque forward declaration */
typedef struct _dlist DList;
typedef struct _dlist_node DListIterator;

DList*  dlist_create        (void);
void    dlist_free          (DList *dlist);
void    dlist_free_all      (DList *dlist, FreeFn freefn);
int     dlist_append        (DList *dlist, void *data);
int     dlist_prepend       (DList *dlist, void *data);
int     dlist_insert        (DList *dlist, void *data,
                             unsigned long index);
void*   dlist_remove_index  (DList *dlist, unsigned long index);
int     dlist_remove_data   (DList *dlist, const void *data);
void*   dlist_index         (DList *dlist, unsigned long index);
int     dlist_is_empty      (DList *dlist);

unsigned long dlist_size    (DList *dlist);

/* Iterators */
DListIterator*  dlist_begin (DList *dlist);
DListIterator*  dlist_end   (DList *dlist);
DListIterator*  dlist_next  (DListIterator *it);
DListIterator*  dlist_prev  (DListIterator *it);

DListIterator*  dlist_insert_at     (DListIterator *it, void *data);
DListIterator*  dlist_insert_before (DListIterator *it, void *data);
DListIterator*  dlist_insert_after  (DListIterator *it, void *data);

void*   dlist_remove_at (DListIterator *it);

void*   dlist_get_data  (DListIterator *it);

#if __cplusplus
}
#endif

#endif
