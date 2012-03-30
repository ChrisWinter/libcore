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
#include <libcore/slist.h>

static SList *test_slist = NULL;

unsigned long* make_ulong_ptr(unsigned long value)
{
    unsigned long *val = NULL;

    val = malloc(sizeof(unsigned long));
    if(val != NULL) {
        *val = value;
    }

    return val;
}
void slist_setup_ints(void)
{
    unsigned long i, *val;

    test_slist = slist_create();

    assert_true(test_slist != NULL);
    assert_true(slist_is_empty(test_slist));

    for(i = 0; i < 1000; i++) {
        val = make_ulong_ptr(i);
        if(val != NULL) {
            slist_append(test_slist, val);
        }
    }
}

void slist_teardown(void)
{
    assert_true(test_slist != NULL);

    slist_free_all(test_slist, NULL);
    test_slist = NULL;
}

void test_slist_create(void)
{
    test_slist = slist_create();

    assert_true(test_slist != NULL);
    assert_true(slist_size(test_slist) == 0);
    assert_true(slist_is_empty(test_slist));

    slist_free(test_slist);
    test_slist = NULL;
}

void test_fixture_slist_create(void)
{
    test_fixture_start();
    run_test(test_slist_create);
    test_fixture_end();
}


void test_slist_index(void)
{
    unsigned long *val;

    /* Index the front */
    val = slist_index(test_slist, 0);
    assert_ulong_equal(0, *val);

    /* Index the back */
    val = slist_index(test_slist, slist_size(test_slist) - 1);
    assert_ulong_equal(999, *val);

    /* Index some random place */
    val = slist_index(test_slist, 47);
    assert_ulong_equal(47, *val);
}

void test_fixture_slist_index(void)
{
    test_fixture_start();
    fixture_setup(slist_setup_ints);
    fixture_teardown(slist_teardown);
    run_test(test_slist_index);
    test_fixture_end();
}

void test_slist_insert_at_front(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = slist_size(test_slist);
    val = make_ulong_ptr(8888);
    assert_true(slist_insert(test_slist, 0, val) == 0);

    /* Verify */
    val = NULL;
    val = slist_index(test_slist, 0);
    assert_ulong_equal(8888, *val);
    assert_true((old_size + 1) == slist_size(test_slist));
}

void test_slist_insert_at_back(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = slist_size(test_slist);
    val = make_ulong_ptr(7777);
    assert_true(slist_insert(test_slist, old_size, val) == 0);

    /* Verify */
    val = NULL;
    val = slist_index(test_slist, old_size);
    assert_ulong_equal(7777, *val);
    assert_true((old_size + 1) == slist_size(test_slist));
}

void test_slist_insert_in_middle(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = slist_size(test_slist);
    val = make_ulong_ptr(6666);
    assert_true(slist_insert(test_slist, old_size / 2, val) == 0);

    /* Verify */
    val = NULL;
    val = slist_index(test_slist, old_size / 2);
    assert_ulong_equal(6666, *val);
    assert_true((old_size + 1) == slist_size(test_slist));
}

void test_fixture_slist_insert(void)
{
    test_fixture_start();
    fixture_setup(slist_setup_ints);
    fixture_teardown(slist_teardown);
    run_test(test_slist_insert_at_front);
    run_test(test_slist_insert_at_back);
    run_test(test_slist_insert_in_middle);
    test_fixture_end();
}

void test_slist_append_to_empty(void)
{
    unsigned long *val = NULL;

    assert_true(test_slist == NULL);

    test_slist = slist_create();

    assert_true(test_slist != NULL);
    assert_true(slist_is_empty(test_slist));

    val = make_ulong_ptr(9999);
    assert_true(val != NULL);

    assert_true(slist_append(test_slist, val) == 0);

    /* Verify */
    val = NULL;
    val = slist_index(test_slist, 0);
    assert_true(val != NULL);
    assert_ulong_equal(9999, *val);
    assert_true(slist_size(test_slist) == 1);

    slist_free_all(test_slist, NULL);
    test_slist = NULL;
}

void test_slist_append_to_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = slist_size(test_slist);
    val = make_ulong_ptr(7777);
    assert_true(slist_append(test_slist, val) == 0);

    /* Verify */
    val = NULL;
    val = slist_index(test_slist, (slist_size(test_slist) - 1));
    assert_ulong_equal(7777, *val);
    assert_true((old_size + 1) == slist_size(test_slist));
}

void test_fixture_slist_append(void)
{
    test_fixture_start();

    run_test(test_slist_append_to_empty);

    fixture_setup(slist_setup_ints);
    fixture_teardown(slist_teardown);

    run_test(test_slist_append_to_existing);

    test_fixture_end();
}

