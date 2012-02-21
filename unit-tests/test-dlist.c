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
#include <dlist.h>

static DList *test_dlist = NULL;

unsigned long* make_ulong_ptr(unsigned long value)
{
    unsigned long *val = NULL;

    val = malloc(sizeof(unsigned long));
    if(val != NULL) {
        *val = value;
    }

    return val;
}
void dlist_setup_ints(void)
{
    unsigned long i, *val;

    test_dlist = dlist_create();

    assert_true(test_dlist != NULL);
    assert_true(dlist_is_empty(test_dlist));

    for(i = 0; i < 1000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            dlist_append(test_dlist, val);
        }
    }
}

void dlist_teardown(void)
{
    assert_true(test_dlist != NULL);

    dlist_free_all(test_dlist, NULL);
    test_dlist = NULL;
}

void test_dlist_create(void)
{
    test_dlist = dlist_create();

    assert_true(test_dlist != NULL);
    assert_true(dlist_size(test_dlist) == 0);
    assert_true(dlist_is_empty(test_dlist));

    dlist_free(test_dlist);
    test_dlist = NULL;
}

void test_fixture_dlist_create(void)
{
    test_fixture_start();
    run_test(test_dlist_create);
    test_fixture_end();
}


void test_dlist_index(void)
{
    unsigned long *val;

    /* Index the front */
    val = dlist_index(test_dlist, 0);
    assert_ulong_equal(0, *val);

    /* Index the back */
    val = dlist_index(test_dlist, dlist_size(test_dlist) - 1);
    assert_ulong_equal(999, *val);

    /* Index some random place */
    val = dlist_index(test_dlist, 47);
    assert_ulong_equal(47, *val);
}

void test_fixture_dlist_index(void)
{
    test_fixture_start();
    fixture_setup(dlist_setup_ints);
    fixture_teardown(dlist_teardown);
    run_test(test_dlist_index);
    test_fixture_end();
}

void test_dlist_insert_at_front(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = dlist_size(test_dlist);
    val = make_ulong_ptr(8888);
    assert_true(dlist_insert(test_dlist, val, 0) == 0);

    /* Verify */
    val = NULL;
    val = dlist_index(test_dlist, 0);
    assert_ulong_equal(8888, *val);
    assert_true((old_size + 1) == dlist_size(test_dlist));
}

void test_dlist_insert_at_back(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = dlist_size(test_dlist);
    val = make_ulong_ptr(7777);
    assert_true(dlist_insert(test_dlist, val, old_size) == 0);

    /* Verify */
    val = NULL;
    val = dlist_index(test_dlist, old_size);
    assert_ulong_equal(7777, *val);
    assert_true((old_size + 1) == dlist_size(test_dlist));
}

void test_dlist_insert_in_middle(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = dlist_size(test_dlist);
    val = make_ulong_ptr(6666);
    assert_true(dlist_insert(test_dlist, val, old_size / 2) == 0);

    /* Verify */
    val = NULL;
    val = dlist_index(test_dlist, old_size / 2);
    assert_ulong_equal(6666, *val);
    assert_true((old_size + 1) == dlist_size(test_dlist));
}

void test_fixture_dlist_insert(void)
{
    test_fixture_start();
    fixture_setup(dlist_setup_ints);
    fixture_teardown(dlist_teardown);
    run_test(test_dlist_insert_at_front);
    run_test(test_dlist_insert_at_back);
    run_test(test_dlist_insert_in_middle);
    test_fixture_end();
}

void test_dlist_append_to_empty(void)
{
    unsigned long *val = NULL;

    assert_true(test_dlist == NULL);

    test_dlist = dlist_create();

    assert_true(test_dlist != NULL);
    assert_true(dlist_is_empty(test_dlist));

    val = make_ulong_ptr(9999);
    assert_true(val != NULL);

    assert_true(dlist_append(test_dlist, val) == 0);

    /* Verify */
    val = NULL;
    val = dlist_index(test_dlist, 0);
    assert_true(val != NULL);
    assert_ulong_equal(9999, *val);
    assert_true(dlist_size(test_dlist) == 1);

    dlist_free_all(test_dlist, NULL);
    test_dlist = NULL;
}

void test_dlist_append_to_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = dlist_size(test_dlist);
    val = make_ulong_ptr(7777);
    assert_true(dlist_append(test_dlist, val) == 0);

    /* Verify */
    val = NULL;
    val = dlist_index(test_dlist, (dlist_size(test_dlist) - 1));
    assert_ulong_equal(7777, *val);
    assert_true((old_size + 1) == dlist_size(test_dlist));
}

void test_fixture_dlist_append(void)
{
    test_fixture_start();

    run_test(test_dlist_append_to_empty);

    fixture_setup(dlist_setup_ints);
    fixture_teardown(dlist_teardown);

    run_test(test_dlist_append_to_existing);

    test_fixture_end();
}

