#include <stdio.h>
#include <stdlib.h>
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

  uintptr_t w_mem_start = current;  // writable memory start.
  w_mem_start = (w_mem_start + alignment - 1) & ~(alignment - 1);

  size_t total_size = (w_mem_start - current) + size; 

  if((arena->offset + total_size) > arena->capacity) {
    fprintf(stderr, "Error: arena capacity full.");
    return NULL;
  }

  uint8_t *ptr;

  ptr = (uint8_t *)w_mem_start;

  arena->offset += total_size;

  return ptr;
} 


/**
  * Reset all allocations in the arena for later use.
  *
  * @param Arena *arena Pointer to the arena to reset.
  */
void arena_reset(Arena *arena) {
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
