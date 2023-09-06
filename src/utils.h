#pragma once

#include <stddef.h>

/**
 * @brief Logs the given message using SDL and exits with EXIT_FAILURE
 */
void die(const char *fmt, ...);

/**
 * @brief Allocates memory and calls `die` on failure.
 *
 * @param size The size of the memory to allocate.
 * @return Pointer to the allocated memory.
 */
void *xmalloc(size_t size);

/**
 * @brief Reallocates `ptr` and calls `die` on failure.
 *
 * @param ptr The pointer to reallocate.
 * @param size The size of the memory to reallocate.
 * @return Pointer to the reallocated memory.
 */
void *xrealloc(void *ptr, size_t size);
