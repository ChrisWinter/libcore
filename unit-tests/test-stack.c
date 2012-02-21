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
#include <stack.h>

static Stack *test_stack = NULL;

unsigned long* make_ulong_ptr(unsigned long value)
{
    unsigned long *val = NULL;

    val = malloc(sizeof(unsigned long));
    if(val != NULL) {
        *val = value;
    }

    return val;
}
void stack_setup_ints(void)
{
    unsigned long i, *val;

    test_stack = stack_create();

    assert_true(test_stack != NULL);
    assert_true(stack_is_empty(test_stack));

    for(i = 0; i < 1000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            stack_push(test_stack, val);
        }
    }

    assert_true(stack_size(test_stack) == i);
    assert_false(stack_is_empty(test_stack));
}

void stack_teardown(void)
{
    assert_true(test_stack != NULL);

    stack_free_all(test_stack, NULL);
    test_stack = NULL;
}

void test_stack_create(void)
{
    test_stack = stack_create();

    assert_true(test_stack != NULL);
    assert_true(stack_size(test_stack) == 0);
    assert_true(stack_is_empty(test_stack));

    stack_free(test_stack);
    test_stack = NULL;
}

void test_fixture_stack_create(void)
{
    test_fixture_start();
    run_test(test_stack_create);
    test_fixture_end();
}


void test_stack_push_empty(void)
{
    unsigned long *val;

    test_stack = stack_create();

    assert_true(test_stack != NULL);
    assert_true(stack_size(test_stack) == 0);
    assert_true(stack_is_empty(test_stack));

    val = make_ulong_ptr(9999);
    assert_true(stack_push(test_stack, val) == 0);

    assert_true(stack_size(test_stack) == 1);
    assert_false(stack_is_empty(test_stack));

    stack_free_all(test_stack, NULL);
    test_stack = NULL;
}

void test_stack_push_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = stack_size(test_stack);

    val = make_ulong_ptr(9999);
    assert_true(stack_push(test_stack, val) == 0);

    assert_true(stack_size(test_stack) == (old_size + 1));
    assert_false(stack_is_empty(test_stack));
}

void test_fixture_stack_push(void)
{
    test_fixture_start();

    run_test(test_stack_push_empty);

    fixture_setup(stack_setup_ints);
    fixture_teardown(stack_teardown);

    run_test(test_stack_push_existing);

    test_fixture_end();
}


void test_stack_pop_empty(void)
{
    test_stack = stack_create();

    assert_true(test_stack != NULL);
    assert_true(stack_size(test_stack) == 0);
    assert_true(stack_is_empty(test_stack));

    assert_true(stack_pop(test_stack) == NULL);

    assert_true(stack_size(test_stack) == 0);
    assert_true(stack_is_empty(test_stack));

    stack_free(test_stack);
    test_stack = NULL;
}

void test_stack_pop_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = stack_size(test_stack);

    val = stack_pop(test_stack);

    assert_true(val != NULL);
    assert_ulong_equal(999, *val);
    assert_true(stack_size(test_stack) == (old_size - 1));

    free(val);
}

void test_stack_pop_until_empty(void)
{
    unsigned long *val;
    unsigned long old_size;

    while(!stack_is_empty(test_stack)) {
        old_size = stack_size(test_stack);

        val = stack_pop(test_stack);

        assert_true(val != NULL);
        assert_true(stack_size(test_stack) == (old_size - 1));

        free(val);
    }

    assert_true(stack_is_empty(test_stack));
    assert_true(stack_size(test_stack) == 0);
}

void test_fixture_stack_pop(void)
{
    test_fixture_start();

    run_test(test_stack_pop_empty);

    fixture_setup(stack_setup_ints);
    fixture_teardown(stack_teardown);

    run_test(test_stack_pop_existing);
    run_test(test_stack_pop_until_empty);

    test_fixture_end();
}


void test_stack_top_empty(void)
{
    test_stack = stack_create();

    assert_true(test_stack != NULL);
    assert_true(stack_size(test_stack) == 0);
    assert_true(stack_is_empty(test_stack));

    assert_true(stack_top(test_stack) == NULL);

    assert_true(stack_size(test_stack) == 0);
    assert_true(stack_is_empty(test_stack));

    stack_free(test_stack);
    test_stack = NULL;
}

void test_stack_top_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = stack_size(test_stack);

    val = stack_top(test_stack);

    assert_true(val != NULL);
    assert_ulong_equal(999, *val);
    assert_true(stack_size(test_stack) == old_size);
}

void test_fixture_stack_top(void)
{
    test_fixture_start();

    run_test(test_stack_top_empty);

    fixture_setup(stack_setup_ints);
    fixture_teardown(stack_teardown);

    run_test(test_stack_top_existing);

    test_fixture_end();
}


void all_tests(void)
{
    test_fixture_stack_create();
    test_fixture_stack_push();
    test_fixture_stack_pop();
    test_fixture_stack_top();
}

int main(int argc, char *argv[])
{
    return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}
