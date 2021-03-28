#ifndef SEARCH_H
#define SEARCH_H
#include <stdbool.h>
void hello();

typedef struct search_match_t {
    node_t *first;
    node_t *last;
    size_t match_depth;
} search_match_t;

typedef struct search_result_t {
    search_match_t **matches;
    size_t size;
    size_t capacity;
} search_result_t;

search_result_t *result_init(size_t initial_capacity);
void search_for_sequence(node_t *root, search_result_t *result,
                         node_index_t *types, size_t length);


#endif
