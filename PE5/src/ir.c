#include <vslc.h>

// Externally visible, for the generator
extern tlhash_t *global_names;
extern char **string_list;
extern size_t n_string_list,stringc;

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
  for ( size_t i=0; i<n_globals; i++ )
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
    for ( size_t s=0; s<stringc; s++ )
        printf  ( "%zu: %s\n", s, string_list[s] );
    printf ( "-- \n" );

    printf ( "Globals:\n" );
    size_t n_globals = tlhash_size(global_names);
    symbol_t *global_list[n_globals];
    tlhash_values ( global_names, (void **)&global_list );
    for ( size_t g=0; g<n_globals; g++ )
    {
        switch ( global_list[g]->type )
        {
            case SYM_FUNCTION:
                printf (
                    "%s: function %zu:\n",
                    global_list[g]->name, global_list[g]->seq
                );
                if ( global_list[g]->locals != NULL )
                {
                    size_t localsize = tlhash_size( global_list[g]->locals );
                    printf (
                        "\t%zu local variables, %zu are parameters:\n",
                        localsize, global_list[g]->nparms
                    );
                    symbol_t *locals[localsize];
                    tlhash_values(global_list[g]->locals, (void **)locals );
                    for ( size_t i=0; i<localsize; i++ )
                    {
                        printf ( "\t%s: ", locals[i]->name );
                        switch ( locals[i]->type )
                        {
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
    else if ( root->entry != NULL )
    {
        switch ( root->entry->type )
        {
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
    for ( size_t c=0; c<root->n_children; c++ )
        print_bindings ( root->children[c] );
}


void
destroy_symbol_table ( void )
{
      destroy_symtab();
}


symbol_t* mksym(node_t *n, symtype_t type) {
    symbol_t* sym = malloc(sizeof(symbol_t));
    sym->name = n->data;
    sym->node = n;
    sym->type = type;
    sym->seq = 0;
    sym->nparms = 0;
    sym->locals = NULL;
    return sym;
}

void insert_global_function(node_t *node) {
    char *key = node->children[0]->data;
    symbol_t *sym = mksym(node, SYM_FUNCTION);
    sym->name = key;
    tlhash_insert(global_names, key, strlen(key) + 1, sym);

    tlhash_t *locals = malloc(sizeof(tlhash_t));
    tlhash_init(locals, 5);
    sym->locals = locals;
    node_t *var_list = node->children[1];
    if (var_list != NULL) {
        for (int i=0; i < var_list->n_children; i++) {
            node_t *param = var_list->children[i];
            tlhash_insert(locals, param->data, strlen(param->data) + 1, mksym(param, SYM_PARAMETER));
            sym->nparms++;
        }
    }
}

void insert_global_variable(node_t *node) {
    char *key = node->data;
    tlhash_insert(global_names, key, strlen(key) + 1, mksym(node, SYM_GLOBAL_VAR));
}

void
find_globals ( void )
{
    node_t *global_list = root->children[0];
    for (int i=0; i < global_list->n_children; i++){ 
        node_t* child = global_list->children[i];
        switch (child->type) {
            case FUNCTION:
                insert_global_function(child);
                break;
            case DECLARATION:
                // DECLARATION -> VARIABLE_LIST -> [identifier_0, identifier_1, ...]
                // --> grandchildren of current node would be the nodes we're
                // looking for.
                for (int j = 0; j < child->children[0]->n_children; j++) {
                    insert_global_variable(child->children[0]->children[j]);
                }
                break;
        }
    }
}

void
bind_names ( symbol_t *function, node_t *root )
{
}

void
destroy_symtab ( void )
{
}
