//
// Created by liuzheng on 2016/11/22.
//

#ifndef COMPILER_MIPS_H
#define COMPILER_MIPS_H

#include "lexical.h"

#define MAX_LOCAL_VAL 1024

typedef struct {
    char name[MAX_ID_LENTH];    //变量名
    int position;   //变量地址
    int is_in_reg;  //是否在寄存器中
} local_val;

void mips_gen();
void print_local_table();

#endif //COMPILER_MIPS_H
