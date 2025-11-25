# Migration from 2014 to 2025 FreeBSD AVL Tree

## Quick Migration Guide

If you're upgrading from the 2014 version (`../mine/`) to this 2025 version:

### Header Changes

**Old (2014):**
```c
#include "avl.h"  // Single file with everything embedded
```

**New (2025):**
```c
#include "avl.h"  // Now includes avl_impl.h automatically
```

### Code Changes Required

#### 1. Tree Structure Access
The `avl_tree` structure changed:
- **Removed**: `avl_size` field
- **Added**: `avl_pad` field (for ABI compatibility)

If you directly accessed `tree->avl_size`, you need to remove those references.

#### 2. New Functions Available

```c
// New in 2025: Swap two trees
avl_swap(avl_tree_t *tree1, avl_tree_t *tree2);
```

#### 3. New Helper Macros

```c
// Sign extraction
int sign = TREE_ISIGN(value);  // Returns -1, 0, or 1

// Compare two values
int cmp = TREE_CMP(a, b);  // Returns -1, 0, or 1

// Compare two pointers
int pcmp = TREE_PCMP(ptr1, ptr2);  // Returns -1, 0, or 1
```

### Example Comparator Update

**Old style (still works):**
```c
int compare(const void *a, const void *b) {
    int ka = ((my_data_t*)a)->key;
    int kb = ((my_data_t*)b)->key;
    if (ka < kb) return -1;
    if (ka > kb) return 1;
    return 0;
}
```

**New style (using helpers):**
```c
int compare(const void *a, const void *b) {
    int ka = ((my_data_t*)a)->key;
    int kb = ((my_data_t*)b)->key;
    return TREE_CMP(ka, kb);  // More concise
}
```

### Compatibility

The new version is **API compatible** with the old version. Existing code using:
- `avl_create()`
- `avl_add()`
- `avl_remove()`
- `avl_find()`
- `avl_first()`, `avl_last()`
- `AVL_NEXT()`, `AVL_PREV()`
- `avl_update()`, `avl_update_lt()`, `avl_update_gt()`
- All other existing functions

will work without modification.

### Binary Compatibility

The `avl_node` structure is **unchanged**, so the on-disk/in-memory layout is compatible.

The `avl_tree` structure changed slightly:
- 2014: Had `avl_size` field
- 2025: Has `avl_pad` field instead

This means **binary compatibility** depends on whether you serialize `avl_tree` structures. The node data itself is compatible.

### Build System Changes

**Old Makefile:**
```makefile
gcc -o prog prog.c avl.c
```

**New Makefile:**
```makefile
# Can use the provided Makefile
make
make test

# Or manually:
gcc -o prog prog.c avl.c
```

No changes needed for simple compilation.

## Testing Your Migration

1. Rebuild your code with the new headers
2. Run your existing test suite
3. Verify tree operations work correctly
4. Check for any warnings about structure access

## Performance

The 2025 version includes optimizations:
- Replaced lookup tables with inline expressions in hot paths
- Uses `memcpy()` for structure copying (better optimized by compilers)
- More efficient balance calculations

You may see slight performance improvements, especially on modern CPUs.

## Summary

For most users: **No code changes required!** Just update your headers and rebuild.

Only if you:
- Directly accessed `tree->avl_size` → Remove these accesses (was removed)
- Want to use new features → See above for `avl_swap()` and helper macros
