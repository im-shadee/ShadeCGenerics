#ifndef BITSET_H
#define BITSET_H

#define DEFAULT_SET_CAPACITY 16
#define UINT_BITSIZE sizeof(uint32_t) * CHAR_BIT

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "list_utils.h"

typedef struct {
    size_t capacity; // Total number of bits
    size_t cardinality; // Amount of words allocated
    uint32_t* words;
} bitset;

/**
 * Creates a bitset with a specified number of bits and a specified capacity.
 * returns: A pointer to the created bitset, or NULL on error.
 */
bitset* bitset_create_with_capacity(size_t size, size_t capacity);

/**
 * Creates a bitset with a specified number of bits.
 * returns: A pointer to the created bitset, or NULL on error.
 */
static inline bitset* bitset_create(size_t size) {
    return bitset_create_with_capacity(size, DEFAULT_SET_CAPACITY);
}

/** Frees the bitset and its internal words buffer. */
static inline void bitset_free(bitset* set) {
    if (set) {
        free(set->words);
        free(set);
    }
}

/** Sets all bits in the bitset to 1. */
void bitset_set_all(bitset* set);

/** Clears all bits in the bitset (sets all bits to 0). */
void bitset_clear_all(bitset* set);

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

/** 
 * Performs bitwise union.
 * remarks: Assumes dest->capacity is at least above max(src1->capacity, src2->capacity).
 */
void bitset_union(bitset* dest, const bitset* src1, const bitset* src2);

/** Performs bitwise intersection. */
void bitset_intersection(bitset* dest, const bitset* src1, const bitset* src2);

/** Performs bitwise difference. */
void bitset_difference(bitset* dest, const bitset* src1, const bitset* src2);

/** Performs set symmetric difference. */
void bitset_symmetric_difference(bitset* dest, const bitset* src1, const bitset* src2);

/** Performs bitwise complement. */
void bitset_complement(bitset* dest, const bitset* src1);

#ifdef SHADEC_DEBUG
/** Prints a bitset and its contents. */
void bitset_print(bitset* set);
#endif

#ifdef BITSET_IMPLEMENTATION

// Internal function definitions
/** Gets the word count in a bitset of specified size. */
static inline size_t get_word_count(size_t size) {
    return (size + UINT_BITSIZE - 1) >> 5;
}

/**
 * Counts the number of active bits in a 32-bit word.
 * returns: The total number of bits set to 1.
 */
static size_t word_popcount(uint32_t word) {
    size_t count = 0;
    while (word != 0) {
        count++;
        word &= (word - 1); // Clears the lowest set bit
    }
    return count;
}

// Bitset functions
bitset* bitset_create_with_capacity(size_t size, size_t capacity) {
    bitset* set = malloc(sizeof(bitset));
    if (!set) return NULL;

    uint32_t* words = calloc(get_word_count(size), sizeof(uint32_t));
    if (!words) {
        free(set);
        return NULL;
    }

    set->capacity = capacity;
    set->words = words;
    set->cardinality = 0;

    return set;
}

void bitset_set_all(bitset* set) {
    if (!set) return;

    size_t word_count = get_word_count(set->capacity);
    for (size_t i = 0; i < word_count; i++) {
        set->words[i] = 0xFFFFFFFF; // all 1
    }

    set->cardinality = set->capacity;
}

void bitset_clear_all(bitset* set) {
    if (!set) return;

    size_t word_count = get_word_count(set->capacity);
    for (size_t i = 0; i < word_count; i++) {
        set->words[i] = 0;
    }

    set->cardinality = 0;
}

int bitset_set(bitset* set, size_t index) {
    if (!set || index_out_of_bounds(index, set->capacity)) return 0;

    size_t word_idx = index >> 5;
    size_t bit_idx  = index & (UINT_BITSIZE - 1);
    uint32_t mask = (1U << bit_idx);

    // Only increment count if the bit wasn't already set
    if (!(set->words[word_idx] & mask)) {
        set->words[word_idx] |= mask;
        set->cardinality++;
    }

    return 1;
}

int bitset_clear(bitset* set, size_t index) {
    if (!set || index_out_of_bounds(index, set->capacity)) return 0;

    size_t word_idx = index >> 5;
    size_t bit_idx  = index & (UINT_BITSIZE - 1);
    uint32_t mask = (1U << bit_idx);

    // Only decrement count if the bit wasn't already set
    if (set->words[word_idx] & mask) {
        set->words[word_idx] &= ~mask;
        set->cardinality--;
    }

    return 1;
}

int bitset_test(bitset* set, size_t index) {
    if (!set || index_out_of_bounds(index, set->capacity)) return 0;

    size_t word_idx = index >> 5;
    size_t bit_idx  = index & (UINT_BITSIZE - 1);

    return (set->words[word_idx] >> bit_idx) & 1;
}

