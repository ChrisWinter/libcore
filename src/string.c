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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libcore/string.h>
#include <libcore/macros.h>
#include <libcore/utilities.h>

#define STRING_MIN_SIZE  32

struct _string {
    char *data;
    unsigned long size;
    unsigned long capacity;
};


/* nchars indicates if the String should grow or shrink. >0 if
 * adding chars, <0 if removing chars.
 */
static int _string_maybe_resize(String *s, long nchars)
{
    void *new_data = NULL;
    unsigned long new_capacity = 0, pow2 = 0;

    if(0 == nchars) {
        return -1;
    }

    /* Grow or shrink the char array. Use MAX because String
     * capacity always starts off at zero.
     */
    if(nchars > 0) {
        pow2 = util_pow2_next(s->capacity + nchars);
        /* +1 for null terminator */
        if((s->size + nchars + 1) >= s->capacity) {
            new_capacity = MAX(pow2, STRING_MIN_SIZE);
        }
    } else {
        pow2 = util_pow2_prev(s->capacity - 1);
        /* +1 for null terminator.
         * Note: nchars is negative here
         */
        if((s->size + nchars + 1) < (pow2 >> 1)) {
            /* Only downsize if string size is substantially
             * smaller than the previous power of 2 of our
             * current capacity. This attempts to eliminiate
             * excessive resizing if string size just hovers
             * around a power of 2 boundary.
             */
            new_capacity = MAX(pow2, STRING_MIN_SIZE);
        }
    }

    /* No need to resize */
    if(0 == new_capacity) {
        return 0;
    }

    new_data = realloc(s->data, (sizeof(char) * new_capacity));
    if(NULL == new_data) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return -1;
    }

    s->data = new_data;
    s->capacity = new_capacity;

    return 0;
}

String* string_create(void)
{
    String *s;

    s = malloc(sizeof(String));

    s->data = NULL;
    s->size = 0;
    s->capacity = 0;

    return s;
}

String* string_create_from_buf(const char *buf, ssize_t len)
{
    String *s;

    s = string_create();

    if(NULL == buf) {
        return s;
    }

    s->size = len;
    s->capacity = s->size + 1;

    s->data = malloc(sizeof(char) * s->capacity);
    if(NULL == s->data) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    memcpy(s->data, buf, len);

    /* Always null-terminate */
    s->data[s->size] = '\0';

    return s;
}

void string_free(String *s)
{
    if(s != NULL) {
        if(s->data != NULL) {
            free(s->data);
        }
        free(s);
    }
}

/* Complexity: O(1) */
int string_append_char(String *s, char c)
{
    assert(s != NULL);

    if(_string_maybe_resize(s, 1) < 0)
        return -1;

    s->data[s->size] = c;

    s->size++;

    /* Always null-terminate */
    s->data[s->size] = '\0';

    return 0;
}

int string_append_buf(String *s, const char *buf, ssize_t len)
{
    assert(s != NULL);
    assert(buf != NULL);

    if(_string_maybe_resize(s, len) < 0)
        return -1;

    memcpy((s->data + s->size), buf, len);

    s->size += len;

    /* Always null-terminate */
    s->data[s->size] = '\0';

    return 0;
}

/* Complexity: This is a very expensive operation. Each call
 *             is always O(n).
 */
int string_prepend_char(String *s, char c)
{
    assert(s != NULL);

    if(_string_maybe_resize(s, 1) < 0)
        return -1;

    memmove((s->data + 1), s->data, s->size);

    s->data[0] = c;

    s->size++;

    /* Always null-terminate */
    s->data[s->size] = '\0';

    return 0;
}

int string_prepend_buf(String *s, const char *buf, ssize_t len)
{
    long nchars_to_add;

    assert(s != NULL);
    assert(buf != NULL);

    if(buf[len] == '\0') {
        nchars_to_add = len;
    } else {
        nchars_to_add = len + 1;
    }

    if(_string_maybe_resize(s, nchars_to_add) < 0)
        return -1;

    memmove((s->data + nchars_to_add), s->data, s->size);
    memcpy(s->data, buf, nchars_to_add);

    s->size += nchars_to_add;

    /* Always null-terminate */
    s->data[s->size] = '\0';

    return 0;
}

/* Complexity: O(n), worst-case */
int string_insert_char(String *s, unsigned long index, char c)
{
    assert(s != NULL);
    assert(index < s->size);

    if(_string_maybe_resize(s, 1) < 0)
        return -1;

    memmove(s->data + (index + 1),
        s->data + index,
        (s->size - index + 1));

    s->data[index] = c;

    s->size++;

    /* Always null-terminate */
    s->data[s->size] = '\0';

    return 0;
}

