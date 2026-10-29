#ifndef HASHTABLE_H
#define HASHTABLE_H

#define DEFAULT_HTABLE_CAPACITY 16
#define REHASH_THRESHOLD 0.75

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

typedef struct bucket_entry {
    void* key;
    void* value;
    uint32_t hash; // Cached hash to avoid recalculations
    struct bucket_entry* next; // Pointer to the next element in the chain (for collision handling)
} bucket_entry;

typedef struct {
    size_t capacity, count;
    size_t key_size, value_size; // Key/value type sizes
    bucket_entry** buckets; // The array of bucket chains
    uint32_t (*hash_function)(const void* key); // Key-specific hash function passed at init time
    int (*key_equals)(const void* lhs, const void* rhs); // Key comparison function passed at init time
} hashtable;

/** 
 * Creates a hashtable and initializes its bucket array.
 * params:
 *  - key_size: the size of the keys held by the buckets.
 *  - value_size: the size of the values held by the buckets.
 *  - hash_function: the hash function used to hash the keys.
 *  - key_equals: Comparison function to compare keys.
 *  - capacity: The initial capacity of the hashtable.
 * 
 * returns: A pointer to the created hashtable, or NULL in case of errors.
 * remarks: The capacity will be rounded to the nearest power of 2 for fast hashing.
 */
hashtable* hashtable_create_with_capacity(
    size_t key_size,
    size_t value_size,
    uint32_t (*hash_function)(const void* key),
    int (*key_equals)(const void* lhs, const void* rhs),
    size_t capacity);

/** 
 * Creates a hashtable and initializes its bucket array.
 * params:
 *  - key_size: the size of the keys held by the buckets.
 *  - value_size: the size of the values held by the buckets.
 *  - hash_function: the hash function used to hash the keys.
 *  - key_equals: Comparison function to compare keys.
 * 
 * returns: A pointer to the created hashtable, or NULL in case of errors.
 */
static inline hashtable* hashtable_create(
    size_t key_size, 
    size_t value_size, 
    uint32_t (*hash_function)(const void* key), 
    int (*key_equals)(const void* lhs, const void* rhs)) 
{
    return hashtable_create_with_capacity(key_size, value_size, hash_function, key_equals, DEFAULT_HTABLE_CAPACITY);
}

/** 
 * Gets the value from a key in a hashtable.
 * returns: A pointer to the value found, or NULL if the key is not in the table.
 */
void* hashtable_get(hashtable* table, void* key);

/**
 * Inserts a key/value pair into a hashtable.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed
 * 
 * remarks: This function will reject the request if the key already exists in the table. Use `hashtable_update` to update a key/value pair.
 */
int hashtable_insert(hashtable* table, void* key, void* value);

/**
 * Updates a key/value pair within a hashtable.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed
 * 
 * remarks: This function will reject the request if the key does not exist in the table.
 */
int hashtable_update(hashtable* table, void* key, void* value);

/**
 * Deletes a key/value pair within a hashtable.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed or if the key does not exist in the hashtable
 */
int hashtable_delete(hashtable* table, void* key);

/**
 * Checks whether a hashtable contains a given key.
 * returns:
 *  - 1 if the table contains the key
 *  - 0 if the table does not contain it
 */
static inline int hashtable_contains(hashtable* table, void* key) {
    return hashtable_get(table, key) != NULL;
}

/** Clears the hashtable's contents. */
void hashtable_clear(hashtable* table);

/** Frees the hashtable and its contents. */
void hashtable_free(hashtable* table);

#ifdef SHADEC_DEBUG
/** 
 * Prints the hashtable and its contents to stdout. 
 * example: If the hashtable contains (1, "A") and (2, "B"), hashtable_print will display {(1, "A"), (2, "B")}.
 * remarks: Pass in a custom formatting function for key/value data types. Only available by defining SHADEC_DEBUG.
 */
void hashtable_print(hashtable* table, void (*print_key)(void* key), void (*print_value)(void* value));
#endif // SHADEC_DEBUG

#ifdef HASHTABLE_IMPLEMENTATION

// Internal functions definition
/** 
 * Creates a standalone hashtable bucket with set key/value size and values. 
 * remarks: Keys and values are stored next to the bucket in memory space.
 * returns: A pointer to the created bucket_entry, or NULL in case of errors.
 */
