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

#include <seatest.h>
#include <darray.h>

static DArray *a, *b;

unsigned long* make_ulong_ptr(unsigned long value)
{
    unsigned long *val = NULL;

    val = malloc(sizeof(unsigned long));
    if(val != NULL) {
        *val = value;
    }

    return val;
}

/* a is greater than b if a is numerically less than b */
int ulong_compare(const unsigned long *a, const unsigned long *b)
{
    if(*a < *b) {
        return 1;
    } else if(*a == *b) {
        return 0;
    } else {
        return -1;
    }
}

void test_darray_create(void)
{
    DArray *a = NULL;

    a = darray_create();

    assert_true(a != NULL);
    assert_true(darray_is_empty(a));
    assert_true(darray_size(a) == 0);
    assert_true(darray_capacity(a) == 0);

    darray_free(a);
}

void test_darray_append(void)
{
    DArray *a = NULL;
    unsigned long i, *val;

    a = darray_create();
    assert_true(a != NULL);
    assert_true(darray_is_empty(a));

    for(i = 0; i < 100; i++) {
        val = malloc(sizeof(unsigned long));
        if(val != NULL) {
            *val = i;
            darray_append(a, val);
            assert_true(darray_size(a) == (i + 1));
            assert_true(darray_capacity(a) >= i);

            val = darray_index(a, i);
            assert_ulong_equal(i, *val);
        }
    }

    assert_false(darray_is_empty(a));
    assert_true(darray_size(a) == 100);
    assert_true(darray_capacity(a) >= 100);

    /* Sanity check end values */
    val = darray_index(a, 0);
    assert_ulong_equal(0, *val);

    val = darray_index(a, darray_size(a) - 1);
    assert_ulong_equal(99, *val);

    darray_free_all(a);
}

void test_darray_index(void)
{
    unsigned long i, *val;

    assert_false(darray_is_empty(a));

    for(i = 0; i < 100000; i++) {
        val = darray_index(a, i);
        assert_ulong_equal(i, *val);
    }

    assert_true(darray_size(a) == 100000);
    assert_true(darray_capacity(a) >= 100000);
}

void test_darray_prepend(void)
{
    DArray *a = NULL;
    unsigned long i, *val;

    a = darray_create();
    assert_true(a != NULL);
    assert_true(darray_is_empty(a));

    for(i = 0; i < 100; i++) {
        val = malloc(sizeof(unsigned long));
        if(val != NULL) {
            *val = i;
            darray_prepend(a, val);
            assert_true(darray_size(a) == (i + 1));
            assert_true(darray_capacity(a) >= i);

            val = darray_index(a, 0);
            assert_ulong_equal(i, *val);
        }
    }

    assert_false(darray_is_empty(a));
    assert_true(darray_size(a) == 100);
    assert_true(darray_capacity(a) >= 100);

    /* Sanity check end values */
    val = darray_index(a, 0);
    assert_ulong_equal(99, *val);

    val = darray_index(a, darray_size(a) - 1);
    assert_ulong_equal(0, *val);

    darray_free_all(a);
}

void test_darray_insert(void)
{
    DArray *a = NULL;
    unsigned long i, *val;

    a = darray_create();
    assert_true(a != NULL);
    assert_true(darray_is_empty(a));

    for(i = 0; i < 100; i++) {
        val = malloc(sizeof(unsigned long));
        if(val != NULL) {
            *val = i;
            darray_insert(a, val, darray_size(a));
            assert_true(darray_size(a) == (i + 1));

            val = darray_index(a, i);
            assert_ulong_equal(i, *val);
        }
    }

    assert_false(darray_is_empty(a));
    assert_true(darray_size(a) == 100);
    assert_true(darray_capacity(a) >= 100);

    darray_free_all(a);
}

