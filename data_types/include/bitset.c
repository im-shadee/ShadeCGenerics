#include "bitset.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

// Internal function definitions
static inline size_t get_word_count(size_t size);

// Bitset functions
bitset* bitset_create(size_t size) {
    return bitset_create_with_capacity(size, DEFAULT_SET_CAPACITY);
}

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
    set->count = 0;

    return set;
}

void bitset_free(bitset* set) {
    if (set) {
        free(set->words);
        free(set);
    }
}

void bitset_union(bitset* dest, const bitset* src1, const bitset* src2) {
    size_t max_capacity = src1->capacity > src2->capacity ? src1->capacity : src2->capacity;
}

// Internal functions
static inline size_t get_word_count(size_t size) {
    const size_t uint_bitsize = sizeof(uint32_t) * CHAR_BIT;
    return (size + uint_bitsize - 1) >> 5;
}
