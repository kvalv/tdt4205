#include <vslc.h>
#include "search.h"

search_match_t *match_init()
{
    search_match_t *t = malloc(sizeof(search_match_t));
    t->first = NULL;
    t->last = NULL;
    t->match_depth = 0;
    return t;
}

search_result_t *result_init(size_t initial_capacity)
{
    search_result_t *res = malloc(sizeof(search_result_t));
    res->matches = calloc(initial_capacity, sizeof(search_match_t) );
    res->capacity = initial_capacity;
    res->size = 0;
    return res;
}

void _search(search_result_t *result, node_t *root, node_index_t *types,
             size_t length, int level, node_t *first, int search_depth)
{
    if (root == NULL) {
        return;
    }
    if (root->type == types[level]) {
        if (level == 0) {
            first = root;
        }
        if (level == length - 1) {  // match found!
            search_match_t *m = match_init();
            m->first = first;
            m->last = root;
            m->match_depth = search_depth;
            if (result->capacity <= result->size - 1) {
                result->matches = realloc(result->matches,
                                          result->capacity * 2 * sizeof(search_match_t));
                result->capacity *= 2;
            }
            *(result->matches + result->size) = m;
            result->size++;
        } else {
            // not complete yet, gotta step one more level
            for (int i = 0; i < root->n_children; i++) {
                node_t *child = root->children[i];
                _search(result, child, types, length, level + 1, first, search_depth + 1);
            }
        }
        return;
    }

    if (level == 0 ) {
        for (int i = 0; i < root->n_children; i++) {
            node_t *child = root->children[i];
            _search(result, child, types, length, level, first, search_depth + 1);
        }
    }

}

void search_for_sequence(node_t *root, search_result_t *result,
                         node_index_t *types, size_t length)
{
    _search(result, root, types, length, 0, NULL, 0);
}

