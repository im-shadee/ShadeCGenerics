#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct node {
    void* data; // Pointer to the data stored
    struct node* next; // Pointer to the next node
    struct node* previous; // Pointer to the previous node
} node;

typedef struct {
    size_t count, elem_size;
    node* head; // Pointer to the first item
    node* tail; // Pointer to the last item
} linkedlist;

/**
 * Creates a new linkedlist node with a fixed element size, and data to populate it with.
 * returns: A pointer to the created node structure, or NULL in case of errors.
 */
node* create_node(size_t elem_size, void* elem);

/** Frees the node and its contents. */
void free_node(node* node);

/**
 * Creates a new linkedlist with a fixed element size.
 * returns: A pointer to the created linkedlist structure, or NULL in case of errors.
 */
linkedlist* linkedlist_create(size_t elem_size);

/**
 * Copies the contents of a source linkedlist to a new linkedlist.
 * returns: A pointer to the copied linkedlist.
 */
linkedlist* linkedlist_copy(linkedlist* source);

/**
 * Gets the element in the linkedlist at the specified index.
 * returns: The pointer to the linkedlist element, or NULL in case of errors.
 */
void* linkedlist_get_at(linkedlist* list, size_t index);

/**
 * Adds an element to a linkedlist.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed (memory allocation issues)
 */
int linkedlist_add(linkedlist* list, void* elem);

/**
 * Adds an element to a linkedlist at a specific index.
 *  * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed (memory allocation issues)
 */
int linkedlist_insert_at(linkedlist* list, size_t index, void* elem);

/**
 * Removes an element from a linkedlist at the specified index.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed
 */
int linkedlist_remove_at(linkedlist* list, size_t index);

/**
 * Removes an element from a linkedlist.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed
 */
int linkedlist_remove(linkedlist* list, void* elem);

/**
 * Removes the element positioned at the tail of a linkedlist.
 * returns:
 *  - 1 if the operation succeeded
 *  - 0 if the operation failed
 */
int linkedlist_remove_last(linkedlist* list);

/** Clears the entire list. */
void linkedlist_clear(linkedlist* list);

/** Frees the list and its contents. */
void linkedlist_free(linkedlist* list);

/** 
 * Prints the list's contents to stdout.
 * example: If the linkedlist contains 1,2,3, linkedlist_print will display [1, 2, 3].
 * remarks: Pass in a custom formatting function for the specific data type that the array contains.
 */
void linkedlist_print(linkedlist* list, void (*print_elem)(void* elem));

#ifdef LINKEDLIST_IMPLEMENTATION

// Internal functions definition
/**
 * Checks whether an index is outside of an linkedlist's bounds.
 * returns:
 *  - 1 if the index is out of bounds
 *  - 0 if the index is in bounds
 */
static int index_out_of_bounds(linkedlist* list, size_t index);

/** Frees all nodes in a linked list. */
static void free_all_nodes(linkedlist* list);
// enddef

// Node functions
node* create_node(size_t elem_size, void* elem) {
    if (!elem) return NULL;

    // Allocate memory for the node
    node* new = malloc(sizeof(node));
    if (!new) return NULL;
    
    // Allocate memory for the node element
    new->data = malloc(elem_size);
    if (!new->data) {
        free(new);
        return NULL;
    }

    // Copy the data passed to the node element
    memcpy(new->data, elem, elem_size);

    // A new node starts with no neighbors
    new->next = NULL;
    new->previous = NULL;

    return new;
}

void free_node(node* node) {
    if (node) {
        free(node->data);
        free(node);
    }
}

// LinkedList functions
linkedlist* linkedlist_create(size_t elem_size) {
    // Allocate memory for the list
    linkedlist* new = malloc(sizeof(linkedlist));
    if (!new) return NULL;

    // Initialize internals
    new->count = 0;
    new->elem_size = elem_size;
    new->head = NULL;
    new->tail = NULL;

    return new;
}

linkedlist* linkedlist_copy(linkedlist* source) {
    if (!source) return NULL;

    linkedlist* dest = linkedlist_create(source->elem_size);
    if (!dest) return NULL;

    node* current = source->head;

    // Traverse every node in the source list
    while (current) {
        if (!linkedlist_add(dest, current->data)) {
            linkedlist_free(dest);
            return NULL;
        }
        current = current->next;
    }

    return dest;
}

void* linkedlist_get_at(linkedlist* list, size_t index) {
    if (index_out_of_bounds(list, index)) return NULL;

    // If the index is closer to the tail; iterate backwards from tail, else forward from head
    int start_from_tail = index > list->count / 2;
    
    // Place current node at head/tail
    node* current = start_from_tail ? list->tail : list->head;
    size_t ix = start_from_tail ? list->count - 1 : 0;

    // Iterate until we find the element (or reach the end of the list)
    while (current) {
        if (index == ix) return current->data;
        
        if (start_from_tail) {
            current = current->previous;
            ix--; // Count down from the tail
        } 
        else {
            current = current->next;
            ix++; // Count up from the head
        }
    }

    return NULL;
}

