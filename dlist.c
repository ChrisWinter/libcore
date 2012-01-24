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

struct _dlist_node {
    void *data;
    struct _dlist_node *next, *prev;
};

struct _dlist {
    struct _dlist_node *head, *tail;
    unsigned long length;
};

void _dlist_free(DList *dlist, int free_data);
struct _dlist_node* get_node_at_index(DList *dlist, unsigned long index);

void _dlist_free(DList *dlist, int free_data)
{
    struct _dlist_node *node;

    for(node = dlist->head; dlist->head != NULL; node = dlist->head) {
        if(free_data && node->data != NULL) {
            free(node->data);
        }
        dlist->head = node->next;
        free(node);
        node = NULL;
    }

    free(dlist);
}

/* Complexity: O(n/2), worst-case */
struct _dlist_node* get_node_at_index(DList *dlist, unsigned long index)
{
    struct _dlist_node *node;
    unsigned long i;

    /* Optimize starting position to reduce
     * complexity to O(n/2)
     */
    if(index < (dlist->length / 2)) {
        for(node = dlist->head->next, i = 0;
                node != NULL && i < index;
                node = node->next, i++) {
            /* No body */
        }
    } else {
        for(node = dlist->tail, i = dlist->length;
                node != NULL && i > index;
                node = node->prev, i--) {
            /* No body */
        }
    }

    return node;
}

DList* dlist_create(void)
{
    DList *new_list;
    struct _dlist_node *dummy_head, *dummy_tail;

    /* List container */
    new_list = malloc(sizeof(struct _dlist));
    if(NULL == new_list) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    /* Sentinel dummy head node */
    dummy_head = malloc(sizeof(struct _dlist_node));
    if(NULL == dummy_head) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    /* Sentinel dummy tail node */
    dummy_tail = malloc(sizeof(struct _dlist_node));
    if(NULL == dummy_tail) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    dummy_head->data = NULL;
    dummy_head->prev = NULL;
    dummy_head->next = dummy_tail;

    dummy_tail->data = NULL;
    dummy_tail->prev = dummy_head;
    dummy_tail->next = NULL;

    new_list->head = dummy_head;
    new_list->tail = dummy_tail;

    new_list->length = 0;

    return new_list;
}

/* Complexity: O(n) */
void dlist_free(DList *dlist)
{
    assert(dlist != NULL);

    /* Only free dlist container and nodes, NOT node data  */
    _dlist_free(dlist, 0);
}

/* Complexity: O(n) */
void dlist_free_all(DList *dlist)
{
    assert(dlist != NULL);

    /* Free dlist container, nodes, and node data  */
    _dlist_free(dlist, 1);
}

/* Complexity: O(1) */
int dlist_append(DList *dlist, void *data)
{
    assert(dlist != NULL);

    return dlist_insert(dlist, data, dlist->length);
}

/* Complexity: O(1) */
int dlist_prepend(DList *dlist, void *data)
{
    assert(dlist != NULL);

    return dlist_insert(dlist, data, 0);
}

/* Complexity: O(n/2), worst-case */
int dlist_insert(DList *dlist, void *data, unsigned long index)
{
    struct _dlist_node *node, *new_node;

    assert(dlist != NULL);
    assert(index <= dlist->length);

    new_node = malloc(sizeof(struct _dlist_node));
    if(NULL == new_node) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return -1;
    }

    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = NULL;

    node = get_node_at_index(dlist, index);
    if(NULL == node) {
        return -1;
    }

    new_node->prev = node->prev;
    new_node->next = node;

    node->prev->next = new_node;
    node->prev = new_node;

    dlist->length++;

    return 0;
}

/* Complexity: O(n/2), worst-case */
void* dlist_remove_index(DList *dlist, unsigned long index)
{
    struct _dlist_node *node;
    void *ret;

    assert(dlist != NULL);

    if(dlist_is_empty(dlist) || index >= dlist->length) {
        return NULL;
    }

    ret = NULL;

    node = get_node_at_index(dlist, index);
    if(NULL != node) {
        ret = node->data;
        node->prev->next = node->next;
        node->next->prev = node->prev;

        free(node);
        dlist->length--;
    }

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
    for(node = dlist->head->next;
            node->next != NULL && node->data != data;
            node = node->next) {
        /* No body */
    }

    if(NULL == node->next) {
        /* Matching node not found */
        return -1;
    } else {
        /* Found a match */
        node->prev->next = node->next;
        node->next->prev = node->prev;

        free(node);
        dlist->length--;
    }

    return 0;
}

/* Complexity: O(n), worst-case */
void* dlist_index(DList *dlist, unsigned long index)
{
    struct _dlist_node *node;

    assert(dlist != NULL);
    assert(index < dlist->length);

    if(dlist_is_empty(dlist) || index >= dlist->length) {
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

    return ((dlist->length == 0)
        && (dlist->head->next == dlist->tail)
        && (dlist->tail->prev == dlist->head));
}

/* Complexity: O(1) */
unsigned long dlist_length(DList *dlist)
{
    assert(dlist != NULL);

    return (dlist->length);
}
