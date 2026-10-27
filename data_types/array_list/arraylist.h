#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_ARRAY_CAPACITY 16

typedef struct {
    size_t elem_size, count, capacity;
    void* content;
} arraylist;

/** 
 * Creates and initializes an arraylist with a fixed element size.
 * returns: A pointer to the created array structure, or NULL in case of errors.
 */
arraylist* arraylist_create(size_t elem_size);

/** 
 * Creates and initializes an arraylist with a custom element size.
 * returns: A pointer to the created array structure, or NULL in case of errors.
 */
arraylist* arraylist_create_with_capacity(size_t elem_size, size_t capacity);

/**
 * Copies the contents of a source arraylist to a new arraylist.
 * returns: A pointer to the copied arraylist.
 */
arraylist* arraylist_copy(arraylist* source);

/**
 * Gets the element in the array list at the specified index.
 * returns: The pointer to the array list element, or NULL in case of errors.
 */
void* arraylist_get_at(arraylist* list, size_t index);

/**
 * Adds an element to an arraylist.
 * remarks: Reallocates the internal data buffer if the size would overflow.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed (memory allocation issues)
 */
int arraylist_add(arraylist* list, void* elem);

/**
 * Removes an element from an arraylist at the specified index.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed
 */
int arraylist_remove_at(arraylist* list, size_t index);

/**
 * Removes an element from an arraylist.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed
 */
int arraylist_remove(arraylist* list, void* elem);

/** Clears the entire array. */
void arraylist_clear(arraylist* list);

/** Frees the array and its contents. */
void arraylist_free(arraylist* list);

/** 
 * Prints the array's contents to stdout.
 * example: If the arraylist contains 1,2,3, arraylist_print will display [1, 2, 3].
 * remarks: Pass in a custom formatting function for the specific data type that the array contains.
 */
void arraylist_print(arraylist* list, void (*print_elem)(void* elem));

#ifdef ARRAYLIST_IMPLEMENTATION

// Internal functions definition
/**
 * Reallocates the contents of an array list.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed (memory allocation issues)
 */
static int reallocate_content(arraylist* list);

/**
 * Checks whether an index is outside of an arraylist's bounds.
 * returns:
 *  - 1 if the index is out of bounds
 *  - 0 if the index is in bounds
 */
static int index_out_of_bounds(arraylist* list, size_t index);
// enddef

arraylist* arraylist_create(size_t elem_size) {
    return arraylist_create_with_capacity(elem_size, DEFAULT_ARRAY_CAPACITY);
}

arraylist* arraylist_create_with_capacity(size_t elem_size, size_t capacity) {
    // Allocate a chunk of memory for the structure
    arraylist* new = malloc(sizeof(arraylist));

    if (!new) return NULL;

    // Initialize the inner data array
    new->content = malloc(elem_size * capacity);
    if (!new->content) {
        free(new);
        return NULL;
    }
    
    // Initialize the list
    new->capacity = capacity;
    new->count = 0;
    new->elem_size = elem_size;

    return new;
}

arraylist* arraylist_copy(arraylist* source) {
    if (!source) return NULL;
    
    arraylist* dest = arraylist_create_with_capacity(source->elem_size, source->capacity);
    if (!dest) return NULL;

    // Copy count
    dest->count = source->count;

    // Iterate over source and add each element to dest
    for (size_t i = 0; i < source->count; i++) {
        size_t offset = source->elem_size * i;

        // Cast to char* for pointer arithmetic
        memcpy((char*)dest->content + offset, (char*)source->content + offset, source->elem_size);
    }

    return dest;
}

void* arraylist_get_at(arraylist* list, size_t index) {
    return index_out_of_bounds(list, index)
        ? NULL 
        : (char*)list->content + index * list->elem_size;
}

int arraylist_add(arraylist* list, void* elem) {
    if (!list || ! elem) return 0;

    // Check capacity: reallocate if size would overflow
    if (list->count >= list->capacity) {
        // reallocate_content returns 0 for issues, 1 otherwise
        if (!reallocate_content(list)) return 0;
    }

    // Add the element at the end of the list
    void* target = (char*)list->content + (list->count * list->elem_size);
    memcpy(target, elem, list->elem_size);
    list->count++;
    return 1;
}

int arraylist_remove_at(arraylist* list, size_t index) {
    if (index_out_of_bounds(list, index)) return 0;
    
    int remainingBytes = (list->count - index - 1) * list->elem_size;
    int offset = list->elem_size * index;

    void* current = (char*)list->content + offset;
    void* next = (char*)list->content + offset + list->elem_size;
            
    memmove(current, next, remainingBytes);
    list->count--;
    return 1;
}

int arraylist_remove(arraylist* list, void* elem) {
    if (!list || !elem) return 0;

    for (size_t i = 0; i < list->count; i++) {
        size_t offset = list->elem_size * i;
        void* current = (char*)list->content + offset;

        if (memcmp(current, elem, list->elem_size) == 0) {
            int remainingBytes = (list->count - i - 1) * list->elem_size;
            void* next = (char*)list->content + offset + list->elem_size;
            
            memmove(current, next, remainingBytes);
            list->count--;
            return 1;
        }
    }

    return 0;
}

void arraylist_clear(arraylist* list) {
    if (!list) return;
    // Setting the count to 0 makes the other functions treat this array as empty.
    // Add operations would overwrite existing data, so we do not need to clear the internal buffer
    list->count = 0;
}

void arraylist_free(arraylist* list) {
    if (list) {
        free(list->content);
        free(list);
    }
}

void arraylist_print(arraylist* list, void (*print_elem)(void* elem)) {
    if (!list || !print_elem) return;
    printf("[");

    for (size_t i = 0; i < list->count; i++) {
        size_t offset = list->elem_size * i;
        void* current = (char*)list->content + offset;

        print_elem(current);

        if (i < list->count - 1) {
            printf(", ");
        }
    }

    printf("]\n");
}

// Internal methods
static int reallocate_content(arraylist* list) {
    if (!list) return 0;
    int new_capacity = list->capacity * 2;
    void* new_content = realloc(list->content, list->elem_size * new_capacity);

    if (!new_content) return 0;
    
    list->content = new_content;
    list->capacity = new_capacity;
    return 1;
}

static int index_out_of_bounds(arraylist* list, size_t index) {
    return !list || index >= list->count;
}

#endif // ARRAYLIST_IMPLEMENTATION

#endif // ARRAYLIST_H
