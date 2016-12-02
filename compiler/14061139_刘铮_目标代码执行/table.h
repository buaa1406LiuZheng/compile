//
// Created by liuzheng on 2016/11/12.
//

#ifndef COMPILER_TABLE_H
#define COMPILER_TABLE_H

#include "lexical.h"

#define MAX_TABLE_LENTH 2048
#define MAX_STRING_CONST_STORAGE_LENTH 4096
#define MAX_CODES_LENTH 2048
#define MAX_FUN_TABLE_LENTH 256

typedef enum {
    NONECA, CONST, VAR, PARA, FUNCTION
} catagory; //种类
typedef enum {
    NONETYPE, INT, CHAR, VOID
} type; //类型
typedef enum {
    NONEOP, ADD, SUB, MUL, DIV,
    BEQ, BNE, BGE, BGT, BLE, BLT,
    JUMP, LABEL, RAR, WAR,
    MOV, NEG,
    PRINTS, PRINTI, PRINTC, PRINTLN,
    SCANI, SCANC,
    PARAIN, CALL, RETURN, RETVAL
} quadop;
typedef struct {
    quadop op;   //操作符
    char x[MAX_ID_LENTH];   //操作数1
    char y[MAX_ID_LENTH];   //操作数2
    char r[MAX_ID_LENTH];   //结果
} quadruples;   //四元式

typedef struct {
    char name[MAX_ID_LENTH];
    catagory ca;
    type typ;
    int value;
    int isglobal;   //是否为全局变量
    int lenth;
} table_item; //符号表
typedef struct {
    char name[MAX_ID_LENTH];
    int para_num;   //函数参数个数
    int begin;  //函数开始地址
    int end;    //函数结束地址
} fun_table_item;

void enter(char *id, catagory c, type t, int v, int g, int l);
void enter_code(quadop opin,char *xin, char *yin, char *rin);
void enter_string_table(char *str);
void enter_fun_table(char* name, int beging, int end, int para_count);
int find(char *id_name);
int find_global(char *id_name);

void print_table();
void print_code_table();
void print_fun_table();
#endif //COMPILER_TABLE_H
