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

static DArray *a;

void test_darray_create(void)
{
    DArray *a = NULL;

    a = darray_create();

    assert_true(a != NULL);
    assert_true(darray_is_empty(a));
    assert_true(darray_get_length(a) == 0);
    assert_true(darray_get_capacity(a) == 0);

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
            assert_true(darray_get_length(a) == (i + 1));
            assert_true(darray_get_capacity(a) >= i);

            val = darray_index(a, i);
            assert_ulong_equal(i, *val);
        }
    }

    assert_false(darray_is_empty(a));
    assert_true(darray_get_length(a) == 100);
    assert_true(darray_get_capacity(a) >= 100);

    /* Sanity check end values */
    val = darray_index(a, 0);
    assert_ulong_equal(0, *val);

    val = darray_index(a, darray_get_length(a) - 1);
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

    assert_true(darray_get_length(a) == 100000);
    assert_true(darray_get_capacity(a) >= 100000);
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
            assert_true(darray_get_length(a) == (i + 1));
            assert_true(darray_get_capacity(a) >= i);

            val = darray_index(a, 0);
            assert_ulong_equal(i, *val);
        }
    }

    assert_false(darray_is_empty(a));
    assert_true(darray_get_length(a) == 100);
    assert_true(darray_get_capacity(a) >= 100);

    /* Sanity check end values */
    val = darray_index(a, 0);
    assert_ulong_equal(99, *val);

    val = darray_index(a, darray_get_length(a) - 1);
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
            darray_insert(a, val, darray_get_length(a));
            assert_true(darray_get_length(a) == (i + 1));

            val = darray_index(a, i);
            assert_ulong_equal(i, *val);
        }
    }

    assert_false(darray_is_empty(a));
    assert_true(darray_get_length(a) == 100);
    assert_true(darray_get_capacity(a) >= 100);

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
    i = darray_get_length(a);
    do {
        i--;
        val = darray_remove(a, i);
        free(val);
        assert_true(darray_get_length(a) == i);
        assert_true(darray_get_capacity(a) >= darray_get_length(a));
    } while(i != 0);

    assert_true(darray_is_empty(a));
    assert_true(darray_get_length(a) == 0);
    assert_true(darray_get_capacity(a) == 32);
}


/* Test fixture setup and teardown */

void darray_setup_ints(void)
{
    unsigned long i, *val;

    a = darray_create();
    assert_true(a != NULL);
    assert_true(darray_is_empty(a));

    for(i = 0; i < 100000; i++) {
        val = malloc(sizeof(unsigned long));
        if(val != NULL) {
            *val = i;
            darray_append(a, val);
        }
    }

    assert_true(darray_get_length(a) == 100000);
}

void darray_teardown(void)
{
    darray_free_all(a);
    a = NULL;
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

void all_tests(void)
{
    test_fixture_darray_create();
    test_fixture_darray_append();
    test_fixture_darray_prepend();
    test_fixture_darray_index();
    test_fixture_darray_remove();
}

int main(int argc, char *argv[])
{
    return run_tests(all_tests);
}
