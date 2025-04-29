#include <stdio.h>
#include <assert.h>
#include "arena_alloc.h"

#define TEST_ALIGNMENT ((size_t)16)
#define TEST_BLOCK_COUNT 5
#define TEST_BLOCK_SIZE 8

// Utility: Check if the pointer is properly aligned
void test_alignment(void *ptr, size_t alignment) {
    assert(((uintptr_t)ptr % alignment) == 0 && "Pointer is not aligned correctly");
}

int main() {
  // --- Test 1: Create the arena and allocate memory
  printf("[Test 1] Creating and allocating blocks...\n");
  Arena arena = arena_create(1024);  // 1 KB arena for testing
  printf("  ✅ Arena created with capacity: %zu bytes\n", arena.capacity);

  // Allocate blocks of memory
  for (int i = 0; i < TEST_BLOCK_COUNT; i++) {
    void *ptr = arena_alloc(&arena, TEST_BLOCK_SIZE, TEST_ALIGNMENT);
    assert(ptr);  // Ensure allocation is successful
    test_alignment(ptr, TEST_ALIGNMENT);  // Test that the pointer is aligned
    printf("  ✅ Allocated block %d at %p\n", i + 1, ptr);
  }

  // --- Test 2: Reset the arena and reuse it for new allocations
  printf("\n[Test 2] Resetting and reusing the arena...\n");
  arena_reset(&arena);  // Reset the arena for reuse
  assert(arena.offset == 0);  // Ensure the arena offset is reset to 0

  // Allocate a block after resetting the arena
  void *ptr = arena_alloc(&arena, TEST_BLOCK_SIZE, TEST_ALIGNMENT);
  assert(ptr);  // Ensure allocation is successful
  printf(" ✅ Arena reset and reused successfully, new allocation at %p\n", ptr);

  // --- Test 3: Try to overflow the arena (requesting more memory than available)
  printf("\n[Test 3] Try to overflow the arena...\n");
  arena_reset(&arena);
  void *overflow = arena_alloc(&arena, 2048, TEST_ALIGNMENT); // Try allocating more than available
  assert(overflow == NULL && "Expected allocation to fail due to arena overflow");
  printf(" ✅ Overflow test passed: allocation failed as expected\n");

  // --- Test 4: Destroy the arena and free resources
  printf("\n[Test 4] Destroying the arena...\n");
  arena_destroy(&arena);  // Clean up the arena
  printf("✅ Arena destroyed. All tests passed.\n");

  return 0;
}
