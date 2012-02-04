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

#include "priority_queue.h"
#include "heap.h"

struct _pqueue {
    Heap *h;
};

PQueue* pqueue_create(CompareFn comparefn)
{
    PQueue *new_pqueue;

    assert(comparefn != NULL);

    /* PQueue container */
    new_pqueue = malloc(sizeof(struct _pqueue));
    if(NULL == new_pqueue) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    /* Heap backend */
    new_pqueue->h = heap_create(comparefn);
    if(NULL == new_pqueue->h) {
        fprintf(stderr, "PQueue creation failed (%s:%d)\n", __FUNCTION__, __LINE__);
        free(new_pqueue);
        return NULL;
    }

    return new_pqueue;
}

/* Complexity: O(1) */
void pqueue_free(PQueue *pqueue)
{
    assert(pqueue != NULL);

    /* Only free pqueue container and heap container,
     * not the data stored in the pqueue */
    heap_free(pqueue->h);
    free(pqueue);
}

/* Complexity: O(n) */
void pqueue_free_all(PQueue *pqueue)
{
    assert(pqueue != NULL);

    /* Free pqueue and heap containers, and all data */
    heap_free_all(pqueue->h);
    free(pqueue);
}

/* Complexity: O(log n), worst-case */
int pqueue_push(PQueue *pqueue, void *data)
{
    assert(pqueue != NULL);

    return heap_push(pqueue->h, data);
}

/* Complexity: O(log n) */
void* pqueue_pop(PQueue *pqueue)
{
    assert(pqueue != NULL);

    return heap_pop(pqueue->h);
}

/* Complexity: O(1) */
void* pqueue_top(PQueue *pqueue)
{
    assert(pqueue != NULL);

    return heap_top(pqueue->h);
}

/* Complexity: O(1) */
int pqueue_is_empty(PQueue *pqueue)
{
    assert(pqueue != NULL);

    return heap_is_empty(pqueue->h);
}

/* Complexity: O(1) */
unsigned long pqueue_size(PQueue *pqueue)
{
    assert(pqueue != NULL);

    return heap_size(pqueue->h);
}
