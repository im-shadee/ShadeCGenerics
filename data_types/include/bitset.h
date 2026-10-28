#ifndef BITSET_H
#define BITSET_H

#define DEFAULT_SET_CAPACITY 16

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>

typedef struct {
    size_t capacity; // Total number of bits
    size_t count; // Amount of words allocated
    uint32_t* words;
} bitset;

/**
 * Creates a bitset with a specified number of bits.
 * returns: A pointer to the created bitset, or NULL on error.
 */
bitset* bitset_create(size_t size);

/**
 * Creates a bitset with a specified number of bits and a specified capacity.
 * returns: A pointer to the created bitset, or NULL on error.
 */
bitset* bitset_create_with_capacity(size_t size, size_t capacity);

/** Frees the bitset and its internal words buffer. */
void bitset_free(bitset* set);

/** Clears all bits in the bitset (sets all bits to 0). */
void bitset_clear_all(bitset* set);

/** Sets all bits in the bitset to 1. */
void bitset_set_all(bitset* set);

/** Sets a specific bit to 1.
 * returns: 
 *  - 1 on success, 
 *  - 0 if out of bounds
 */
int bitset_set(bitset* set, size_t index);

/** Clears a specific bit to 0. 
 * returns: 
 *  - 1 on success, 
 *  - 0 if out of bounds
 */
int bitset_clear(bitset* set, size_t index);

/** Tests if a specific bit is set. 
 * returns: 
 *  - 1 if set
 *  - 0 if clear or out of bounds
 */
int bitset_test(bitset* set, size_t index);

/** Performs bitwise union. */
void bitset_union(bitset* dest, const bitset* src1, const bitset* src2);

/** Performs bitwise intersection. */
void bitset_intersection(bitset* dest, const bitset* src1, const bitset* src2);

/** Performs bitwise difference */
void bitset_difference(bitset* dest, const bitset* src1, const bitset* src2);

#endif // BITSET_H
