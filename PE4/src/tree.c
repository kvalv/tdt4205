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

void
simplify_tree ( node_t **simplified, node_t *root )
{

    for(int i = 0; i < root->n_children; i++){
        
        simplify_tree(simplified, root->children[i]);

        //4.1
        if(root->children[i]->n_children == 1 && root->children[i]->data == NULL){
        root->children[i] = root->children[i]->children[0];
        }

        //4.2 don't know if i'm doing what im supposed to here.
        node_index_t child_type = root->children[i]->type;         
        if(child_type == 1 || 2 < child_type < 10){//List have type value of 1 or between 3 to 9
            node_t *temp = malloc(sizeof(root->children)+sizeof(root->children[i]->children)-sizeof(node_t));
            root->n_children = root->children + root->children[i]->n_children - 1;
            int t = 0;
            for(int j = 0; j < root->n_children; j++){
                if(j==i){
                    for(; j < i + root->children[i]->n_children; j++){
                        temp[j] = *root->children[i]->children[j-i];
                    }
                }
                temp[j] = *root->children[t];
                t++;
            }
            root->children = temp;
        }

        //4.3
        //TODO: Compute the value of the subtrees representing arithmetic operations with constants and replace them with the value.
        //step 1) all expresions that is on the form number operator number should them selvs be the resluting number. 
        //Example: expresion is 1+2 then the expresion can be reduced to the number 3. The whole expresion node shoud 
        //change its semantical data to 3 and type to NUMBER.
        //step 2) all expresions that has an "=" relation to a number is themselvs a number.
        //Example: expresion = 3 then the expresion can be reduced to the number 3. The whole relation node should 
        //change its semantical data to 3 and type to NUMBER.

    }


    *simplified = root;
}
