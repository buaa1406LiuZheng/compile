#include <stdio.h>
#include <stdlib.h>
#include <list>
#include "lexical.h"
#include "syntax.h"
#include "table.h"
#include "mips.h"

FILE *fsrc;
FILE *fout;

symbol sym=nullsym; // current read symbol

char line[MAX_LINE_LENTH]; // buffer for a line of code
int ll=0; // length of current line
int cp=0; // current character pointer
int last_line_number;

char ident[MAX_ID_LENTH];
int int_value = 0;  // value of an integer constant
char ch_value = 0;   // value of a character constant
char string_value[MAX_STRING_LENTH];    // value of a string constant
type type_value;

table_item table[MAX_TABLE_LENTH];  //符号表
std::list<quadruples> quad_codes;  //四元式代码表
char string_table[MAX_STRING_CONST_STORAGE_LENTH];  //字符串常量表
fun_table_item fun_table[MAX_FUN_TABLE_LENTH];

int tp = 0; //符号表头指针
int global_position = 0;    //最后一个全局变量在符号表中的位置
int strp = 0;   //字符串常量表头指针
int funp = 0;   //函数表头指针

int error_count = 0;

int main(int argc, char** argv){

    fsrc = fopen(argv[1],"r");
    if(fsrc==NULL){
        printf("%s not exsit\n",argv[1]);
        exit(-1);
    }
    fout = fopen(argv[2],"w");
    if(fout==NULL){
        printf("%s open failed\n",argv[2]);
        exit(-1);
    }

    programme();
    if(error_count > 0){
        printf("programme has errors");
        return 0;
    }
    mips_gen();
    printf("misp code generation done\n");

    fclose(fsrc);
    fclose(fout);
    return 0;
}