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

#include <libcore/queue.h>
#include <libcore/slist.h>

struct _queue {
    SList *q;
};

Queue* queue_create(void)
{
    return (Queue *)slist_create();
}

/* Complexity: O(n) */
void queue_free(Queue *queue)
{
    assert(queue != NULL);

    /* Only free queue container and slist container,
     * not slist data  */
    slist_free((SList *)queue);
}

/* Complexity: O(n) */
void queue_free_all(Queue *queue, FreeFn freefn)
{
    assert(queue != NULL);

    /* Free queue container, slist, and slist data  */
    slist_free_all((SList *)queue, freefn);
}

/* Complexity: O(1) */
int queue_enqueue(Queue *queue, void *data)
{
    assert(queue != NULL);

    return slist_append((SList *)queue, data);
}

/* Complexity: O(1) */
void* queue_dequeue(Queue *queue)
{
    assert(queue != NULL);

    return slist_remove_index((SList *)queue, 0);
}

/* Complexity: O(1) */
void* queue_front(Queue *queue)
{
    assert(queue != NULL);

    return slist_index((SList *)queue, 0);
}

/* Complexity: O(1) */
void* queue_back(Queue *queue)
{
    assert(queue != NULL);

    if(slist_is_empty((SList *)queue)) {
        return NULL;
    }

    return slist_index((SList *)queue, slist_size((SList *)queue) - 1);
}

/* Complexity: O(1) */
int queue_is_empty(Queue *queue)
{
    assert(queue != NULL);

    return slist_is_empty((SList *)queue);
}

/* Complexity: O(1) */
unsigned long queue_size(Queue *queue)
{
    assert(queue != NULL);

    return slist_size((SList *)queue);
}
