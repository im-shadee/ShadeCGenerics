#define ARRAYLIST_IMPLEMENTATION
#define HASHTABLE_IMPLEMENTATION
#define LINKEDLIST_IMPLEMENTATION
#define BITSET_IMPLEMENTATION
#define SHADEC_DEBUG
#define _POSIX_C_SOURCE 199309L

#include "arraylist.h"
#include "linkedlist.h"
#include "hashtable.h"
#include "bitset.h"
#include <time.h>
#include <stdio.h>

#define ITERATIONS 50000

uint32_t hash_int(const void* key) {
    uint32_t k = *(uint32_t*)key;
    k = ((k >> 16) ^ k) * 0x45d9f3b;
    k = ((k >> 16) ^ k) * 0x45d9f3b;
    k = (k >> 16) ^ k;
    return k;
}

int key_equals_int(const void* lhs, const void* rhs) {
    return *(int*)lhs == *(int*)rhs;
}

static double get_elapsed_us(struct timespec* start, struct timespec* end) {
    long long sec_diff = end->tv_sec - start->tv_sec;
    long long nsec_diff = end->tv_nsec - start->tv_nsec;
    return (sec_diff * 1000000.0) + (nsec_diff / 1000.0); // Convert to microseconds
}

void benchmark_arraylist() {
    printf("--- ArrayList Benchmarks ---\n");
    struct timespec start, end;

    // Creation
    clock_gettime(CLOCK_MONOTONIC, &start);
    arraylist* list = arraylist_create(sizeof(int));
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - creation: %.3f µs\n", get_elapsed_us(&start, &end));

    // Add
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITERATIONS; i++) {
        arraylist_add(list, &i);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - arraylist_add (%d iteµs): %.3f µs\n", ITERATIONS, get_elapsed_us(&start, &end));

    // Get At
    clock_gettime(CLOCK_MONOTONIC, &start);
    int dummy = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        int* val = arraylist_get_at(list, i % list->count);
        if (val) dummy += *val;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - arraylist_get_at (%d lookups): %.3f µs (checksum: %d)\n", ITERATIONS, get_elapsed_us(&start, &end), dummy);

    // Remove At (middle elements cause memmove shifting)
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITERATIONS / 5; i++) {
        arraylist_remove_at(list, 0); // Removing from front tests shift penalty
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - arraylist_remove_at (%d ops): %.3f µs\n", ITERATIONS / 5, get_elapsed_us(&start, &end));

    // Remove by value
    int target = ITERATIONS / 2;
    clock_gettime(CLOCK_MONOTONIC, &start);
    arraylist_remove(list, &target);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - arraylist_remove (1 op): %.3f µs\n", get_elapsed_us(&start, &end));

    // Clear
    clock_gettime(CLOCK_MONOTONIC, &start);
    arraylist_clear(list);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - clear: %.3f µs\n", get_elapsed_us(&start, &end));

    arraylist_free(list);
    printf("\n");
}

