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
#include <deque.h>

static Deque *test_deque = NULL;

unsigned long* make_ulong_ptr(unsigned long value)
{
    unsigned long *val = NULL;

    val = malloc(sizeof(unsigned long));
    if(val != NULL) {
        *val = value;
    }

    return val;
}
void deque_setup_ints(void)
{
    unsigned long i, *val;

    test_deque = deque_create();

    assert_true(test_deque != NULL);
    assert_true(deque_is_empty(test_deque));

    for(i = 0; i < 1000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            deque_push_back(test_deque, val);
        }
    }

    assert_true(deque_size(test_deque) == i);
    assert_false(deque_is_empty(test_deque));
}

void deque_teardown(void)
{
    assert_true(test_deque != NULL);

    deque_free_all(test_deque, NULL);
    test_deque = NULL;
}

void test_deque_create(void)
{
    test_deque = deque_create();

    assert_true(test_deque != NULL);
    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    deque_free(test_deque);
    test_deque = NULL;
}

void test_fixture_deque_create(void)
{
    test_fixture_start();
    run_test(test_deque_create);
    test_fixture_end();
}


void test_deque_push_front_empty(void)
{
    unsigned long *val;

    test_deque = deque_create();

    assert_true(test_deque != NULL);
    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    val = make_ulong_ptr(9999);
    assert_true(deque_push_front(test_deque, val) == 0);

    assert_true(deque_size(test_deque) == 1);
    assert_false(deque_is_empty(test_deque));

    /* Verify */
    assert_ulong_equal(9999, *(unsigned long *)deque_front(test_deque));

    deque_free_all(test_deque, NULL);
    test_deque = NULL;
}

void test_deque_push_front_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = deque_size(test_deque);

    val = make_ulong_ptr(9999);
    assert_true(deque_push_front(test_deque, val) == 0);

    assert_true(deque_size(test_deque) == (old_size + 1));
    assert_false(deque_is_empty(test_deque));

    /* Verify */
    assert_ulong_equal(9999, *(unsigned long *)deque_front(test_deque));
}

void test_fixture_deque_push_front(void)
{
    test_fixture_start();

    run_test(test_deque_push_front_empty);

    fixture_setup(deque_setup_ints);
    fixture_teardown(deque_teardown);

    run_test(test_deque_push_front_existing);

    test_fixture_end();
}


void test_deque_push_back_empty(void)
{
    unsigned long *val;

    test_deque = deque_create();

    assert_true(test_deque != NULL);
    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    val = make_ulong_ptr(9999);
    assert_true(deque_push_back(test_deque, val) == 0);

    assert_true(deque_size(test_deque) == 1);
    assert_false(deque_is_empty(test_deque));

    /* Verify */
    assert_ulong_equal(9999, *(unsigned long *)deque_back(test_deque));

    deque_free_all(test_deque, NULL);
    test_deque = NULL;
}

void test_deque_push_back_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = deque_size(test_deque);

    val = make_ulong_ptr(9999);
    assert_true(deque_push_back(test_deque, val) == 0);

    assert_true(deque_size(test_deque) == (old_size + 1));
    assert_false(deque_is_empty(test_deque));

    /* Verify */
    assert_ulong_equal(9999, *(unsigned long *)deque_back(test_deque));
}

void test_fixture_deque_push_back(void)
{
    test_fixture_start();

    run_test(test_deque_push_back_empty);

    fixture_setup(deque_setup_ints);
    fixture_teardown(deque_teardown);

    run_test(test_deque_push_back_existing);

    test_fixture_end();
}


void test_deque_pop_front_empty(void)
{
    test_deque = deque_create();

    assert_true(test_deque != NULL);
    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    assert_true(deque_pop_front(test_deque) == NULL);

    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    deque_free(test_deque);
    test_deque = NULL;
}

void test_deque_pop_front_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = deque_size(test_deque);

    val = deque_pop_front(test_deque);

    assert_true(val != NULL);
    assert_ulong_equal(0, *val);
    assert_true(deque_size(test_deque) == (old_size - 1));

    free(val);
}

