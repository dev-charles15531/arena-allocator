/**
* arena_alloc.h
*
* Simple memory arena allocator with memory corruption detection for debug build.
*
* @author: Charles Paul <dev.charles15531@gmail.com>
*/

#ifndef ARENA_ALLOC_H
#define ARENA_ALLOC_H

#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>

#define CANARY_VALUE 0xDEADC0DE
#define CANARY_SIZE sizeof(uint32_t)
#define CANARY_ALIGNMENT alignof(uint32_t)

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
