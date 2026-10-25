#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_ARRAY_CAPACITY 16

typedef struct {
    int elem_size, count, capacity;
    void* content;
} arraylist;

/** 
 * Creates and initializes an arraylist with a fixed element size.
 * returns: A pointer to the created array structure, or NULL in case of errors.
 */
arraylist* arraylist_create(int elem_size);

/** 
 * Creates and initializes an arraylist with a custom element size.
 * returns: A pointer to the created array structure, or NULL in case of errors.
 */
arraylist* arraylist_create_with_capacity(int elem_size, int capacity);

/**
 * Copies the contents of a source arraylist to a new arraylist.
 * returns: A pointer to the copied arraylist.
 */
arraylist* arraylist_copy(arraylist* source);

/**
 * Gets the element in the array list at the specified index.
 * returns: The pointer to the array list element, or NULL in case of errors.
 */
void* arraylist_get_at(arraylist* arraylist, int index);

/**
 * Adds an element to an arraylist.
 * remarks: Reallocates the internal data buffer if the size would overflow.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed (memory allocation issues)
 */
int arraylist_add(arraylist* arraylist, void* elem);

/**
 * Removes an element from an arraylist at the specified index.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed
 */
int arraylist_remove_at(arraylist* arraylist, int index);

/**
 * Removes an element from an arraylist.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed
 */
int arraylist_remove(arraylist* arraylist, void* elem);

/** Clears the entire array. */
void arraylist_clear(arraylist* arraylist);

/** Frees the array and its contents. */
void arraylist_free(arraylist* arraylist);

/** 
 * Prints the array's contents to stdout.
 * example: If the arraylist contains 1,2,3, arraylist_print will display [1, 2, 3].
 * remarks: Pass in a custom formatting function for the specific data type that the array contains.
 */
void arraylist_print(arraylist* arraylist, void (*print_elem)(void* elem));

#ifdef ARRAYLIST_IMPLEMENTATION

// Internal functions definition
/**
 * Reallocates the contents of an array list.
 * returns:
 *  1 if the operation succeeded
 *  0 if the operation failed (memory allocation issues)
 */
static int reallocate_content(arraylist* arraylist);

/**
 * Checks whether an index is outside of an arraylist's bounds.
 * returns:
 *  1 if the index is out of bounds
 *  0 if the index is in bounds
 */
static int index_out_of_bounds(arraylist* arraylist, int index);
// enddef

arraylist* arraylist_create(int elem_size) {
    return arraylist_create_with_capacity(elem_size, DEFAULT_ARRAY_CAPACITY);
}

arraylist* arraylist_create_with_capacity(int elem_size, int capacity) {
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
    arraylist* dest = arraylist_create_with_capacity(source->elem_size, source->capacity);
    if (!dest) return NULL;

    // Copy count
    dest->count = source->count;

    // Iterate over source and add each element to dest
    for (int i = 0; i < source->count; i++) {
        int offset = source->elem_size * i;

        // Cast to char* for pointer arithmetic
        memcpy((char*)dest->content + offset, (char*)source->content + offset, source->elem_size);
    }

    return dest;
}

void* arraylist_get_at(arraylist* arraylist, int index) {
    return index_out_of_bounds(arraylist, index)
        ? NULL 
        : (char*)arraylist->content + index * arraylist->elem_size;
}

int arraylist_add(arraylist* arraylist, void* elem) {
    // Check capacity: reallocate if size would overflow
    if (arraylist->count >= arraylist->capacity) {
        // reallocate_content returns 0 for issues, 1 otherwise
        if (!reallocate_content(arraylist)) return 0;
    }

    // Add the element at the end of the list
    void* target = (char*)arraylist->content + (arraylist->count * arraylist->elem_size);
    memcpy(target, elem, arraylist->elem_size);
    arraylist->count++;
    return 1;
}

int arraylist_remove_at(arraylist* arraylist, int index) {
    if (index_out_of_bounds(arraylist, index)) return 0;
    
    int remainingBytes = (arraylist->count - index - 1) * arraylist->elem_size;
    int offset = arraylist->elem_size * index;

    void* current = (char*)arraylist->content + offset;
    void* next = (char*)arraylist->content + offset + arraylist->elem_size;
            
    memmove(current, next, remainingBytes);
    arraylist->count--;
    return 1;
}

int arraylist_remove(arraylist* arraylist, void* elem) {
    for (int i = 0; i < arraylist->count; i++) {
        int offset = arraylist->elem_size * i;
        void* current = (char*)arraylist->content + offset;

        if (memcmp(current, elem, arraylist->elem_size) == 0) {
            int remainingBytes = (arraylist->count - i - 1) * arraylist->elem_size;
            void* next = (char*)arraylist->content + offset + arraylist->elem_size;
            
            memmove(current, next, remainingBytes);
            arraylist->count--;
            return 1;
        }
    }

    return 0;
}

void arraylist_clear(arraylist* arraylist) {
    // Setting the count to 0 makes the other functions treat this array as empty.
    // Add operations would overwrite existing data, so we do not need to clear the internal buffer
    arraylist->count = 0;
}

void arraylist_free(arraylist* arraylist) {
    if (arraylist) {
        free(arraylist->content);
        free(arraylist);
    }
}

void arraylist_print(arraylist* arraylist, void (*print_elem)(void* elem)) {
    printf("[");

    for (int i = 0; i < arraylist->count; i++) {
        int offset = arraylist->elem_size * i;
        void* current = (char*)arraylist->content + offset;

        print_elem(current);

        if (i < arraylist->count - 1) {
            printf(", ");
        }
    }

    printf("]\n");
}

// Internal methods
static int reallocate_content(arraylist* arraylist) {
    int new_capacity = arraylist->capacity * 2;
    void* new_content = realloc(arraylist->content, arraylist->elem_size * new_capacity);

    if (!new_content) return 0;
    
    arraylist->content = new_content;
    arraylist->capacity = new_capacity;
    return 1;
}

static int index_out_of_bounds(arraylist* arraylist, int index) {
    return index < 0 || index >= arraylist->count;
}

#endif // ARRAYLIST_IMPLEMENTATION

#endif // ARRAYLIST_H
