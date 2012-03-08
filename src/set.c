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

#include <libcore/rbtree.h>
#include <libcore/set.h>

struct _set {
    RBTree *s;
};

struct _set_iterator {
    RBTreeIterator *it;
};

/* Time Complexity: O(1) */
Set* set_create(CompareFn comparefn)
{
    return (Set *)rbtree_create(comparefn);
}

/* Time Complexity: O(1) */
void set_free(Set *set)
{
    assert(set != NULL);

    rbtree_free((RBTree *)set);
}

/* Time Complexity: O(|set|)
 * Space Complexity: O(2 * log(|set|))
 */
void set_free_all(Set *set, FreeFn freefn)
{
    assert(set != NULL);

    rbtree_free_all((RBTree *)set, freefn);
}

/* Time Complexity: O(log(|set|)) */
int set_insert(Set *set, void *value)
{
    assert(set != NULL);

    return rbtree_insert_unique((RBTree *)set, value, value);
}

/* Time Complexity: O(log(|set|)) */
void* set_remove(Set *set, const void *value)
{
    assert(set != NULL);

    return rbtree_remove((RBTree *)set, value);
}

/* Time Complexity: O(|set1| * log(|set1|) + |set2| * log(|set1| + |set2|)) */
Set* set_union(Set *set1, Set *set2)
{
    SetIterator *it;
    Set *ret;

    assert(set1 != NULL);
    assert(set2 != NULL);

    ret = set_create(rbtree_get_comparefn((RBTree *)set1));
    if(ret == NULL) {
        return NULL;
    }

    for(it = set_begin(set1); it != NULL; it = set_next(it)) {
        set_insert(ret, set_get_value(it));
    }
    for(it = set_begin(set2); it != NULL; it = set_next(it)) {
        set_insert(ret, set_get_value(it));
    }

    return ret;
}

/* Time Complexity: O(|set1| * log(|set2|)) */
Set* set_intersect(Set *set1, Set *set2)
{
    CompareFn comparefn1, comparefn2;
    SetIterator *it1,*it2;
    int result;
    Set *ret;

    assert(set1 != NULL);
    assert(set2 != NULL);

    comparefn1 = rbtree_get_comparefn((RBTree *)set1);
    comparefn2 = rbtree_get_comparefn((RBTree *)set2);

    ret = set_create(comparefn1);
    if(ret == NULL) {
        return NULL;
    }

    /* A \cap B = {x : x \in A and x \in B}  */
    if(comparefn1 == comparefn2) {
        /* O(|set1| + |set2|), supposedly... */
        for(it1 = set_begin(set1), it2 = set_begin(set2);
                it1 != NULL && it2 != NULL; ) {
            result = comparefn1(set_get_value(it1), set_get_value(it2));
            if(result == 0) {
                set_insert(ret, set_get_value(it1));
                it1 = set_next(it1);
                it2 = set_next(it2);
            } else if(result < 0) {
                it1 = set_next(it1);
            } else {
                it2 = set_next(it2);
            }
        }
    } else {
        for(it1 = set_begin(set1); it1 != NULL; it1 = set_next(it1)) {
            if(set_is_member(set2, set_get_value(it1))) {
                set_insert(ret, set_get_value(it1));
            }
        }
    }

    return ret;
}

/* Time Complexity: O(|set1| * log(|set2|)) */
Set* set_diff(Set *set1, Set *set2)
{
    SetIterator *it1, *it2;
    CompareFn comparefn1, comparefn2;
    int result;
    Set *ret;

    assert(set1 != NULL);
    assert(set2 != NULL);

    comparefn1 = rbtree_get_comparefn((RBTree *)set1);
    comparefn2 = rbtree_get_comparefn((RBTree *)set2);

    ret = set_create(comparefn1);
    if(ret == NULL) {
        return NULL;
    }

    /* A - B = {x : x \in A and x \notin B}  */
    if(comparefn1 == comparefn2) {
        /* O(|set1| + |set2|), supposedly... */
        for(it1 = set_begin(set1), it2 = set_begin(set2);
                it1 != NULL && it2 != NULL; ) {
            result =  comparefn1(set_get_value(it1), set_get_value(it2));
            if(result == 0) {
                it1 = set_next(it1);
                it2 = set_next(it2);
            } else if(result < 0) {
                set_insert(ret, set_get_value(it1));
                it1 = set_next(it1);
            } else {
                it2 = set_next(it2);
            }
        }

        while(it1 != NULL) {
            set_insert(ret, set_get_value(it1));
            it1 = set_next(it1);
        }
    } else {
        for(it1 = set_begin(set1); it1 != NULL; it1 = set_next(it1)) {
            if(!set_is_member(set2, set_get_value(it1))) {
                set_insert(ret, set_get_value(it1));
            }
        }
    }

    return ret;
}

