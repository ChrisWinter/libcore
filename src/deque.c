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

#include <libcore/deque.h>
#include <libcore/dlist.h>

struct _deque {
    DList *q;
};

Deque* deque_create(void)
{
    return (Deque*)dlist_create();
}

/* Complexity: O(n) */
void deque_free(Deque *deque)
{
    assert(deque != NULL);

    /* Only free deque container and dlist container,
     * not dlist data  */
    dlist_free((DList *)deque);
}

/* Complexity: O(n) */
void deque_free_all(Deque *deque, FreeFn freefn)
{
    assert(deque != NULL);

    /* Free deque container, dlist, and dlist data  */
    dlist_free_all((DList *)deque, freefn);
}

/* Complexity: O(1) */
int deque_push_front(Deque *deque, void *data)
{
    assert(deque != NULL);

    return dlist_prepend((DList *)deque, data);
}

/* Complexity: O(1) */
int deque_push_back(Deque *deque, void *data)
{
    assert(deque != NULL);

    return dlist_append((DList *)deque, data);
}

/* Complexity: O(1) */
void* deque_pop_front(Deque *deque)
{
    assert(deque != NULL);

    return dlist_remove_index((DList *)deque, 0);
}

/* Complexity: O(1) */
void* deque_pop_back(Deque *deque)
{
    assert(deque != NULL);

    return dlist_remove_index((DList *)deque, dlist_size((DList *)deque) - 1);
}

/* Complexity: O(1) */
void* deque_front(Deque *deque)
{
    assert(deque != NULL);

    return dlist_index((DList *)deque, 0);
}

/* Complexity: O(1) */
void* deque_back(Deque *deque)
{
    assert(deque != NULL);

    if(dlist_is_empty((DList *)deque)) {
        return NULL;
    }

    return dlist_index((DList *)deque, dlist_size((DList *)deque) - 1);
}

/* Complexity: O(1) */
int deque_is_empty(Deque *deque)
{
    assert(deque != NULL);

    return dlist_is_empty((DList *)deque);
}

/* Complexity: O(1) */
unsigned long deque_size(Deque *deque)
{
    assert(deque != NULL);

    return dlist_size((DList *)deque);
}
