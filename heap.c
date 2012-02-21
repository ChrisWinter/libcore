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

#include "heap.h"
#include "darray.h"

struct _heap {
    DArray *h;
    CompareFn comparefn;
};


static unsigned long parent_of(unsigned long index);
static unsigned long left_child_of(unsigned index);
static unsigned long right_child_of(unsigned index);
static void heapify_up(Heap *heap, unsigned long index);
static void heapify_down(Heap *heap, unsigned long index);


static unsigned long parent_of(unsigned long index)
{
    return ((index - 1) / 2);
}

static unsigned long left_child_of(unsigned index)
{
    return ((2 * index) + 1);
}

static unsigned long right_child_of(unsigned index)
{
    return ((2 * index) + 2);
}

/* Complexity: O(log n) */
static void heapify_up(Heap *heap, unsigned long index)
{
    while(index > 0 &&
            heap->comparefn(darray_index(heap->h, index),
                            darray_index(heap->h, parent_of(index))) > 0) {
        darray_swap(heap->h, index, parent_of(index));
        index = parent_of(index);
    }
}

/* Complexity: O(log n) */
static void heapify_down(Heap *heap, unsigned long index)
{
    unsigned long left, right, largest;
    int done;

    done = 0;

    while(!done) {
        left = left_child_of(index);
        right = right_child_of(index);

        largest = index;

        if(left < heap_size(heap) &&
                heap->comparefn(darray_index(heap->h, left),
                                darray_index(heap->h, index)) > 0) {
            largest = left;
        }

        if(right < heap_size(heap) &&
                heap->comparefn(darray_index(heap->h, right),
                                darray_index(heap->h, largest)) > 0) {
            largest = right;
        }

        if(largest != index) {
            darray_swap(heap->h, index, largest);
            index = largest;
        } else {
            done = 1;
        }
    }
}

Heap* heap_create(CompareFn comparefn)
{
    Heap *new_heap;

    assert(comparefn != NULL);

    /* Heap container */
    new_heap = malloc(sizeof(struct _heap));
    if(NULL == new_heap) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    new_heap->h = darray_create();
    if(NULL == new_heap->h) {
        fprintf(stderr, "Heap creation failed (%s:%d)\n", __FUNCTION__, __LINE__);
        free(new_heap);
        return NULL;
    }

    new_heap->comparefn = comparefn;

    return new_heap;
}

/* Complexity: o(1) */
void heap_free(Heap *heap)
{
    assert(heap != NULL);

    /* Only free heap container and darray container,
     * not the data stored in the heap */
    darray_free(heap->h);
    free(heap);
}

/* Complexity: O(n) */
void heap_free_all(Heap *heap, FreeFn freefn)
{
    assert(heap != NULL);

    /* Free heap and darray containers, and all data */
    darray_free_all(heap->h, freefn);
    free(heap);
}

/* Complexity: O(log n), worst-case */
int heap_push(Heap *heap, void *data)
{
    assert(heap != NULL);

    darray_append(heap->h, data);
    heapify_up(heap, darray_size(heap->h) - 1);

    return 0;
}

/* Complexity: O(log n) */
void* heap_pop(Heap *heap)
{
    void *ret = NULL;

    assert(heap != NULL);

    if(heap_is_empty(heap)) {
        return NULL;
    }

    if(darray_swap(heap->h, 0, darray_size(heap->h) - 1) < 0) {
        return NULL;
    }

    ret = darray_remove(heap->h, darray_size(heap->h) - 1);
    heapify_down(heap, 0);

    return ret;
}

/* Complexity: O(1) */
void* heap_top(Heap *heap)
{
    assert(heap != NULL);

    if(heap_is_empty(heap)) {
        return NULL;
    }

    return darray_index(heap->h, 0);
}

/* Complexity: O(n log n), worst-case if data is located
 * at the right-most leaf node on the lowest level of the
 * tree.
 */
int heap_remove(Heap *heap, const void *data)
{
    unsigned long i;

    assert(heap != NULL);

    if(heap_is_empty(heap)) {
        return -1;
    }

    for(i = 0; i < heap_size(heap); i++) {
        if(darray_index(heap->h, i) == data) {
            if(darray_swap(heap->h, i, darray_size(heap->h) - 1) < 0) {
                return -1;
            }

            /* Don't care about the return value */
            darray_remove(heap->h, darray_size(heap->h) - 1);
            heapify_down(heap, i);

            return 0;
        }
    }

    return -1;
}

/* Complexity: O(size(heap1) + 2 * size(heap2)) => O(n) */
int heap_merge(Heap *heap1, Heap* heap2)
{
    unsigned long i;

    assert(heap1 != NULL);
    assert(heap2 != NULL);

    if(heap_is_empty(heap1) && heap_is_empty(heap2)) {
        return -1;
    }

    /* O(size(heap2)) */
    if(darray_concat(heap1->h, heap2->h) < 0) {
        return -1;
    }

    /* O(size(heap1) + size(heap2)) */
    for(i = (darray_size(heap1->h) - 1) / 2; i > 0; i--) {
        heapify_down(heap1, i);
    }
    heapify_down(heap1, 0); /* Edge-case for loop 0 index */

    return 0;
}

/* Complexity: O(1) */
int heap_is_empty(Heap *heap)
{
    assert(heap != NULL);

    return darray_is_empty(heap->h);
}

/* Complexity: O(n)
 *
 * Checks that the heap relational property is valid
 * throughout the heap. Looks at every node.
 */
int heap_is_valid(Heap *heap)
{
    unsigned long i;

    assert(heap != NULL);

    if(heap_size(heap) < 2) {
        return 1;
    }

    for(i = 1; i < heap_size(heap); i++) {
        if(heap->comparefn(darray_index(heap->h, parent_of(i)),
                           darray_index(heap->h, i)) < 0) {
            return 0;
        }
    }

    return 1;

    /* return check_heap(heap, 0); */
}

/* Complexity: O(1) */
unsigned long heap_size(Heap *heap)
{
    assert(heap != NULL);

    return darray_size(heap->h);
}
