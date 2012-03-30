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
#include <libcore/heap.h>

static Heap *test_heap = NULL;
static Heap *test_heap2 = NULL;

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

/* Test fixture setup and teardown */

void heap_setup_ints(void)
{
    unsigned long i, *val;

    test_heap = heap_create((CompareFn)ulong_compare);
    assert_true(test_heap != NULL);
    assert_true(heap_is_empty(test_heap));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            heap_push(test_heap, val);
        }
    }

    assert_true(heap_is_valid(test_heap));
    assert_true(heap_size(test_heap) == 100000);
}

void heap_setup_ints_random(void)
{
    unsigned long i, *val;

    test_heap = heap_create((CompareFn)ulong_compare);
    assert_true(test_heap != NULL);
    assert_true(heap_is_empty(test_heap));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(rand() % 100000);
        if(val != NULL) {
            heap_push(test_heap, val);
        }
    }

    assert_true(heap_is_valid(test_heap));
    assert_true(heap_size(test_heap) == 100000);
}

void heap_double_setup_ints(void)
{
    unsigned long i, *val;

    test_heap = heap_create((CompareFn)ulong_compare);
    assert_true(test_heap != NULL);
    assert_true(heap_is_empty(test_heap));

    test_heap2 = heap_create((CompareFn)ulong_compare);
    assert_true(test_heap2 != NULL);
    assert_true(heap_is_empty(test_heap2));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            heap_push(test_heap, val);
        }

        val = make_ulong_ptr(i);
        if(val != NULL) {
            heap_push(test_heap2, val);
        }
    }

    assert_true(heap_is_valid(test_heap));
    assert_true(heap_is_valid(test_heap2));
    assert_true(heap_size(test_heap) == 100000);
    assert_true(heap_size(test_heap2) == 100000);
}

void heap_double_setup_ints_random(void)
{
    unsigned long i, *val;

    test_heap = heap_create((CompareFn)ulong_compare);
    assert_true(test_heap != NULL);
    assert_true(heap_is_empty(test_heap));

    test_heap2 = heap_create((CompareFn)ulong_compare);
    assert_true(test_heap2 != NULL);
    assert_true(heap_is_empty(test_heap2));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(rand() % 100000);
        if(val != NULL) {
            heap_push(test_heap, val);
        }

        val = make_ulong_ptr(rand() % 100000);
        if(val != NULL) {
            heap_push(test_heap2, val);
        }
    }

    assert_true(heap_is_valid(test_heap));
    assert_true(heap_is_valid(test_heap2));
    assert_true(heap_size(test_heap) == 100000);
    assert_true(heap_size(test_heap2) == 100000);
}

void heap_teardown(void)
{
    heap_free_all(test_heap, NULL);
    test_heap = NULL;
}

void heap_double_teardown(void)
{
    heap_free_all(test_heap, NULL);
    heap_free_all(test_heap2, NULL);
    test_heap = test_heap2 = NULL;
}

void heap_double_teardown_merge_safe(void)
{
    heap_free_all(test_heap, NULL);
    heap_free(test_heap2);
    test_heap = test_heap2 = NULL;
}

void test_heap_create(void)
{
    test_heap = heap_create((CompareFn)ulong_compare);

    assert_true(test_heap != NULL);
    assert_true(heap_size(test_heap) == 0);
    assert_true(heap_is_empty(test_heap));

    heap_free(test_heap);
    test_heap = NULL;
}

void test_fixture_heap_create(void)
{
    test_fixture_start();
    run_test(test_heap_create);
    test_fixture_end();
}


void test_heap_push_empty(void)
{
    unsigned long *val;

    test_heap = heap_create((CompareFn)ulong_compare);

    assert_true(test_heap != NULL);
    assert_true(heap_size(test_heap) == 0);
    assert_true(heap_is_empty(test_heap));

    val = make_ulong_ptr(9999);
    assert_true(heap_push(test_heap, val) == 0);

    /* Verify */
    assert_true(heap_size(test_heap) == 1);
    assert_true(heap_is_valid(test_heap));
    assert_false(heap_is_empty(test_heap));

    assert_ulong_equal(9999, *(unsigned long *)heap_top(test_heap));

    heap_free_all(test_heap, NULL);
    test_heap = NULL;
}

