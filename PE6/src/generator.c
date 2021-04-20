#include "vslc.h"

#define MIN(a,b) (((a)<(b)) ? (a):(b))
static const char *record[6] = {
    "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"
};


static void
generate_stringtable ( void )
{
    /* These can be used to emit numbers, strings and a run-time
     * error msg. from main
     */ 
    puts ( ".data" );
    puts ( "intout: .asciz \"\%ld \"" );
    puts ( "strout: .asciz \"\%s \"" );
    puts ( "errout: .asciz \"Wrong number of arguments\"" );

    /* TODO:  handle the strings from the program */
    for (int i=0; i < stringc ; i++) {
        printf("STR%-3d: .asciz %s\n", i, string_list[i]);
    }

}

static void
generate_global_variables ( void )
{
    /* Task b */ 
    puts ( ".data" );

    symbol_t** elems = calloc(tlhash_size(global_names), sizeof(tlhash_element_t));
    tlhash_values(global_names, (void**) elems);
    for (int i=0; i < tlhash_size(global_names); i++) {
        symbol_t *sym = elems[i];
        if (sym != NULL && sym->type == SYM_GLOBAL_VAR) {
            printf ("_%-10s .zero 8\n", sym->name);
        }
    }
    free(elems);
}

static void
arithmetic_expression ( node_t* op ) {
    // variable --> copy the content to rax
    // number --> copy to rax
    // expression1 -> rax -> push to stack
    // expression2 -> rax -> do OP to other...

    node_t *lhs = op->children[0];
    node_t *rhs = op->children[1];

}

static void
expand_expression(symbol_t *func, node_t *root) {
    if (root->type == EXPRESSION) {
        node_t *left = root->children[0];
        node_t *right = root->children[1];
        expand_expression(func, left);
        puts("\tpushq %rax");
        expand_expression(func, right);
        char* op = root->data;
        puts("\tpopq %r10");
        if (strcmp(op, "+") == 0) {
            puts("\taddq %r10, %rax");
        } else if (strcmp(op, "-") == 0) {
            puts("\tsubq %r10, %rax");
        } else if (strcmp(op, "/") == 0) {
            fprintf(stderr, "not supported / ");exit(1);
            // TODO
        } else if (strcmp(op, "*") == 0) {
            // TODO
            fprintf(stderr, "not supported * ");exit(1);
        }
    } else if (root->type == IDENTIFIER_DATA) {
        symbol_t *param;
        int offset;
        if ((tlhash_lookup(func->locals, root->data, strlen(root->data), (void*) &param)) == TLHASH_SUCCESS) {
            offset = -16 - 8 * param->seq;
        } else {
            // TODO
            //perror("unhandled"); exit(1);
        }
        printf("\tmovq %d(%%rbp), %%rsi  # get %s\n", offset, root->data);
    } else if (root->type == NUMBER_DATA) {
        int *v = root->data;
        printf("\tmovq $%d, %%rax\n", *v);
    }

    
}

static void
expand_print_statement (symbol_t* func, node_t *root ) {
    for (int i=0; i < root->n_children; i++) {
        node_t *child = root->children[i];
        if (child->type == STRING_DATA) {
            size_t *id = child->data;
            printf("\tleaq STR%zu(%%rip), %%rsi\n", *id);
            puts ( "\tleaq strout(%rip), %rdi" );
        } else if (child->type == NUMBER_DATA) {
            int *value = child->data;
            printf("\tmovq $%d, %%rsi\n", *value);
            puts ( "\tleaq intout(%rip), %rdi" );
        } else if (child->type == IDENTIFIER_DATA) {
            symbol_t *param;
            int offset;
            if ((tlhash_lookup(func->locals, child->data, strlen(child->data), (void*) &param)) == TLHASH_SUCCESS) {
                offset = -16 - 8 * param->seq;
            } else {
                // TODO
                perror("unhandled"); exit(1);
            }
            printf("\tmovq %d(%%rbp), %%rsi\n", offset);
            puts ( "\tleaq intout(%rip), %rdi" );
        } else if (child->type == EXPRESSION) {
            printf("DEBUG -- expression call...\n");
            puts ( "\tleaq intout(%rip), %rdi" );
            expand_expression(func, child);
            puts("\tmovq %rax, %rsi");
        }
        printf("\tcall printf\n");
    }
}

