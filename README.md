# Memory Arena Allocator with Canary Debugging

This is a simple and efficient **memory arena allocator** written in C. It supports **linear allocation** from a pre-allocated memory block and includes **canary values** for detecting memory corruption in **debug builds**.

## Features

- Linear (bump) allocation from a contiguous memory region.
- Fast and simple, with zero overhead in release builds.
- Canary checks to detect memory buffer overflows or underflows in debug mode.
- Manual reset and reuse of the arena.

## How It Works

The memory arena reserves a large block of memory and hands out chunks from the beginning. In debug builds, each allocation includes metadata(canary values and size tracker) before and after the user-visible memory to catch writes outside the allocated bounds and to track allocation size.

In debug mode:

```
[Alloc size tracker][ Canary ][ User Memory ][ Canary ]
```

In release mode:

```
[ User Memory ]
```

## Usage

```c
#define ARENA_SIZE ((size_t)1024) // 1 KB
#define ALIGNMENT ((size_t)16)

Arena arena = arena_create(ARENA_SIZE);

// Allocate memory
void* data = arena_alloc(&arena, ARENA_SIZE, ALIGNMENT);

// Optional: Reset arena to reuse memory
arena_reset(&arena);

// Free arena when done
arena_destroy(&arena);
```

## API

```c
void arena_create(size_t size);
void* arena_alloc(Arena *arena, size_t size, size_t alignment);
void arena_reset(Arena *arena);
void arena_destroy(Arena *arena);
```

In debug mode, an assertion failure will be triggered if a buffer overrun/underrun is detected during a check (typically at `arena_reset()`).

## Canary Implementation (Debug Builds Only)

- Canary values are placed before and after each allocation.
- Canary values are checked at `arena_reset` only.
- If any canary is overwritten, a `ASSERT` triggers a failure.

### Example Canary Setup

```c
#define CANARY_VALUE 0xDEADC0DE
```

## Building

Use `DEBUG=1` to enable canary checking during compilation:

```bash
make DEBUG=1
```

In release mode, exclude `DEBUG=1` to disable canary checks and minimize overhead.

## Limitations

- Not thread-safe.
- Allocations cannot be individually freed.
- All allocations are reset at once using `arena_reset()`.

## License

MIT License - see the [LICENSE](LICENSE) file for details.