void test_slist_prepend_to_empty(void)
{
    unsigned long *val = NULL;

    assert_true(test_slist == NULL);

    test_slist = slist_create();

    assert_true(test_slist != NULL);
    assert_true(slist_is_empty(test_slist));

    val = make_ulong_ptr(9999);
    assert_true(val != NULL);

    assert_true(slist_prepend(test_slist, val) == 0);

    /* Verify */
    val = NULL;
    val = slist_index(test_slist, 0);
    assert_true(val != NULL);
    assert_ulong_equal(9999, *val);
    assert_true(slist_size(test_slist) == 1);

    slist_free_all(test_slist, NULL);
    test_slist = NULL;
}

void test_slist_prepend_to_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = slist_size(test_slist);
    val = make_ulong_ptr(7777);
    assert_true(slist_prepend(test_slist, val) == 0);

    /* Verify */
    val = NULL;
    val = slist_index(test_slist, 0);
    assert_ulong_equal(7777, *val);
    assert_true((old_size + 1) == slist_size(test_slist));
}

void test_fixture_slist_prepend(void)
{
    test_fixture_start();

    run_test(test_slist_prepend_to_empty);

    fixture_setup(slist_setup_ints);
    fixture_teardown(slist_teardown);

    run_test(test_slist_prepend_to_existing);

    test_fixture_end();
}

void test_slist_remove_index_from_empty(void)
{
    assert_true(test_slist == NULL);

    test_slist = slist_create();

    assert_true(test_slist != NULL);
    assert_true(slist_is_empty(test_slist));

    assert_true(slist_remove_index(test_slist, 0) == NULL);

    slist_free_all(test_slist, NULL);
    test_slist = NULL;
}

void test_slist_remove_index_from_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = slist_size(test_slist);

    /* Verify */
    val = NULL;
    val = slist_remove_index(test_slist, 0);
    assert_ulong_equal(0, *val);
    assert_true((old_size - 1) == slist_size(test_slist));

    free(val);
}

void test_slist_remove_index_from_existing_until_empty(void)
{
    unsigned long *val;
    unsigned long i;

    val = NULL;
    i = slist_size(test_slist);
    while(i > 0) {
        val = slist_remove_index(test_slist, --i);
        assert_true(val != NULL);
        assert_ulong_equal(i, *val);
        assert_true(i == slist_size(test_slist));
    }

    assert_true(slist_is_empty(test_slist));
}

void test_fixture_slist_remove_index(void)
{
    test_fixture_start();

    run_test(test_slist_remove_index_from_empty);

    fixture_setup(slist_setup_ints);
    fixture_teardown(slist_teardown);

    run_test(test_slist_remove_index_from_existing);
    run_test(test_slist_remove_index_from_existing_until_empty);

    test_fixture_end();
}

void test_slist_remove_data_from_empty(void)
{
    assert_true(test_slist == NULL);

    test_slist = slist_create();

    assert_true(test_slist != NULL);
    assert_true(slist_is_empty(test_slist));

    assert_true(slist_remove_data(test_slist, NULL) == -1);

    slist_free_all(test_slist, NULL);
    test_slist = NULL;
}

void test_slist_remove_data_from_existing(void)
{
    unsigned long *val;
    unsigned long old_size;

    old_size = slist_size(test_slist);

    val = NULL;
    val = slist_index(test_slist, 10);
    assert_true(val != NULL);

    assert_true(slist_remove_data(test_slist, val) == 0);
    assert_true((old_size - 1) == slist_size(test_slist));
}

void test_slist_remove_data_from_existing_until_empty(void)
{
    unsigned long *val;
    unsigned long i;

    val = NULL;
    i = slist_size(test_slist);
    while(i > 0) {
        val = slist_index(test_slist, --i);
        assert_true(val != NULL);
        assert_true(slist_remove_data(test_slist, val) == 0);
        assert_true(i == slist_size(test_slist));
    }

    assert_true(slist_is_empty(test_slist));
}

void test_fixture_slist_remove_data(void)
{
    test_fixture_start();

    run_test(test_slist_remove_data_from_empty);

    fixture_setup(slist_setup_ints);
    fixture_teardown(slist_teardown);

    run_test(test_slist_remove_data_from_existing);
    run_test(test_slist_remove_data_from_existing_until_empty);

    test_fixture_end();
}

void all_tests(void)
{
    test_fixture_slist_create();
    test_fixture_slist_index();
    test_fixture_slist_insert();
    test_fixture_slist_append();
    test_fixture_slist_prepend();
    test_fixture_slist_remove_index();
    test_fixture_slist_remove_data();
}

int main(int argc, char *argv[])
{
    return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}