void test_darray_remove(void)
{
    unsigned long i, *val;

    assert_false(darray_is_empty(a));

    /* Remove from the end. Use do-while because of
     * loop counter i is unsigned long and we need
     * to count down to and remove the zeroth item
     */
    i = darray_size(a);
    do {
        i--;
        val = darray_remove(a, i);
        free(val);
        assert_true(darray_size(a) == i);
        assert_true(darray_capacity(a) >= darray_size(a));
    } while(i != 0);

    assert_true(darray_is_empty(a));
    assert_true(darray_size(a) == 0);
    assert_true(darray_capacity(a) == 32);
}


/* Test fixture setup and teardown */

void darray_setup_ints(void)
{
    unsigned long i, *val;

    a = darray_create();
    assert_true(a != NULL);
    assert_true(darray_is_empty(a));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            darray_append(a, val);
        }
    }

    assert_true(darray_size(a) == 100000);
}

void darray_setup_ints_random(void)
{
    unsigned long i, *val;

    a = darray_create();
    assert_true(a != NULL);
    assert_true(darray_is_empty(a));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(arc4random() % 100000);
        if(val != NULL) {
            darray_append(a, val);
        }
    }

    assert_true(darray_size(a) == 100000);
}

void darray_double_setup_ints(void)
{
    unsigned long i, *val;

    a = darray_create();
    assert_true(a != NULL);
    assert_true(darray_is_empty(a));

    b = darray_create();
    assert_true(b != NULL);
    assert_true(darray_is_empty(b));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            darray_append(a, val);
        }
        val = make_ulong_ptr(i);
        if(val != NULL) {
            darray_append(b, val);
        }
    }

    assert_true(darray_size(a) == 100000);
    assert_true(darray_size(b) == 100000);
}

void darray_double_setup_random_ints(void)
{
    unsigned long i, *val;

    a = darray_create();
    assert_true(a != NULL);
    assert_true(darray_is_empty(a));

    b = darray_create();
    assert_true(b != NULL);
    assert_true(darray_is_empty(b));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(arc4random() % 100000);
        if(val != NULL) {
            darray_append(a, val);
        }
        val = make_ulong_ptr(arc4random() % 100000);
        if(val != NULL) {
            darray_append(b, val);
        }
    }

    assert_true(darray_size(a) == 100000);
    assert_true(darray_size(b) == 100000);
}

void darray_teardown(void)
{
    darray_free_all(a);
    a = NULL;
}

void darray_double_teardown(void)
{
    darray_free_all(a);
    darray_free_all(b);
    a = b = NULL;
}

void darray_double_teardown_concat_safe(void)
{
    darray_free_all(a);
    darray_free(b);
    a = b = NULL;
}


/* Fixtures */

void test_fixture_darray_create(void)
{
    test_fixture_start();
    run_test(test_darray_create);
    test_fixture_end();
}

void test_fixture_darray_remove(void)
{
    test_fixture_start();
    fixture_setup(darray_setup_ints);
    fixture_teardown(darray_teardown);
    run_test(test_darray_remove);
    test_fixture_end();
}

void test_fixture_darray_index(void)
{
    test_fixture_start();
    fixture_setup(darray_setup_ints);
    fixture_teardown(darray_teardown);
    run_test(test_darray_index);
    test_fixture_end();
}

void test_fixture_darray_append(void)
{
    test_fixture_start();
    run_test(test_darray_append);
    test_fixture_end();
}

void test_fixture_darray_prepend(void)
{
    test_fixture_start();
    run_test(test_darray_prepend);
    test_fixture_end();
}


void test_darray_replace_empty(void)
{
    unsigned long *val = NULL;

    a = darray_create();

    assert_true(a != NULL);
    assert_true(darray_is_empty(a));
    assert_true(darray_size(a) == 0);
    assert_true(darray_capacity(a) == 0);

    val = make_ulong_ptr(9999);
    assert_true(val != NULL);

    assert_true(darray_replace(a, 0, val) == -1);
    assert_true(darray_replace(a, 100, val) == -1);

    darray_free(a);
    free(val);
}