void bitset_union(bitset* dest, const bitset* src1, const bitset* src2) {
    if (!dest || !src1 || !src2) return;

    size_t words1 = get_word_count(src1->capacity);
    size_t words2 = get_word_count(src2->capacity);

    size_t min_words = words1 < words2 ? words1 : words2;
    size_t max_words = words1 > words2 ? words1 : words2;

    assert(get_word_count(dest->capacity) >= max_words);

    size_t i = 0;
    size_t new_cardinality = 0;

    // Loop through the overlapping region where both sets have words
    for (; i < min_words; i++) {
        dest->words[i] = src1->words[i] | src2->words[i];
        new_cardinality += word_popcount(dest->words[i]);
    }

    // Copy the remaining words from whichever set is larger
    const bitset* larger = words1 > words2 ? src1 : src2;
    for (; i < max_words; i++) {
        dest->words[i] = larger->words[i];
        new_cardinality += word_popcount(dest->words[i]);
    }

    dest->cardinality = new_cardinality;
}

void bitset_intersection(bitset* dest, const bitset* src1, const bitset* src2) {
    if (!dest || !src1 || !src2) return;

    size_t words1 = get_word_count(src1->capacity);
    size_t words2 = get_word_count(src2->capacity);

    size_t min_words = words1 < words2 ? words1 : words2;
    size_t max_words = words1 > words2 ? words1 : words2;

    assert(get_word_count(dest->capacity) >= max_words);

    size_t i = 0;
    size_t new_cardinality = 0;

    for (; i < min_words; i++) {
        dest->words[i] = src1->words[i] & src2->words[i];
        new_cardinality += word_popcount(dest->words[i]);
    }

    for (; i < max_words; i++) {
        dest->words[i] = 0;
    }

    dest->cardinality = new_cardinality;
}

void bitset_difference(bitset* dest, const bitset* src1, const bitset* src2) {
    if (!dest || !src1 || !src2) return;

    size_t words1 = get_word_count(src1->capacity);
    size_t words2 = get_word_count(src2->capacity);

    size_t min_words = words1 < words2 ? words1 : words2;
    size_t max_words = words1 > words2 ? words1 : words2;

    assert(get_word_count(dest->capacity) >= max_words);

    size_t i = 0;
    size_t new_cardinality = 0;

    for (; i < min_words; i++) {
        dest->words[i] = src1->words[i] & ~src2->words[i];
        new_cardinality += word_popcount(dest->words[i]);
    }

    if (words1 > words2) {
        // src1 is larger; keep its remaining words unchanged
        for (; i < max_words; i++) {
            dest->words[i] = src1->words[i];
            new_cardinality += word_popcount(dest->words[i]);
        }
    } 
    else {
        // src2 is larger; subtracting from implicit zeros yields zero
        for (; i < max_words; i++) {
            dest->words[i] = 0;
        }
    }

    dest->cardinality = new_cardinality;
}

void bitset_symmetric_difference(bitset* dest, const bitset* src1, const bitset* src2) {
    if (!dest || !src1 || !src2) return;

    size_t words1 = get_word_count(src1->capacity);
    size_t words2 = get_word_count(src2->capacity);

    size_t min_words = words1 < words2 ? words1 : words2;
    size_t max_words = words1 > words2 ? words1 : words2;

    assert(get_word_count(dest->capacity) >= max_words);

    size_t i = 0;
    size_t new_cardinality = 0;

    for (; i < min_words; i++) {
        dest->words[i] = src1->words[i] ^ src2->words[i];
        new_cardinality += word_popcount(dest->words[i]);
    }

    const bitset* larger = words1 > words2 ? src1 : src2;
    for (; i < max_words; i++) {
        dest->words[i] = larger->words[i];
        new_cardinality += word_popcount(dest->words[i]);
    }

    dest->cardinality = new_cardinality;
}

void bitset_complement(bitset* dest, const bitset* src) {
    if (!dest || !src) return;

    size_t word_count = get_word_count(src->capacity);
    assert(get_word_count(dest->capacity) >= word_count);

    size_t new_cardinality = 0;
    for (size_t i = 0; i < word_count; i++) {
        dest->words[i] = ~src->words[i];
        new_cardinality += word_popcount(dest->words[i]);
    }

    dest->cardinality = new_cardinality;
}

#ifdef SHADEC_DEBUG
void bitset_print(bitset* set) {
    if (!set) return; 

    printf("Bits: [");
    for (size_t i = 0; i < set->capacity; i++) {
        printf("%d", bitset_test(set, i));
        if (i < set->capacity - 1) printf(", ");
    }
    printf("]\n");
}
#endif

#endif // BITSET_IMPLEMENTATION
#endif // BITSET_H