int linkedlist_add(linkedlist* list, void* elem) {
    if (!list || !elem) return 0;

    // Create a node with the passed element
    node* new_node = create_node(list->elem_size, elem);
    if (!new_node) return 0;

    if (!list->head) {
        list->head = new_node;
        list->tail = new_node;
    }
    else {
        list->tail->next = new_node;
        new_node->previous = list->tail;
        list->tail = new_node;
    }

    list->count++;
    return 1;
}

int linkedlist_insert_at(linkedlist* list, size_t index, void* elem) {
    if (index_out_of_bounds(list, index)) return 0;

    // If the index if the end of the list, append at the end
    if (index == list->count) return linkedlist_add(list, elem);

    // Create a node with the passed element
    node* new_node = create_node(list->elem_size, elem);
    if (!new_node) return 0;
    
    if (index == 0) {
        new_node->next = list->head;
        if (list->head) {
            list->head->previous = new_node;
        }
        list->head = new_node;
        list->count++;
        return 1;
    }

    // If the index is closer to the tail; iterate backwards from tail, else forward from head
    int start_from_tail = index > list->count / 2;
    node* current = start_from_tail ? list->tail : list->head;
    size_t ix = start_from_tail ? list->count - 1 : 0;

    while (current) {
        if (index == ix) {
            // Wire up the new node between current->previous and current
            new_node->previous = current->previous;
            new_node->next = current;

            if (current->previous) current->previous->next = new_node;
            current->previous = new_node;
            
            list->count++;
            return 1;
        }

        if (start_from_tail) {
            current = current->previous;
            ix--; // Count down from the tail
        } 
        else {
            current = current->next;
            ix++; // Count up from the head
        }
    }

    // Cleanup in case something went wrong
    free(new_node->data);
    free(new_node);
    return 0;
}

int linkedlist_remove_at(linkedlist* list, size_t index) {
    if (index_out_of_bounds(list, index)) return 0;

    // If the index is closer to the tail; iterate backwards from tail, else forward from head
    int start_from_tail = index > list->count / 2;
    node* current = start_from_tail ? list->tail : list->head;
    size_t ix = start_from_tail ? list->count - 1 : 0;

    while (current) {
        if (index == ix) break;

        if (start_from_tail) {
            current = current->previous;
            ix--; // Count down from the tail
        } 
        else {
            current = current->next;
            ix++; // Count up from the head
        }
    }

    if (!current) return 0;

    // Unlink the node from its neighbors
    if (current->previous) {
        current->previous->next = current->next;
    } 
    else {
        list->head = current->next;
    }

    if (current->next) {
        current->next->previous = current->previous;
    } 
    else {
        list->tail = current->previous;
    }

    free(current->data);
    free(current);

    list->count--;
    return 1;
}

int linkedlist_remove(linkedlist* list, void* elem) {
    if (!list || !elem) return 0;
    node* current = list->head;

    while (current) {
        if (memcmp(current->data, elem, list->elem_size) == 0) {
            if (current->previous) {
                current->previous->next = current->next;
            } 
            else {
                list->head = current->next; // Removing the head
            }

            if (current->next) {
                current->next->previous = current->previous;
            } 
            else {
                list->tail = current->previous; // Removing the tail
            }

            // Free memory to prevent leaks
            free(current->data);
            free(current);

            list->count--;
            return 1;
        }
        current = current->next;
    }

    return 0; // Element not found
}

int linkedlist_remove_last(linkedlist* list) {
    if (!list || !list->head) return 0;
    node* target = list->tail;

    // Check if it's a single-node list
    if (list->head == list->tail) {
        list->head = NULL;
        list->tail = NULL;
    } 
    else {
        // Move the tail pointer backward
        list->tail = target->previous;
        list->tail->next = NULL;
    }

    // Free the data buffer and the node itself to prevent memory leaks
    free(target->data);
    free(target);

    list->count--;
    return 1;
}

void linkedlist_clear(linkedlist* list) {
    if (!list) return;

    free_all_nodes(list);

    // Reset metadata after memory freeing
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

void linkedlist_free(linkedlist* list) {
    if (list) {
        free_all_nodes(list);
        free(list);
    }
}

void linkedlist_print(linkedlist* list, void (*print_elem)(void* elem)) {
    if (!list || !print_elem) return;
    printf("[");

    node* current = list->head;
    while (current) {
        print_elem(current->data);
        current = current->next;
    }

    printf("]");
}

// Interal functions
static int index_out_of_bounds(linkedlist* list, size_t index) {
    return !list || index >= list->count;
}

static void free_all_nodes(linkedlist* list) {
    if (!list) return;

    node* current = list->head;
    while (current != NULL) {
        node* next_node = current->next; // Save the next pointer before we free current
        free_node(current);
        current = next_node;
    }
}

#endif // LINKEDLIST_IMPLEMENTATION

#endif // LINKEDLIST_H
