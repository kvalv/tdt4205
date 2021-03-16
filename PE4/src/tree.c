#include <vslc.h>


void
node_print ( node_t *root, int nesting )
{
    if ( root != NULL )
    {
        printf ( "%*c%s", nesting, ' ', node_string[root->type] );
        if ( root->type == IDENTIFIER_DATA ||
             root->type == STRING_DATA ||
             root->type == RELATION ||
             root->type == EXPRESSION ) 
            printf ( "(%s)", (char *) root->data );
        else if ( root->type == NUMBER_DATA )
            printf ( "(%ld)", *((int64_t *)root->data) );
        putchar ( '\n' );
        for ( int64_t i=0; i<root->n_children; i++ )
            node_print ( root->children[i], nesting+1 );
    }
    else
        printf ( "%*c%p\n", nesting, ' ', root );
}


void
node_init (node_t *nd, node_index_t type, void *data, uint64_t n_children, ...)
{
    va_list child_list;
    *nd = (node_t) {
        .type = type,
        .data = data,
        .entry = NULL,
        .n_children = n_children,
        .children = (node_t **) malloc ( n_children * sizeof(node_t *) )
    };
    va_start ( child_list, n_children );
    for ( uint64_t i=0; i<n_children; i++ )
        nd->children[i] = va_arg ( child_list, node_t * );
    va_end ( child_list );
}


void
node_finalize ( node_t *discard )
{
    if ( discard != NULL )
    {
        free ( discard->data );
        free ( discard->children );
        free ( discard );
    }
}


void
destroy_subtree ( node_t *discard )
{
    if ( discard != NULL )
    {
        for ( uint64_t i=0; i<discard->n_children; i++ )
            destroy_subtree ( discard->children[i] );
        node_finalize ( discard );
    }
}

//TODO: Implement simplify_tree.
int is_container_type(node_t *node) {
    return node->type == GLOBAL_LIST || 
        (node->type >= STATEMENT_LIST && node->type <= DECLARATION_LIST);
}

void
simplify_tree ( node_t **simplified, node_t *root )
{


    for(int i = 0; i < root->n_children; i++){
        simplify_tree(simplified, root->children[i]);
    }
        

    /* //4.1 */
    /* for (int i = 0; i < root->n_children; i++) { */
    /*     node_t* child = root->children[i]; */
    /*     if(child->n_children == 1 && child->data == NULL){ */
    /*         root->children[i] = child->children[0]; */
    /*     } */
    /* } */


    // 4.2 -- flattening list structure
    if (is_container_type(root)) {
        for (int j = 0; j < root->n_children; j++) {
            node_t *child = root->children[j];
            if (child->type == root->type) {  // steal its children
                switch (child->n_children) {
                    case 0:
                        fprintf(stderr, "Expected at least one child.");
                        exit(1);
                    case 1:
                        root->children[j] = child->children[0];
                        break;
                    default:
                        root->children[j] = child->children[0];
                        root->children = realloc(
                            root->children, 
                            sizeof(node_t*) * (root->n_children + child->n_children - 1)
                        );
                        if (root->children == NULL) { fprintf(stderr, "Unable to realloc."); exit(1); }
                        for (int j = 1; j < child->n_children; j++) {
                            root->children[root->n_children++] = child->children[j];
                        }
                        break;
                }
            }
        }
    }

    // 4.2 print statement
    if ( root->type == PRINT_STATEMENT) {
        if (root->n_children != 1) {
            fprintf(stderr, "Expected node of PRINT_STATEMENT type to have only one child.");
            exit(1);
        }
        node_t *child = root->children[0];
        root->children = child->children;
        root->n_children = child->n_children;
    }

    //4.3
    //TODO: Compute the value of the subtrees representing arithmetic operations with constants and replace them with the value.
    //step 1) all expresions that is on the form number operator number should them selvs be the resluting number. 
    //Example: expresion is 1+2 then the expresion can be reduced to the number 3. The whole expresion node shoud 
    //change its semantical data to 3 and type to NUMBER.
    //step 2) all expresions that has an "=" relation to a number is themselvs a number.
    //Example: expresion = 3 then the expresion can be reduced to the number 3. The whole relation node should 
    //change its semantical data to 3 and type to NUMBER.



    *simplified = root;
}
