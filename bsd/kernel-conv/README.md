# FreeBSD AVL Tree - Userspace Port (2025 Version)

This directory contains the userspace port of the FreeBSD AVL tree implementation (2025 version), adapted from the kernel code reorganization.

## What Changed from 2014 to 2025

The FreeBSD AVL tree code was reorganized in 2025 with the following changes:

1. **File Structure**: Split into separate files
   - `avl.h` - Main public API header
   - `avl_impl.h` - Internal implementation structures (new file)
   - `avl.c` - Implementation

2. **Header Organization**:
   - Moved internal structures from `avl.h` to new `avl_impl.h`
   - Changed include paths from embedded definitions to `#include "avl_impl.h"`

3. **New Features**:
   - Added `avl_swap()` function to swap contents of two trees
   - Added helper macros: `TREE_ISIGN()`, `TREE_CMP()`, `TREE_PCMP()`
   - Changed `avl_tree` structure to include `avl_pad` field for ABI compatibility

4. **Code Improvements**:
   - Updated to use `memcpy()` instead of struct assignment in `avl_remove()`
   - Changed child calculation from lookup table to inline expression `(diff > 0)`
   - Changed left calculation in `avl_remove()` from lookup table to `(old_balance > 0)`
   - Updated from `ASSERT(tree->avl_root == NULL)` to `ASSERT0P(tree->avl_root)`
   - Changed from `#ifdef DEBUG` to `#ifdef ZFS_DEBUG` in some places

## Userspace Adaptations

The following kernel-specific code was adapted for userspace:

1. **Header Includes**:
   - Changed `<sys/avl_impl.h>` → `"avl_impl.h"`
   - Changed `<sys/avl.h>` → `"avl.h"`
   - Removed `<sys/debug.h>`, `<sys/cmn_err.h>`, `<sys/mod.h>`
   - Added `<assert.h>`, `<string.h>`

2. **Type Definitions** (in `avl_impl.h`):
   - Added `typedef uint64_t ulong_t;`
   - Added `typedef uint8_t boolean_t;`
   - Added `#define B_TRUE 1` and `#define B_FALSE 0`

3. **Macros** (in `avl.c`):
   - `ASSERT` → `assert`
   - `ASSERT0(x)` → `assert((x) == 0)`
   - `ASSERT0P(x)` → `assert((x) == NULL)`
   - `ASSERT3P(a, op, b)` → `assert((a) op (b))`
   - `ASSERT3U(a, op, b)` → `assert((a) op (b))`
   - `VERIFY(x)` → `assert(x)`

4. **Removed**:
   - `EXPORT_SYMBOL()` macros (kernel module exports)
   - Visibility attribute macros (`_AVL_H`, `_AVL_IMPL_H`)
   - `_KERNEL` conditionals
   - `#ifndef _KERNEL` section (kept the `avl_pad` field unconditionally)

5. **Removed avl_size field**:
   - The new version removed the `avl_size` field from `avl_tree` struct
   - Only kept `avl_pad` for ABI compatibility

## Files

- `avl.h` - Public API header
- `avl_impl.h` - Internal structures and macros
- `avl.c` - Implementation
- `simple_test.c` - Test program
- `Makefile` - Build configuration
- `README.md` - This file

## Building

```bash
# Build the library
make

# Build and run tests
make test

# Clean build artifacts
make clean

# Install to system (requires root)
sudo make install DESTDIR=/
```

## Usage

```c
#include "avl.h"

typedef struct {
    int key;
    avl_node_t node;
} my_data_t;

int compare(const void *a, const void *b) {
    int ka = ((my_data_t*)a)->key;
    int kb = ((my_data_t*)b)->key;
    return (ka > kb) - (ka < kb);  // or TREE_CMP(ka, kb)
}

avl_tree_t tree;
avl_create(&tree, compare, sizeof(my_data_t), offsetof(my_data_t, node));

my_data_t data;
data.key = 42;
avl_add(&tree, &data);

// Traverse
for (my_data_t *p = avl_first(&tree); p; p = AVL_NEXT(&tree, p)) {
    printf("key=%d\n", p->key);
}

avl_destroy(&tree);
```

## License

CDDL-1.0 (Common Development and Distribution License)

## Origin

- **Source**: FreeBSD kernel AVL tree implementation
- **Original**: Ported from OpenSolaris/Illumos
- **Copyright**: Sun Microsystems, Delphix, Nexenta Systems
- **Ported to userspace**: 2025

## References

- [Illumos AVL man page](https://illumos.org/man/9f/avl)
- [FreeBSD source](https://github.com/freebsd/freebsd-src)