int string_insert_buf(String *s, unsigned long index,
        const char *buf, ssize_t len)
{
    long nchars_to_add;

    assert(s != NULL);
    assert(buf != NULL);
    assert(index < s->size);

    if(buf[len] == '\0') {
        nchars_to_add = len;
    } else {
        nchars_to_add = len + 1;
    }

    if(_string_maybe_resize(s, nchars_to_add) < 0)
        return -1;

    memmove(s->data + (index + nchars_to_add + 1),
        s->data + index,
        (s->size - index + 1));

    memcpy((s->data + index), buf, nchars_to_add);

    s->size += nchars_to_add;

    /* Always null-terminate */
    s->data[s->size] = '\0';

    return 0;
}

/* Complexity: O(n), worst-case */
char string_remove_char(String *s, unsigned long index)
{
    char ret;

    assert(s != NULL);
    assert(index < s->size);

    ret = s->data[index];

    memmove((s->data + index),
            (s->data + index) + 1,
            (s->size - index) + 1);

    s->size--;

    if(_string_maybe_resize(s, -1) < 0) {
        return -1;
    }

    /* Always null-terminate */
    s->data[s->size] = '\0';

    return ret;
}

String* string_remove_substring(String *s, unsigned long start_index,
        unsigned long end_index)
{
    String *substring;

    assert(s != NULL);
    assert(start_index < end_index);
    assert(end_index < s->size);

    substring = string_create_from_buf((s->data + start_index),
            (end_index - start_index) + 1);
    if(NULL == substring) {
        fprintf(stderr, "Substring creation failed (%s:%d)\n",
                __FUNCTION__, __LINE__);
        return NULL;
    }

    memmove((s->data + start_index),
            (s->data + end_index) + 1,
            (s->size - end_index) + 1);

    s->size -= (end_index - start_index) + 1;

    if(_string_maybe_resize(s, -1) < 0) {
        /* TODO Error */
    }

    /* Always null-terminate */
    s->data[s->size] = '\0';

    return substring;
}

int string_delete_substring(String *s, unsigned long start_index,
        unsigned long end_index)
{
    assert(s != NULL);
    assert(start_index < end_index);
    assert(end_index < s->size);

    memmove((s->data + start_index),
            (s->data + end_index) + 1,
            (s->size - end_index) + 1);

    s->size -= (end_index - start_index) + 1;

    if(_string_maybe_resize(s, -1) < 0) {
        return -1;
    }

    /* Always null-terminate */
    s->data[s->size] = '\0';

    return 0;
}

int string_replace_char(String *s, unsigned long index, char c)
{
    assert(s != NULL);
    assert(index < s->size);

    s->data[index] = c;

    return 0;
}

int string_swap_char(String *s, unsigned long index1, unsigned long index2)
{
    char tmp;

    assert(s != NULL);
    assert(index1 < index2);
    assert(index2 < s->size);

    tmp = s->data[index1];
    s->data[index1] = s->data[index2];
    s->data[index2] = tmp;

    return 0;
}


/* Immutable Operations */

/* Complexity: O(1) */
char string_char_at(const String *s, unsigned long index)
{
    assert(s != NULL);
    assert(index < s->size);

    return s->data[index];
}

int string_compare(const String *s1, const String *s2)
{
    assert(s1 != NULL);
    assert(s2 != NULL);

    return strcmp(s1->data, s2->data);
}

int string_write(const String *s, int fd)
{
    assert(s != NULL);

    return (int)write(fd, s->data, s->size);
}

int string_find(const String *s, const char *substring,
        unsigned long *found_index)
{
    assert(s != NULL);
    assert(substring != NULL);
    assert(found_index != NULL);

    return string_find_after(s, substring, 0, found_index);
}

int string_find_after(const String *s, const char *substring,
        unsigned long start_index, unsigned long *found_index)
{
    char *c;

    assert(s != NULL);
    assert(substring != NULL);
    assert(start_index < s->size);
    assert(found_index != NULL);

    c = strstr((s->data + start_index), substring);
    if(NULL == c) {
        return -1;
    }

    *found_index = (unsigned long)(c - s->data);

    return 0;
}

int string_rfind(const String *s, const char *substring,
        unsigned long *found_index)
{
    assert(s != NULL);
    assert(substring != NULL);
    assert(found_index != NULL);

    return string_rfind_before(s, substring, (s->size - 1), found_index);
}

int string_rfind_before(const String *s, const char *substring,
        unsigned long start_index, unsigned long *found_index)
{
    unsigned long save_index;
    unsigned long substring_length;

    assert(s != NULL);
    assert(substring != NULL);
    assert(start_index < s->size);
    assert(found_index != NULL);

    substring_length = strlen(substring);

    save_index = 0;
    *found_index = 0;
    while((*found_index < start_index) &&
            string_find_after(s, substring,
                (save_index + substring_length), found_index) == 0) {
        save_index = *found_index;
    }

    if(0 == save_index) {
        return -1;
    }

    return 0;
}

