#include <stdio.h>
#include <stdlib.h>
#include "lexical.h"

int main(int argc, char** argv){
    int i=0;

    fsrc = fopen(argv[1],"r");
    if(fsrc==NULL){
        printf("%s not exsit\n",argv[1]);
        exit(-1);
    }

    printf("src\tsymbol\t\tvalue\n");
    while (feof(fsrc)==0){
        nextsym();
        i=i+1;
        if(sym==0){
            printf("\tnullsym\n");
        }
        else if(sym<256){
            printf("\t%c\n",sym);
        }
        else{
            printf("\t%s\t",symbol_name[sym-256]);
            if(sym == identsym){
                printf("%s\n",ident);
            }
            else if(sym == integersym || sym == zerosym){
                printf("%d\n",int_value);
            }
            else if(sym == charactersym){
                printf("%c\n",ch_value);
            }
            else if(sym == stringsym){
                printf("%s\n",string_value);
            }
            else{
                printf("\n");
            }
        }
    };
    return 0;
}