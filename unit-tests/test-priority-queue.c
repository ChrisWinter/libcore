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
#include <libcore/priority_queue.h>

static PQueue *test_pq = NULL;
static PQueue *test_pq2 = NULL;

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

void pqueue_setup_ints(void)
{
    unsigned long i, *val;

    test_pq = pqueue_create((CompareFn)ulong_compare);
    assert_true(test_pq != NULL);
    assert_true(pqueue_is_empty(test_pq));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            pqueue_push(test_pq, val);
        }
    }

    assert_true(pqueue_size(test_pq) == 100000);
}

void pqueue_setup_ints_random(void)
{
    unsigned long i, *val;

    test_pq = pqueue_create((CompareFn)ulong_compare);
    assert_true(test_pq != NULL);
    assert_true(pqueue_is_empty(test_pq));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(arc4random() % 100000);
        if(val != NULL) {
            pqueue_push(test_pq, val);
        }
    }

    assert_true(pqueue_size(test_pq) == 100000);
}

void pqueue_double_setup_ints(void)
{
    unsigned long i, *val;

    test_pq = pqueue_create((CompareFn)ulong_compare);
    assert_true(test_pq != NULL);
    assert_true(pqueue_is_empty(test_pq));

    test_pq2 = pqueue_create((CompareFn)ulong_compare);
    assert_true(test_pq2 != NULL);
    assert_true(pqueue_is_empty(test_pq2));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            pqueue_push(test_pq, val);
        }

        val = make_ulong_ptr(i);
        if(val != NULL) {
            pqueue_push(test_pq2, val);
        }
    }

    assert_true(pqueue_size(test_pq) == 100000);
    assert_true(pqueue_size(test_pq2) == 100000);
}

void pqueue_double_setup_ints_random(void)
{
    unsigned long i, *val;

    test_pq = pqueue_create((CompareFn)ulong_compare);
    assert_true(test_pq != NULL);
    assert_true(pqueue_is_empty(test_pq));

    test_pq2 = pqueue_create((CompareFn)ulong_compare);
    assert_true(test_pq2 != NULL);
    assert_true(pqueue_is_empty(test_pq2));

    for(i = 0; i < 100000; i++) {
        val = make_ulong_ptr(arc4random() % 100000);
        if(val != NULL) {
            pqueue_push(test_pq, val);
        }

        val = make_ulong_ptr(arc4random() % 100000);
        if(val != NULL) {
            pqueue_push(test_pq2, val);
        }
    }

    assert_true(pqueue_size(test_pq) == 100000);
    assert_true(pqueue_size(test_pq2) == 100000);
}

void pqueue_teardown(void)
{
    pqueue_free_all(test_pq, NULL);
    test_pq = NULL;
}

void pqueue_double_teardown(void)
{
    pqueue_free_all(test_pq, NULL);
    pqueue_free_all(test_pq2,  NULL);
    test_pq = test_pq2 = NULL;
}

void pqueue_double_teardown_merge_safe(void)
{
    pqueue_free_all(test_pq, NULL);
    pqueue_free(test_pq2);
    test_pq = test_pq2 = NULL;
}

void test_pqueue_create(void)
{
    test_pq = pqueue_create((CompareFn)ulong_compare);

    assert_true(test_pq != NULL);
    assert_true(pqueue_size(test_pq) == 0);
    assert_true(pqueue_is_empty(test_pq));

    pqueue_free(test_pq);
    test_pq = NULL;
}

void test_fixture_pqueue_create(void)
{
    test_fixture_start();
    run_test(test_pqueue_create);
    test_fixture_end();
}


void test_pqueue_push_empty(void)
{
    unsigned long *val;

    test_pq = pqueue_create((CompareFn)ulong_compare);

    assert_true(test_pq != NULL);
    assert_true(pqueue_size(test_pq) == 0);
    assert_true(pqueue_is_empty(test_pq));

    val = make_ulong_ptr(9999);
    assert_true(pqueue_push(test_pq, val) == 0);

    /* Verify */
    assert_true(pqueue_size(test_pq) == 1);
    assert_false(pqueue_is_empty(test_pq));

    assert_ulong_equal(9999, *(unsigned long *)pqueue_top(test_pq));

    pqueue_free_all(test_pq, NULL);
    test_pq = NULL;
}

