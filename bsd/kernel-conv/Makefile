# Makefile for userspace AVL tree library
# Ported from FreeBSD kernel AVL tree (2025 version)

CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC
AR = ar
ARFLAGS = rcs

# Library files
LIB = libavl.a
SOLIB = libavl.so
OBJS = avl.o

# Test program
TEST = simple_test
TEST_SRC = simple_test.c

# Headers
HEADERS = avl.h avl_impl.h

.PHONY: all clean test

all: $(LIB) $(SOLIB)

# Static library
$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

# Shared library
$(SOLIB): $(OBJS)
	$(CC) -shared -o $@ $^

# Object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Test program
$(TEST): $(TEST_SRC) $(LIB)
	$(CC) $(CFLAGS) -o $@ $< -L. -lavl

test: $(TEST)
	./$(TEST)

clean:
	rm -f $(OBJS) $(LIB) $(SOLIB) $(TEST)

install: $(LIB) $(SOLIB)
	install -d $(DESTDIR)/usr/local/lib
	install -d $(DESTDIR)/usr/local/include
	install -m 644 $(LIB) $(DESTDIR)/usr/local/lib/
	install -m 755 $(SOLIB) $(DESTDIR)/usr/local/lib/
	install -m 644 $(HEADERS) $(DESTDIR)/usr/local/include/
