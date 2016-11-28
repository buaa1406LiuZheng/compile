//
// Created by liuzheng on 2016/11/14.
//

#include <string.h>
#include <stdio.h>
#include "table.h"
#include "error.h"

extern quadruples quad_codes[MAX_CODES_LENTH];  //四元式代码表
extern table_item table[MAX_TABLE_LENTH];
extern char string_table[MAX_STRING_CONST_STORAGE_LENTH];  //字符串常量表
extern fun_table_item fun_table[MAX_FUN_TABLE_LENTH];   //函数表

extern int tp; //符号表头指针
extern int global_position; //函数定义在符号表中的位置
extern int qp;  //四元式表头指针
extern int strp;   //字符串常量表头指针
extern int funp;   //函数表头指针

const char quarop_string[][20] = {
    "NONEOP", "ADD", "SUB", "MUL", "DIV",
    "BEQ", "BNE", "BGE", "BGT", "BLE", "BLT",
    "JUMP", "LABEL", "RAR", "WAR",
    "MOV", "NEG",
    "PRINTS", "PRINTI", "PRINTC", "PRINTLN",
    "SCANI", "SCANC",
    "PARAIN", "CALL", "RETURN", "RETVAL"
};

void enter(char *id, catagory c, type t, int v, int g, int l){
    if(tp == MAX_TABLE_LENTH){  //符号表满
        fatal_error();
    } else{
        strcpy(table[tp].name, id);
        table[tp].ca = c;
        table[tp].typ = t;
        table[tp].value = v;
        table[tp].isglobal = g;
        table[tp].lenth = l;
    }
    if(g){
        global_position = tp;
    }
    tp = tp+1;
}

void enter_code(quadop opin,char *xin, char *yin, char *rin){
    quadruples qc;

    if(tp == MAX_CODES_LENTH){  //四元式表满
        fatal_error();
    } else{
        qc.op = opin;
        strcpy(qc.x,xin);
        strcpy(qc.y,yin);
        strcpy(qc.r,rin);
        quad_codes[qp++] = qc;
    }
}

void enter_string_table(char *str){
    strcpy(&string_table[strp],str);
    strp += (strlen(str)+1);
}

void enter_fun_table(char* name, int begin, int end, int para_count){
    fun_table_item fitem;

    if(funp == MAX_FUN_TABLE_LENTH){    //函数表满
        fatal_error();
    } else {
        strcpy(fitem.name, name);
        fitem.begin = begin;
        fitem.end = end;
        fitem.para_num = para_count;
        fun_table[funp++] = fitem;
    }

}

int find(char *id_name){
    int i;
    table_item tmpt;
    for(i = global_position+1;i<tp;i++){
        tmpt = table[i];
        if(strcmp(id_name,tmpt.name)==0){
            return i;
        }
    }
    for(i=0;i<=global_position;){
        tmpt = table[i];
        if(strcmp(id_name,tmpt.name)==0){
            return i;
        }
        if(tmpt.ca == FUNCTION){
            i = i + tmpt.lenth + 1;
        }
        else{
            i+=1;
        }

    }
    return -1;
}

int find_global(char *id_name){
    int i;
    table_item tmpt;

    for(i=0;i<=global_position;){
        tmpt = table[i];
        if(strcmp(id_name,tmpt.name)==0){
            return i;
        }
        if(tmpt.ca == FUNCTION){
            i = i + tmpt.lenth + 1;
        }
        else{
            i+=1;
        }

    }
    return -1;
}

void print_table(){
    int i;
    table_item t;
    printf("\nord\t%-10s\tca\ttyp\tval\tgb\tlen\n","id");
    for(i=0;i<tp;i++){
        t = table[i];
        printf("%d\t%-10s\t%d\t%d\t%d\t%d\t%d\n",i,
        t.name, t.ca, t.typ, t.value, t.isglobal, t.lenth);
    }
    printf("print done\n\n");
}

void print_code_table(){
    int i;
    char s[20];
    quadruples t;
    printf("\n%-5s\t%-5s\t%-5s\t%-7s\t%-5s\n","ord","r","x","op","y");
    for(i=0;i<qp;i++){
        t = quad_codes[i];
        strcpy(s,quarop_string[t.op]);
        printf("%-5d\t%-5s\t%-5s\t%-7s\t%-5s\n",
               i, t.r, t.x, s, t.y);
    }
}

void print_fun_table(){
    int i;
    fun_table_item t;

    printf("\n%-15s\t%-5s\tend\t%-5s\n","name","begin","para");
    for(i=0;i<funp;i++){
        t = fun_table[i];
        printf("%-15s\t%-5d\t%d\t%-5d\n",
               t.name,t.begin,t.end,t.para_num);
    }
}