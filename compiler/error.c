//
// Created by liuzheng on 2016/11/14.
//

#include <stdio.h>
#include <stdlib.h>
#include "lexical.h"
#include "error.h"

extern symbol sym;
extern int error_count;

void error(int n){
    error_count++;
    if(n == 0) {
        printf("\nlexical error\n");
    }
    else if(n == 1) {
        printf("\nsyntax error\n");
        while(sym!=';' && sym!='}'){
            nextsym();
        }
    }
    else if(n==2){
        printf("\nsemantic error\n");
    }
    else if(n==3){
        printf("\nwarning error\n");
        error_count--;
    }
    else{
        printf("\nerror\n");
    }
}

void fatal_error(){
    printf("fatal error\n");
    exit(0);
}

int is_in_symbolset(symbol *symbolset){
    int i = 0;
    while(sym != symbolset[i]){
        i = i+1;
    }
    if(symbolset[i] == nullsym){
        return 0;
    }
    return 1;
}

void skip(symbol *symbolset){

    char symbol_set[((SYMBOL_NUM-1)/4+1)] = {0};
    int i;

    while(!is_in_symbolset(symbolset)){
        nextsym();
    }

}