static void
generate_global_function ( symbol_t *func  )
{

    printf("_%s:\n", func->name);  // _main: 
    puts ( "\tmovq %rsp, %rbp" );

    puts ( "\tpushq %rbp" );

    for (int i=0; i < func->nparms; i++) {
        // TODO: support > 6 args
        printf("DEBUG - adding arg...\n");
        printf("\tpushq %s\n", record[i]);
    }

    if (func->nparms % 2 == 1) {
        puts("\tpushq $0");
    }


    node_t *decl_list = NULL;
    node_t *stmt_list = NULL;
    if (func->node->n_children == 2) {
        decl_list = func->node->children[0];
        stmt_list = func->node->children[1];
    } else {
        stmt_list = func->node->children[0];
    }

    expand_print_statement(func, stmt_list->children[0]);

    // recursively call

    // recursive search here. Handle prints,
    // expressions, ...

    // one arg provided, let's put that in rsi
    //puts("\tmovq %rdi %rsi");
    //puts("\tleaq strout(%rip), %rdi");
    //puts ( "\tmovq %rsp, %rsi" );
    //puts ( "\tleaq intout(%rip), %rdi" );
    //puts ("\tcall printf" );
    //puts ( "\tcall printf" );
    //puts ( "\tmovq $'\\n', %rdi" );
    //puts ( "\tcall putchar" );

    puts ( "\tmovq %rdi, %rax" );
    puts ( "\tmovq %rbp, %rsp" );
    puts ( "\tret" );

}

void tlhash_print_keys(tlhash_t *tab) {
    symbol_t **elems = calloc(tlhash_size(tab), sizeof(tlhash_element_t));
    tlhash_values(tab, (void**) elems);
    for (int i=0; i < tlhash_size(tab); i++) {
        symbol_t *sym = elems[i];
        printf("sym = %s\n", sym->name);
    }
    free(elems);
}



static void
generate_global_functions( void )
{
    symbol_t** elems = calloc(tlhash_size(global_names), sizeof(tlhash_element_t));
    tlhash_values(global_names, (void**) elems);
    for (int i=0; i < tlhash_size(global_names); i++) {
        symbol_t *sym = elems[i];
        if (sym != NULL && sym->type == SYM_FUNCTION) {
            generate_global_function(sym);
        }
    }
    free(elems);
}




static void
generate_main ( symbol_t *first )
{
    puts ( ".globl main" );
    puts ( ".text" );
    puts ( "main:" );
    puts ( "\tpushq %rbp" );
    puts ( "\tmovq %rsp, %rbp" );

    puts ( "\tsubq $1, %rdi" );
    printf ( "\tcmpq\t$%zu,%%rdi\n", first->nparms );
    puts ( "\tjne ABORT" );
    puts ( "\tcmpq $0, %rdi" );
    puts ( "\tjz SKIP_ARGS" );

    puts ( "\tmovq %rdi, %rcx" );
    printf ( "\taddq $%zu, %%rsi\n", 8*first->nparms );
    puts ( "PARSE_ARGV:" );
    puts ( "\tpushq %rcx" );
    puts ( "\tpushq %rsi" );

    puts ( "\tmovq (%rsi), %rdi" );
    puts ( "\tmovq $0, %rsi" );
    puts ( "\tmovq $10, %rdx" );
    puts ( "\tcall strtol" );

    /*  Now a new argument is an integer in rax */
    puts ( "\tpopq %rsi" );
    puts ( "\tpopq %rcx" );
    puts ( "\tpushq %rax" );
    puts ( "\tsubq $8, %rsi" );
    puts ( "\tloop PARSE_ARGV" );

    /* Now the arguments are in order on stack */
    for ( int arg=0; arg<MIN(6,first->nparms); arg++ )
        printf ( "\tpopq\t%s\n", record[arg] );

    puts ( "SKIP_ARGS:" );
    printf ( "\tcall\t_%s\n", first->name );
    puts ( "\tjmp END" );
    puts ( "ABORT:" );
    puts ( "\tleaq errout(%rip), %rdi" );
    puts ( "\tcall puts" );

    puts ( "END:" );
    puts ( "\tmovq %rax, %rdi" );
    puts ( "\tcall exit" );
}


void
generate_program ( void )
{
    generate_stringtable();
    // generate_global_variables();

    symbol_t f;
    symbol_t *pf = &f;
    tlhash_lookup(global_names, "main", strlen("main"), (void**) &pf);
    generate_main(pf);

    generate_global_functions();

    /* /1* Put some dummy stuff to keep the skeleton from crashing *1/ */
    /* puts ( ".text" ); */
    /* puts ( ".globl main" ); */
    /* puts ( "main:" ); */
    /* puts ( "\tcall _hello" ); */
    /* puts ( "\tret" ); */

}
