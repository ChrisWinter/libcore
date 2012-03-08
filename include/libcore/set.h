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
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SURBTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __LIBCORE_SET_H__
#define __LIBCORE_SET_H__

#if __cplusplus
extern "C" {
#endif

#include <libcore/types.h>

/* Opaque forward declarations */
typedef struct _set Set;
typedef struct _set_iterator SetIterator;

Set*    set_create      (CompareFn comparefn);
void    set_free        (Set *set);
void    set_free_all    (Set *set, FreeFn freefn);
int     set_insert      (Set *set, void *value);
void*   set_remove      (Set *set, const void *value);

Set*    set_union       (Set *set1, Set *set2);
Set*    set_intersect   (Set *set1, Set *set2);
Set*    set_diff        (Set *set1, Set *set2);
Set*    set_symdiff     (Set *set1, Set *set2);

int     set_is_equal    (Set *set1, Set *set2);
int     set_is_subset   (Set *set1, Set *set2);
int     set_is_member   (Set *set, void *value);
int     set_is_empty    (Set *set);

unsigned long set_size  (Set *set);

/* Iterators */
void*           set_remove_at    (Set *set, SetIterator *it);
SetIterator*    set_find         (Set *set, const void *value);
SetIterator*    set_begin        (Set *set);
SetIterator*    set_end          (Set *set);
SetIterator*    set_next         (SetIterator *it);
SetIterator*    set_prev         (SetIterator *it);

void*           set_get_value    (SetIterator *it);

#if __cplusplus
}
#endif

#endif