void test_pqueue_push_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = pqueue_size(test_pq);

    val = make_ulong_ptr(9999);
    assert_true(pqueue_push(test_pq, val) == 0);

    /* Verify */
    assert_true(pqueue_size(test_pq) == (old_size + 1));
    assert_false(pqueue_is_empty(test_pq));
}

void test_fixture_pqueue_push(void)
{
    test_fixture_start();

    run_test(test_pqueue_push_empty);

    fixture_setup(pqueue_setup_ints_random);
    fixture_teardown(pqueue_teardown);

    run_test(test_pqueue_push_existing);

    test_fixture_end();
}


void test_pqueue_pop_empty(void)
{
    test_pq = pqueue_create((CompareFn)ulong_compare);

    assert_true(test_pq != NULL);
    assert_true(pqueue_size(test_pq) == 0);
    assert_true(pqueue_is_empty(test_pq));

    assert_true(pqueue_pop(test_pq) == NULL);

    assert_true(pqueue_size(test_pq) == 0);
    assert_true(pqueue_is_empty(test_pq));

    pqueue_free(test_pq);
    test_pq = NULL;
}

void test_pqueue_pop_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = pqueue_size(test_pq);

    val = pqueue_pop(test_pq);

    assert_true(val != NULL);
    assert_ulong_equal(0, *val);
    assert_true(pqueue_size(test_pq) == (old_size - 1));

    free(val);
}

void test_pqueue_pop_until_empty(void)
{
    unsigned long *val;
    unsigned long old_val, old_size;

    old_val = *(unsigned long *)pqueue_top(test_pq);
    while(!pqueue_is_empty(test_pq)) {
        old_size = pqueue_size(test_pq);

        val = pqueue_pop(test_pq);

        assert_true(val != NULL);
        assert_true(ulong_compare(&old_val, val) >= 0);
        assert_true(pqueue_size(test_pq) == (old_size - 1));

        old_val = *val;
        free(val);
    }

    assert_true(pqueue_is_empty(test_pq));
    assert_true(pqueue_size(test_pq) == 0);
}

void test_fixture_pqueue_pop(void)
{
    test_fixture_start();

    run_test(test_pqueue_pop_empty);

    fixture_setup(pqueue_setup_ints);
    fixture_teardown(pqueue_teardown);
    run_test(test_pqueue_pop_existing);

    fixture_setup(pqueue_setup_ints_random);
    fixture_teardown(pqueue_teardown);
    run_test(test_pqueue_pop_until_empty);

    test_fixture_end();
}


void test_pqueue_top_empty(void)
{
    test_pq = pqueue_create((CompareFn)ulong_compare);

    assert_true(test_pq != NULL);
    assert_true(pqueue_size(test_pq) == 0);
    assert_true(pqueue_is_empty(test_pq));

    assert_true(pqueue_top(test_pq) == NULL);

    assert_true(pqueue_size(test_pq) == 0);
    assert_true(pqueue_is_empty(test_pq));

    pqueue_free(test_pq);
    test_pq = NULL;
}

void test_pqueue_top_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = pqueue_size(test_pq);

    val = pqueue_top(test_pq);

    assert_true(val != NULL);
    assert_ulong_equal(0, *val);
    assert_true(pqueue_size(test_pq) == old_size);
}

void test_fixture_pqueue_top(void)
{
    test_fixture_start();

    run_test(test_pqueue_top_empty);

    fixture_setup(pqueue_setup_ints);
    fixture_teardown(pqueue_teardown);

    run_test(test_pqueue_top_existing);

    test_fixture_end();
}


void all_tests(void)
{
    test_fixture_pqueue_create();
    test_fixture_pqueue_push();
    test_fixture_pqueue_pop();
    test_fixture_pqueue_top();
}

int main(int argc, char *argv[])
{
    return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}
