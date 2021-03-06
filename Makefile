# Copyright (c) 2012, Chris Winter <wintercni@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#    2. Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#    3. Neither the name of the copyright holder nor the
#       names of contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

CC= gcc
CFLAGS= -Wall -Werror -fPIC -O3 -ansi -pedantic
INCLUDES= -I./include/ -I/usr/local/include

CFLAGS_TESTS= -Wall -Werror -O0 -ansi -pedantic -g
LDFLAGS_TESTS= -L.
LIBS_TESTS= -lcore

INSTALL= install
INSTALL_DIR= /usr/local
INSTALL_INCDIR= $(INSTALL_DIR)/include
INSTALL_LIBDIR= $(INSTALL_DIR)/lib

TEST_DIR= unit-tests

SEATEST_OBJS= \
	ext/seatest/seatest.o

LIBCORE_LIB= libcore.so.0.0

LIBCORE_OBJS= \
	src/utilities.o \
	src/darray.o \
	src/slist.o \
	src/dlist.o \
	src/stack.o \
	src/queue.o \
	src/deque.o \
	src/heap.o \
	src/priority_queue.o \
	src/rbtree.o \
	src/set.o \
	src/map.o \
	src/string.o \
	src/graph.o \
	src/graph-algorithms.o

UNIT_TESTS= \
	test-darray \
	test-slist \
	test-dlist \
	test-stack \
	test-queue \
	test-deque \
	test-heap \
	test-priority-queue \
	test-rbtree \
	test-set \
	test-graph

TEST_PROGRAMS= $(addprefix $(TEST_DIR)/, $(UNIT_TESTS))
TEST_OBJS= $(addsuffix .o, $(TEST_PROGRAMS))

all: tests

DEPS= $(LIBCORE_OBJS:.o=.d)
DEPS+= $(TEST_OBJS:.o=.d)

-include $(DEPS)

.PHONY: tests clean all install uninstall

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -c $< -o $@

$(LIBCORE_LIB): $(LIBCORE_OBJS)
	ar rcs libcore.a $(LIBCORE_OBJS)
	$(CC) -shared -Wl,-soname,libcore.so -o $(LIBCORE_LIB) $(LIBCORE_OBJS)

tests: $(LIBCORE_LIB) $(SEATEST_OBJS) $(TEST_PROGRAMS)

$(TEST_PROGRAMS): % : %.c
	$(CC) $(CFLAGS_TESTS) $(INCLUDES) -I./ext/seatest/ -MMD $(SEATEST_OBJS) -o $@ $< $(LDFLAGS_TESTS) $(LIBS_TESTS)

clean:
	rm $(DEPS) $(LIBCORE_OBJS) *.so.* *.a $(TEST_PROGRAMS)

install: $(LIBCORE_LIB)
	$(INSTALL) -d -m 755 '$(INSTALL_INCDIR)'
	$(INSTALL) -d -m 755 '$(INSTALL_LIBDIR)'
	cp -r include/libcore $(INSTALL_INCDIR)/
	cp *.so.* $(INSTALL_LIBDIR)
	cp *.a $(INSTALL_LIBDIR)
	ln -s $(INSTALL_LIBDIR)/$(LIBCORE_LIB) $(INSTALL_LIBDIR)/libcore.so

uninstall:
	rm -fr $(INSTALL_INCDIR)/libcore
	rm $(INSTALL_LIBDIR)/libcore.*
