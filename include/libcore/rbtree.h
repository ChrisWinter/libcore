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

#ifndef __LIBCORE_RBTREE_H__
#define __LIBCORE_RBTREE_H__

#if __cplusplus
extern "C" {
#endif

#include <libcore/types.h>

/* Opaque forward declarations */
typedef struct _rbtree RBTree;
typedef struct _rbtree_node RBTreeIterator;

RBTree* rbtree_create       (CompareFn comparefn);
void    rbtree_free         (RBTree *rbtree);
void    rbtree_free_all     (RBTree *rbtree, FreeFn freefn);
int     rbtree_insert_equal (RBTree *rbtree, const void *key, void *value);
int     rbtree_insert_unique(RBTree *rbtree, const void *key, void *value);
void*   rbtree_remove       (RBTree *rbtree, const void *key);
int     rbtree_is_empty     (RBTree *rbtree);
int     rbtree_is_valid     (RBTree *rbtree);

unsigned long   rbtree_size         (RBTree *rbtree);

CompareFn       rbtree_get_comparefn(RBTree *rbtree);

/* Iterators */
RBTreeIterator* rbtree_insert_equal_at  (RBTree *rbtree, RBTreeIterator *it,
                                         const void *key, void *value,
                                         int *success);
RBTreeIterator* rbtree_insert_unique_at (RBTree *rbtree, RBTreeIterator *it,
                                         const void *key, void *value,
                                         int *success);
void*           rbtree_remove_at    (RBTree *rbtree, RBTreeIterator *it);
RBTreeIterator* rbtree_find         (RBTree *rbtree, const void *key);
RBTreeIterator* rbtree_begin        (RBTree *rbtree);
RBTreeIterator* rbtree_end          (RBTree *rbtree);
RBTreeIterator* rbtree_next         (RBTreeIterator *it);
RBTreeIterator* rbtree_prev         (RBTreeIterator *it);

const void* rbtree_get_key      (RBTreeIterator *it);
void*       rbtree_get_value    (RBTreeIterator *it);

#if __cplusplus
}
#endif

#endif
