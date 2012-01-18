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

#define DARRAY_MIN_SIZE     32
#define SIZE_OF_VOIDP       sizeof(void *)

struct _darray {
    void **data;
    unsigned long length;
    unsigned long capacity;
};

static int darray_maybe_resize(DArray *darray, long nitems);

DArray* darray_create(void)
{
    DArray *a;

    a = malloc(sizeof(DArray));

    a->data = NULL;
    a->length = 0;
    a->capacity = 0;

    return a;
}

/* Complexity: O(1) */
void darray_free(DArray *darray)
{
    if(darray != NULL) {
        if(darray->data != NULL) {
            free(darray->data);
            darray->data = NULL;
        }
        free(darray);
    }
}

/* Complexity: O(n) */
void darray_free_all(DArray *darray)
{
    unsigned long i;
    void *item;

    if(darray != NULL) {
        if(darray->data != NULL) {
            for(i = 0; i < darray->length; i++) {
                item = darray->data[i];
                if(item != NULL) {
                    free(item);
                }
            }

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
    void *new_data = NULL;
    unsigned long new_capacity = 0;

    if(0 == nitems) {
        return -1;
    }

    /* Grow or shrink the array. Use MAX because darray
     * capacity always starts off at zero.
     */
    if(nitems > 0) {
        if((darray->length + nitems) >= darray->capacity) {
            new_capacity = MAX(darray->capacity * 2, DARRAY_MIN_SIZE);
        }
    } else {
        if((darray->length + nitems) < (darray->capacity / 2)) {
            new_capacity = MAX(darray->capacity / 2, DARRAY_MIN_SIZE);
        }
    }

    /* No need to resize */
    if(0 == new_capacity) {
        return 0;
    }

    new_data = realloc(darray->data, (SIZE_OF_VOIDP * new_capacity));

    if(NULL == new_data) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return -1;
    }

    darray->data = new_data;
    darray->capacity = new_capacity;

    return 0;
}

/* Complexity: O(1) */
int darray_append(DArray *darray, void *data)
{
    assert(darray != NULL);

    if(darray_maybe_resize(darray, 1) < 0) {
        return -1;
    }

    darray->data[darray->length++] = data;

    return 0;
}

/* Complexity: This is a very expensive operation. Each call
 *             is always O(n).
 */
int darray_prepend(DArray *darray, void *data)
{
    assert(darray != NULL);
    assert(data != NULL);

    if(darray_maybe_resize(darray, 1) < 0) {
        return -1;
    }

    if(darray->length > 0) {
        memmove(&darray->data[1],
            darray->data,
            SIZE_OF_VOIDP * darray->length);
    }

    darray->data[0] = data;

    darray->length++;

    return 0;
}

/* Complexity: O(n), worst-case */
int darray_insert(DArray *darray, void *data, unsigned long index)
{
    assert(darray != NULL);
    assert(data != NULL);
    assert(index <= darray->length);

    if(darray_maybe_resize(darray, 1) < 0) {
        return -1;
    }

    if(darray->length > 0) {
        memmove(&darray->data[index + 1],
            &darray->data[index],
            SIZE_OF_VOIDP * (darray->length - index));
    }

    darray->data[index] = data;

    darray->length++;

    return 0;
}

/* Complexity: O(n), worst-case */
void* darray_remove(DArray *darray, unsigned long index)
{
    void *ret;

    assert(darray != NULL);
    assert(darray->data != NULL);
    assert(index < darray->length);

    ret = darray->data[index];

    memmove(&darray->data[index + 1],
        &darray->data[index],
        SIZE_OF_VOIDP * (darray->length - index - 1));

    darray->data[darray->length - 1] = NULL;
    darray->length--;

    if(darray_maybe_resize(darray, -1) < 0) {
        fprintf(stderr, "DArray resize failed (%s:%d)\n", __FUNCTION__, __LINE__);
    }

    return ret;
}

/* Complexity: O(1) */
void* darray_index(DArray *darray, unsigned long index)
{
    assert(darray != NULL);
    assert(darray->data != NULL);
    assert(index < darray->length);

    return (darray->data[index]);
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
