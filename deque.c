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

#include "deque.h"
#include "dlist.h"

struct _deque {
    DList *q;
};

Deque* deque_create(void)
{
    Deque *new_deque;

    /* Deque container */
    new_deque = malloc(sizeof(struct _deque));
    if(NULL == new_deque) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    new_deque->q = dlist_create();
    if(NULL == new_deque->q) {
        fprintf(stderr, "Deque creation failed (%s:%d)\n", __FUNCTION__, __LINE__);
        free(new_deque);
        return NULL;
    }

    return new_deque;
}

/* Complexity: O(n) */
void deque_free(Deque *deque)
{
    assert(deque != NULL);

    /* Only free deque container and dlist container,
     * not dlist data  */
    dlist_free(deque->q);
    free(deque);
}

/* Complexity: O(n) */
void deque_free_all(Deque *deque)
{
    assert(deque != NULL);

    /* Free deque container, dlist, and dlist data  */
    dlist_free_all(deque->q);
    free(deque);
}

/* Complexity: O(1) */
int deque_push_front(Deque *deque, void *data)
{
    assert(deque != NULL);

    return dlist_prepend(deque->q, data);
}

/* Complexity: O(1) */
int deque_push_back(Deque *deque, void *data)
{
    assert(deque != NULL);

    return dlist_append(deque->q, data);
}

/* Complexity: O(1) */
void* deque_pop_front(Deque *deque)
{
    assert(deque != NULL);

    return dlist_remove_index(deque->q, 0);
}

/* Complexity: O(1) */
void* deque_pop_back(Deque *deque)
{
    assert(deque != NULL);

    return dlist_remove_index(deque->q, dlist_size(deque->q) - 1);
}

/* Complexity: O(1) */
void* deque_front(Deque *deque)
{
    assert(deque != NULL);

    return dlist_index(deque->q, 0);
}

/* Complexity: O(1) */
void* deque_back(Deque *deque)
{
    assert(deque != NULL);

    if(dlist_is_empty(deque->q)) {
        return NULL;
    }

    return dlist_index(deque->q, dlist_size(deque->q) - 1);
}

/* Complexity: O(1) */
int deque_is_empty(Deque *deque)
{
    assert(deque != NULL);

    return dlist_is_empty(deque->q);
}

/* Complexity: O(1) */
unsigned long deque_size(Deque *deque)
{
    assert(deque != NULL);

    return dlist_size(deque->q);
}
