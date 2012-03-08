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

#ifndef __LIBCORE_DARRAY_H__
#define __LIBCORE_DARRAY_H__

#if __cplusplus
extern "C" {
#endif

#include <libcore/types.h>

/* Opaque forward declaration */
typedef struct _darray DArray;

DArray* darray_create   (void);
void    darray_free     (DArray *darray);
void    darray_free_all (DArray *darray, FreeFn freefn);
int     darray_append   (DArray *darray, void *data);
int     darray_prepend  (DArray *darray, void *data);
int     darray_insert   (DArray *darray, void *data,
                         unsigned long index);
void*   darray_remove   (DArray *darray, unsigned long index);
void*   darray_index    (DArray *darray, unsigned long index);
int     darray_replace  (DArray *darray, unsigned long index,
                         void *data);
int     darray_swap     (DArray *darray, unsigned long index1,
                         unsigned long index2);
int     darray_concat   (DArray *darray1, DArray *darray2);
int     darray_sort     (DArray *darray, CompareFn comparefn);
int     darray_merge    (DArray *darray1, DArray *darray2,
                         CompareFn comparefn);

int     darray_is_sorted    (DArray *darray, CompareFn comparefn);
int     darray_is_empty     (DArray *darray);

unsigned long darray_size       (DArray *darray);
unsigned long darray_capacity   (DArray *darray);

#if __cplusplus
}
#endif

#endif