static bucket_entry* create_bucket(size_t key_size, size_t value_size, void* key, void* value, uint32_t hash) {
    if (!key || !value) return NULL;

    // Allocate and null-check the created bucket entry
    bucket_entry* bucket = malloc(sizeof(bucket_entry) + key_size + value_size);
    if (!bucket) return NULL;

    void* key_offset = (char*)bucket + sizeof(bucket_entry);
    void* value_offset = (char*)bucket + sizeof(bucket_entry) + key_size;

    // Use pointer arithmetic to put the memory for the kvp after the bucket
    bucket->key = memcpy(key_offset, key, key_size);
    bucket->value = memcpy(value_offset, value, value_size);
    bucket->hash = hash;
    bucket->next = NULL;

    return bucket;
}

/** Frees the current bucket entry as well as its contents. */
static inline void free_bucket(bucket_entry* bucket) {
    if (bucket) free(bucket);
}

static void rehash(hashtable* table) {
    if (!table) return;

    // Assuming the capacity is already a power of 2, multiply by 2 to get the next one
    size_t new_capacity = table->capacity * 2;

    // Create a new array of buckets with the new capacity
    bucket_entry** expanded_buckets = calloc(new_capacity, sizeof(bucket_entry*));
    if (!expanded_buckets) return;

    // Migrate the old bucket array's contents into the new array
    for (size_t i = 0; i < table->capacity; i++) {
        bucket_entry* current = table->buckets[i];
        while (current) {
            // Store a reference to the next node
            bucket_entry* next = current->next;

            // // Reuse the cached hash directly instead of recalculating it
            size_t new_index = current->hash & (new_capacity - 1);

            // Link current into its new bucket array (fast-prepend the nodes)
            // We don't need to order them correctly as we treat collision chains as unordered sets
            current->next = expanded_buckets[new_index];
            expanded_buckets[new_index] = current;

            current = next;
        }
    }

    free(table->buckets);
    table->buckets = expanded_buckets;
    table->capacity = new_capacity;
}

/** Computes and returns the nearest power of 2 of a number. */
static size_t compute_nearest_power_of_2(size_t n) {
    // Ensure that n is always at least one (to avoid decrements below 0)
    if (n < 2) n = 1;

    // Decrement n (so a size of 8 doesn't get rounded to 16 for instance)
    n--;

    // Smear chain the number for each power of 2 until we fill the entire number
    // Uses sizeof(size_t) * CHAR_BIT to dynamically adapt to the system
    // Instead of calculating powers of 2 by hand, we use the for loop index and multiply it by 2 each time
    for (size_t i = 1; i < sizeof(size_t) * CHAR_BIT; i *= 2) {
        n |= n >> i;
    }

    // Increment n back to get the final number
    n++;
    return n;
}

/** Iterates on the bucket array and frees each bucket chain. */
static void free_all_buckets(hashtable* table) {
    // Free each bucket entry in each collision chain of the bucket array
    for (size_t i = 0; i < table->capacity; i++) {
        if (!table->buckets[i]) continue;

        bucket_entry* current = table->buckets[i];
        while (current) {
            // Save the reference to the next entry to assign it back
            bucket_entry* next = current->next;

            // Free the current node
            free(current);
            current = next;
        }
    }
}

// Hashtable functions
hashtable* hashtable_create_with_capacity(
    size_t key_size,
    size_t value_size,
    uint32_t (*hash_function)(const void* key),
    int (*key_equals)(const void* lhs, const void* rhs),
    size_t capacity)
{
    // Allocate and null-check the created hashtable
    hashtable* htable = malloc(sizeof(hashtable));
    if (!htable) return NULL;

    // Round up the capacity to the nearest power of 2. Allows for fast index calculations during get, insert, etc. operations
    capacity = compute_nearest_power_of_2(capacity);

    // Allocate bucket array separately (safer rehash)
    // calloc populates all slots with zeros
    bucket_entry** buckets = calloc(capacity, sizeof(bucket_entry*));
    if (!buckets) {
        free(htable);
        return NULL;
    }

    // Initialize the hashtable internals
    htable->capacity = capacity;
    htable->count = 0;

    htable->key_size = key_size;
    htable->value_size = value_size;
    
    htable->buckets = buckets;
    htable->hash_function = hash_function;
    htable->key_equals = key_equals;
    
    return htable;
}

