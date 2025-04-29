#ifndef ARENA_ALLOC_H
#define ARENA_ALLOC_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
  uint8_t *memory_top;
  size_t offset;
  size_t capacity;
} Arena;

// create the arena.
Arena arena_create(size_t size);

// allocate block in the arena
void *arena_alloc(Arena *arena, size_t size, size_t alignment);

// reset arena
void arena_reset(Arena *arena);

// destroy arena
void arena_destroy(Arena *arena);

#endif