void test_darray_replace_existing_at_front(void)
{
    unsigned long *val = NULL, *old = NULL;

    assert_true(a != NULL);
    assert_false(darray_is_empty(a));
    assert_true(darray_capacity(a) >= darray_size(a));

    val = make_ulong_ptr(9999);
    assert_true(val != NULL);

    old = darray_index(a, 0);
    assert_true(darray_replace(a, 0, val) == 0);

    free(old);

    /* Verify */
    assert_ulong_equal(*val, *(unsigned long *)darray_index(a, 0));
}

void test_darray_replace_existing_at_back(void)
{
    unsigned long *val = NULL, *old = NULL;

    assert_true(a != NULL);
    assert_false(darray_is_empty(a));
    assert_true(darray_capacity(a) >= darray_size(a));

    val = make_ulong_ptr(9999);
    assert_true(val != NULL);

    old = darray_index(a, darray_size(a) - 1);
    assert_true(darray_replace(a, darray_size(a) - 1, val) == 0);

    free(old);

    /* Verify */
    assert_ulong_equal(*val,
        *(unsigned long *)darray_index(a, darray_size(a) - 1));
}

void test_fixture_darray_replace(void)
{
    test_fixture_start();

    run_test(test_darray_replace_empty);

    fixture_setup(darray_setup_ints);
    fixture_teardown(darray_teardown);

    run_test(test_darray_replace_existing_at_front);
    run_test(test_darray_replace_existing_at_back);

    test_fixture_end();
}


void test_darray_swap_empty(void)
{
    a = darray_create();

    assert_true(a != NULL);
    assert_true(darray_is_empty(a));
    assert_true(darray_size(a) == 0);
    assert_true(darray_capacity(a) == 0);

    assert_true(darray_swap(a, 0, 1) == -1);
    assert_true(darray_swap(a, 100, 1) == -1);

    darray_free(a);
}

void test_darray_swap_existing(void)
{
    unsigned long *val1 = NULL, *val2 = NULL;

    assert_true(a != NULL);
    assert_false(darray_is_empty(a));
    assert_true(darray_capacity(a) >= darray_size(a));

    val1 = darray_index(a, 5);
    val2 = darray_index(a, 10);

    assert_true(darray_swap(a, 5, 10) == 0);

    /* Verify */
    assert_ulong_equal(*val1, *(unsigned long *)darray_index(a, 10));
    assert_ulong_equal(*val2, *(unsigned long *)darray_index(a, 5));
}

void test_fixture_darray_swap(void)
{
    test_fixture_start();

    run_test(test_darray_swap_empty);

    fixture_setup(darray_setup_ints);
    fixture_teardown(darray_teardown);

    run_test(test_darray_swap_existing);

    test_fixture_end();
}


void test_darray_concat_empty_with_empty(void)
{
    DArray *b;

    a = darray_create();
    b = darray_create();

    assert_true(a != NULL);
    assert_true(b != NULL);
    assert_true(darray_is_empty(a));
    assert_true(darray_is_empty(b));
    assert_true(darray_size(a) == 0);
    assert_true(darray_size(b) == 0);
    assert_true(darray_capacity(a) == 0);
    assert_true(darray_capacity(b) == 0);

    assert_true(darray_concat(a, b) == 0);

    /* Verify */
    assert_true(darray_is_empty(a));
    assert_true(darray_is_empty(b));
    assert_true(darray_size(a) == 0);
    assert_true(darray_size(b) == 0);
    assert_true(darray_capacity(a) == 0);
    assert_true(darray_capacity(b) == 0);

    darray_free(a);
    darray_free(b);
}

void test_darray_concat_existing_with_empty(void)
{
    unsigned long old_a_size, old_a_capacity;
    DArray *b;

    b = darray_create();

    assert_true(a != NULL);
    assert_true(b != NULL);

    assert_false(darray_is_empty(a));
    assert_true(darray_is_empty(b));

    old_a_size = darray_size(a);
    old_a_capacity = darray_capacity(a);

    assert_true(darray_concat(a, b) == 0);

    /* Verify */
    assert_false(darray_is_empty(a));
    assert_true(darray_is_empty(b));

    assert_true(darray_size(a) == old_a_size);
    assert_true(darray_size(b) == 0);

    assert_true(darray_capacity(a) == old_a_capacity);
    assert_true(darray_capacity(b) == 0);

    darray_free(b);
}