void test_deque_pop_front_until_empty(void)
{
    unsigned long *val;
    unsigned long old_size;

    while(!deque_is_empty(test_deque)) {
        old_size = deque_size(test_deque);

        val = deque_pop_front(test_deque);

        assert_true(val != NULL);
        assert_true(deque_size(test_deque) == (old_size - 1));

        free(val);
    }

    assert_true(deque_is_empty(test_deque));
    assert_true(deque_size(test_deque) == 0);
}

void test_fixture_deque_pop_front(void)
{
    test_fixture_start();

    run_test(test_deque_pop_front_empty);

    fixture_setup(deque_setup_ints);
    fixture_teardown(deque_teardown);

    run_test(test_deque_pop_front_existing);
    run_test(test_deque_pop_front_until_empty);

    test_fixture_end();
}


void test_deque_pop_back_empty(void)
{
    test_deque = deque_create();

    assert_true(test_deque != NULL);
    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    assert_true(deque_pop_back(test_deque) == NULL);

    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    deque_free(test_deque);
    test_deque = NULL;
}

void test_deque_pop_back_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = deque_size(test_deque);

    val = deque_pop_back(test_deque);

    assert_true(val != NULL);
    assert_ulong_equal(999, *val);
    assert_true(deque_size(test_deque) == (old_size - 1));

    free(val);
}

void test_deque_pop_back_until_empty(void)
{
    unsigned long *val;
    unsigned long old_size;

    while(!deque_is_empty(test_deque)) {
        old_size = deque_size(test_deque);

        val = deque_pop_back(test_deque);

        assert_true(val != NULL);
        assert_true(deque_size(test_deque) == (old_size - 1));

        free(val);
    }

    assert_true(deque_is_empty(test_deque));
    assert_true(deque_size(test_deque) == 0);
}

void test_fixture_deque_pop_back(void)
{
    test_fixture_start();

    run_test(test_deque_pop_back_empty);

    fixture_setup(deque_setup_ints);
    fixture_teardown(deque_teardown);

    run_test(test_deque_pop_back_existing);
    run_test(test_deque_pop_back_until_empty);

    test_fixture_end();
}


void test_deque_front_empty(void)
{
    test_deque = deque_create();

    assert_true(test_deque != NULL);
    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    assert_true(deque_front(test_deque) == NULL);

    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    deque_free(test_deque);
    test_deque = NULL;
}

void test_deque_front_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = deque_size(test_deque);

    val = deque_front(test_deque);

    assert_true(val != NULL);
    assert_ulong_equal(0, *val);
    assert_true(deque_size(test_deque) == old_size);
}

void test_fixture_deque_front(void)
{
    test_fixture_start();

    run_test(test_deque_front_empty);

    fixture_setup(deque_setup_ints);
    fixture_teardown(deque_teardown);

    run_test(test_deque_front_existing);

    test_fixture_end();
}


void test_deque_back_empty(void)
{
    test_deque = deque_create();

    assert_true(test_deque != NULL);
    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    assert_true(deque_back(test_deque) == NULL);

    assert_true(deque_size(test_deque) == 0);
    assert_true(deque_is_empty(test_deque));

    deque_free(test_deque);
    test_deque = NULL;
}

void test_deque_back_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = deque_size(test_deque);

    val = deque_back(test_deque);

    assert_true(val != NULL);
    assert_ulong_equal(999, *val);
    assert_true(deque_size(test_deque) == old_size);
}

void test_fixture_deque_back(void)
{
    test_fixture_start();

    run_test(test_deque_back_empty);

    fixture_setup(deque_setup_ints);
    fixture_teardown(deque_teardown);

    run_test(test_deque_back_existing);

    test_fixture_end();
}


void all_tests(void)
{
    test_fixture_deque_create();
    test_fixture_deque_push_front();
    test_fixture_deque_push_back();
    test_fixture_deque_pop_front();
    test_fixture_deque_pop_back();
    test_fixture_deque_front();
    test_fixture_deque_back();
}

int main(int argc, char *argv[])
{
    return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}
