#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdalign.h>
#include "arena_alloc.h"

#define TEST_ALIGNMENT ((size_t)16)
#define TEST_BLOCK_COUNT 5
#define TEST_BLOCK_SIZE 8

#define TEST_ASSERT(cond, message) \
  do { \
    if (!(cond)) { \
      fprintf(stderr, " ❌ Assertion failed: %s\n", message); \
    } \
  } while (0)

// Utility: Fill the allocated memory with a specific pattern for validation
void fill_pattern(void *ptr, size_t size, uint8_t pattern) {
    memset(ptr, pattern, size);
}

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

  #ifdef DEBUG
    printf("\n[Test 3.1] Canary overrun test...\n");
    arena_destroy(&arena);
    arena = arena_create(1024);
    void *test_ptr = arena_alloc(&arena, TEST_BLOCK_SIZE, TEST_ALIGNMENT);
    assert(test_ptr);

    uintptr_t user_ptr = (uintptr_t)test_ptr;

    // Recompute metadata start
    uintptr_t metadata = user_ptr - (sizeof(size_t) + CANARY_SIZE + CANARY_ALIGNMENT); // overestimate to be safe
    uintptr_t metadata_start = (metadata + alignof(size_t) - 1) & ~(alignof(size_t) - 1);

    // Read stored user size
    size_t alloc_size = *(size_t *)metadata_start;

    // Recompute writable memory start
    uintptr_t w_mem_start = metadata_start + sizeof(size_t) + sizeof(uint32_t);
    uintptr_t aligned_w_mem_start = (w_mem_start + TEST_ALIGNMENT - 1) & ~(TEST_ALIGNMENT - 1);

    // Recompute end canary address (aligned)
    uintptr_t w_mem_end = aligned_w_mem_start + alloc_size;
    uintptr_t aligned_w_mem_end = (w_mem_end + CANARY_ALIGNMENT - 1) & ~(CANARY_ALIGNMENT - 1);

    // write to the aligned address
    uint32_t *end_canary = (uint32_t *)aligned_w_mem_end;
    *end_canary = 0xDEADBEEF;

    // arena_reset should assert
    printf(" ⚠️  Canary corrupted. Expecting assertion failure...\n");
    TEST_ASSERT(*end_canary == CANARY_VALUE, "End canary corrupted");

    // Restore canary to clean up
    *end_canary = CANARY_VALUE;

    arena_reset(&arena); // this reset should work
  #endif

  // --- Test 4: Destroy the arena and free resources
  printf("\n[Test 4] Destroying the arena...\n");
  arena_destroy(&arena);  // Clean up the arena
  printf("✅ Arena destroyed. All tests passed.\n");

  return 0;
}
