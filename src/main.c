#define ARRAYLIST_IMPLEMENTATION
#define HASHTABLE_IMPLEMENTATION
#define LINKEDLIST_IMPLEMENTATION

#include "arraylist.h"
#include "linkedlist.h"
#include "hashtable.h"

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

    hashtable_free(table);
    printf("\n");
}

int main() {
    test_arraylist();
    test_linkedlist();
    test_hashtable();

    return 0;
}