void* hashtable_get(hashtable* table, void* key) {
    if (!table || !key) return NULL;
    
    uint32_t hash = table->hash_function(key);
    size_t index = hash & (table->capacity - 1);
    bucket_entry* bucket = table->buckets[index];

    // If the bucket exists, we need to compare and find our key in the bucket list
    while (bucket) {
        if (bucket->hash == hash && table->key_equals(key, bucket->key)) {
            return bucket->value; // Immediately return to avoid additional iterations
        }
        bucket = bucket->next;
    }

    return NULL; // No bucket/matching key found
}

int hashtable_insert(hashtable* table, void* key, void* value) {
    if (!table || !key || !value) return 0;

    // Block insertions if the table already contains the key
    if (hashtable_contains(table, key)) return 0;

    // Rehash if the count would reach the capacity
    if (table->count >= table->capacity * REHASH_THRESHOLD) rehash(table);

    uint32_t hash = table->hash_function(key);
    size_t index = hash & (table->capacity - 1);
    
    // Create the bucket to add
    bucket_entry* new = create_bucket(table->key_size, table->value_size, key, value, hash);
    if (!new) return 0;

    // Else, prepend the new element to the bucket chain
    new->next = table->buckets[index];
    table->buckets[index] = new;

    // Increase the count and return
    table->count++;
    return 1;
}

int hashtable_update(hashtable* table, void* key, void* value) {
    if (!table || !key || !value) return 0;

    // Get the direct pointer to the value (if it exists)
    void* existing_value = hashtable_get(table, key);
    if (!existing_value) return 0; // Key DNE

    // Overwrite the old value with the new data
    memcpy(existing_value, value, table->value_size);
    return 1;
}

int hashtable_delete(hashtable* table, void* key) {
    if (!table || !key) return 0;

    uint32_t hash = table->hash_function(key);
    size_t index = hash & (table->capacity - 1);
    
    bucket_entry* current = table->buckets[index];
    bucket_entry* prev = NULL;

    // Traverse chain with hash pre-filtering
    while (current) {
        if (current->hash == hash && table->key_equals(key, current->key)) {
            break; // Found the entry to delete
        }
        prev = current;
        current = current->next;
    }

    // If current is null, either the bucket DNE or we didn't find a matching key
    if (!current) return 0;

    // If previous is still null, the bucket has one element; set the list slot to null entirely
    if (!prev) {
        table->buckets[index] = current->next;
    }
    // Else, we're in the middle of a chain; unlink the entry
    else {
        prev->next = current->next;
    }

    // Free the deleted node, decrease the count and return
    free(current);
    table->count--;
    return 1;
}

void hashtable_clear(hashtable* table) {
    if (!table) return;

    // Free each bucket chain in the bucket array
    free_all_buckets(table);

    // Ensure the array slots are reset to 0
    memset(table->buckets, 0, table->capacity * sizeof(bucket_entry*));

    // Reset the count. Keep the capacity available for reuse
    table->count = 0;
}

void hashtable_free(hashtable* table) {
    if (!table) return;

    // Free each bucket chain in the bucket array, then the array itself and the table
    free_all_buckets(table);
    free(table->buckets);
    free(table);
}

#ifdef SHADEC_DEBUG
void hashtable_print(hashtable* table, void (*print_key)(void* key), void (*print_value)(void* value)) {
    if (!table || !print_key || !print_value) return;
    printf("{");
    int first = 1;

    // Iterate through the bucket array
    for (size_t i = 0; i < table->capacity; i++) {
        bucket_entry* current = table->buckets[i];
        
        // Iterate through the collision chain and print kvp individually until we run out of entries
        while (current) {
            if (!first) {
                printf(", ");
                first = 0;
            }

            printf("(");
            print_key(current->key);

            printf(", ");
            print_value(current->value);
            printf(")");

            current = current->next;
            first = 0;
        }
    }

    printf("}\n");
}
#endif

// Internal 

#endif // HASHTABLE_IMPLEMENTATION
#endif // HASHTABLE_H