void test_heap_push_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = heap_size(test_heap);

    val = make_ulong_ptr(9999);
    assert_true(heap_push(test_heap, val) == 0);

    /* Verify */
    assert_true(heap_size(test_heap) == (old_size + 1));
    assert_true(heap_is_valid(test_heap));
    assert_false(heap_is_empty(test_heap));
}

void test_fixture_heap_push(void)
{
    test_fixture_start();

    run_test(test_heap_push_empty);

    fixture_setup(heap_setup_ints_random);
    fixture_teardown(heap_teardown);

    run_test(test_heap_push_existing);

    test_fixture_end();
}


void test_heap_pop_empty(void)
{
    test_heap = heap_create((CompareFn)ulong_compare);

    assert_true(test_heap != NULL);
    assert_true(heap_size(test_heap) == 0);
    assert_true(heap_is_empty(test_heap));

    assert_true(heap_pop(test_heap) == NULL);

    assert_true(heap_size(test_heap) == 0);
    assert_true(heap_is_valid(test_heap));
    assert_true(heap_is_empty(test_heap));

    heap_free(test_heap);
    test_heap = NULL;
}

void test_heap_pop_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = heap_size(test_heap);

    val = heap_pop(test_heap);

    assert_true(val != NULL);
    assert_ulong_equal(0, *val);
    assert_true(heap_size(test_heap) == (old_size - 1));

    free(val);
}

void test_heap_pop_until_empty(void)
{
    unsigned long *val;
    unsigned long old_val, old_size;

    old_val = *(unsigned long *)heap_top(test_heap);
    while(!heap_is_empty(test_heap)) {
        old_size = heap_size(test_heap);

        val = heap_pop(test_heap);

        assert_true(val != NULL);
        assert_true(ulong_compare(&old_val, val) >= 0);
        assert_true(heap_size(test_heap) == (old_size - 1));
        /* assert_true(heap_is_valid(test_heap)); */

        old_val = *val;
        free(val);
    }

    assert_true(heap_is_empty(test_heap));
    assert_true(heap_size(test_heap) == 0);
}

void test_fixture_heap_pop(void)
{
    test_fixture_start();

    run_test(test_heap_pop_empty);

    fixture_setup(heap_setup_ints);
    fixture_teardown(heap_teardown);
    run_test(test_heap_pop_existing);

    fixture_setup(heap_setup_ints_random);
    fixture_teardown(heap_teardown);
    run_test(test_heap_pop_until_empty);

    test_fixture_end();
}


void test_heap_top_empty(void)
{
    test_heap = heap_create((CompareFn)ulong_compare);

    assert_true(test_heap != NULL);
    assert_true(heap_size(test_heap) == 0);
    assert_true(heap_is_empty(test_heap));

    assert_true(heap_top(test_heap) == NULL);

    assert_true(heap_size(test_heap) == 0);
    assert_true(heap_is_empty(test_heap));

    heap_free(test_heap);
    test_heap = NULL;
}

void test_heap_top_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = heap_size(test_heap);

    val = heap_top(test_heap);

    assert_true(val != NULL);
    assert_ulong_equal(0, *val);
    assert_true(heap_size(test_heap) == old_size);
}

void test_fixture_heap_top(void)
{
    test_fixture_start();

    run_test(test_heap_top_empty);

    fixture_setup(heap_setup_ints);
    fixture_teardown(heap_teardown);

    run_test(test_heap_top_existing);

    test_fixture_end();
}


void test_heap_remove_empty(void)
{
    unsigned long *val;

    test_heap = heap_create((CompareFn)ulong_compare);

    assert_true(test_heap != NULL);
    assert_true(heap_size(test_heap) == 0);
    assert_true(heap_is_empty(test_heap));

    val = make_ulong_ptr(0);
    assert_true(heap_remove(test_heap, val) == -1);

    assert_true(heap_size(test_heap) == 0);
    assert_true(heap_is_empty(test_heap));

    heap_free(test_heap);
    free(val);
    test_heap = NULL;
}

void test_heap_remove_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = heap_size(test_heap);

    val = heap_top(test_heap);
    assert_true(heap_remove(test_heap, val) == 0);

    assert_true(val != NULL);
    assert_true(heap_size(test_heap) == (old_size - 1));
}

void test_heap_remove_existing_but_not_present(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = heap_size(test_heap);

    val = make_ulong_ptr(0);
    assert_true(heap_remove(test_heap, val) == -1);

    assert_true(val != NULL);
    assert_true(heap_size(test_heap) == old_size);
}

