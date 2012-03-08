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
#include <libcore/queue.h>

static Queue *test_queue = NULL;

unsigned long* make_ulong_ptr(unsigned long value)
{
    unsigned long *val = NULL;

    val = malloc(sizeof(unsigned long));
    if(val != NULL) {
        *val = value;
    }

    return val;
}
void queue_setup_ints(void)
{
    unsigned long i, *val;

    test_queue = queue_create();

    assert_true(test_queue != NULL);
    assert_true(queue_is_empty(test_queue));

    for(i = 0; i < 1000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            queue_enqueue(test_queue, val);
        }
    }

    assert_true(queue_size(test_queue) == i);
    assert_false(queue_is_empty(test_queue));
}

void queue_teardown(void)
{
    assert_true(test_queue != NULL);

    queue_free_all(test_queue, NULL);
    test_queue = NULL;
}

void test_queue_create(void)
{
    test_queue = queue_create();

    assert_true(test_queue != NULL);
    assert_true(queue_size(test_queue) == 0);
    assert_true(queue_is_empty(test_queue));

    queue_free(test_queue);
    test_queue = NULL;
}

void test_fixture_queue_create(void)
{
    test_fixture_start();
    run_test(test_queue_create);
    test_fixture_end();
}


void test_queue_enqueue_empty(void)
{
    unsigned long *val;

    test_queue = queue_create();

    assert_true(test_queue != NULL);
    assert_true(queue_size(test_queue) == 0);
    assert_true(queue_is_empty(test_queue));

    val = make_ulong_ptr(9999);
    assert_true(queue_enqueue(test_queue, val) == 0);

    assert_true(queue_size(test_queue) == 1);
    assert_false(queue_is_empty(test_queue));

    queue_free_all(test_queue, NULL);
    test_queue = NULL;
}

void test_queue_enqueue_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = queue_size(test_queue);

    val = make_ulong_ptr(9999);
    assert_true(queue_enqueue(test_queue, val) == 0);

    assert_true(queue_size(test_queue) == (old_size + 1));
    assert_false(queue_is_empty(test_queue));
}

void test_fixture_queue_enqueue(void)
{
    test_fixture_start();

    run_test(test_queue_enqueue_empty);

    fixture_setup(queue_setup_ints);
    fixture_teardown(queue_teardown);

    run_test(test_queue_enqueue_existing);

    test_fixture_end();
}


void test_queue_dequeue_empty(void)
{
    test_queue = queue_create();

    assert_true(test_queue != NULL);
    assert_true(queue_size(test_queue) == 0);
    assert_true(queue_is_empty(test_queue));

    assert_true(queue_dequeue(test_queue) == NULL);

    assert_true(queue_size(test_queue) == 0);
    assert_true(queue_is_empty(test_queue));

    queue_free(test_queue);
    test_queue = NULL;
}

void test_queue_dequeue_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = queue_size(test_queue);

    val = queue_dequeue(test_queue);

    assert_true(val != NULL);
    assert_ulong_equal(0, *val);
    assert_true(queue_size(test_queue) == (old_size - 1));

    free(val);
}

void test_queue_dequeue_until_empty(void)
{
    unsigned long *val;
    unsigned long old_size;

    while(!queue_is_empty(test_queue)) {
        old_size = queue_size(test_queue);

        val = queue_dequeue(test_queue);

        assert_true(val != NULL);
        assert_true(queue_size(test_queue) == (old_size - 1));

        free(val);
    }

    assert_true(queue_is_empty(test_queue));
    assert_true(queue_size(test_queue) == 0);
}

void test_fixture_queue_dequeue(void)
{
    test_fixture_start();

    run_test(test_queue_dequeue_empty);

    fixture_setup(queue_setup_ints);
    fixture_teardown(queue_teardown);

    run_test(test_queue_dequeue_existing);
    run_test(test_queue_dequeue_until_empty);

    test_fixture_end();
}


void test_queue_front_empty(void)
{
    test_queue = queue_create();

    assert_true(test_queue != NULL);
    assert_true(queue_size(test_queue) == 0);
    assert_true(queue_is_empty(test_queue));

    assert_true(queue_front(test_queue) == NULL);

    assert_true(queue_size(test_queue) == 0);
    assert_true(queue_is_empty(test_queue));

    queue_free(test_queue);
    test_queue = NULL;
}

void test_queue_front_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = queue_size(test_queue);

    val = queue_front(test_queue);

    assert_true(val != NULL);
    assert_ulong_equal(0, *val);
    assert_true(queue_size(test_queue) == old_size);
}

void test_fixture_queue_front(void)
{
    test_fixture_start();

    run_test(test_queue_front_empty);

    fixture_setup(queue_setup_ints);
    fixture_teardown(queue_teardown);

    run_test(test_queue_front_existing);

    test_fixture_end();
}


void test_queue_back_empty(void)
{
    test_queue = queue_create();

    assert_true(test_queue != NULL);
    assert_true(queue_size(test_queue) == 0);
    assert_true(queue_is_empty(test_queue));

    assert_true(queue_back(test_queue) == NULL);

    assert_true(queue_size(test_queue) == 0);
    assert_true(queue_is_empty(test_queue));

    queue_free(test_queue);
    test_queue = NULL;
}

void test_queue_back_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = queue_size(test_queue);

    val = queue_back(test_queue);

    assert_true(val != NULL);
    assert_ulong_equal(999, *val);
    assert_true(queue_size(test_queue) == old_size);
}

void test_fixture_queue_back(void)
{
    test_fixture_start();

    run_test(test_queue_back_empty);

    fixture_setup(queue_setup_ints);
    fixture_teardown(queue_teardown);

    run_test(test_queue_back_existing);

    test_fixture_end();
}


void all_tests(void)
{
    test_fixture_queue_create();
    test_fixture_queue_enqueue();
    test_fixture_queue_dequeue();
    test_fixture_queue_front();
    test_fixture_queue_back();
}

int main(int argc, char *argv[])
{
    return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}
