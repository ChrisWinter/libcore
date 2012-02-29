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
#include <macros.h>
#include <set.h>

#define ARRAY_LEN(x)    sizeof((x)) / sizeof((x)[0])

static Set *test_set1 = NULL;
static Set *test_set2 = NULL;

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

/* Utility function to test if a set's contents match
 * the contents of a given reference array
 */
int contents_match(Set *s, const unsigned long *ref, unsigned long reflen)
{
    unsigned long i;
    SetIterator *it;

    if(set_size(s) != reflen) {
        return -1;
    }

    for(it = set_begin(s), i = 0;
            it != NULL && i < reflen; it = set_next(it), i++) {
        if(ulong_compare(set_get_value(it), &ref[i]) != 0) {
            return 0;
        }
    }

    return 1;
}


/* Test fixture setup and teardown */
static unsigned long test_data1[] = { 0, 1, 2, 3, 4, 6, 9, 10, 1, 10 };
static unsigned long test_data2[] = { 5, 6, 7, 8, 9, 22, 1, 3, 99 };

/* Desired results of set operations using the above set creation data */
static unsigned long union_td1_td2[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 22, 99 };
static unsigned long intersect_td1_td2[] = { 1, 3, 6, 9 };
static unsigned long diff_td1_td2[] = { 0, 2, 4, 10 };
static unsigned long symdiff_td1_td2[] = { 0, 2, 4, 5, 7, 8, 10, 22, 99 };

void set_setup_known_ints(void)
{
    unsigned long i, len;

    test_set1 = set_create((CompareFn)ulong_compare);
    assert_true(test_set1 != NULL);
    assert_true(set_is_empty(test_set1));

    test_set2 = set_create((CompareFn)ulong_compare);
    assert_true(test_set2 != NULL);
    assert_true(set_is_empty(test_set2));

    len = sizeof(test_data1) / sizeof(test_data1[0]);
    for(i = 0; i < len; i++) {
        set_insert(test_set1, make_ulong_ptr(test_data1[i]));
    }

    assert_ulong_equal(8, set_size(test_set1));

    len = sizeof(test_data2) / sizeof(test_data2[0]);
    for(i = 0; i < len; i++) {
        set_insert(test_set2, make_ulong_ptr(test_data2[i]));
    }

    assert_ulong_equal(9, set_size(test_set2));
}

void set_teardown_known_ints(void)
{
    set_free_all(test_set1, NULL);
    set_free_all(test_set2, NULL);
    test_set1 = NULL;
    test_set2 = NULL;
}

void set_setup_ints(void)
{
    unsigned long i, *val, count = ((arc4random() % 100000) + 1);

    test_set1 = set_create((CompareFn)ulong_compare);
    assert_true(test_set1 != NULL);
    assert_true(set_is_empty(test_set1));

    for(i = 0; i < count; i++) {
        val = make_ulong_ptr((arc4random() % 50000));
        if(val != NULL) {
            set_insert(test_set1, val);
        }
    }

    assert_true(set_size(test_set1) > 0);
}

void set_teardown(void)
{
    set_free_all(test_set1, NULL);
    test_set1 = NULL;
}


void test_set_create(void)
{
    test_set1 = set_create((CompareFn)ulong_compare);

    assert_true(test_set1 != NULL);
    assert_true(set_size(test_set1) == 0);
    assert_true(set_is_empty(test_set1));

    set_free(test_set1);
    test_set1 = NULL;
}

void test_fixture_set_create(void)
{
    test_fixture_start();
    run_test(test_set_create);
    test_fixture_end();
}


void test_set_insert(void)
{
    unsigned long *val, old_size;

    assert_true(test_set1 != NULL);
    assert_false(set_is_empty(test_set1));

    old_size = set_size(test_set1);

    val = make_ulong_ptr(500000);
    if(val != NULL) {
        assert_true(set_insert(test_set1, val) == 0);
    }

    assert_true(set_size(test_set1) == (old_size + 1));
    assert_true(set_is_member(test_set1, val));
}

void test_fixture_set_insert(void)
{
    test_fixture_start();
    fixture_setup(set_setup_ints);
    fixture_teardown(set_teardown);
    run_test(test_set_insert);
    test_fixture_end();
}

void test_set_remove(void)
{
    unsigned long search_key, old_size;
    unsigned long *val;

    assert_true(test_set1 != NULL);
    assert_false(set_is_empty(test_set1));

    old_size = set_size(test_set1);

    val = NULL;
    search_key = 7;

    val = set_remove(test_set1, &search_key);
    if(val != NULL) {
        free(val);
        assert_true(set_size(test_set1) == (old_size - 1));
    }
}
void test_fixture_set_remove(void)
{
    test_fixture_start();
    fixture_setup(set_setup_ints);
    fixture_teardown(set_teardown);
    run_test(test_set_remove);
    test_fixture_end();
}

