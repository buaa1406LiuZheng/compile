//
// Created by liuzheng on 2016/11/14.
//

#include "error.h"

extern symbol sym;
extern int error_count;
extern int last_line_number;
extern int cp;

void error(int n){
    error_count++;
    printf("line %d, ",last_line_number);
    if(n<10) {
        printf("lexical error,");
    }
    else if(n<100) {
        printf("syntax error,");
    }
    else if(n<150){
        printf("semantic error,");
    }
    else if(n<200){
        printf("warning error,");
        error_count--;
    }
    else{
        printf("error,");
    }
    printf(" error code %d:",n);
    switch(n){
        case 0: {printf("name too long\n");break;}
        case 1: {printf("integer too long\n");break;}
        case 2: {printf("integer begin with 0\n");break;}
        case 3: {printf("illegal character in string\n");break;}
        case 4: {printf("string too long\n");break;}
        case 5: {printf("illegal character\n");break;}
        case 11: {printf("identity missing\n");break;}
        case 12: {printf("( missing\n");break;}
        case 13: {printf(") missing\n");break;}
        case 14: {printf("() missing\n");break;}
        case 15: {printf("{ missing\n");break;}
        case 16: {printf("} missing\n");break;}
        case 17: {printf("[ missing\n");break;}
        case 18: {printf("] missing\n");break;}
        case 19: {printf("= missing\n");break;}
        case 21: {printf("an integer wanted\n");break;}
        case 22: {printf("an unsigned integer wanted\n");break;}
        case 23: {printf("int or char wanted\n");break;}
        case 24: {printf("character wanted\n");break;}
        case 25: {printf("+ or - wanted\n");break;}
        case 26: {printf("main missing\n");break;}
        case 27: {printf("no void before main\n");break;}
        case 28: {printf("while missing\n");break;}
        case 31: {printf("not a statement\n");break;}
        case 32: {printf("not an expression\n");break;}
        case 39: {printf("; missing\n");break;}
        case 100: {printf("redefined\n");break;}
        case 101: {printf("undefined\n");break;}
        case 102: {printf("unmatched number of parameters\n");break;}
        case 103: {printf("too many parameters\n");break;}
        case 104: {printf("not an array variable\n");break;}
        case 105: {printf("not a not-array variable\n");break;}
        case 106: {printf("not a function\n");break;}
        case 107: {printf("not a function or function has no return value\n");break;}
        case 108: {printf("has return value in void function\n");break;}
        case 109: {printf("has no return value in not-void function\n");break;}
        case 110: {printf("not a constant or a not-array variable\n");break;}
        case 150: {printf("iterate variable\n");break;}
        default:{}
    }
}

void fatal_error(){
    printf("fatal error\n");
    exit(0);
}

void skip(std::set<symbol> skipset){

    std::set<symbol>::iterator cmp;

    do{
        cmp = skipset.find(sym);
        if(cmp!=skipset.end()){ //sym在skipset中
            break;
        }
        nextsym();
    }while(true);

}
