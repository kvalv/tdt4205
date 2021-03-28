#include <vslc.h>
#include "search.h"

// Externally visible, for the generator
extern tlhash_t *global_names;
extern char **string_list;
extern size_t n_string_list, stringc;

/* External interface */

void create_symbol_table(void);
void print_symbol_table(void);
void print_symbols(void);
void print_bindings(node_t *root);
void destroy_symbol_table(void);
void find_globals(void);
void bind_names(symbol_t *function, node_t *root);
void destroy_symtab(void);

void
create_symbol_table ( void )
{
    if ( global_names == NULL ) {
        global_names = malloc( sizeof (tlhash_t) );
        tlhash_init(global_names, 25);
    }
    find_globals();
    size_t n_globals = tlhash_size ( global_names );
    symbol_t *global_list[n_globals];
    tlhash_values ( global_names, (void **)&global_list );
    for ( size_t i = 0; i < n_globals; i++ )
        if ( global_list[i]->type == SYM_FUNCTION )
            bind_names ( global_list[i], global_list[i]->node );
}


void
print_symbol_table ( void )
{
    print_symbols();
    print_bindings(root);
}



void
print_symbols ( void )
{
    printf ( "String table:\n" );
    for ( size_t s = 0; s < stringc; s++ )
        printf  ( "%zu: %s\n", s, string_list[s] );
    printf ( "-- \n" );

    printf ( "Globals:\n" );
    size_t n_globals = tlhash_size(global_names);
    symbol_t *global_list[n_globals];
    tlhash_values ( global_names, (void **)&global_list );
    for ( size_t g = 0; g < n_globals; g++ ) {
        switch ( global_list[g]->type ) {
        case SYM_FUNCTION:
            printf (
                "%s: function %zu:\n",
                global_list[g]->name, global_list[g]->seq
            );
            if ( global_list[g]->locals != NULL ) {
                size_t localsize = tlhash_size( global_list[g]->locals );
                printf (
                    "\t%zu local variables, %zu are parameters:\n",
                    localsize, global_list[g]->nparms
                );
                symbol_t *locals[localsize];
                tlhash_values(global_list[g]->locals, (void **)locals );
                for ( size_t i = 0; i < localsize; i++ ) {
                    printf ( "\t%s: ", locals[i]->name );
                    switch ( locals[i]->type ) {
                    case SYM_PARAMETER:
                        printf ( "parameter %zu\n", locals[i]->seq );
                        break;
                    case SYM_LOCAL_VAR:
                        printf ( "local var %zu\n", locals[i]->seq );
                        break;
                    }
                }
            }
            break;
        case SYM_GLOBAL_VAR:
            printf ( "%s: global variable\n", global_list[g]->name );
            break;
        }
    }
    printf ( "-- \n" );
}


void
print_bindings ( node_t *root )
{
    if ( root == NULL )
        return;
    else if ( root->entry != NULL ) {
        switch ( root->entry->type ) {
        case SYM_GLOBAL_VAR:
            printf ( "Linked global var '%s'\n", root->entry->name );
            break;
        case SYM_FUNCTION:
            printf ( "Linked function %zu ('%s')\n",
                     root->entry->seq, root->entry->name
                   );
            break;
        case SYM_PARAMETER:
            printf ( "Linked parameter %zu ('%s')\n",
                     root->entry->seq, root->entry->name
                   );
            break;
        case SYM_LOCAL_VAR:
            printf ( "Linked local var %zu ('%s')\n",
                     root->entry->seq, root->entry->name
                   );
            break;
        }
    } else if ( root->type == STRING_DATA ) {
        size_t string_index = *((size_t *)root->data);
        if ( string_index < stringc )
            printf ( "Linked string %zu\n", *((size_t *)root->data) );
        else
            printf ( "(Not an indexed string)\n" );
    }
    for ( size_t c = 0; c < root->n_children; c++ )
        print_bindings ( root->children[c] );
}


void
destroy_symbol_table ( void )
{
    destroy_symtab();
}


symbol_t *mksym(node_t *n, symtype_t type)
{
    symbol_t *sym = malloc(sizeof(symbol_t));
    sym->name = n->data;
    sym->node = n;
    sym->type = type;
    sym->seq = 0;
    sym->nparms = 0;
    sym->locals = NULL;
    return sym;
}

void insert_global_function(node_t *node, int seq)
{
    char *func_name = node->children[0]->data;
    symbol_t *sym = mksym(node, SYM_FUNCTION);
    sym->name = func_name;
    sym->seq = seq;
    if ( (tlhash_insert(global_names, func_name, strlen(func_name) + 1,
                        sym)) != 0 ) {
        fprintf(stderr, "unable to insert\n"); exit(1);
    }

}

void insert_global_variable(node_t *node, int key)
{
    if ( (tlhash_insert(global_names, &key, 1, mksym(node,
                        SYM_GLOBAL_VAR))) != 0) {
        fprintf(stderr, "unable to insert\n"); exit(1);
    }
}

