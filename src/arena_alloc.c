/**
* arena_alloc.c
*
* Simple memory arena allocator with memory corruption detection for debug build.
*
* @author: Charles Paul <dev.charles15531@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <arena_alloc.h>

/**
  * Creates an arena with given size.
  *
  * @param size_t size The size of memory of the arena
  *
  * @return Arena The created arena.
  */
Arena arena_create(size_t size) {
  Arena arena;

  arena.memory_top = malloc(size);
  if(arena.memory_top == NULL) {
    fprintf(stderr, "Error: malloc failed at %s:%d\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }

  arena.offset = 0;
  arena.capacity = size;

  return arena;
}


/**
  * Allocates desired block of memory for use from the arena.
  *
  * @param Arena *arena Pointer to the arena to create allocation in.
  * @param size_t size The size to allocate in the arena.
  * @param size_t alignment Alignment for data type to create this allocation for.
  *
  * @return void * Block of allocated memory.
  */
void *arena_alloc(Arena *arena, size_t size, size_t alignment) {
  uintptr_t current = (uintptr_t)(arena->memory_top + arena->offset);
  size_t metadata_size = 0;
  uintptr_t metadata_start = current;

  #ifdef DEBUG
    // Align metadata
    size_t metadata_alignment = alignof(size_t);
    metadata_start = (current + metadata_alignment - 1) & ~(metadata_alignment - 1);
    size_t metadata_padding = metadata_start - current;

    metadata_size = sizeof(size_t) + CANARY_SIZE;
  #endif

  uintptr_t w_mem_start = metadata_start + metadata_size;  // writable memory start.
  w_mem_start = (w_mem_start + alignment - 1) & ~(alignment - 1);

  #ifdef DEBUG
    // get writable memory padding.
    size_t padding = w_mem_start - (metadata_start + metadata_size);

    // Predict where the append canary would be
    uintptr_t raw_canary_pos = w_mem_start + size;
    uintptr_t aligned_canary_pos = (raw_canary_pos + CANARY_ALIGNMENT - 1) & ~(CANARY_ALIGNMENT - 1);
    size_t canary_padding = aligned_canary_pos - raw_canary_pos;

    size_t total_size = metadata_padding + metadata_size + padding + size + canary_padding + CANARY_SIZE;
  #else
    size_t total_size = (w_mem_start - current) + size; 
  #endif

  if((arena->offset + total_size) > arena->capacity) {
    fprintf(stderr, "Error: arena capacity full.");
    return NULL;
  }

  uint8_t *ptr;

  #ifdef DEBUG
    ptr = (uint8_t *)metadata_start;
    *(size_t *)(ptr) = size; // to track allocation size
    *(uint32_t *)(ptr + sizeof(size_t)) = CANARY_VALUE; // prepend canary
  
    // write data goes here.
    // printf("Total size: %zu\n", total_size);
    ptr += metadata_size + padding;

    *(uint32_t *)aligned_canary_pos = CANARY_VALUE;  // append canary.
  #else
    ptr = (uint8_t *)w_mem_start;
  #endif

  arena->offset += total_size;

  return ptr;
} 


/**
  * Reset all allocations in the arena for later use.
  *
  * @param Arena *arena Pointer to the arena to reset.
  */
void arena_reset(Arena *arena) {
  #ifdef DEBUG
    size_t offset = 0;

    while(offset < arena->offset) {
      // Get the start of the current allocation
      uint8_t *current = arena->memory_top + offset;

      // Align to metadata alignment (alignof(size_t))
      size_t metadata_alignment = alignof(size_t);
      uintptr_t metadata_start = ((uintptr_t)current + metadata_alignment - 1) & ~(metadata_alignment - 1);

      // Read total_size from metadata (includes metadata_padding)
      size_t alloc_size = *(size_t *)metadata_start;

      // Validate start canary (immediately after size_t)
      uint32_t start_canary = *(uint32_t *)((char *)metadata_start + sizeof(size_t));
      assert(start_canary == CANARY_VALUE);

      // Compute where writable memory started
      uintptr_t w_mem_start = metadata_start + sizeof(size_t) + sizeof(uint32_t);
      w_mem_start = (w_mem_start + alignof(max_align_t) - 1) & ~(alignof(max_align_t) - 1); // conservative alignment using max_align_t

      // Compute where end canary is
      uintptr_t w_mem_end = w_mem_start + alloc_size;
      uintptr_t aligned_end = (w_mem_end + CANARY_ALIGNMENT - 1) & ~(CANARY_ALIGNMENT - 1);

      // Validate end canary
      uint32_t end_canary = *(uint32_t *)aligned_end;
      assert(end_canary == CANARY_VALUE);

      // Move to next allocation
      size_t total_size = aligned_end + CANARY_SIZE - (uintptr_t)current;

      offset += total_size; // Advance to the next allocation
    }
  #endif

  arena->offset = 0;
}


/**
  * Destroys the arena.
  *
  * @param Arena *arena Pointer to the arena to destroy
  */
void arena_destroy(Arena *arena) {
  free(arena->memory_top);

  arena->memory_top = NULL;
  arena->offset = 0;
  arena->capacity = 0;
}
