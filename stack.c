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

#include "stack.h"
#include "slist.h"

struct _stack {
    SList *s;
};

Stack* stack_create(void)
{
    Stack *new_stack;

    /* Stack container */
    new_stack = malloc(sizeof(struct _stack));
    if(NULL == new_stack) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    new_stack->s = slist_create();
    if(NULL == new_stack->s) {
        fprintf(stderr, "Stack creation failed (%s:%d)\n", __FUNCTION__, __LINE__);
        free(new_stack);
        return NULL;
    }

    return new_stack;
}

/* Complexity: O(n) */
void stack_free(Stack *stack)
{
    assert(stack != NULL);

    /* Only free stack container and slist container,
     * not slist data  */
    slist_free(stack->s);
    free(stack);
}

/* Complexity: O(n) */
void stack_free_all(Stack *stack)
{
    assert(stack != NULL);

    /* Free stack container, slist, and slist data  */
    slist_free_all(stack->s);
    free(stack);
}

/* Complexity: O(1) */
int stack_push(Stack *stack, void *data)
{
    assert(stack != NULL);

    return slist_prepend(stack->s, data);
}

/* Complexity: O(1) */
void* stack_pop(Stack *stack)
{
    assert(stack != NULL);

    return slist_remove_index(stack->s, 0);
}

/* Complexity: O(1) */
void* stack_top(Stack *stack)
{
    assert(stack != NULL);

    return slist_index(stack->s, 0);
}

/* Complexity: O(1) */
int stack_is_empty(Stack *stack)
{
    assert(stack != NULL);

    return slist_is_empty(stack->s);
}

/* Complexity: O(1) */
unsigned long stack_size(Stack *stack)
{
    assert(stack != NULL);

    return slist_size(stack->s);
}
