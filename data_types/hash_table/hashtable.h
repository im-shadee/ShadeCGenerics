#ifndef HASHTABLE_H
#define HASHTABLE_H

#define DEFAULT_HTABLE_CAPACITY 16

#include <stdint.h>

typedef struct bucket_entry {
    void* key;
    void* value;
    struct bucket_entry* next; // Pointer to the next element in the chain (for collision handling)
} bucket_entry;

typedef struct {
    size_t capacity, count;
    size_t key_size, value_size; // Key/value type sizes
    bucket_entry** buckets; // The array of bucket chains
    uint32_t (*hash_function)(const void* key); // Key-specific hash function passed at init time
    int (*key_equals)(const void* lhs, const void* rhs); // Key comparison function passed at init time
} hashtable;

bucket_entry* create_bucket(size_t key_size, size_t value_size, void* key, void* value);
void free_bucket(bucket_entry* bucket);

hashtable* hashtable_create(
    size_t key_size, 
    size_t value_size, 
    uint32_t (*hash_function)(const void* key), 
    int (*key_equals)(const void* lhs, const void* rhs));

hashtable* hashtable_create_with_capacity(
    size_t key_size,
    size_t value_size,
    uint32_t (*hash_function)(const void* key),
    int (*key_equals)(const void* lhs, const void* rhs),
    size_t capacity);

void* hashtable_get(hashtable* table, void* key);

int hashtable_insert(hashtable* table, void* key, void* value);
int hashtable_update(hashtable* table, void* key, void* value);
int hashtable_delete(hashtable* table, void* key);
int hashtable_contains(hashtable* table, void* key);

/*#ifdef HASHTABLE_IMPLEMENTATION



#endif // HASHTABLE_IMPLEMENTATION*/

#endif // HASHTABLE_H