void test_set_random_insert_and_remove(void)
{
    unsigned long i, old_size, *val;
    unsigned long count = 100000;
    unsigned long max_size = 0, inserts = 0, removes = 0;

    test_set1 = set_create((CompareFn)ulong_compare);
    assert_true(test_set1 != NULL);
    assert_true(set_is_empty(test_set1));

    for(i = 0; i < count; i++) {
        val = make_ulong_ptr((arc4random_uniform(100)));
        if(val != NULL) {
            old_size = set_size(test_set1);
            if(arc4random_uniform(2)) {
                if(set_insert(test_set1, val) == 0) {
                    inserts++;
                    assert_true(set_size(test_set1) == (old_size + 1));
                }
            } else {
                val = set_remove(test_set1, val);
                if(val != NULL) {
                    removes++;
                    assert_true(set_size(test_set1) == (old_size - 1));
                    free(val);
                }
            }
        }
        max_size = MAX(max_size, set_size(test_set1));
    }

    printf("Max size: %lu\n", max_size);
    printf("Inserts: %lu\n", inserts);
    printf("Removes: %lu\n", removes);

    set_free_all(test_set1, NULL);
    test_set1 = NULL;
}

void test_fixture_set_random_insert_and_remove(void)
{
    test_fixture_start();
    run_test(test_set_random_insert_and_remove);
    test_fixture_end();
}


void test_set_is_equal(void)
{
    assert_true(set_is_equal(test_set1, test_set1));
}

void test_set_is_not_equal(void)
{
    assert_false(set_is_equal(test_set1, test_set2));
}

void test_fixture_set_is_equal(void)
{
    test_fixture_start();
    fixture_setup(set_setup_known_ints);
    fixture_teardown(set_teardown_known_ints);
    run_test(test_set_is_equal);
    run_test(test_set_is_not_equal);
    test_fixture_end();
}


void test_set_is_subset(void)
{
    assert_true(set_is_subset(test_set1, test_set1));
}

void test_set_is_not_subset(void)
{
    assert_false(set_is_subset(test_set1, test_set2));
}

void test_fixture_set_is_subset(void)
{
    test_fixture_start();
    fixture_setup(set_setup_known_ints);
    fixture_teardown(set_teardown_known_ints);
    run_test(test_set_is_subset);
    run_test(test_set_is_not_subset);
    test_fixture_end();
}


void test_set_is_member(void)
{
    assert_true(set_is_member(test_set1, set_begin(test_set1)));
}

void test_set_is_not_member(void)
{
    unsigned long val = 23423424;
    assert_false(set_is_member(test_set1, &val));
}

void test_fixture_set_is_member(void)
{
    test_fixture_start();
    fixture_setup(set_setup_known_ints);
    fixture_teardown(set_teardown_known_ints);
    run_test(test_set_is_member);
    run_test(test_set_is_not_member);
    test_fixture_end();
}


void test_set_union(void)
{
    Set *result = NULL;

    result = set_union(test_set1, test_set2);

    assert_true(result != NULL);
    assert_true(contents_match(result, union_td1_td2, ARRAY_LEN(union_td1_td2)));
    set_free(result);
}

void test_fixture_set_union(void)
{
    test_fixture_start();
    fixture_setup(set_setup_known_ints);
    fixture_teardown(set_teardown_known_ints);
    run_test(test_set_union);
    test_fixture_end();
}


void test_set_intersect(void)
{
    Set *result = NULL;

    result = set_intersect(test_set1, test_set2);

    assert_true(result != NULL);
    assert_true(contents_match(result, intersect_td1_td2, ARRAY_LEN(intersect_td1_td2)));
    set_free(result);
}

void test_fixture_set_intersect(void)
{
    test_fixture_start();
    fixture_setup(set_setup_known_ints);
    fixture_teardown(set_teardown_known_ints);
    run_test(test_set_intersect);
    test_fixture_end();
}


void test_set_diff(void)
{
    Set *result = NULL;

    result = set_diff(test_set1, test_set2);

    assert_true(result != NULL);
    assert_true(contents_match(result, diff_td1_td2, ARRAY_LEN(diff_td1_td2)));
    set_free(result);
}

void test_fixture_set_diff(void)
{
    test_fixture_start();
    fixture_setup(set_setup_known_ints);
    fixture_teardown(set_teardown_known_ints);
    run_test(test_set_diff);
    test_fixture_end();
}


void test_set_symdiff(void)
{
    Set *result = NULL;

    result = set_symdiff(test_set1, test_set2);

    assert_true(result != NULL);
    assert_true(contents_match(result, symdiff_td1_td2, ARRAY_LEN(symdiff_td1_td2)));
    set_free(result);
}

void test_fixture_set_symdiff(void)
{
    test_fixture_start();
    fixture_setup(set_setup_known_ints);
    fixture_teardown(set_teardown_known_ints);
    run_test(test_set_symdiff);
    test_fixture_end();
}

void all_tests(void)
{
    test_fixture_set_create();
    test_fixture_set_insert();
    test_fixture_set_remove();
    test_fixture_set_random_insert_and_remove();
    test_fixture_set_is_equal();
    test_fixture_set_is_subset();
    test_fixture_set_is_member();
    test_fixture_set_union();
    test_fixture_set_intersect();
    test_fixture_set_diff();
    test_fixture_set_symdiff();
}

int main(int argc, char *argv[])
{
    return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}