/* Time Complexity: O(|set2| * log(|set1|)) */
Set* set_symdiff(Set *set1, Set *set2)
{
    SetIterator *it1, *it2;
    CompareFn comparefn1, comparefn2;
    int result;
    Set *ret;

    assert(set1 != NULL);
    assert(set2 != NULL);

    comparefn1 = rbtree_get_comparefn((RBTree *)set1);
    comparefn2 = rbtree_get_comparefn((RBTree *)set2);

    /* A \ominus B = (A - B) \cup (B - A)  */
    if(comparefn1 == comparefn2) {
        ret = set_create(comparefn1);
        if(ret == NULL) {
            return NULL;
        }

        /* O(|set1| + |set2|), supposedly... */
        for(it1 = set_begin(set1), it2 = set_begin(set2);
                it1 != NULL && it2 != NULL; ) {
            result =  comparefn1(set_get_value(it1), set_get_value(it2));
            if(result == 0) {
                it1 = set_next(it1);
                it2 = set_next(it2);
            } else if(result < 0) {
                set_insert(ret, set_get_value(it1));
                it1 = set_next(it1);
            } else {
                set_insert(ret, set_get_value(it2));
                it2 = set_next(it2);
            }
        }

        while(it1 != NULL) {
            set_insert(ret, set_get_value(it1));
            it1 = set_next(it1);
        }

        while(it2 != NULL) {
            set_insert(ret, set_get_value(it2));
            it2 = set_next(it2);
        }
    } else {
        ret = set_diff(set1, set2);

        for(it1 = set_begin(set2); it1 != NULL; it1 = set_next(it1)) {
            if(!set_is_member(set1, set_get_value(it1))) {
                set_insert(ret, set_get_value(it1));
            }
        }
    }

    return ret;
}

/* Time Complexity: O(|set2| * log(|set1|)) */
int set_is_equal(Set *set1, Set *set2)
{
    SetIterator *it;

    assert(set1 != NULL);
    assert(set2 != NULL);

    /* set2 should be the same size as set1 */
    if(set_size(set1) != set_size(set2)) {
        return 0;
    }

    for(it = set_begin(set2); it != NULL; it = set_next(it)) {
        if(!set_is_member(set1, set_get_value(it))) {
            return 0;
        }
    }

    return 1;
}

/* Time Complexity: O(|set2| * log(|set1|)) */
int set_is_subset(Set *set1, Set *set2)
{
    SetIterator *it;

    assert(set1 != NULL);
    assert(set2 != NULL);

    /* set2 size should be smaller than or equal to set1 */
    if(set_size(set1) < set_size(set2)) {
        return 0;
    }

    for(it = set_begin(set2); it != NULL; it = set_next(it)) {
        if(!set_is_member(set1, set_get_value(it))) {
            return 0;
        }
    }

    return 1;
}

/* Time Complexity: O(log(|set|)) */
int set_is_member(Set *set, void *value)
{
    RBTreeIterator *found;

    assert(set != NULL);
    assert(value != NULL);

    found = rbtree_find((RBTree *)set, value);
    if(found != NULL) {
        return 1;
    }

    return 0;
}

/* Time Complexity: O(1) */
int set_is_empty(Set *set)
{
    assert(set != NULL);

    return (set_size(set) == 0);
}

/* Time Complexity: O(1) */
unsigned long set_size(Set *set)
{
    assert(set != NULL);

    return rbtree_size((RBTree *)set);
}

/* Time Complexity: O(log(|set|)) */
void* set_remove_at(Set *set, SetIterator *it)
{
    assert(set != NULL);
    assert(it != NULL);

    return rbtree_remove_at((RBTree *)set, (RBTreeIterator *)it);
}

/* Time Complexity: O(log(|set|)) */
SetIterator* set_find(Set *set, const void *value)
{
    assert(set != NULL);
    assert(value != NULL);

    return (SetIterator *)rbtree_find((RBTree *)set, value);
}

/* Time Complexity: O(log(|set|)) */
SetIterator* set_begin(Set *set)
{
    assert(set != NULL);

    return (SetIterator *)rbtree_begin((RBTree *)set);
}

/* Time Complexity: O(log(|set|)) */
SetIterator* set_end(Set *set)
{
    assert(set != NULL);

    return (SetIterator *)rbtree_end((RBTree *)set);
}

/* Time Complexity: O(log(|set|)) */
SetIterator* set_next(SetIterator *it)
{
    assert(it != NULL);

    return (SetIterator *)rbtree_next((RBTreeIterator *)it);
}

/* Time Complexity: O(log(|set|)) */
SetIterator* set_prev(SetIterator *it)
{
    assert(it != NULL);

    return (SetIterator *)rbtree_prev((RBTreeIterator *)it);
}

/* Time Complexity: O(1) */
void* set_get_value(SetIterator *it)
{
    assert(it != NULL);

    return rbtree_get_value((RBTreeIterator *)it);
}
