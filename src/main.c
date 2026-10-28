#define ARRAYLIST_IMPLEMENTATION
#define HASHTABLE_IMPLEMENTATION
#define LINKEDLIST_IMPLEMENTATION
#define BITSET_IMPLEMENTATION
#define SHADEC_DEBUG

#include "arraylist.h"
#include "linkedlist.h"
#include "hashtable.h"
#include "bitset.h"

// Printing/hash helpers
// For printing integers
void print_int(void* elem) {
    printf("%d", *(int*)elem);
}

// For printing string pointers stored in hash table values
void print_string(void* elem) {
    printf("\"%s\"", *(char**)elem);
}

// Simple integer hash function for the hash table
uint32_t hash_int(const void* key) {
    uint32_t k = *(uint32_t*)key;
    k = ((k >> 16) ^ k) * 0x45d9f3b;
    k = ((k >> 16) ^ k) * 0x45d9f3b;
    k = (k >> 16) ^ k;
    return k;
}

// Key equality function for integer keys
int key_equals_int(const void* lhs, const void* rhs) {
    return *(int*)lhs == *(int*)rhs;
}

// Test routines
void test_arraylist() {
    printf("--- ArrayList ---\n");
    arraylist* list = arraylist_create(sizeof(int));

    int a = 10, b = 20, c = 30;
    arraylist_add(list, &a);
    arraylist_add(list, &b);
    arraylist_add(list, &c);

    printf("Initial: ");
    arraylist_print(list, print_int);

    arraylist_remove_at(list, 1); // Remove '20'
    printf("After removing index 1: ");
    arraylist_print(list, print_int);

    arraylist_free(list);
    printf("\n");
}

void test_linkedlist() {
    printf("--- LinkedList ---\n");
    linkedlist* list = linkedlist_create(sizeof(int));

    int x = 100, y = 200, z = 300;
    linkedlist_add(list, &x);
    linkedlist_add(list, &y);
    linkedlist_add(list, &z);

    printf("Initial: ");
    linkedlist_print(list, print_int);
    printf("\n");

    linkedlist_remove_last(list);
    printf("After remove_last: ");
    linkedlist_print(list, print_int);
    printf("\n");

    linkedlist_free(list);
    printf("\n");
}

void test_hashtable() {
    printf("--- HashTable ---\n");
    // Mapping int keys to char* values
    hashtable* table = hashtable_create(sizeof(int), sizeof(char*), hash_int, key_equals_int);

    int k1 = 1, k2 = 2, k3 = 3;
    char* v1 = "Apple";
    char* v2 = "Banana";
    char* v3 = "Cherry";

    hashtable_insert(table, &k1, &v1);
    hashtable_insert(table, &k2, &v2);
    hashtable_insert(table, &k3, &v3);

    printf("Initial Table: ");
    hashtable_print(table, print_int, print_string);

    // Test lookup
    char** found = hashtable_get(table, &k2);
    if (found) {
        printf("Found key 2 -> value: %s\n", *found);
    }

    // Test deletion
    hashtable_delete(table, &k1);
    printf("After deleting key 1: ");
    hashtable_print(table, print_int, print_string);

    // Test clearing
    hashtable_clear(table);
    printf("After clearing: ");
    hashtable_print(table, print_int, print_string);

    hashtable_free(table);
    printf("\n");
}

void test_bitset() {
    printf("--- Bitset ---\n");
    
    // Create a bitset of 16 bits
    bitset* bs1 = bitset_create(16);
    bitset* bs2 = bitset_create(16);
    bitset* result = bitset_create(16);

    // Test setting individual bits
    bitset_set(bs1, 0);
    bitset_set(bs1, 2);
    bitset_set(bs1, 4);
    bitset_set(bs1, 8);
    printf("bs1 after setting bits 0, 2, 4, 8:\n  ");
    bitset_print(bs1);

    // Test clearing a bit
    bitset_clear(bs1, 2);
    printf("bs1 after clearing bit 2:\n  ");
    bitset_print(bs1);

    // Populate bs2
    bitset_set(bs2, 4);
    bitset_set(bs2, 8);
    bitset_set(bs2, 10);
    printf("bs2:\n  ");
    bitset_print(bs2);

    // Test Union (bs1 | bs2)
    bitset_union(result, bs1, bs2);
    printf("Union (bs1 | bs2):\n  ");
    bitset_print(result);

    // Test Intersection (bs1 & bs2)
    bitset_intersection(result, bs1, bs2);
    printf("Intersection (bs1 & bs2):\n  ");
    bitset_print(result);

    // Test Difference (bs1 \ bs2)
    bitset_difference(result, bs1, bs2);
    printf("Difference (bs1 \\ bs2):\n  ");
    bitset_print(result);

    // Test Complement (~bs1)
    bitset_complement(result, bs1);
    printf("Complement (~bs1):\n  ");
    bitset_print(result);

    // Cleanup
    bitset_free(bs1);
    bitset_free(bs2);
    bitset_free(result);
    printf("\n");
}

int main() {
    test_arraylist();
    test_linkedlist();
    test_hashtable();
    test_bitset();

    return 0;
}
