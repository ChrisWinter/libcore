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

#include <stdlib.h>
#include <sys/time.h>

#include <seatest.h>
#include <libcore/macros.h>
#include <libcore/rbtree.h>

static RBTree *test_tree = NULL;

unsigned long* make_ulong_ptr(unsigned long value)
{
    unsigned long *val = NULL;

    val = malloc(sizeof(unsigned long));
    if(val != NULL) {
        *val = value;
    }

    return val;
}

/* a is greater than b if a is numerically greater than b */
int ulong_compare(const unsigned long *a, const unsigned long *b)
{
    if(*a > *b) {
        return 1;
    } else if(*a == *b) {
        return 0;
    } else {
        return -1;
    }
}

/* Test fixture setup and teardown */

void rbtree_setup_ints(void)
{
    unsigned long i, *val, count = ((rand() % 100000) + 1);

    test_tree = rbtree_create((CompareFn)ulong_compare);
    assert_true(test_tree != NULL);
    assert_true(rbtree_is_empty(test_tree));

    for(i = 0; i < count; i++) {
        val = make_ulong_ptr((rand() % 10000));
        if(val != NULL) {
            rbtree_insert_equal(test_tree, val, val);
        }
    }

    /* assert_true(rbtree_is_valid(test_rbtree)); */
    assert_ulong_equal(count, rbtree_size(test_tree));
}

void rbtree_teardown(void)
{
    rbtree_free_all(test_tree, free);
    test_tree = NULL;
}


void test_rbtree_create(void)
{
    test_tree = rbtree_create((CompareFn)ulong_compare);

    assert_true(test_tree != NULL);
    assert_true(rbtree_size(test_tree) == 0);
    assert_true(rbtree_is_empty(test_tree));
    assert_true(rbtree_is_valid(test_tree));

    rbtree_free(test_tree);
    test_tree = NULL;
}

void test_fixture_rbtree_create(void)
{
    test_fixture_start();
    run_test(test_rbtree_create);
    test_fixture_end();
}


void test_rbtree_insert(void)
{
    unsigned long *val, old_size;

    assert_true(test_tree != NULL);
    assert_false(rbtree_is_empty(test_tree));
    assert_true(rbtree_is_valid(test_tree));

    old_size = rbtree_size(test_tree);

    val = make_ulong_ptr(500000);
    if(val != NULL) {
        assert_true(rbtree_insert_equal(test_tree, val, val) == 0);
    }

    assert_true(rbtree_size(test_tree) == (old_size + 1));
    assert_true(rbtree_is_valid(test_tree));
}

void test_fixture_rbtree_insert(void)
{
    test_fixture_start();
    fixture_setup(rbtree_setup_ints);
    fixture_teardown(rbtree_teardown);
    run_test(test_rbtree_insert);
    test_fixture_end();
}

void test_rbtree_remove(void)
{
    unsigned long *val, search_key, old_size;

    assert_true(test_tree != NULL);
    assert_false(rbtree_is_empty(test_tree));
    assert_true(rbtree_is_valid(test_tree));

    old_size = rbtree_size(test_tree);

    search_key = 7;

    val = rbtree_remove(test_tree, &search_key);

    assert_true(rbtree_is_valid(test_tree));

    /* FIXME Case of val not found should be its own test case */
    if(val) {
        assert_ulong_equal(search_key, *(unsigned long *)val);
        assert_true(rbtree_size(test_tree) == (old_size - 1));

        free(val);
    }
}
void test_fixture_rbtree_remove(void)
{
    test_fixture_start();
    fixture_setup(rbtree_setup_ints);
    fixture_teardown(rbtree_teardown);
    run_test(test_rbtree_remove);
    test_fixture_end();
}

void test_rbtree_random_insert_and_remove(void)
{
    unsigned long i, old_size, *val, *remove_val;
    unsigned long count = 100000;
    unsigned long max_size = 0, inserts = 0, removes = 0;

    test_tree = rbtree_create((CompareFn)ulong_compare);
    assert_true(test_tree != NULL);
    assert_true(rbtree_is_empty(test_tree));

    for(i = 0; i < count; i++) {
        val = make_ulong_ptr((rand() % 100));
        if(val != NULL) {
            old_size = rbtree_size(test_tree);
            if(rand() % 2) {
                if(rbtree_insert_equal(test_tree, val, val) == 0) {
                    inserts++;
                    assert_true(rbtree_size(test_tree) == (old_size + 1));
                }
            } else {
                remove_val = NULL;
                remove_val = rbtree_remove(test_tree, val);
                if(remove_val) {
                    removes++;
                    assert_ulong_equal(*val, *(unsigned long *)remove_val);
                    assert_true(rbtree_size(test_tree) == (old_size - 1));

                    free(val);
                }
            }
        }
        max_size = MAX(max_size, rbtree_size(test_tree));
        assert_true(rbtree_is_valid(test_tree));
    }

    printf("Max size: %lu\n", max_size);
    printf("Inserts: %lu\n", inserts);
    printf("Removes: %lu\n", removes);

    rbtree_free_all(test_tree, free);
    test_tree = NULL;
}

void test_fixture_rbtree_random_insert_and_remove(void)
{
    test_fixture_start();
    run_test(test_rbtree_random_insert_and_remove);
    test_fixture_end();
}

void all_tests(void)
{
    test_fixture_rbtree_create();
    test_fixture_rbtree_insert();
    test_fixture_rbtree_remove();
    test_fixture_rbtree_random_insert_and_remove();
}

int main(int argc, char *argv[])
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    srand(tv.tv_usec * tv.tv_sec);

    return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}
