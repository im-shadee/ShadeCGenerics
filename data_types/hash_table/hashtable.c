#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

// Internal functions definition
void rehash(hashtable* table);
// enddef

bucket_entry* create_bucket(size_t key_size, size_t value_size, void* key, void* value) {
    if (!key || !value) return NULL;

    // Allocate and null-check the created bucket entry
    bucket_entry* bucket = malloc(sizeof(bucket_entry) + key_size + value_size);
    if (!bucket) return NULL;

    void* key_offset = (char*)bucket + sizeof(bucket_entry);
    void* value_offset = (char*)bucket + sizeof(bucket_entry) + key_size;

    // Use pointer arithmetic to put the memory for the kvp after the bucket
    bucket->key = memcpy(key_offset, key, key_size);
    bucket->value = memcpy(value_offset, value, value_size);

    return bucket;
}

void free_bucket(bucket_entry* bucket) {
    if (bucket) free(bucket);
}