void
find_globals ( void )
{
    node_t *global_list = root->children[0];
    int nfuncs = 0;
    for (int i = 0; i < global_list->n_children; i++) {
        node_t *child = global_list->children[i];
        switch (child->type) {
        case FUNCTION:
            insert_global_function(child, nfuncs);
            nfuncs ++;
            break;
        case DECLARATION:
            for (int j = 0; j < child->children[0]->n_children; j++) {
                insert_global_variable(child->children[0]->children[j], j);
            }
            break;
        }
    }
}


void string_to_global_literal_index(node_t *node)
{
    //NOT_NULL(123)
    if (node->type !=  STRING_DATA) {
        fprintf(stderr, "Expected STRING_DATA type\n"); exit(1);
    }
    if ( string_list == NULL ) {
        string_list = calloc(n_string_list, sizeof(char *));
    }
    if (n_string_list <= stringc - 1) {
        string_list = realloc(string_list, n_string_list * 2 * sizeof(char *));
        n_string_list *= 2;
    }
    char *s = node->data;
    size_t *index = malloc(sizeof(size_t));
    *index = stringc;
    string_list[stringc++] = s;
    node->data = index;
}

void bind_local_variables(node_t *node, symbol_t *function,
                          tlhash_t **scope_stack, size_t size)
{
    node_t *root = node;
    if (node == NULL) {
        return;
    }
    if (node->type == BLOCK) {
        tlhash_t *local_scope = malloc(sizeof(tlhash_t));
        if ((tlhash_init(local_scope, 5)) != TLHASH_SUCCESS) {
            fprintf(stderr, "could not init tlhash\n"); exit(1);
        };

        // declarations can only happen first in the block so it's
        // sufficient to just check the first children.
        node_t statements;
        if (node->children[0]->type == DECLARATION_LIST) {
            search_result_t *res = result_init(2);
            node_index_t seq[] = {IDENTIFIER_DATA};
            search_for_sequence(node->children[0], res, seq, 1);
            for (int i = 0; i < res->size; i++) {
                node_t *var = res->matches[i]->last;
                symbol_t *s = mksym(var, SYM_LOCAL_VAR);
                int key = tlhash_size(function->locals) - function->nparms;
                s->seq = key;
                int r = tlhash_insert(local_scope, s->name, strlen(s->name) + 1, s);
                r |= tlhash_insert(function->locals, &key, 1, s);
                if (r != 0) {
                    exit(1);
                }
            }
            scope_stack[size++] = local_scope;
            root = node->children[1];
        } else {
            root = node->children[0];
        }
    }

    for (int i = 0; i < root->n_children; i++) {
        node_t *n = root->children[i];
        if ( n == NULL ) {
            continue;
        }

        if (n->type == IDENTIFIER_DATA) {
            char *key = n->data;
            symbol_t sym;
            symbol_t *x = &sym;

            int r = 0;
            for (int j = size - 1; j >= 0; j--) {
                r = tlhash_lookup(scope_stack[j], key, strlen(key) + 1, (void **) &x);
                if  (r == TLHASH_SUCCESS) {
                    break;
                }
            }
            if (x == NULL) {
                printf("Undefined variable: %s\n", n->data);
                exit(1);
            } else {
                n->entry = x;
            }

        } else {
            bind_local_variables(n, function, scope_stack, size);
        }
    }
}


void
bind_names ( symbol_t *function, node_t *root )
{

    tlhash_t *locals = malloc(sizeof(tlhash_t));
    tlhash_init(locals, 5);
    function->locals = locals;

    search_result_t res = {};
    res.capacity = 2;
    res.matches = malloc(sizeof(search_match_t) * res.capacity);

    // --- function parameters ---
    node_index_t param_sequence[] = {FUNCTION, VARIABLE_LIST, IDENTIFIER_DATA};
    search_for_sequence(root, &res, param_sequence, 3);
    for (int i = 0; i < res.size; i++) {
        node_t *node = res.matches[i]->last;
        symbol_t *s = mksym(node, SYM_PARAMETER);
        s->seq = function->nparms++;
        tlhash_insert(locals, node->data,  strlen(node->data) + 1, s);
    }

    // --- function strings ---
    node_index_t string_sequence[] = {STRING_DATA};
    res.size = 0;
    search_for_sequence(root, &res, string_sequence, 1);
    for (int i = 0; i < res.size; i++) {
        node_t *node = res.matches[i]->last;
        string_to_global_literal_index(node);
    }

    // --- local variables ---
    node_index_t block_sequence[] = {BLOCK};
    res.size = 0;
    search_for_sequence(root, &res, block_sequence, 1);

    tlhash_t **scope_stack = malloc(sizeof(tlhash_t) * 10);
    scope_stack[0] = global_names;
    scope_stack[1] = function->locals;

    for (int i = 0; i < root->n_children; i++) {
        node_t *child = root->children[i];
        bind_local_variables(child, function, scope_stack, 2);
    }

}


void
destroy_symtab ( void )
{
}