void benchmark_linkedlist() {
    printf("--- LinkedList Benchmarks ---\n");
    struct timespec start, end;
    int limit = ITERATIONS / 4; // Lower limit due to pointer-chasing complexity

    // Creation
    clock_gettime(CLOCK_MONOTONIC, &start);
    linkedlist* list = linkedlist_create(sizeof(int));
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - creation: %.3f µs\n", get_elapsed_us(&start, &end));

    // Add
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < limit; i++) {
        linkedlist_add(list, &i);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - linkedlist_add (%d iteµs): %.3f µs\n", limit, get_elapsed_us(&start, &end));

    // Get At
    clock_gettime(CLOCK_MONOTONIC, &start);
    int dummy = 0;
    for (int i = 0; i < limit; i++) {
        int* val = linkedlist_get_at(list, i % list->count);
        if (val) dummy += *val;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - linkedlist_get_at (%d lookups): %.3f µs (checksum: %d)\n", limit, get_elapsed_us(&start, &end), dummy);

    // Insert At
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 500; i++) {
        linkedlist_insert_at(list, list->count / 2, &i);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - linkedlist_insert_at (500 ops): %.3f µs\n", get_elapsed_us(&start, &end));

    // Remove At
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 500; i++) {
        linkedlist_remove_at(list, 5);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - linkedlist_remove_at (500 ops): %.3f µs\n", get_elapsed_us(&start, &end));

    // Remove by value
    int target = limit / 2;
    clock_gettime(CLOCK_MONOTONIC, &start);
    linkedlist_remove(list, &target);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - linkedlist_remove (1 op): %.3f µs\n", get_elapsed_us(&start, &end));

    // Clear
    clock_gettime(CLOCK_MONOTONIC, &start);
    linkedlist_clear(list);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - clear: %.3f µs\n", get_elapsed_us(&start, &end));

    linkedlist_free(list);
    printf("\n");
}

void benchmark_hashtable() {
    printf("--- HashTable Benchmarks ---\n");
    struct timespec start, end;

    // Creation
    clock_gettime(CLOCK_MONOTONIC, &start);
    hashtable* table = hashtable_create(sizeof(int), sizeof(int), hash_int, key_equals_int);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - creation: %.3f µs\n", get_elapsed_us(&start, &end));

    // Insert
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITERATIONS; i++) {
        hashtable_insert(table, &i, &i);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - insert (%d iteµs): %.3f µs\n", ITERATIONS, get_elapsed_us(&start, &end));

    // Contains
    clock_gettime(CLOCK_MONOTONIC, &start);
    int found_count = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        if (hashtable_contains(table, &i)) found_count++;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - contains (%d checks): %.3f µs (found: %d)\n", ITERATIONS, get_elapsed_us(&start, &end), found_count);

    // Get
    clock_gettime(CLOCK_MONOTONIC, &start);
    int get_sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        int* val = hashtable_get(table, &i);
        if (val) get_sum += *val;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - get (%d lookups): %.3f µs (sum: %d)\n", ITERATIONS, get_elapsed_us(&start, &end), get_sum);

    // Update
    clock_gettime(CLOCK_MONOTONIC, &start);
    int new_val = 999;
    for (int i = 0; i < ITERATIONS; i++) {
        hashtable_update(table, &i, &new_val);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - update (%d ops): %.3f µs\n", ITERATIONS, get_elapsed_us(&start, &end));

    // Delete
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITERATIONS / 2; i++) {
        hashtable_delete(table, &i);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - delete (%d iteµs): %.3f µs\n", ITERATIONS / 2, get_elapsed_us(&start, &end));

    // Clear
    clock_gettime(CLOCK_MONOTONIC, &start);
    hashtable_clear(table);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - clear: %.3f µs\n", get_elapsed_us(&start, &end));

    hashtable_free(table);
    printf("\n");
}

void benchmark_bitset() {
    printf("--- Bitset Benchmarks ---\n");
    struct timespec start, end;
    size_t bits = ITERATIONS * 32;

    // Creation
    clock_gettime(CLOCK_MONOTONIC, &start);
    bitset* bs1 = bitset_create(bits);
    bitset* bs2 = bitset_create(bits);
    bitset* result = bitset_create(bits);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - creation: %.3f µs\n", get_elapsed_us(&start, &end));

    // Set
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < bits; i += 2) {
        bitset_set(bs1, i);
        bitset_set(bs2, i + 1);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - set (%zu bits): %.3f µs\n", bits / 2, get_elapsed_us(&start, &end));

    // Test
    clock_gettime(CLOCK_MONOTONIC, &start);
    int test_sum = 0;
    for (size_t i = 0; i < bits; i++) {
        test_sum += bitset_test(bs1, i);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - test (%zu checks): %.3f µs (set bits found: %d)\n", bits, get_elapsed_us(&start, &end), test_sum);

    // Union
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000; i++) {
        bitset_union(result, bs1, bs2);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - union (1000 ops): %.3f µs\n", get_elapsed_us(&start, &end));

    // Intersection
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000; i++) {
        bitset_intersection(result, bs1, bs2);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - intersection (1000 ops): %.3f µs\n", get_elapsed_us(&start, &end));

    // Difference
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000; i++) {
        bitset_difference(result, bs1, bs2);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - difference (1000 ops): %.3f µs\n", get_elapsed_us(&start, &end));

    // Symmetric Difference
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000; i++) {
        bitset_symmetric_difference(result, bs1, bs2);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - sym diff (1000 ops): %.3f µs\n", get_elapsed_us(&start, &end));

    // Complement
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000; i++) {
        bitset_complement(result, bs1);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - complement (1000 ops): %.3f µs\n", get_elapsed_us(&start, &end));

    // Set All
    clock_gettime(CLOCK_MONOTONIC, &start);
    bitset_set_all(bs1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - setall: %.3f µs\n", get_elapsed_us(&start, &end));

    // Clear All
    clock_gettime(CLOCK_MONOTONIC, &start);
    bitset_clear_all(bs1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  - clearall: %.3f µs\n", get_elapsed_us(&start, &end));

    bitset_free(bs1);
    bitset_free(bs2);
    bitset_free(result);
    printf("\n");
}

int main() {
    printf("=== RUNNING BENCHMARKS ===\n\n");

    benchmark_arraylist();
    benchmark_linkedlist();
    benchmark_hashtable();
    benchmark_bitset();

    printf("=== BENCHMARKS COMPLETE ===\n");
    return 0;
}
