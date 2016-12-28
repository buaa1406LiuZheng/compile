//
// Created by liuzheng on 2016/11/14.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"
#include "table.h"

#include "lexical.h"


extern FILE* fsrc;
extern symbol sym; // current read symbol

extern int last_line_number;
int line_number = 1;
char line[MAX_LINE_LENTH]; // buffer for a line of code
int ll; // length of current line
int cp; // current character pointerextern
char ch = ' '; // next character

extern char ident[MAX_ID_LENTH];
extern int int_value;  // value of an integer constant
extern char ch_value;   // value of a character constant
extern char string_value[MAX_STRING_LENTH];    // value of a string constant
extern type type_value;

const char reserved_words[RESERVED_WORDS_NUM][MAX_ID_LENTH] = {
        "char","const","do","else",
        "for","if","int", "main",
        "printf","return","scanf",
        "void","while"};
const symbol reserved_words_symbol[RESERVED_WORDS_NUM] = {
        charsym,constsym,dosym,
        elsesym,forsym,ifsym,intsym,mainsym,
        printfsym,returnsym,scanfsym,
        voidsym,whilesym
};



void nextch(){
    char* eof_indicator;

    if(cp>=ll){ //end of line
        eof_indicator = fgets(line,MAX_LINE_LENTH,fsrc);
        if(line[0] == 0){
            printf("\nprogramme incomplete\n");
            exit(1);
        }
        if(eof_indicator==NULL){
            line[0] = 0;
        }
        ll=(int)strlen(line);
        cp=0;
    }
    ch = line[cp];
    cp = cp+1;
}

void identifier(){
    //identifier
    int i=0,j;
    do{
        ident[i] = ch;
        i=i+1;
        nextch();
        if(i==MAX_ID_LENTH){
            while ((ch>='a' && ch <='z') || (ch>='A' && ch <='Z') ||
                   (ch=='_') || (ch>='0' && ch <='9')){
                nextch();
            }
            break;
        }
    }while ((ch>='a' && ch <='z') || (ch>='A' && ch <='Z') ||
            (ch=='_') || (ch>='0' && ch <='9'));
    if(i==MAX_ID_LENTH){    //identifier name too long
        error(0);
        i--;
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
        if(sym == intsym){
            type_value = INT;
        }
        if(sym == charsym){
            type_value = CHAR;
        }
        if(sym == voidsym){
            type_value = VOID;
        }
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
        nextch();
        if(i==MAX_INT_LENTH){
            while(ch>='0' && ch <='9'){
                nextch();
            }
            break;
        }
    }while(ch>='0' && ch <='9');
    if(i==MAX_INT_LENTH){   // int too long
        error(1);
    }
    if(c=='0'){
        if(i==1){
            sym = zerosym;
        } else{ //circumstance like 01
            sym = integersym;
            error(2);
        }
    } else {
        sym = integersym;
    }
}

void strings(){
    // string
    int i=0;
    nextch();
    while (ch!='\"'){
        if(ch<32 || ch>126){  // illegal character
            error(3);
            if(ch == '\n'){
                sym = nullsym;
                return;
            }
            continue;
        }
        string_value[i] = ch;
        i=i+1;
        nextch();
        if(i==MAX_STRING_LENTH){
            while (ch!='\"'){
                nextch();
            }
            break;
        }
    }
    if(i==MAX_STRING_LENTH){    // string constant too long
        error(4);
        i--;
    }
    string_value[i] = '\0';
    sym = stringsym;
    nextch();
}

void character(){
    // character
    nextch();
    if(!((ch>='a' && ch <='z') || (ch>='A' && ch <='Z') || (ch=='_') ||
            (ch>='0' && ch <='9') || ch=='+' || ch=='-' || ch=='*' || ch=='/')) {
        error(5);
    }
    ch_value = ch;
    nextch();
    if (ch == '\'') {
        sym = charactersym;
        nextch();
    } else {
        sym = nullsym;
        ch_value = 0;
    }
}

void greater_than(){
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

void less_than(){
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

void not_equal(){
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

void equal(){
    // eq or '='
    nextch();
    if(ch=='='){
        sym = eqsym;
        nextch();
    }
    else{
        sym = (symbol)'=';
    }
}

void nextsym(){

    last_line_number = line_number;
    while(ch==' ' || ch=='\t' || ch=='\n'){
        if(ch=='\n'){
            line_number++;
        }
        nextch();   // skip white space
    }

    if((ch>='a' && ch <='z') || (ch>='A' && ch <='Z') || (ch=='_')){
        identifier();
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
        sym = (symbol)ch;
        nextch();
    }
}