void test_darray_concat_empty_with_existing(void)
{
    unsigned long old_a_size, old_a_capacity;
    DArray *b;

    b = darray_create();

    assert_true(a != NULL);
    assert_true(b != NULL);

    assert_false(darray_is_empty(a));
    assert_true(darray_is_empty(b));

    old_a_size = darray_size(a);
    old_a_capacity = darray_capacity(a);

    assert_true(darray_concat(b, a) == 0);

    /* Verify */
    assert_false(darray_is_empty(a));
    assert_false(darray_is_empty(b));

    assert_true(darray_size(a) == old_a_size);
    assert_true(darray_size(b) == darray_size(a));

    assert_true(darray_capacity(a) == old_a_capacity);
    assert_true(darray_capacity(b) == darray_capacity(a));

    darray_free(b);
}

void test_darray_concat_existing_with_existing(void)
{
    unsigned long old_a_size, old_a_capacity;
    unsigned long old_b_size, old_b_capacity;

    assert_true(a != NULL);
    assert_true(b != NULL);

    assert_false(darray_is_empty(a));
    assert_false(darray_is_empty(b));

    old_a_size = darray_size(a);
    old_a_capacity = darray_capacity(a);

    old_b_size = darray_size(b);
    old_b_capacity = darray_capacity(b);

    assert_true(darray_concat(a, b) == 0);

    /* Verify */
    assert_false(darray_is_empty(a));
    assert_false(darray_is_empty(b));

    assert_true(darray_size(a) == (old_a_size + old_b_size));
    assert_true(darray_size(b) == old_b_size);

    assert_true(darray_capacity(a) >= darray_size(a));
    assert_true(darray_capacity(b) == old_b_capacity);
}

void test_fixture_darray_concat(void)
{
    test_fixture_start();

    run_test(test_darray_concat_empty_with_empty);

    fixture_setup(darray_setup_ints);
    fixture_teardown(darray_teardown);

    run_test(test_darray_concat_existing_with_empty);
    run_test(test_darray_concat_empty_with_existing);

    fixture_setup(darray_double_setup_ints);
    fixture_teardown(darray_double_teardown_concat_safe);

    run_test(test_darray_concat_existing_with_existing);

    test_fixture_end();
}


void test_darray_sort_empty(void)
{
    a = darray_create();

    assert_true(a != NULL);
    assert_true(darray_is_empty(a));
    assert_true(darray_size(a) == 0);
    assert_true(darray_capacity(a) == 0);

    assert_true(darray_sort(a, (CompareFn)ulong_compare) == -1);

    /* Verify */
    assert_false(darray_is_sorted(a, (CompareFn)ulong_compare));

    darray_free(a);
}

void test_darray_sort_existing(void)
{
    unsigned long old_size, old_capacity;

    assert_true(a != NULL);
    assert_false(darray_is_empty(a));

    old_size = darray_size(a);
    old_capacity = darray_capacity(a);

    assert_true(darray_sort(a, (CompareFn)ulong_compare) == 0);

    /* Verify */
    assert_true(darray_is_sorted(a, (CompareFn)ulong_compare));
    assert_true(darray_size(a) == old_size);
    assert_true(darray_capacity(a) == old_capacity);
}

void test_fixture_darray_sort(void)
{
    test_fixture_start();

    run_test(test_darray_sort_empty);

    fixture_setup(darray_setup_ints_random);
    fixture_teardown(darray_teardown);

    run_test(test_darray_sort_existing);

    test_fixture_end();
}


void test_darray_merge_empty_with_empty(void)
{
    DArray *b;

    a = darray_create();
    b = darray_create();

    assert_true(a != NULL);
    assert_true(b != NULL);
    assert_true(darray_is_empty(a));
    assert_true(darray_is_empty(b));
    assert_true(darray_size(a) == 0);
    assert_true(darray_size(b) == 0);
    assert_true(darray_capacity(a) == 0);
    assert_true(darray_capacity(b) == 0);

    assert_true(darray_merge(a, b, (CompareFn)ulong_compare) < 0);

    /* Verify */
    assert_true(darray_is_empty(a));
    assert_true(darray_is_empty(b));
    assert_true(darray_size(a) == 0);
    assert_true(darray_size(b) == 0);
    assert_true(darray_capacity(a) == 0);
    assert_true(darray_capacity(b) == 0);

    darray_free(a);
    darray_free(b);
}

