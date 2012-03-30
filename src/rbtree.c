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

typedef enum {RED, BLACK} _node_color;

struct _rbtree_node {
    _node_color color;
    struct _rbtree_node *parent, *left, *right;
    const void *key;
    void *value;
};

struct _rbtree {
    /* TODO: maintain pointers to min and max nodes
     * to make iterator begin and end operations O(1)
     */
    struct _rbtree_node *root;
    CompareFn comparefn;
    unsigned long size;
};

/* The insert and delete algorithms are based on those in
 * "Introduction to Algorithms" by Cormen, Leiserson, and
 * Rivest (MIT Press, 1990).
 */

static int _rbtree_is_valid(RBTree *rbtree, struct _rbtree_node *node)
{
    unsigned long black_height_left, black_height_right;

    if(NULL == node) {
        return 1;
    } else {
        black_height_left = _rbtree_is_valid(rbtree, node->left);
        black_height_right = _rbtree_is_valid(rbtree, node->right);

        if(black_height_left == 0 || black_height_right == 0) {
            assert(0);
            return 0;
        }

        /* Every simple path from a node to a descendant leaf
         * contains the same number of black nodes
         */
        if(black_height_left != black_height_right) {
            assert(0);
            return 0;
        }

        /* Every RED node has BLACK children */
        if(RED == node->color) {
            if((node->left != NULL) && (BLACK != node->left->color)) {
                assert(0);
                return 0;
            }
            if((node->right != NULL) && (BLACK != node->right->color)) {
                assert(0);
                return 0;
            }

            return black_height_left;
        }

        /* Every node is either red or black */
        if(node->color != BLACK) {
            assert(0);
            return 0;
        }

        /* Verify binary tree property
         *
         * Since the tree could contain duplicates, the key in the
         * left child could be <= the key of the current node. Therefore,
         * we only test if the left child's key is > the current node's
         * key.
         */
        if((node->left != NULL) &&
                (rbtree->comparefn(node->key, node->left->key) < 0)) {
            assert(0);
            return 0;
        }
        if((node->right != NULL) &&
                (rbtree->comparefn(node->key, node->right->key) > 0)) {
            assert(0);
            return 0;
        }

        return black_height_left + 1;
    }
}


/*
 *       |                              |
 *       y      right_rotate(T, y)      x
 *      / \     =================>     / \
 *     /   c                          a   \
 *    x         <=================         y
 *   / \         left_rotate(T,x)         / \
 *  a   b                                b   c
 */
