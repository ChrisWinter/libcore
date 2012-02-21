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

#include "dlist.h"

#define head(dl)    (dl)->nil->next
#define tail(dl)    (dl)->nil->prev

struct _dlist_node {
    void *data;
    struct _dlist_node *next, *prev;
};

struct _dlist {
    struct _dlist_node *nil;
    unsigned long size;
};

static void _dlist_free(DList *dlist, int free_data, FreeFn freefn);
static struct _dlist_node* get_node_at_index(DList *dlist, unsigned long index);

static void _dlist_free(DList *dlist, int free_data, FreeFn freefn)
{
    struct _dlist_node *node;

    if(NULL == freefn) {
        /* Default to stdlib free */
        freefn = (FreeFn)free;
    }

    for(node = head(dlist); node != dlist->nil; node = head(dlist)) {
        if(free_data && node->data != NULL) {
            free(node->data);
        }
        head(dlist) = node->next;
        free(node);
    }

    free(dlist->nil);
    free(dlist);
}

/* Complexity: O(n/2), worst-case */
static struct _dlist_node* get_node_at_index(DList *dlist, unsigned long index)
{
    struct _dlist_node *node;
    unsigned long i;

    if(dlist_is_empty(dlist)) {
        return dlist->nil;
    }

    /* Optimize starting position to reduce
     * complexity to O(n/2)
     */
    if(index < (dlist->size / 2)) {
        for(node = head(dlist), i = 0;
                node != dlist->nil && i != index;
                node = node->next, i++) {
            /* No body */
        }
    } else {
        for(node = tail(dlist), i = (dlist->size - 1);
                node != dlist->nil && i != index;
                node = node->prev, i--) {
            /* No body */
        }
    }

    return node;
}

DList* dlist_create(void)
{
    DList *new_list;
    struct _dlist_node *sentinel;

    /* List container */
    new_list = malloc(sizeof(struct _dlist));
    if(NULL == new_list) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    /* Sentinel node */
    sentinel = malloc(sizeof(struct _dlist_node));
    if(NULL == sentinel) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    sentinel->data = NULL;
    sentinel->prev = sentinel;
    sentinel->next = sentinel;

    new_list->nil  = sentinel;

    new_list->size = 0;

    return new_list;
}

/* Complexity: O(n) */
void dlist_free(DList *dlist)
{
    assert(dlist != NULL);

    /* Only free dlist container and nodes, NOT node data  */
    _dlist_free(dlist, 0, NULL);
}

/* Complexity: O(n) */
void dlist_free_all(DList *dlist, FreeFn freefn)
{
    assert(dlist != NULL);

    /* Free dlist container, nodes, and node data  */
    _dlist_free(dlist, 1, freefn);
}

/* Complexity: O(1) */
int dlist_append(DList *dlist, void *data)
{
    return dlist_insert(dlist, data, dlist->size);
}

/* Complexity: O(1) */
int dlist_prepend(DList *dlist, void *data)
{
    return dlist_insert(dlist, data, 0);
}

/* Complexity: O(n/2), worst-case */
int dlist_insert(DList *dlist, void *data, unsigned long index)
{
    struct _dlist_node *node, *new_node;

    assert(dlist != NULL);
    assert(index <= dlist->size);

    new_node = malloc(sizeof(struct _dlist_node));
    if(NULL == new_node) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return -1;
    }

    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = NULL;

    node = get_node_at_index(dlist, index);

    new_node->prev = node->prev;
    new_node->next = node;

    node->prev->next = new_node;
    node->prev = new_node;

    dlist->size++;

    return 0;
}

/* Complexity: O(n/2), worst-case */
void* dlist_remove_index(DList *dlist, unsigned long index)
{
    struct _dlist_node *node;
    void *ret;

    assert(dlist != NULL);

    if(dlist_is_empty(dlist) || (index >= dlist->size)) {
        return NULL;
    }

    ret = NULL;

    node = get_node_at_index(dlist, index);

    assert(node != dlist->nil);

    ret = node->data;
    node->prev->next = node->next;
    node->next->prev = node->prev;

    free(node);
    dlist->size--;

    return ret;
}

/* Complexity: O(n), worst-case */
int dlist_remove_data(DList *dlist, const void *data)
{
    struct _dlist_node *node;

    assert(dlist != NULL);

    if(dlist_is_empty(dlist)) {
        return -1;
    }

    /* Terminates either with node = NULL for an empty list,
     * or with node pointing to the node at position index - 1
     */
    for(node = head(dlist);
            node != dlist->nil && node->data != data;
            node = node->next) {
        /* No body */
    }

    if(node == dlist->nil) {
        /* Matching node not found */
        return -1;
    } else {
        /* Found a match */
        node->prev->next = node->next;
        node->next->prev = node->prev;

        free(node);
        dlist->size--;
    }

    return 0;
}

/* Complexity: O(n), worst-case */
void* dlist_index(DList *dlist, unsigned long index)
{
    struct _dlist_node *node;

    assert(dlist != NULL);

    if(dlist_is_empty(dlist) || (index >= dlist->size)) {
        return NULL;
    }

    node = get_node_at_index(dlist, index);
    if(NULL == node) {
        return NULL;
    }

    return (node->data);
}

/* Complexity: O(1) */
int dlist_is_empty(DList *dlist)
{
    assert(dlist != NULL);

    return ((dlist->size == 0) &&
            (head(dlist) == tail(dlist)));
}

/* Complexity: O(1) */
unsigned long dlist_size(DList *dlist)
{
    assert(dlist != NULL);

    return (dlist->size);
}