void test_fixture_heap_remove(void)
{
    test_fixture_start();

    run_test(test_heap_remove_empty);

    fixture_setup(heap_setup_ints);
    fixture_teardown(heap_teardown);

    run_test(test_heap_remove_existing);
    run_test(test_heap_remove_existing_but_not_present);

    test_fixture_end();
}


void test_heap_merge_empty_with_empty(void)
{
    test_heap = heap_create((CompareFn)ulong_compare);
    test_heap2 = heap_create((CompareFn)ulong_compare);

    assert_true(test_heap != NULL);
    assert_true(test_heap2 != NULL);
    assert_true(heap_size(test_heap) == 0);
    assert_true(heap_size(test_heap2) == 0);
    assert_true(heap_is_empty(test_heap));
    assert_true(heap_is_empty(test_heap2));

    assert_true(heap_merge(test_heap, test_heap2) == -1);

    assert_true(heap_is_valid(test_heap));
    assert_true(heap_is_valid(test_heap2));
    assert_true(heap_size(test_heap) == 0);
    assert_true(heap_size(test_heap2) == 0);
    assert_true(heap_is_empty(test_heap));
    assert_true(heap_is_empty(test_heap2));

    heap_free(test_heap);
    heap_free(test_heap2);
    test_heap = NULL;
    test_heap2 = NULL;
}

void test_heap_merge_empty_with_existing(void)
{
    test_heap2 = heap_create((CompareFn)ulong_compare);

    assert_true(test_heap != NULL);
    assert_true(test_heap2 != NULL);
    assert_true(heap_size(test_heap) == 100000);
    assert_true(heap_size(test_heap2) == 0);
    assert_false(heap_is_empty(test_heap));
    assert_true(heap_is_empty(test_heap2));

    assert_true(heap_merge(test_heap2, test_heap) == 0);

    assert_true(heap_is_valid(test_heap));
    assert_true(heap_is_valid(test_heap2));
    assert_true(heap_size(test_heap) == 100000);
    assert_true(heap_size(test_heap2) == 100000);
    assert_false(heap_is_empty(test_heap));
    assert_false(heap_is_empty(test_heap2));

    heap_free(test_heap2);
    test_heap2 = NULL;
}

void test_heap_merge_existing_with_empty(void)
{
    test_heap2 = heap_create((CompareFn)ulong_compare);

    assert_true(test_heap != NULL);
    assert_true(test_heap2 != NULL);
    assert_true(heap_size(test_heap) == 100000);
    assert_true(heap_size(test_heap2) == 0);
    assert_false(heap_is_empty(test_heap));
    assert_true(heap_is_empty(test_heap2));

    assert_true(heap_merge(test_heap, test_heap2) == 0);

    assert_true(heap_is_valid(test_heap));
    assert_true(heap_is_valid(test_heap2));
    assert_true(heap_size(test_heap) == 100000);
    assert_true(heap_size(test_heap2) == 0);
    assert_false(heap_is_empty(test_heap));
    assert_true(heap_is_empty(test_heap2));

    heap_free(test_heap2);
    test_heap2 = NULL;
}

void test_heap_merge_existing_with_existing(void)
{
    assert_true(heap_merge(test_heap, test_heap2) == 0);

    assert_true(heap_is_valid(test_heap));
    assert_true(heap_is_valid(test_heap2));
    assert_true(heap_size(test_heap) == 200000);
    assert_true(heap_size(test_heap2) == 100000);
    assert_false(heap_is_empty(test_heap));
    assert_false(heap_is_empty(test_heap2));
}

void test_fixture_heap_merge(void)
{
    test_fixture_start();

    run_test(test_heap_merge_empty_with_empty);

    fixture_setup(heap_setup_ints_random);
    fixture_teardown(heap_teardown);

    run_test(test_heap_merge_empty_with_existing);
    run_test(test_heap_merge_existing_with_empty);

    fixture_setup(heap_double_setup_ints_random);
    fixture_teardown(heap_double_teardown_merge_safe);

    run_test(test_heap_merge_existing_with_existing);

    test_fixture_end();
}


void all_tests(void)
{
    test_fixture_heap_create();
    test_fixture_heap_push();
    test_fixture_heap_pop();
    test_fixture_heap_top();
    test_fixture_heap_remove();
    test_fixture_heap_merge();
}

int main(int argc, char *argv[])
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    srand(tv.tv_usec * tv.tv_sec);

    return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}
