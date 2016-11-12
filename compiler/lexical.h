//
// Created by liuzheng on 2016/11/7.
//

#ifndef COMPILER_LEXICAL_H
#define COMPILER_LEXICAL_H

#include <stdio.h>
#include <string.h>
FILE* fsrc;
char* eof_indicator = 1;

#define MAX_ID_LENTH 31
#define MAX_INT_LENTH 10
#define MAX_STRING_LENTH 501
#define MAX_LINE_LENTH 201

#define RESERVED_WORDS_NUM 13
const char reserved_words[RESERVED_WORDS_NUM][MAX_ID_LENTH] = {
    "char","const","do","else",
    "for","if","int", "main",
    "printf","return","scanf",
    "void","while"};

typedef enum symbol_enum{
    nullsym,charsym=256,constsym,dosym,
    elsesym,forsym,ifsym,intsym,mainsym,
    printfsym,returnsym,scanfsym,
    voidsym,whilesym,
    ltsym,lesym,gtsym,gesym,nesym,eqsym,
    identsym,charactersym,
    zerosym,integersym,stringsym} symbol;
const char symbol_name[24][40] = {
        "charsym","constsym","dosym",
        "elsesym","forsym","ifsym","intsym",
        "mainsym","printfsym","returnsym",
        "scanfsym","voidsym","whilesym",
        "ltsym","lesym","gtsym","gesym",
        "nesym","eqsym","identsym","charactersym",
        "zerosym","integersym","stringsym"};
const symbol reserved_words_symbol[RESERVED_WORDS_NUM] = {
    charsym,constsym,dosym,
    elsesym,forsym,ifsym,intsym,mainsym,
    printfsym,returnsym,scanfsym,
    voidsym,whilesym
};
symbol sym=nullsym; // current read symbol
char ch = ' '; // next character

char line[MAX_LINE_LENTH]; // buffer for a line of code
int ll=0; // length of current line
int cp=0; // current character pointer

char ident[MAX_ID_LENTH];
int int_value = 0;  // value of an integer constant
char ch_value = 0;   // value of a character constant
char string_value[MAX_STRING_LENTH];    // value of a string constant

void error(){
    printf("error\n");
}

void nextch(){
//    char* eof_indicator = 0;

    if(cp==ll){ //end of line
        eof_indicator = fgets(line,MAX_LINE_LENTH,fsrc);
        if(eof_indicator==NULL){
//            printf("program incomplete");
//            exit(-1);
//            line[0] = 0;
        }
        ll=strlen(line);
        cp=0;
    }
    ch = line[cp];
    cp = cp+1;
}

void indentifier(){
    //identifier
    int i=0,j;
    do{
        ident[i] = ch;
        i=i+1;
        printf("%c",ch);nextch();
        if(i==MAX_ID_LENTH){
            while ((ch>='a' && ch <='z') || (ch>='A' && ch <='Z') ||
                    (ch=='_') || (ch>='0' && ch <='9')){
                printf("%c",ch);nextch();
            }
            break;
        }
    }while ((ch>='a' && ch <='z') || (ch>='A' && ch <='Z') ||
            (ch=='_') || (ch>='0' && ch <='9'));
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

void integer(){
    // integer
    int i=0;
    char c;

    int_value = 0;
    c = ch; //the first digit
    do{
        int_value = int_value * 10 + (ch - '0');
        i = i + 1;
        printf("%c",ch);nextch();
        if(i==MAX_INT_LENTH){
            while(ch>='0' && ch <='9'){
                printf("%c",ch);nextch();
            }
            break;
        }
    }while(ch>='0' && ch <='9');
    if(i==MAX_INT_LENTH){   // int too long
        error();
    }
    if(c=='0'){
        if(i==1){
            sym = zerosym;
        } else{ //circumstance like 01
            sym = integersym;
            error();
        }
    } else {
        sym = integersym;
    }
}

void strings(){
    // string
    int i=0;
    do{
        printf("%c",ch);nextch();
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
    printf("%c",ch);nextch();
}

void character(){
    // character
    printf("%c",ch);nextch();
    if((ch>='a' && ch <='z') || (ch=='_') || (ch>='0' && ch <='9')
       || ch=='+' || ch=='-' || ch=='*' || ch=='\\') {
        ch_value = ch;
        printf("%c",ch);nextch();
        if (ch == '\'') {
            sym = charactersym;
            printf("%c",ch);nextch();
        } else {
            sym = nullsym;
            ch_value = 0;
        }
    } else{ //invalid symbol
        sym = nullsym;
        ch_value = 0;
    }
}

void greater_than(){
    // gt or ge
    printf("%c",ch);nextch();
    if(ch=='='){
        sym = gesym;
        printf("%c",ch);nextch();
    }
    else{
        sym = gtsym;
    }
}

void less_than(){
    // lt or le
    printf("%c",ch);nextch();
    if(ch=='='){
        sym = lesym;
        printf("%c",ch);nextch();
    }
    else{
        sym = ltsym;
    }
}

void not_equal(){
    // ne
    printf("%c",ch);nextch();
    if(ch=='='){
        sym = nesym;
        printf("%c",ch);nextch();
    }
    else{
        sym = nullsym;
    }
}

void equal(){
    // eq or '='
    printf("%c",ch);nextch();
    if(ch=='='){
        sym = eqsym;
        printf("%c",ch);nextch();
    }
    else{
        sym = '=';
    }
}
void nextsym(){

    while(ch==' ' || ch=='\t' || ch=='\n'){
        /*printf("%c",ch);*/nextch();   // skip white space
    }

    if((ch>='a' && ch <='z') || (ch>='A' && ch <='Z') || (ch=='_')){
        indentifier();
    }
    else if(ch>='0' && ch <='9'){
        integer();
    }
    else if(ch=='\''){
        character();
    }
    else if(ch=='\"'){
        strings();
    }
    else if(ch=='>'){
        greater_than();
    }
    else if(ch=='<'){
        less_than();
    }
    else if(ch=='!'){
        not_equal();
    }
    else if(ch=='='){
        equal();
    }
    else {
        sym = ch;
        printf("%c",ch);nextch();
    }
}

#endif //COMPILER_LEXICAL_H
