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

#ifndef __LIBCORE_MAP_H__
#define __LIBCORE_MAP_H__

#if __cplusplus
extern "C" {
#endif

#include <libcore/types.h>

/* Opaque forward declarations */
typedef struct _map Map;
typedef struct _map_iterator MapIterator;

Map*    map_create      (CompareFn comparefn);
void    map_free        (Map *map);
void    map_free_all    (Map *map, FreeFn freefn);
int     map_insert      (Map *map, const void *key, void *value);
void*   map_remove      (Map *map, const void *key);

unsigned long   map_size            (Map *map);

CompareFn       map_get_comparefn   (Map *map);

/* Iterators */
void*           map_remove_at   (Map *map, MapIterator *it);
MapIterator*    map_find        (Map *map, const void *key);
MapIterator*    map_begin       (Map *map);
MapIterator*    map_end         (Map *map);
MapIterator*    map_next        (MapIterator *it);
MapIterator*    map_prev        (MapIterator *it);

const void*     map_get_key     (MapIterator *it);
void*           map_get_value   (MapIterator *it);

#if __cplusplus
}
#endif

#endif
