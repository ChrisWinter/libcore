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

#include "slist.h"

struct _slist_node {
    void *data;
    struct _slist_node *next;
};

struct _slist {
    struct _slist_node *head, *tail;
    unsigned long length;
};

void _slist_free(SList *slist, int free_data);
struct _slist_node* get_node_at_index(SList *slist, unsigned long index);

void _slist_free(SList *slist, int free_data)
{
    struct _slist_node *node;

    for(node = slist->head; node != NULL; node = slist->head) {
        if(free_data && node->data != NULL) {
            free(node->data);
        }
        slist->head = node->next;
        free(node);
    }

    free(slist);
}

struct _slist_node* get_node_before_index(SList *slist, unsigned long index)
{
    struct _slist_node *node;
    unsigned long i;

    for(node = slist->head, i = 0;
            node != NULL && i < index;
            node = node->next, i++) {
        /* No body */
    }

    return node;
}

SList* slist_create(void)
{
    SList *new_list;
    struct _slist_node *dummy;

    /* List container */
    new_list = malloc(sizeof(struct _slist));
    if(NULL == new_list) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    /* Sentinel dummy node */
    dummy = malloc(sizeof(struct _slist_node));
    if(NULL == dummy) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    dummy->data = NULL;
    dummy->next = NULL;

    new_list->head = dummy;
    new_list->tail = dummy;

    new_list->length = 0;

    return new_list;
}

/* Complexity: O(n) */
void slist_free(SList *slist)
{
    assert(slist != NULL);

    /* Only free slist container and nodes, not node data  */
    _slist_free(slist, 0);
}

/* Complexity: O(n) */
void slist_free_all(SList *slist)
{
    assert(slist != NULL);

    /* Free slist container, nodes, and node data  */
    _slist_free(slist, 1);
}

/* Complexity: O(1) */
int slist_append(SList *slist, void *data)
{
    assert(slist != NULL);

    return slist_insert(slist, data, slist->length);
}

/* Complexity: O(1) */
int slist_prepend(SList *slist, void *data)
{
    assert(slist != NULL);

    return slist_insert(slist, data, 0);
}

/* Complexity: O(n), worst-case */
int slist_insert(SList *slist, void *data, unsigned long index)
{
    struct _slist_node *node, *new_node;

    assert(slist != NULL);
    assert(index <= slist->length);

    new_node = malloc(sizeof(struct _slist_node));
    if(NULL == new_node) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return -1;
    }

    new_node->data = data;
    new_node->next = NULL;

    /* Special case for append */
    if(index == slist->length) {
        node = slist->tail;
    } else {
        node = get_node_before_index(slist, index);
    }

    if(NULL == node) {
        return -1;
    }

    new_node->next = node->next;
    node->next = new_node;

    /* Update the tail if appending */
    if(index == slist->length) {
        slist->tail = new_node;
    }

    slist->length++;

    return 0;
}

/* Complexity: O(n), worst-case */
void* slist_remove_index(SList *slist, unsigned long index)
{
    struct _slist_node *node, *tmp;
    void *ret;

    assert(slist != NULL);

    if(slist_is_empty(slist) || index >= slist->length) {
        return NULL;
    }

    ret = NULL;

    node = get_node_before_index(slist, index);

    tmp = node->next;
    ret = tmp->data;
    node->next = tmp->next;

    /* Update tail if removing from end */
    if(index == slist->length - 1) {
        slist->tail = node;
    }

    free(tmp);
    slist->length--;

    return ret;
}

/* Complexity: O(n), worst-case */
int slist_remove_data(SList *slist, const void *data)
{
    struct _slist_node *node, *tmp;

    assert(slist != NULL);

    if(slist_is_empty(slist)) {
        return -1;
    }

    /* Terminates either with node = NULL for an empty list,
     * or with node pointing to the node at position index - 1
     */
    for(node = slist->head;
            node->next != NULL && node->next->data != data;
            node = node->next) {
        /* No body */
    }

    if(NULL == node->next) {
        /* Matching node not found */
        return -1;
    } else {
        /* Found a match */
        tmp = node->next;
        node->next = tmp->next;

        /* Update tail if removing from end */
        if(tmp == slist->tail) {
            slist->tail = node;
        }

        free(tmp);
        slist->length--;
    }

    return 0;
}

/* Complexity: O(n), worst-case */
void* slist_index(SList *slist, unsigned long index)
{
    struct _slist_node *node;
    unsigned long i;

    assert(slist != NULL);
    assert(index < slist->length);

    if(slist_is_empty(slist) || index >= slist->length) {
        return NULL;
    }

    /* Special case for indexing the last
     * node in the list
     */
    if(index == slist->length - 1) {
        node = slist->tail;
    } else {
        for(node = slist->head->next, i = 0;
                node != NULL && i < index;
                node = node->next, i++) {
            /* No body */
        }
    }

    return (node->data);
}

/* Complexity: O(1) */
int slist_is_empty(SList *slist)
{
    assert(slist != NULL);

    return ((slist->length == 0) && (slist->head == slist->tail));
}

/* Complexity: O(1) */
unsigned long slist_length(SList *slist)
{
    assert(slist != NULL);

    return (slist->length);
}
