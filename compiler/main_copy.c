#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexical.h"

void error(){

}

void nextch(){
    char* eof_indicator = 0;

    if(cp==ll){ //end of line
        eof_indicator = fgets(line,MAX_LINE_LENTH,fsrc);
        if(eof_indicator==NULL){
            printf("program incomplete\n");
            exit(-1);
        }
        ll=strlen(line);
        cp=0;
    }
    ch = line[cp];
    cp = cp+1;
}

void nextsym(){
    int i,j;
    char c; // temp variable

    while(ch==' ' || ch=='\t' || ch=='\n'){
        nextch();   // skip white space
    }

    if((ch>='a' && ch <='z') || (ch=='_')){
        //identifier
        i=0;
        do{
            ident[i] = ch;
            i=i+1;
            nextch();
            if(i==MAX_ID_LENTH){
                while ((ch>='a' && ch <='z') || (ch=='_') || (ch>='0' && ch <='9')){
                    nextch();
                }
                break;
            }
        }while ((ch>='a' && ch <='z') || (ch=='_') || (ch>='0' && ch <='9'));
        if(i==MAX_ID_LENTH){    //identifier name too long
            error();
        }
        ident[i] = '\0';
        for(j = 0;j<RESERVED_WORDS_NUM;j++){
            if(strcmp(ident,reserved_words[j])==0){
                break;
            }
        }
        if(j==RESERVED_WORDS_NUM) {
            sym = identsym;
        } else{
            sym = reserved_words_symbol[j];
        }
    }
    else if(ch>='0' && ch <='9'){
        // integer
        i=0;
        int_value = 0;
        c = ch; //the first digit
        do{
            int_value = int_value * 10 + (ch - '0');
            i = i + 1;
            nextch();
            if(i==MAX_INT_LENTH){
                while(ch>='0' && ch <='9'){
                    nextch();
                }
                break;
            }
        }while(ch>='0' && ch <='9');
        if(i==MAX_INT_LENTH){   // int too long
            error();
        }
        if(c=='0' && i>1){  //circumstance like 01
            error();
        }
        sym = integersym;
    }
    else if(ch=='\''){
        // character
        nextch();
        if((ch>='a' && ch <='z') || (ch=='_') || (ch>='0' && ch <='9')
                || ch=='+' || ch=='-' || ch=='*' || ch=='\\') {
            ch_value = ch;
            nextch();
            if (ch == '\'') {
                sym = charactersym;
                nextch();
            } else {
                sym = nullsym;
                ch_value = 0;
            }
        } else{ //invalid symbol
            sym = nullsym;
            ch_value = 0;
        }
    }
    else if(ch=='\"'){
        // string
        i=0;
        do{
            nextch();
            if(ch<32 || ch>126){  // illegal character
                error();
                continue;
            }
            if(i==MAX_STRING_LENTH-1){
                continue;
            }
            string_value[i] = ch;
            i=i+1;
        }while (ch!='\"');
        if(i==MAX_STRING_LENTH-1){    // string constant too long
            error();
        }
        string_value[i-1] = '\0';
        sym = stringsym;
        nextch();
    }
    else if(ch=='>'){
        // gt or ge
        nextch();
        if(ch=='='){
            sym = gesym;
            nextch();
        }
        else{
            sym = gtsym;
        }
    }
    else if(ch=='<'){
        // lt or le
        nextch();
        if(ch=='='){
            sym = lesym;
            nextch();
        }
        else{
            sym = ltsym;
        }
    }
    else if(ch=='!'){
        // ne
        nextch();
        if(ch=='='){
            sym = nesym;
            nextch();
        }
        else{
            sym = nullsym;
        }
    }
    else if(ch=='='){
        // eq or '='
        nextch();
        if(ch=='='){
            sym = eqsym;
            nextch();
        }
        else{
            sym = '=';
        }
    }
    else {
        sym = ch;
        nextch();
    }
}

int main(int argc, char** argv){
    const char a[3][5] = {"aa","bbcc"};
    char s;
    int i=0;

    fsrc = fopen(argv[1],"r");
    if(fsrc==NULL){
        printf("%s not exsit\n",argv[1]);
        exit(-1);
    }
    while (1){
        nextsym();
        i=i+1;

//        if(sym>=charsym && sym<=whilesym){
//            printf("%s: %ssym\n",ident,reserved_words[sym-256]);
//        }
//        else if(sym == identsym){
//            printf("%s: indentsym\n",ident);
//        }
//        else if(sym ==)
    };
    return 0;
}