void test_dlist_prepend_to_empty(void)
{
    unsigned long *val = NULL;

    assert_true(test_dlist == NULL);

    test_dlist = dlist_create();

    assert_true(test_dlist != NULL);
    assert_true(dlist_is_empty(test_dlist));

    val = make_ulong_ptr(9999);
    assert_true(val != NULL);

    assert_true(dlist_prepend(test_dlist, val) == 0);

    /* Verify */
    val = NULL;
    val = dlist_index(test_dlist, 0);
    assert_true(val != NULL);
    assert_ulong_equal(9999, *val);
    assert_true(dlist_size(test_dlist) == 1);

    dlist_free_all(test_dlist, NULL);
    test_dlist = NULL;
}

void test_dlist_prepend_to_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = dlist_size(test_dlist);
    val = make_ulong_ptr(7777);
    assert_true(dlist_prepend(test_dlist, val) == 0);

    /* Verify */
    val = NULL;
    val = dlist_index(test_dlist, 0);
    assert_ulong_equal(7777, *val);
    assert_true((old_size + 1) == dlist_size(test_dlist));
}

void test_fixture_dlist_prepend(void)
{
    test_fixture_start();

    run_test(test_dlist_prepend_to_empty);

    fixture_setup(dlist_setup_ints);
    fixture_teardown(dlist_teardown);

    run_test(test_dlist_prepend_to_existing);

    test_fixture_end();
}

void test_dlist_remove_index_from_empty(void)
{
    assert_true(test_dlist == NULL);

    test_dlist = dlist_create();

    assert_true(test_dlist != NULL);
    assert_true(dlist_is_empty(test_dlist));

    assert_true(dlist_remove_index(test_dlist, 0) == NULL);

    dlist_free_all(test_dlist, NULL);
    test_dlist = NULL;
}

void test_dlist_remove_index_from_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = dlist_size(test_dlist);

    /* Verify */
    val = NULL;
    val = dlist_remove_index(test_dlist, 0);
    assert_ulong_equal(0, *val);
    assert_true((old_size - 1) == dlist_size(test_dlist));

    free(val);
}

void test_dlist_remove_index_from_existing_until_empty(void)
{
    unsigned long *val;
    unsigned long i;

    val = NULL;
    i = dlist_size(test_dlist);
    while(i > 0) {
        val = dlist_remove_index(test_dlist, --i);
        assert_true(val != NULL);
        assert_ulong_equal(i, *val);
        assert_true(i == dlist_size(test_dlist));
    }

    assert_true(dlist_is_empty(test_dlist));
}

void test_fixture_dlist_remove_index(void)
{
    test_fixture_start();

    run_test(test_dlist_remove_index_from_empty);

    fixture_setup(dlist_setup_ints);
    fixture_teardown(dlist_teardown);

    run_test(test_dlist_remove_index_from_existing);
    run_test(test_dlist_remove_index_from_existing_until_empty);

    test_fixture_end();
}

void test_dlist_remove_data_from_empty(void)
{
    assert_true(test_dlist == NULL);

    test_dlist = dlist_create();

    assert_true(test_dlist != NULL);
    assert_true(dlist_is_empty(test_dlist));

    assert_true(dlist_remove_data(test_dlist, NULL) == -1);

    dlist_free_all(test_dlist, NULL);
    test_dlist = NULL;
}

void test_dlist_remove_data_from_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = dlist_size(test_dlist);

    val = NULL;
    val = dlist_index(test_dlist, 10);
    assert_true(val != NULL);

    assert_true(dlist_remove_data(test_dlist, val) == 0);
    assert_true((old_size - 1) == dlist_size(test_dlist));
}

void test_dlist_remove_data_from_existing_until_empty(void)
{
    unsigned long *val;
    unsigned long i;

    val = NULL;
    i = dlist_size(test_dlist);
    while(i > 0) {
        val = dlist_index(test_dlist, --i);
        assert_true(val != NULL);
        assert_true(dlist_remove_data(test_dlist, val) == 0);
        assert_true(i == dlist_size(test_dlist));
    }

    assert_true(dlist_is_empty(test_dlist));
}

void test_fixture_dlist_remove_data(void)
{
    test_fixture_start();

    run_test(test_dlist_remove_data_from_empty);

    fixture_setup(dlist_setup_ints);
    fixture_teardown(dlist_teardown);

    run_test(test_dlist_remove_data_from_existing);
    run_test(test_dlist_remove_data_from_existing_until_empty);

    test_fixture_end();
}

void all_tests(void)
{
    test_fixture_dlist_create();
    test_fixture_dlist_index();
    test_fixture_dlist_insert();
    test_fixture_dlist_append();
    test_fixture_dlist_prepend();
    test_fixture_dlist_remove_index();
    test_fixture_dlist_remove_data();
}

int main(int argc, char *argv[])
{
    return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}
