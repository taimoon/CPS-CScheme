#include<stdio.h>
#include<stdbool.h>
#include"./eval.h"
#include"./parser.h"
int main() {
    POOL = ht_create();
    init_lisp_objs();

    FILE *fptr;
    fptr = fopen("./input.ss", "r");
    EXP = read(fptr);
    
    printf(";;; input\n");
    print_sexp(EXP); printf("\n");
    printf(";;; res\n");
    value_of(EXP);
    print_sexp(VAL);
    printf("\n;;; end\n");
    
    return 0;
}