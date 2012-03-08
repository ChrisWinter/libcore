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

#include <libcore/darray.h>
#include <libcore/macros.h>
#include <libcore/utilities.h>

#define DARRAY_MIN_SIZE     32
#define SIZE_OF_VOIDP       sizeof(void *)

struct _darray {
    void **data;
    unsigned long size;
    unsigned long capacity;
};


static int darray_maybe_resize(DArray *darray, long nitems);
static int merge(DArray *src, unsigned long start, unsigned long middle,
        unsigned long end, CompareFn comparefn);
static int merge_sort(DArray *src,
        unsigned long start, unsigned long end, CompareFn comparefn);


/* nitems indicates if the DArray should grow or shrink. >0 if
 * adding items, <0 if removing items.
 */
static int darray_maybe_resize(DArray *darray, long nitems)
{
    void *new_data = NULL;
    unsigned long new_capacity = 0, pow2 = 0;

    if(0 == nitems) {
        return -1;
    }

    /* Grow or shrink the array. Use MAX because darray
     * capacity always starts off at zero.
     */
    if(nitems > 0) {
        pow2 = util_pow2_next(darray->capacity + nitems);
        if((darray->size + nitems) >= darray->capacity) {
            new_capacity = MAX(pow2, DARRAY_MIN_SIZE);
        }
    } else {
        pow2 = util_pow2_prev(darray->capacity - 1);
        /* Note: nitems is negative here */
        if((darray->size + nitems) < (pow2 >> 1)) {
            /* Only downsize if darray size is substantially
             * smaller than the previous power of 2 of our
             * current capacity. This attempts to eliminiate
             * excessive resizing if darray size just hovers
             * around a power of 2 boundary.
             */
            new_capacity = MAX(pow2, DARRAY_MIN_SIZE);
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

/* Complexity: O(n) space; O(n log n) time, worst-case */
static int merge(DArray *src, unsigned long start, unsigned long middle,
        unsigned long end, CompareFn comparefn)
{
    unsigned long start1, start2;
    DArray *tmp;

    tmp = darray_create();

    start1 = start;
    start2 = middle + 1;

    while(start1 <= middle && start2 <= end) {
        /* >= ensures merge sort is stable */
        if(comparefn(darray_index(src, start1),
                   darray_index(src, start2)) >= 0) {
            if(darray_append(tmp, darray_index(src, start1++)) < 0) {
                return -1;
            }
        } else {
            if(darray_append(tmp, darray_index(src, start2++)) < 0) {
                return -1;
            }
        }
    }

    while(start1 <= middle) {
        if(darray_append(tmp, darray_index(src, start1++)) < 0) {
            return -1;
        }
    }

    while(start2 <= end) {
        if(darray_append(tmp, darray_index(src, start2++)) < 0) {
            return -1;
        }
    }

    /* Replace the unsorted merge section of the src
     * array with the sorted merged contents of the
     * tmp array.
     */
    memmove(&src->data[start],
        tmp->data,
        (SIZE_OF_VOIDP * (end - start + 1)));

    darray_free(tmp);

    return 0;
}

/* Complexity: O(n log n) */
static int merge_sort(DArray *src,
        unsigned long start, unsigned long end, CompareFn comparefn)
{
    unsigned long middle;

    if(start < end) {
        middle = start + (end - start) / 2;
        merge_sort(src, start, middle, comparefn);
        merge_sort(src, middle + 1, end, comparefn);
        return merge(src, start, middle, end, comparefn);
    }

    return -1;
}

DArray* darray_create(void)
{
    DArray *a;

    a = malloc(sizeof(DArray));

    a->data = NULL;
    a->size = 0;
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
void darray_free_all(DArray *darray, FreeFn freefn)
{
    unsigned long i;
    void *item;

    if(NULL == freefn) {
        /* Default to stdlib free */
        freefn = (FreeFn)free;
    }

    if(darray != NULL) {
        if(darray->data != NULL) {
            for(i = 0; i < darray->size; i++) {
                item = darray->data[i];
                if(item != NULL) {
                    freefn(item);
                }
            }

            free(darray->data);
        }
        free(darray);
    }
}

/* Complexity: O(1) */
int darray_append(DArray *darray, void *data)
{
    assert(darray != NULL);

    if(darray_maybe_resize(darray, 1) < 0) {
        return -1;
    }

    darray->data[darray->size++] = data;

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

    if(darray->size > 0) {
        memmove(&darray->data[1],
            darray->data,
            SIZE_OF_VOIDP * darray->size);
    }

    darray->data[0] = data;

    darray->size++;

    return 0;
}

/* Complexity: O(n), worst-case */
int darray_insert(DArray *darray, void *data, unsigned long index)
{
    assert(darray != NULL);

    if(index > darray->size) {
        return -1;
    }

    if(darray_maybe_resize(darray, 1) < 0) {
        return -1;
    }

    if(darray->size > 0) {
        memmove(&darray->data[index + 1],
            &darray->data[index],
            SIZE_OF_VOIDP * (darray->size - index));
    }

    darray->data[index] = data;

    darray->size++;

    return 0;
}

/* Complexity: O(n), worst-case */
void* darray_remove(DArray *darray, unsigned long index)
{
    void *ret;

    assert(darray != NULL);
    assert(darray->data != NULL);
    assert(index < darray->size);

    ret = darray->data[index];

    memmove(&darray->data[index + 1],
        &darray->data[index],
        SIZE_OF_VOIDP * (darray->size - index - 1));

    darray->data[darray->size - 1] = NULL;
    darray->size--;

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
    assert(index < darray->size);

    return (darray->data[index]);
}

/* Complexity: O(1) */
int darray_replace(DArray *darray, unsigned long index, void *data)
{
    assert(darray != NULL);

    if(darray_is_empty(darray) || index >= darray->size) {
        return -1;
    }

    darray->data[index] = data;

    return 0;
}

/* Complexity: O(1) */
int darray_swap(DArray *darray, unsigned long index1, unsigned long index2)
{
    void *swp = NULL;

    assert(darray != NULL);

    if(darray_is_empty(darray) ||
            index1 >= darray->size || index2 >= darray->size) {
        return -1;
    }

    swp = darray_index(darray, index1);

    if(darray_replace(darray, index1, darray_index(darray, index2)) < 0) {
        return -1;
    }

    if(darray_replace(darray, index2, swp) < 0) {
        /* Try to restore the darray to its original state */
        darray_replace(darray, index1, swp);
        return -1;
    }

    return 0;
}

/* Complexity: O(size(darray2)) */
int darray_concat(DArray *darray1, DArray *darray2)
{
    unsigned long new_size, new_capacity;

    assert(darray1 != NULL);
    assert(darray2 != NULL);

    /* Don't try to concat the same darray to itself */
    if(darray1 == darray2) {
        return -1;
    }

    /* No work to do if darray2 is empty. If darray2 is
     * not empty then it can still be concatenated with
     * an empty darray1, which is why we don't check if
     * darray1 is empty.
     */
    if(darray_is_empty(darray2)) {
        return 0;
    }

    new_size = darray1->size + darray2->size;
    new_capacity = util_pow2_next(new_size);

    darray1->data = realloc(darray1->data, (SIZE_OF_VOIDP * new_capacity));
    if(NULL == darray1->data) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memmove(&darray1->data[darray1->size],
        darray2->data,
        (SIZE_OF_VOIDP * darray2->size));

    darray1->size = new_size;
    darray1->capacity = new_capacity;

    return 0;
}

/* Complexity: O(n log n), worst-case */
int darray_sort(DArray *darray, CompareFn comparefn)
{
    assert(darray != NULL);
    assert(comparefn != NULL);

    if(darray_is_empty(darray)) {
        return -1;
    }

    return merge_sort(darray, 0, (darray_size(darray) - 1), comparefn);
}

/* Complexity: O(n log n), worst-case */
int darray_merge(DArray *darray1, DArray *darray2, CompareFn comparefn)
{
    unsigned long middle;

    assert(darray1 != NULL);
    assert(darray2 != NULL);
    assert(comparefn != NULL);

    /* Don't merge if the darrays are the same */
    if(darray1 == darray2) {
        return -1;
    }

    /* No work to do if either darray is empty */
    if(darray_is_empty(darray1) || darray_is_empty(darray2)) {
        return -1;
    }

    /* Mid-point is the last element of darray1 */
    middle = darray1->size - 1;

    if(darray_concat(darray1, darray2) < 0) {
        return -1;
    }

    return merge(darray1, 0, middle, darray_size(darray1) - 1, comparefn);
}

/* Complexity: O(n) */
int darray_is_sorted(DArray *darray, CompareFn comparefn)
{
    unsigned long i;

    assert(darray != NULL);
    assert(comparefn != NULL);

    /* Is an empty array sorted? */
    if(darray_is_empty(darray)) {
        return 0;
    }

    if(darray_size(darray) < 2) {
        return 1;
    }

    for(i = 0; i < darray_size(darray) - 1; i++) {
        if(comparefn(darray_index(darray, i), darray_index(darray, i + 1)) < 0) {
            return 0;
        }
    }

    return 1;
}

/* Complexity: O(1) */
int darray_is_empty(DArray *darray)
{
    assert(darray != NULL);

    return (darray->size == 0);
}

/* Complexity: O(1) */
unsigned long darray_size(DArray *darray)
{
    assert(darray != NULL);

    return (darray->size);
}

/* Complexity: O(1) */
unsigned long darray_capacity(DArray *darray)
{
    assert(darray != NULL);

    return (darray->capacity);
}