String* string_concat(const String *s1, const String *s2)
{
    String *new_string;

    assert(s1 != NULL);
    assert(s2 != NULL);

    new_string = string_create_from_buf(s1->data, s1->size);
    if(NULL == new_string) {
        fprintf(stderr, "String creation failed (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    if(!string_is_empty(s2) &&
            string_append_buf(new_string, s2->data, s2->size) < 0) {
        fprintf(stderr, "Failed to append second string (%s:%d)\n",
                __FUNCTION__, __LINE__);
        string_free(new_string);
        return NULL;
    }

    assert(string_is_empty(new_string) == 0);
    assert(string_length(new_string) == (string_length(s1) + string_length(s2)));

    return new_string;
}

String* string_substring(const String *s, unsigned long start_index,
        unsigned long end_index)
{
    String *new_string;

    assert(s != NULL);
    assert(start_index <= end_index);
    assert(end_index < s->size);

    /* Can't substring a string of length < 2 */
    if(s->size < 2)
        return NULL;

    new_string = string_create_from_buf((s->data + start_index),
            ((end_index - start_index) + 1));

    if(NULL == new_string) {
        fprintf(stderr, "Failed to create substring(%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    /* +1 to account for zero index */
    assert(string_length(new_string) == ((end_index - start_index) + 1));

    return new_string;
}

String* string_duplicate(const String *s)
{
    String *new_string;

    assert(s != NULL);

    if(string_is_empty(s)) {
        new_string = string_create();
    } else {
        new_string = string_create_from_buf(s->data, s->size);
    }

    if(NULL == new_string) {
        fprintf(stderr, "Failed to create new string(%s:%d)\n",
                __FUNCTION__, __LINE__);
        return NULL;
    }

    assert(string_length(new_string) == string_length(s));

    return new_string;
}

DArray* string_split(const String *s, char delim)
{
    DArray *strings;
    char *start, *end;

    assert(s != NULL);

    /* Can't split a string with 1 or 0 chars */
    if(string_length(s) < 2) {
        return NULL;
    }

    strings = NULL;

    start = s->data;
    end = strchr(start, delim);
    if(NULL != end) {
        strings = darray_create();

        while(NULL != end) {
            darray_append(strings,
                    string_create_from_buf(start, (end - start)));
            start = end + 1;
            end = strchr(start, delim);
        }

        if(*start != '\0') {
            darray_append(strings,
                    string_create_from_buf(start,
                        /* +1 to count start char inclusively */
                        (((s->data + s->size) - start) + 1)));
        }
    }

    return strings;
}

String* string_join(const DArray *strings)
{
    String *new_string, *tmp;
    unsigned long i, len;

    assert(strings != NULL);

    if(darray_is_empty(strings)) {
        return NULL;
    }

    new_string = string_create();
    len = 0;

    for(i = 0; i < darray_size(strings); i++) {
        tmp = (String *)darray_index(strings, i);
        len += string_length(tmp);

        tmp = string_concat(new_string, tmp); /* Reuse tmp */

        /* Reassign to new_string */
        string_free(new_string);
        new_string = tmp;
    }

    assert(string_length(new_string) == len);

    return new_string;
}

String* string_join_with_delim(const DArray *strings, char delim)
{
    String *new_string, *tmp;
    unsigned long i, len;

    assert(strings != NULL);

    if(darray_is_empty(strings)) {
        return NULL;
    }

    new_string = string_create();
    len = 0;

    for(i = 0; i < (darray_size(strings) - 1); i++) {
        tmp = (String *)darray_index(strings, i);
        len += string_length(tmp);

        tmp = string_concat(new_string, tmp); /* Reuse tmp */
        string_append_char(tmp, delim);
        len++; /* Count delimiter */

        /* Reassign to new_string */
        string_free(new_string);
        new_string = tmp;
    }

    /* Join last string without a delimiter */
    tmp = (String *)darray_index(strings, i);
    len += string_length(tmp);
    tmp = string_concat(new_string, tmp);
    string_free(new_string);
    new_string = tmp;

    assert(string_length(new_string) == len);

    return new_string;
}

const char* string_get_chars(const String *s)
{
    assert(s != NULL);

    return s->data;
}

int string_is_empty(const String *s)
{
    assert(s != NULL);

    return (s->size == 0);
}

unsigned long string_length(const String *s)
{
    assert(s != NULL);

    return s->size;
}
