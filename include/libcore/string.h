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

#ifndef __LIBCORE_STRING_H__
#define __LIBCORE_STRING_H__

#if __cplusplus
extern "C" {
#endif

#include <string.h>

#include <libcore/darray.h>
#include <libcore/types.h>

/* Opaque forward declaration */
typedef struct _string String;

String* string_create           (void);
String* string_create_from_buf  (const char *buf, ssize_t len);
void    string_free             (String *s);

/* Mutable Operations */
int     string_append_char      (String *s, char c);
int     string_append_buf       (String *s, const char *buf, ssize_t len);
int     string_prepend_char     (String *s, char c);
int     string_prepend_buf      (String *s, const char *buf, ssize_t len);
int     string_insert_char      (String *s, unsigned long index, char c);
int     string_insert_buf       (String *s, unsigned long index,
                                 const char *buf, ssize_t len);
char    string_remove_char      (String *s, unsigned long index);
String* string_remove_substring (String *s, unsigned long start_index,
                                 unsigned long end_index);
int     string_delete_substring (String *s, unsigned long start_index,
                                 unsigned long end_index);
int     string_replace_char     (String *s, unsigned long index, char c);
int     string_swap_char        (String *s, unsigned long index1,
                                 unsigned long index2);

/* Immutable Operations */
char    string_char_at          (const String *s, unsigned long index);
int     string_compare          (const String *s1, const String *s2);
int     string_write            (const String *s, int fd);
int     string_find             (const String *s, const char *substring,
                                 unsigned long *found_index);
int     string_find_after       (const String *s, const char *substring,
                                 unsigned long start_index,
                                 unsigned long *found_index);
int     string_rfind            (const String *s, const char *substring,
                                 unsigned long *found_index);
int     string_rfind_before     (const String *s, const char *substring,
                                 unsigned long start_index,
                                 unsigned long *found_index);
String* string_concat           (const String *s1, const String *s2);
String* string_substring        (const String *s, unsigned long start_index,
                                 unsigned long end_index);
String* string_duplicate        (const String *s);
DArray* string_split            (const String *s, char delim);
String* string_join             (const DArray *strings);
String* string_join_with_delim  (const DArray *strings, char delim);

const char*     string_get_chars    (const String *s);
int             string_is_empty     (const String *s);

unsigned long   string_length       (const String *s);

#if __cplusplus
}
#endif

#endif