static void _rotate_left(RBTree *rbtree, struct _rbtree_node *x)
{
    struct _rbtree_node *y;

    assert(x->right != NULL);

    y = x->right;

    /* y's left subtree becomes node's right subtree */
    x->right = y->left;
    if(y->left != NULL) {
        y->left->parent = x;
    }

    /* Link x's parent to y */
    y->parent = x->parent;

    if(x == rbtree->root) {
        rbtree->root = y;
    } else if(x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

static void _rotate_right(RBTree *rbtree, struct _rbtree_node *y)
{
    struct _rbtree_node *x;

    assert(y->left != NULL);

    x = y->left;

    /* x's right subtree becomes y's left subtree */
    y->left = x->right;
    if(x->right != NULL) {
        x->right->parent = y;
    }

    /* Link y's parent to x */
    x->parent = y->parent;

    if(y == rbtree->root) {
        rbtree->root = x;
    } else if(y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;
}

#define grandparent_of(x)   (x)->parent->parent

static void _insert_fixup(RBTree *rbtree, struct _rbtree_node *node)
{
    struct _rbtree_node *uncle;

    while((node != rbtree->root) && (RED == node->parent->color)) {
        if(node->parent == grandparent_of(node)->left) {
            uncle = grandparent_of(node)->right;

            if((uncle != NULL) && (RED == uncle->color)) {
                /* Case 1 */
                node->parent->color = BLACK;
                uncle->color = BLACK;
                grandparent_of(node)->color = RED;
                node = grandparent_of(node);
            } else {
                if(node == node->parent->right) {
                    /* Case 2 */
                    node = node->parent;
                    _rotate_left(rbtree, node);
                }

                /* Case 3 */
                node->parent->color = BLACK;
                grandparent_of(node)->color = RED;
                _rotate_right(rbtree, grandparent_of(node));
            }
        } else {
            uncle = grandparent_of(node)->left;

            if((uncle != NULL) && (RED == uncle->color)) {
                /* Case 1 */
                node->parent->color = BLACK;
                uncle->color = BLACK;
                grandparent_of(node)->color = RED;
                node = grandparent_of(node);
            } else {
                if(node == node->parent->left) {
                    /* Case 2 */
                    node = node->parent;
                    _rotate_right(rbtree, node);
                }

                /* Case 3 */
                node->parent->color = BLACK;
                grandparent_of(node)->color = RED;
                _rotate_left(rbtree, grandparent_of(node));
            }
        }
    }

    rbtree->root->color = BLACK;
}

static void _remove_fixup(RBTree *rbtree, struct _rbtree_node *node,
        struct _rbtree_node *node_parent)
{
    struct _rbtree_node *w;

    while((node != rbtree->root) && ((node == NULL) || (BLACK == node->color))) {
        assert(node_parent != NULL);
        if(node == node_parent->left) {
            w = node_parent->right;
            assert(w != NULL);

            if(RED == w->color) {
                /* Case 1 */
                w->color = BLACK;
                node_parent->color = RED;
                _rotate_left(rbtree, node_parent);
                w = node_parent->right;
            }

            assert(w != NULL);
            if(((w->left == NULL) || (BLACK == w->left->color)) &&
                    ((w->right == NULL ) || (BLACK == w->right->color))) {
                /* Case 2 */
                w->color = RED;
                node = node_parent;
                node_parent = node_parent->parent;
            } else {
                if((w->right == NULL) || (BLACK == w->right->color)) {
                    /* Case 3 */
                    assert(w->left != NULL);
                    w->left->color = BLACK;
                    w->color = RED;
                    _rotate_right(rbtree, w);
                    w = node_parent->right;
                    assert(w != NULL);
                }

                /* Case 4 */
                w->color = node_parent->color;
                node_parent->color = BLACK;
                if(w->right != NULL) {
                    w->right->color = BLACK;
                }
                _rotate_left(rbtree, node_parent);
                node = rbtree->root;
            }
        } else {
            w = node_parent->left;
            assert(w != NULL);

            if(RED == w->color) {
                /* Case 1 */
                w->color = BLACK;
                node_parent->color = RED;
                _rotate_right(rbtree, node_parent);
                w = node_parent->left;
            }

            assert(w != NULL);
            if(((w->left == NULL) || (BLACK == w->left->color)) &&
                    ((w->right == NULL ) || (BLACK == w->right->color))) {
                /* Case 2 */
                w->color = RED;
                node = node_parent;
                node_parent = node_parent->parent;
            } else {
                if((w->left == NULL) || (BLACK == w->left->color)) {
                    /* Case 3 */
                    assert(w->right != NULL);
                    w->right->color = BLACK;
                    w->color = RED;
                    _rotate_left(rbtree, w);
                    w = node_parent->left;
                    assert(w != NULL);
                }

                /* Case 4 */
                w->color = node_parent->color;
                node_parent->color = BLACK;
                if(w->left != NULL) {
                    w->left->color = BLACK;
                }
                _rotate_right(rbtree, node_parent);
                node = rbtree->root;
            }
        }
    }

    if(node != NULL) {
        node->color = BLACK;
    }
}

static struct _rbtree_node* _tree_minimum(struct _rbtree_node *node)
{
    while(node->left != NULL) {
        node = node->left;
    }

    return node;
}

static struct _rbtree_node* _tree_maximum(struct _rbtree_node *node)
{
    while(node->right != NULL) {
        node = node->right;
    }

    return node;
}

static struct _rbtree_node* _rbtree_insert(RBTree *rbtree,
        struct _rbtree_node *x, const void *key, void *value,
        int duplicates_allowed)
{
    struct _rbtree_node *node, *y;
    int result;

    node = malloc(sizeof(struct _rbtree_node));
    if(NULL == node) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    node->color = RED;
    node->key = key;
    node->value = value;
    node->parent = node->left = node->right = NULL;

    if(NULL == rbtree->root) {
        rbtree->root = node;
    } else {
        y = NULL;
        if(NULL == x) {
            x = rbtree->root;
        }

        /* Traverse the tree and find the right insertion point */
        while(NULL != x) {
            y = x;
            result = rbtree->comparefn(node->key, x->key);
            if(result < 0) {
                x = x->left;
            } else {
                if(result == 0 && !duplicates_allowed) {
                    free(node);
                    return NULL;
                }
                x = x->right;
            }
        }

        node->parent = y;

        if(NULL == y) {
            rbtree->root = node;
        } else if(result < 0) {
            y->left = node;
        } else {
            y->right = node;
        }
    }

    /* Rebalance */
    _insert_fixup(rbtree, node);

    rbtree->size++;

    return node;
}

static void* _rbtree_remove(RBTree *rbtree, struct _rbtree_node *z)
{
    struct _rbtree_node *x, *x_parent, *y;
    _node_color y_color;
    void *ret;

    x_parent = NULL;

    if(NULL == z->left || NULL == z->right) {
        /* z has at most one child */
        y = z;
    } else {
        /* z has two children. Successor, y, has no left child */
        y = rbtree_next(z);
    }

    /* x is the child of y */
    if(y->left != NULL) {
        x = y->left;
    } else {
        x = y->right;
    }

    /* Splice out y, where y is either the node to be deleted,
     * or it's successor
     */
    if(x != NULL) {
        x->parent = y->parent;
        x_parent = y->parent;
    }

    if(NULL == y->parent) {
        /* y is the root node, so y's child has become the root */
        rbtree->root = x;
    } else {
        /* Fix up y's parent to point to its new child, x */
        if(y->parent->left == y) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }
    }

    /* y is the successor of z, so it needs to be
     * relinked in place of z
     */
    if(y != z) {
        /* Relink the successor, y, into the place of z
         * rather than copying over the contents of
         * the node (as described in Cormen, et al.).
         * This approach invalidates only those iterators
         * that refer to the deleted node.
         */
        if(NULL == x) {
            if(z != y->parent) {
                x_parent = y->parent;
            } else {
                x_parent = y;
            }
        }

        y->parent = z->parent;
        y->left = z->left;
        y->right = z->right;

        if(z->left != NULL) {
            z->left->parent = y;
        }
        if(z->right != NULL) {
            z->right->parent = y;
        }

        if(NULL != z->parent) {
            /* z is not the root. */
            if(z->parent->left == z) {
                z->parent->left = y;
            } else {
                z->parent->right = y;
            }
        } else {
            /* z is the root. Update container to point to y */
            rbtree->root = y;
        }

        y_color = y->color;
        y->color = z->color;
        z->color = y_color;

        y = z;
    } else {
        if(NULL == x) {
            x_parent = y->parent;
        }
    }

    rbtree->size--;

    if(BLACK == y->color) {
        _remove_fixup(rbtree, x, x_parent);
    }

    ret = z->value;
    free(z);

    return ret;
}

static void _rbtree_free_all(struct _rbtree_node *node, FreeFn freefn)
{
    if(NULL != node) {
        _rbtree_free_all(node->left, freefn);
        _rbtree_free_all(node->right, freefn);
        freefn(node->value);
        free(node);
    }
}


RBTree* rbtree_create(CompareFn comparefn)
{
    RBTree *new_rbtree;

    assert(comparefn != NULL);

    /* RBTree container */
    new_rbtree = malloc(sizeof(struct _rbtree));
    if(NULL == new_rbtree) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    new_rbtree->root = NULL;
    new_rbtree->comparefn = comparefn;
    new_rbtree->size = 0;

    return new_rbtree;
}

/* Complexity: O(1) */
void rbtree_free(RBTree *rbtree)
{
    assert(rbtree != NULL);

    /* Free container and nodes only */
    free(rbtree);
}

/* Complexity: O(n) in time, O(log n) in space */
void rbtree_free_all(RBTree *rbtree, FreeFn freefn)
{
    assert(rbtree != NULL);

    if(NULL == freefn) {
        /* Default to free */
        freefn = (FreeFn)free;
    }

    /* Free container, nodes, keys, and values */
    _rbtree_free_all(rbtree->root, freefn);

    free(rbtree);
}

/* Complexity: O(log n) */
int rbtree_insert_equal(RBTree *rbtree, const void *key, void *value)
{
    int ret;

    assert(rbtree != NULL);

    ret = 0;
    if(_rbtree_insert(rbtree, rbtree->root, key, value, 1) == NULL) {
        ret = -1;
    }

    return ret;
}

/* Complexity: O(log n) */
int rbtree_insert_unique(RBTree *rbtree, const void *key, void *value)
{
    int ret;

    assert(rbtree != NULL);

    ret = 0;
    if(_rbtree_insert(rbtree, rbtree->root, key, value, 0) == NULL) {
        ret = -1;
    }

    return ret;
}

/* Complexity: O(log n) */
void* rbtree_remove(RBTree *rbtree, const void *key)
{
    struct _rbtree_node *z;

    assert(rbtree != NULL);

    z = rbtree_find(rbtree, key);

    if(NULL == z) {
        return NULL;
    }

    return _rbtree_remove(rbtree, z);
}

/* Complexity: O(1) */
int rbtree_is_empty(RBTree *rbtree)
{
    assert(rbtree != NULL);

    return (rbtree->size == 0);
}

/* Complexity: O(2n) in time, O(log n) in space */
int rbtree_is_valid(RBTree *rbtree)
{
    RBTreeIterator *it, *prev;

    assert(rbtree != NULL);

    if(rbtree_is_empty(rbtree)) {
        return 1;
    } else {
        it = prev = NULL;
        for(it = rbtree_begin(rbtree); it != NULL; it = rbtree_next(it)); {
            if(prev != NULL) {
                if(rbtree->comparefn(prev->key, it->key) > 0) {
                    assert(0);
                    return 0;
                }
            }
            prev = it;
        }
    }

    if(rbtree->root->parent != NULL) {
        assert(0);
        return 0;
    }

    return _rbtree_is_valid(rbtree, rbtree->root);
}


/* Complexity: O(1) */
unsigned long rbtree_size(RBTree *rbtree)
{
    assert(rbtree != NULL);

    return rbtree->size;
}

CompareFn rbtree_get_comparefn(RBTree *rbtree)
{
    assert(rbtree != NULL);

    return rbtree->comparefn;
}

/* Returns an iterator at the position where the new key,value
 * pair is actually inserted. The supplied position, *it, is the
 * position of the first element to be compared for the insertion
 * operation.
 */
/* Complexity: O(1) in time, best-case; O(log n) in time, worst-case */
RBTreeIterator* rbtree_insert_equal_at(RBTree *rbtree, RBTreeIterator *it,
        const void *key, void *value, int *success)
{
    struct _rbtree_node *min, *max, *ret;

    assert(rbtree != NULL);
    assert(it != NULL);

    if(rbtree->comparefn(key, it->key) >= 0) {
        max = rbtree_end(rbtree);
        if((max != NULL) && (rbtree->comparefn(key, max->key) >= 0)) {
            ret = _rbtree_insert(rbtree, max, key, value, 1);
        } else {
            ret = _rbtree_insert(rbtree, it, key, value, 1);
        }
    } else {
        min = rbtree_begin(rbtree);
        if((min != NULL) && (rbtree->comparefn(key, min->key) <= 0)) {
            ret = _rbtree_insert(rbtree, min, key, value, 1);
        } else {
            do {
                it = rbtree_next(it);
            } while(rbtree->comparefn(key, it->key) < 0);

            ret = _rbtree_insert(rbtree, it, key, value, 1);
        }
    }

    /* Return value handling */
    if(NULL == ret) {
        if(success != NULL) {
            *success = 0;
        }
        ret = it;
    } else {
        if(success != NULL) {
            *success = 1;
        }
    }

    return ret;
}

/* Complexity: O(1) in time, best-case; O(log n) in time, worst-case */
RBTreeIterator* rbtree_insert_unique_at(RBTree *rbtree, RBTreeIterator *it,
        const void *key, void *value, int *success)
{
    struct _rbtree_node *min, *max, *ret;
    int result;

    assert(rbtree != NULL);
    assert(it != NULL);

    result = rbtree->comparefn(key, it->key);
    if(result == 0) {
        /* Same keys */
        ret = NULL;
    } else if(result > 0) {
        max = rbtree_end(rbtree);
        if((max != NULL) && (rbtree->comparefn(key, max->key) >= 0)) {
            ret = _rbtree_insert(rbtree, max, key, value, 0);
        } else {
            ret = _rbtree_insert(rbtree, it, key, value, 0);
        }
    } else {
        min = rbtree_begin(rbtree);
        if((min != NULL) && (rbtree->comparefn(key, min->key) <= 0)) {
            ret = _rbtree_insert(rbtree, min, key, value, 0);
        } else {
            do {
                it = rbtree_next(it);
            } while(rbtree->comparefn(key, it->key) < 0);

            ret = _rbtree_insert(rbtree, it, key, value, 0);
        }
    }

    /* Return value handling */
    if(NULL == ret) {
        if(success != NULL) {
            *success = 0;
        }
        ret = it;
    } else {
        if(success != NULL) {
            *success = 1;
        }
    }

    return ret;
}

/* Complexity: O(log n) */
void* rbtree_remove_at(RBTree *rbtree, RBTreeIterator *it)
{
    assert(rbtree != NULL);
    assert(it != NULL);

    return _rbtree_remove(rbtree, it);
}

/* Complexity: O(log n) */
RBTreeIterator* rbtree_find(RBTree *rbtree, const void *key)
{
    struct _rbtree_node *node, *save;
    int cmp_result;

    assert(rbtree != NULL);
    assert(key != NULL);

    node = rbtree->root;

    while(node != NULL) {
        cmp_result = rbtree->comparefn(key, node->key);
        if(cmp_result < 0) {
            node = node->left;
        } else if(cmp_result > 0) {
            node = node->right;
        } else {
            /* Tree could contain duplicates. If so, return
             * the left-most one.
             */
            do {
                save = node;
                node = node->left;
                while((node != NULL) && (rbtree->comparefn(key, node->key) != 0)) {
                    node = node->right;
                }
            } while(node != NULL);
            node = save;
            break;
        }
    }

    assert((node == NULL) || (rbtree->comparefn(key, node->key) == 0));

    return node;
}

/* Complexity: O(log n) */
RBTreeIterator* rbtree_begin(RBTree *rbtree)
{
    if(NULL == rbtree || rbtree_is_empty(rbtree)) {
        return NULL;
    }

    return _tree_minimum(rbtree->root);
}

/* Complexity: O(log n) */
RBTreeIterator* rbtree_end(RBTree *rbtree)
{
    if(NULL == rbtree || rbtree_is_empty(rbtree)) {
        return NULL;
    }

    return _tree_maximum(rbtree->root);
}

/* Complexity: O(log n) */
RBTreeIterator* rbtree_next(RBTreeIterator *it)
{
    struct _rbtree_node *node;

    if(NULL == it) {
        return NULL;
    }

    if(it->right != NULL) {
        return _tree_minimum(it->right);
    }

    node = it->parent;

    /* Successor is the lowest ancestor of node 'it'
     * whose left child is also an ancestor of node 'it'.
     * Go up the tree until we find a node that is the
     * left child of its parent
     */
    while((node != NULL) && (it == node->right)) {
        it = node;
        node = node->parent;
    }

    return node;
}

/* Complexity: O(log n) */
RBTreeIterator* rbtree_prev(RBTreeIterator *it)
{
    struct _rbtree_node *node;

    if(NULL == it) {
        return NULL;
    }

    if(it->left != NULL) {
        return _tree_maximum(it->left);
    }

    node = it->parent;

    /* Predecessor is the greatest ancestor of node 'it'
     * whose right child is also an ancestor of node 'it'.
     * Go up the tree until we find a node that is the
     * right child of its parent
     */
    while((node != NULL) && (it == node->left)) {
        it = node;
        node = node->parent;
    }

    return node;
}

/* Complexity: O(1) */
const void* rbtree_get_key(RBTreeIterator *it)
{
    if(NULL == it) {
        return NULL;
    }

    return it->key;
}

/* Complexity: O(1) */
void* rbtree_get_value(RBTreeIterator *it)
{
    if(NULL == it) {
        return NULL;
    }

    return it->value;
}
