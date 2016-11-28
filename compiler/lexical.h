//
// Created by liuzheng on 2016/11/7.
//

#ifndef COMPILER_LEXICAL_H
#define COMPILER_LEXICAL_H

#define MAX_ID_LENTH 31
#define MAX_INT_LENTH 10
#define MAX_STRING_LENTH 501
#define MAX_LINE_LENTH 201

#define RESERVED_WORDS_NUM 13
#define SYMBOL_NUM 24
typedef enum {
    nullsym,charsym=256,constsym,dosym,
    elsesym,forsym,ifsym,intsym,mainsym,
    printfsym,returnsym,scanfsym,
    voidsym,whilesym,
    ltsym,lesym,gtsym,gesym,nesym,eqsym,
    identsym,charactersym,
    zerosym,integersym,stringsym} symbol;

void nextsym();

#endif //COMPILER_LEXICAL_H