void test_darray_merge_existing_with_empty(void)
{
    unsigned long old_a_size, old_a_capacity;
    DArray *b;

    b = darray_create();

    assert_true(a != NULL);
    assert_true(b != NULL);

    assert_false(darray_is_empty(a));
    assert_true(darray_is_empty(b));

    old_a_size = darray_size(a);
    old_a_capacity = darray_capacity(a);

    assert_true(darray_merge(a, b, (CompareFn)ulong_compare) < 0);

    /* Verify */
    assert_false(darray_is_empty(a));
    assert_true(darray_is_empty(b));

    assert_true(darray_size(a) == old_a_size);
    assert_true(darray_size(b) == 0);

    assert_true(darray_capacity(a) == old_a_capacity);
    assert_true(darray_capacity(b) == 0);

    darray_free(b);
}

void test_darray_merge_empty_with_existing(void)
{
    unsigned long old_a_size, old_a_capacity;
    DArray *b;

    b = darray_create();

    assert_true(a != NULL);
    assert_true(b != NULL);

    assert_false(darray_is_empty(a));
    assert_true(darray_is_empty(b));

    old_a_size = darray_size(a);
    old_a_capacity = darray_capacity(a);

    assert_true(darray_merge(b, a, (CompareFn)ulong_compare) < 0);

    /* Verify */
    assert_false(darray_is_empty(a));
    assert_true(darray_is_empty(b));

    assert_true(darray_size(a) == old_a_size);
    assert_true(darray_size(b) == 0);

    assert_true(darray_capacity(a) == old_a_capacity);
    assert_true(darray_capacity(b) == 0);

    darray_free(b);
}

void test_darray_merge_existing_with_existing(void)
{
    unsigned long old_a_size, old_a_capacity;
    unsigned long old_b_size, old_b_capacity;

    assert_true(a != NULL);
    assert_true(b != NULL);

    assert_false(darray_is_empty(a));
    assert_false(darray_is_empty(b));

    old_a_size = darray_size(a);
    old_a_capacity = darray_capacity(a);

    old_b_size = darray_size(b);
    old_b_capacity = darray_capacity(b);

    assert_true(darray_merge(a, b, (CompareFn)ulong_compare) == 0);

    /* Verify */
    assert_false(darray_is_empty(a));
    assert_false(darray_is_empty(b));

    assert_true(darray_size(a) == (old_a_size + old_b_size));
    assert_true(darray_size(b) == old_b_size);

    assert_true(darray_capacity(a) >= darray_size(a));
    assert_true(darray_capacity(b) == old_b_capacity);

    assert_true(darray_is_sorted(a, (CompareFn)ulong_compare));
}

void test_fixture_darray_merge(void)
{
    test_fixture_start();

    run_test(test_darray_merge_empty_with_empty);

    fixture_setup(darray_setup_ints);
    fixture_teardown(darray_teardown);

    run_test(test_darray_merge_existing_with_empty);
    run_test(test_darray_merge_empty_with_existing);

    fixture_setup(darray_double_setup_ints);
    fixture_teardown(darray_double_teardown_concat_safe);

    run_test(test_darray_merge_existing_with_existing);

    test_fixture_end();
}

void all_tests(void)
{
    test_fixture_darray_create();
    test_fixture_darray_append();
    test_fixture_darray_prepend();
    test_fixture_darray_index();
    test_fixture_darray_remove();
    test_fixture_darray_replace();
    test_fixture_darray_swap();
    test_fixture_darray_concat();
    test_fixture_darray_sort();
    test_fixture_darray_merge();
}

int main(int argc, char *argv[])
{
    return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}
