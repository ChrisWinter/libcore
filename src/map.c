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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <libcore/rbtree.h>
#include <libcore/map.h>

struct _map {
    RBTree *m;
};

struct _map_iterator {
    RBTreeIterator *it;
};

/* Time Complexity: O(1) */
Map* map_create(CompareFn comparefn)
{
    return (Map *)rbtree_create(comparefn);
}

/* Time Complexity: O(1) */
void map_free(Map *map)
{
    assert(map != NULL);

    rbtree_free((RBTree *)map);
}

/* Time Complexity: O(|map|)
 * Space Complexity: O(2 * log(|map|))
 */
void map_free_all(Map *map, FreeFn freefn)
{
    assert(map != NULL);

    rbtree_free_all((RBTree *)map, freefn);
}

/* Time Complexity: O(log(|map|)) */
int map_insert(Map *map, const void *key, void *value)
{
    assert(map != NULL);

    return rbtree_insert_unique((RBTree *)map, key, value);
}

/* Time Complexity: O(log(|map|)) */
void* map_remove(Map *map, const void *key)
{
    assert(map != NULL);

    return rbtree_remove((RBTree *)map, key);
}

/* Time Complexity: O(1) */
int map_is_empty(Map *map)
{
    assert(map != NULL);

    return (map_size(map) == 0);
}

/* Time Complexity: O(1) */
unsigned long map_size(Map *map)
{
    assert(map != NULL);

    return rbtree_size((RBTree *)map);
}

/* Time Complexity: O(1) */
CompareFn map_get_comparefn(Map *map)
{
    assert(map != NULL);

    return rbtree_get_comparefn((RBTree *)map);
}

/* Time Complexity: O(log(|map|)) */
void* map_remove_at(Map *map, MapIterator *it)
{
    assert(map != NULL);
    assert(it != NULL);

    return rbtree_remove_at((RBTree *)map, (RBTreeIterator *)it);
}

/* Time Complexity: O(log(|map|)) */
MapIterator* map_find(Map *map, const void *key)
{
    assert(map != NULL);
    assert(key != NULL);

    return (MapIterator *)rbtree_find((RBTree *)map, key);
}

/* Time Complexity: O(log(|map|)) */
MapIterator* map_begin(Map *map)
{
    assert(map != NULL);

    return (MapIterator *)rbtree_begin((RBTree *)map);
}

/* Time Complexity: O(log(|map|)) */
MapIterator* map_end(Map *map)
{
    assert(map != NULL);

    return (MapIterator *)rbtree_end((RBTree *)map);
}

/* Time Complexity: O(log(|map|)) */
MapIterator* map_next(MapIterator *it)
{
    assert(it != NULL);

    return (MapIterator *)rbtree_next((RBTreeIterator *)it);
}

/* Time Complexity: O(log(|map|)) */
MapIterator* map_prev(MapIterator *it)
{
    assert(it != NULL);

    return (MapIterator *)rbtree_prev((RBTreeIterator *)it);
}

/* Time Complexity: O(1) */
const void* map_get_key(MapIterator *it)
{
    assert(it != NULL);

    return rbtree_get_key((RBTreeIterator *)it);
}

/* Time Complexity: O(1) */
void* map_get_value(MapIterator *it)
{
    assert(it != NULL);

    return rbtree_get_value((RBTreeIterator *)it);
}
