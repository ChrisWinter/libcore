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
#include <string.h>

#include "darray.h"
#include "utilities.h"

#define DARRAY_MIN_SIZE  32

struct _darray {
    char *data;
    unsigned long length;
    unsigned long capacity;
    size_t element_size;        /* Bytes    */
};

static int darray_maybe_resize(DArray *darray, long nitems);

DArray* darray_create(size_t element_size)
{
    DArray *a;

    a = malloc(sizeof(DArray));

    a->data = NULL;
    a->length = 0;
    a->capacity = 0;
    a->element_size = element_size;

    return a;
}

void darray_free(DArray *darray)
{
    if(darray != NULL) {
        if(darray->data != NULL) {
            free(darray->data);
        }
        free(darray);
    }
}

/* nitems indicates if the DArray should grow or shrink. >0 if
 * adding items, <0 if removing items.
 */
static int darray_maybe_resize(DArray *darray, long nitems)
{
    char *new_data = NULL;
    unsigned long new_capacity = darray->capacity;

    if(0 == nitems)
        return -1;

    /* Grow or shrink the array. Use MAX because darray
     * capacity always starts off at zero.
     */
    if((darray->length + nitems) >= darray->capacity) {
        new_capacity = MAX(darray->capacity * 2, DARRAY_MIN_SIZE);
    } else if((darray->length + nitems) < (darray->capacity / 2)) {
        new_capacity = MAX(darray->capacity / 2, DARRAY_MIN_SIZE);
    } else {
        /* No need to resize */
        return 0;
    }

    new_data = realloc(darray->data, (darray->element_size * new_capacity));

    if(NULL == new_data) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return -1;
    }

    darray->data = new_data;
    darray->capacity = new_capacity;

    return 0;
}

/* Complexity: O(1) */
int darray_append(DArray *darray, const void *data)
{
    assert(darray != NULL);
    assert(data != NULL);

    if(darray_maybe_resize(darray, 1) < 0)
        return -1;

    memcpy(darray->data + (darray->element_size * darray->length),
        data,
        darray->element_size);

    darray->length++;

    return 0;
}

/* Complexity: This is a very expensive operation. Each call
 *             is always O(n).
 */
int darray_prepend(DArray *darray, const void *data)
{
    assert(darray != NULL);
    assert(data != NULL);

    if(darray_maybe_resize(darray, 1) < 0)
        return -1;

    memmove(darray->data + darray->element_size,
        darray->data,
        darray->element_size * darray->length);

    memcpy(darray->data, data, darray->element_size);

    darray->length++;

    return 0;
}

/* Complexity: O(n), worst-case */
int darray_insert(DArray *darray, unsigned long index, const void *data)
{
    assert(darray != NULL);
    assert(data != NULL);
    assert(index < darray->length);

    if(darray_maybe_resize(darray, 1) < 0)
        return -1;

    memmove(darray->data + (darray->element_size * (index + 1)),
        darray->data + (darray->element_size * index),
        darray->element_size * (darray->length - index));

    memcpy(darray->data + (darray->element_size * index),
        data,
        darray->element_size);

    darray->length++;

    return 0;
}

/* Complexity: O(n), worst-case */
int darray_remove(DArray *darray, unsigned long index)
{
    assert(darray != NULL);
    assert(index < darray->length);

    memmove(darray->data + (darray->element_size * (index + 1)),
        darray->data + (darray->element_size * index),
        darray->element_size * (darray->length - index - 1));

    memcpy(darray->data + (darray->element_size * index),
        darray->data,
        darray->element_size);

    darray->length--;

    if(darray_maybe_resize(darray, -1) < 0)
        return -1;

    return 0;
}

/* Complexity: O(1) */
void* darray_index(DArray *darray, unsigned long index)
{
    assert(darray != NULL);
    assert(index < darray->length);

    return (void *)(darray->data + (darray->element_size * index));
}

/*
 * Complexity: O(n), worst-case
 *
 * Note: Caller is responsible for freeing returned DArray.
 */
DArray* darray_slice(DArray *darray, unsigned long start_index, unsigned long end_index)
{
    DArray *ret;
    unsigned long new_length, new_capacity;

    assert(darray != NULL);
    assert(start_index <= end_index);
    assert(end_index < darray->length);
    
    /* Can't slice a zero length darray */
    if(darray->length == 0)
        return NULL;

    /* Set up the new DArray */
    ret = darray_create(darray->element_size);
    new_length = end_index - start_index + 1;
    new_capacity = new_length + DARRAY_MIN_SIZE;

    ret->data = malloc(darray->element_size * new_capacity);
    if(NULL == ret->data) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    /* Copy over the slice contents */
    memcpy(ret->data,
        darray->data + (darray->element_size * start_index),
        darray->element_size * new_length);

    ret->length = new_length;
    ret->capacity = new_capacity;

    return ret;
}

unsigned long darray_get_length(DArray *darray)
{
    assert(darray != NULL);

    return (darray->length);
}

unsigned long darray_get_capacity(DArray *darray)
{
    assert(darray != NULL);

    return (darray->capacity);
}
