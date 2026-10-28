#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

/**
 * Checks whether an index is outside of a data structure's bounds.
 * returns:
 *  - 1 if the index is out of bounds
 *  - 0 if the index is in bounds
 */
static inline int index_out_of_bounds(size_t index, size_t upper_bound) {
    return index >= upper_bound;
}

#endif // UTILS_H
