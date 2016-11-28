//
// Created by liuzheng on 2016/11/14.
//

#include <stdio.h>
#include <string.h>
#include "syntax.h"
#include "lexical.h"
#include "table.h"
#include "error.h"

extern symbol sym;

extern char ident[MAX_ID_LENTH];
extern int int_value;  // value of an integer constant
extern char ch_value;   // value of a character constant
extern char string_value[MAX_STRING_LENTH];    // value of a string constant
extern type type_value;

extern quadruples quad_codes[MAX_CODES_LENTH];  //四元式代码表
extern table_item table[MAX_TABLE_LENTH];
extern fun_table_item fun_table[MAX_FUN_TABLE_LENTH];

extern int tp; //符号表头指针
extern int global_position; //函数定义在符号表中的位置
extern int qp;  //四元式表头指针
extern int strp;   //字符串常量表头指针
extern int funp;

char tempval[MAX_ID_LENTH]; //临时变量
char label[MAX_ID_LENTH];   //跳转标签

void new_tempval(){
    static int i=0;
    sprintf(tempval,"#%d",i++);
}

void new_label(){
    static int i=0;
    sprintf(label,"$$%d",i++);
    enter_code(LABEL,"","",label);
}

void programme(){
/*程序解析入口*/
    int get_void = 0;
    int begin, end; //main函数的开始与结束

    nextsym();
    while(sym == constsym){ // 常量声明
        const_def(1);
    }

    while(sym == intsym || sym == charsym){ // 变量声明
        nextsym();
        if(sym == identsym){
            nextsym();
            if(sym == '[' || sym == ',' || sym == ';'){
                var_def(1);
            }
            else if(sym == '('){    // 函数声明
                fun_def();
                break;
            }
            else{   //需要"[,;("
                error(1);
            }
        } else{ //不是标识符，报错
            error(1);
        }
    }

    while(sym == intsym || sym == charsym || sym == voidsym){   // 函数声明
        if(sym == voidsym) {
            nextsym();
            if (sym == identsym) {
                nextsym();
                if(sym == '('){
                    fun_def();
                } else{
                    error(1);
                }
            } else if (sym == mainsym) {    // 读到了main
                get_void = 1;
                break;
            } else {   //void后不是标识符也不是main
                error(1);
            }
        } else{
            nextsym();
            if (sym == identsym) {
                nextsym();
                fun_def();
            } else {   //int、char后不是标识符
                error(1);
            }
        }
    }

    if(sym == mainsym){
        enter("main", FUNCTION, VOID, 0, 1, 0);
        if(get_void){
            symbol sym1, sym2;
            nextsym();
            sym1 = sym;
            nextsym();
            sym2 = sym;
            if(sym1 == '(' && sym2 == ')'){
                nextsym();
                if(sym == '{'){

                    begin = qp;

                    nextsym();
                    block();

                    end =qp - 1;
                    table[global_position].value = funp;
                    enter_fun_table("main", begin, end, 0);
                } else{ //需要一个{
                    error(1);
                }
            } else{ //main后面需要一对括号
                error(1);
            }
        } else { //main 前面没有void
            error(1);
        }
    } else {    //没有main
        error(1);
    }

    printf("\t#### this is a programme\n");
}

void const_def(int isglobal){
/* 入口symbol为constsym
出口symbol为分号下一个*/

    int is_minus = 0;
    int i;

    nextsym();
    if(sym == intsym){
        do {
            nextsym();
            if (sym == identsym) {
                nextsym();
                if (sym == '=') {
                    nextsym();
                    if (sym == integersym) {

                        i = find(ident);
                        if(i == -1 || table[i].isglobal != isglobal) {
                            enter(ident, CONST, INT, int_value, isglobal, 0);
                        } else{ //重复定义
                            error(2);
                        }

                        nextsym();
                    }
                    else if (sym == zerosym) {

                        i = find(ident);
                        if(i == -1 || table[i].isglobal != isglobal) {
                            enter(ident, CONST, INT, 0, isglobal, 0);
                        } else{ //重复定义
                            error(2);
                        }

                        nextsym();
                    }
                    else if (sym == '+' || sym == '-') {
                        if (sym == '-') {
                            is_minus = 1;
                        }
                        nextsym();
                        if (sym == integersym) {
                            if (is_minus) {
                                int_value = -int_value;
                            }

                            i = find(ident);
                            if(i == -1 || table[i].isglobal != isglobal) {
                                enter(ident, CONST, INT, int_value, isglobal, 0);
                            } else{ //重复定义
                                error(2);
                            }

                            nextsym();
                        } else { //需要一个无符号整数
                            error(1);
                        }
                    }
                    else { //需要一个整数
                        error(1);
                    }
                } else { //需要一个=
                    error(1);
                }
            } else { //需要一个标识符
                error(1);
            }
        }while (sym == ',');
    }
    else if(sym == charsym){
        do {
            nextsym();
            if (sym == identsym) {
                nextsym();
                if (sym == '=') {
                    nextsym();
                    if (sym == charactersym) {

                        i = find(ident);
                        if(i == -1 || table[i].isglobal != isglobal) {
                            enter(ident, CONST, CHAR, ch_value, isglobal, 0);
                        } else{ //重复定义
                            error(2);
                        }

                        nextsym();
                    } else { //需要一个字符
                        error(1);
                    }
                } else { //需要一个=
                    error(1);
                }
            } else { //需要一个标识符
                error(1);
            }
        }while (sym == ',');
    }
    else{   //需要一个int 或 char
        error(1);
    }
    semicolon();

    printf("\t#### this is a const define\n");
}

void var_def(int isglobal){
/* 入口symbol为int，char（局部变量)或 ",;[" （全局变量）
出口symbol为分号下一个*/

    int i;
    int skip = 0;  //在全局变量处理时会预读入了一个标识符符号，需要跳过一次读标识符
    if(isglobal){
        skip = 1;
    }

    do {
        if(!skip) {
            nextsym();
            if (sym == identsym) {
                nextsym();
            } else { //不是标识符，报错
                error(1);
            }
        } else{
            skip = 0;
        }

        if (sym == ',' || sym == ';') {

            i = find(ident);
            if(i == -1 || table[i].isglobal != isglobal) {
                enter(ident, VAR, type_value, 0, isglobal, 0);
            } else{ //重复定义
                error(2);
            }

        }
        else if (sym == '[') {
            nextsym();
            if(sym == integersym){

                i = find(ident);
                if(i == -1 || table[i].isglobal != isglobal) {
                    enter(ident, VAR, type_value, 0, isglobal, int_value);
                } else{ //重复定义
                    error(2);
                }

                nextsym();
            } else{ //数组大小不是整数
                error(1);
            }

            if(sym == ']'){
                nextsym();
                if(sym == ';' || sym == ','){}
                else{   //需要;或,
                    error(1);
                }
            } else{ // 需要一个]
                error(1);
            }
        }
        else {  //需要;或,或[
            error(1);
        }
    }while(sym == ',');
    semicolon();

    printf("\t#### this is a variable define\n");
}

void fun_def(){
/* 入口symbol为"("
出口symbol为"}"下一个*/

    int i;
    int para_count = 0; //参数个数
    char fun_name[MAX_ID_LENTH];
    int begin = 0, end = 0; //函数开始、结束的四元式代码位置

    i = find(ident);
    if(i == -1) {
        enter(ident, FUNCTION, type_value, funp, 1, 0);
    } else{ //重复定义
        error(2);
        enter("", FUNCTION, type_value, funp, 1, 0);
    }
    strcpy(fun_name,ident);

    do{ //参数声明部分
        nextsym();
        if(sym == intsym || sym == charsym){
            nextsym();
            if(sym == identsym){

                i = find(ident);
                if(i == -1 || table[i].isglobal) {
                    enter(ident, PARA, type_value, 0, 0, 0);
                    para_count += 1;
                } else{ //重复定义
                    error(2);
                }

                nextsym();
            } else{ //需要一个标识符
                error(1);
            }
        }
        else if(sym == ')'){}
        else{ //需要int或char或)
            error(1);
        }
    }while(sym == ',');
    enter_fun_table(fun_name,0,0,para_count);

    if(sym == ')'){
        //进入函数声明体
        nextsym();
        if(sym == '{'){
            begin = qp;

            nextsym();
            block();
            end = qp - 1;
            if(sym == '}'){
                nextsym();
            } else{ //需要一个}
                error(1);
            }
        } else{ //需要一个{
            error(1);
        }
    } else{ //需要一个)
        error(1);
    }
    table[global_position].lenth = tp - global_position-1;
    fun_table[funp-1].begin = begin;
    fun_table[funp-1].end = end;
    printf("\t#### this is a function define\n");
}

void block(){
/* 当前symbol为 "{" 的下一个
出口symbol为"}"*/
    while(sym == constsym){ // 常量声明
        const_def(0);
    }

    while(sym == intsym || sym == charsym){ // 变量声明
        var_def(0);
    }

    multi_state();  //解析语句列

    printf("\t#### this is a block\n");
}

void statement(){
/*入口为一个语句的第一个symbol
出口为一条语句后面一个symbol*/

    int i,j;

    if(sym == ifsym){
        if_state();
    }
    else if(sym == dosym){
        do_while_state();
    }
    else if(sym == forsym){
        for_state();
    }
    else if(sym == '{'){
        nextsym();
        multi_state();
        nextsym();
    }
    else if(sym == identsym){
        nextsym();
        if(sym == '('){

            i = find(ident);
            if(i == -1){    //标识符未定义
                error(2);
            } else if(table[i].ca != FUNCTION){
                error(2);   //不是函数
            }

            j = call_state(ident);
            if(i != -1 && fun_table[table[i].value].para_num!=j){   //参数个数不符
                error(2);
            }
            semicolon();
        }
        else if(sym == '=' || sym == '['){
            assign_state();
            semicolon();
        }
        else{   //需要"(=["
            error(1);
        }
    }
    else if(sym == scanfsym){
        scanf_state();
        semicolon();
    }
    else if(sym == printfsym){
        printf_state();
        semicolon();
    }
    else if(sym == returnsym){
        return_state();
        semicolon();
    }
    else if(sym == ';'){
        //空语句
        nextsym();
    }
    else{   //符号不在语句头符号集中
        error(1);
    }
}

void multi_state(){
/*入口为一个语句的第一个symbol
出口为"}"*/
    while(sym != '}'){
        statement();
    }
    printf("\t#### this is a multi statements\n");
}

void if_state(){
/*入口为if
出口为一条语句后面一个symbol*/

    int branchp;  //if为假的跳转指令地址
    int jumpp;  //有else时，if为真语句后的跳转指令地址

    nextsym();
    if(sym == '('){
        nextsym();
    } else{
        error(1);
    }
    branchp = condition(0);
    if(sym == ')'){
        nextsym();
    } else{
        error(1);
    }

    statement();    //if成立的语句
    if(sym == elsesym){ //有else
        jumpp = qp;
        enter_code(JUMP,"","","");  //if成立要跳过else的语句

        new_label();    //if为假跳到这里
        strcpy(quad_codes[branchp].r,label);

        nextsym();
        statement();    //else部分语句

        new_label();    //if为真跳过else部分语句
        strcpy(quad_codes[jumpp].r,label);
    }
    else{   //没有else，if为假跳到这里
        new_label();
        strcpy(quad_codes[branchp].r,label);
    }

    printf("\t#### this is a if statement\n");
}

void do_while_state(){
/*入口为do
出口为")"后面一个symbol*/

    int branchp;    //while条件跳转指令地址
    char label_save[MAX_ID_LENTH];  //保存跳转label
    new_label();
    strcpy(label_save,label);

    nextsym();
    statement();

    if(sym == whilesym) {
        nextsym();
    } else{ //需要一个while
        error(1);
    }
    if(sym == '('){
        nextsym();
    } else{
        error(1);
    }
    branchp = condition(1);
    strcpy(quad_codes[branchp].r,label_save);
    if(sym == ')'){
        nextsym();
    } else{
        error(1);
    }

    printf("\t#### this is a do-while statement\n");
}

void for_state(){
/*入口为for
出口为")"后面一个symbol*/

    char exp[MAX_ID_LENTH];
    char id1[MAX_ID_LENTH] = "id1"; //第1个标识符
    char id2[MAX_ID_LENTH] = "id2"; //第2个标识符
    char id3[MAX_ID_LENTH] = "id3"; //第3个标识符
    char step[MAX_ID_LENTH];   //步长
    char label_save[MAX_ID_LENTH];
    int isinc = 1;  //是否加步长
    int branchp = -1;
    int i;

    nextsym();
    if(sym == '('){
        nextsym();
    } else{
        error(1);
    }

    if(sym == identsym){

        strcpy(id1,ident);
        i = find(id1);
        if(i == -1){    //标识符未定义
            error(2);
        }

        nextsym();
        if(sym == '='){
            nextsym();
            expression(exp);

            enter_code(MOV,"",exp,id1);

            if(sym == ';'){

                new_label();    //循环结束无条件跳转位置
                strcpy(label_save,label);

                nextsym();
                branchp = condition(0);

                if(sym == ';'){
                    nextsym();
                    if(sym == identsym){

                        strcpy(id2,ident);
                        i = find(id2);
                        if(i == -1){    //标识符未定义
                            error(2);
                        } else if(strcmp(id1,id2)!=0){  //此标识符与循环变量不是同一个
                            error(3);
                        }

                        nextsym();
                        if(sym == '='){
                            nextsym();
                            if(sym == identsym){

                                strcpy(id3,ident);
                                i = find(id3);
                                if(i == -1){    //标识符未定义
                                    error(2);
                                } else if(strcmp(id1,id3)!=0){  //此标识符与循环变量不是同一个
                                    error(3);
                                }

                                nextsym();
                                if(sym == '+' || sym == '-'){

                                    if(sym == '+'){
                                        isinc = 1;
                                    } else{
                                        isinc = 0;
                                    }

                                    nextsym();
                                    if(sym == integersym){
                                        sprintf(step,"%d",int_value);
                                        nextsym();
                                    } else{ //需要一个无符号整数
                                        error(1);
                                    }
                                } else{ //需要"+-"
                                    error(1);
                                }
                            } else{
                                error(1);
                            }
                        } else{ //需要一个"="
                            error(1);
                        }
                    } else{ //需要一个标识符
                        error(1);
                    }
                } else{ //需要一个";"
                    error(1);
                }
            } else{ //需要一个";"
                error(1);
            }
        } else{ //需要一个"="
            error(1);
        }
    } else{ //需要一个标识符
        error(1);
    }

    if(sym == ')'){
        nextsym();
    } else{
        error(1);
    }

    statement();
    if(isinc){  //增加步长
        enter_code(ADD,id3,step,id2);
    } else{
        enter_code(SUB,id3,step,id2);
    }
    enter_code(JUMP,"","",label_save);   //跳回判断处

    if(branchp !=-1) {  //没有语法错误生成了跳转语句
        new_label();
        strcpy(quad_codes[branchp].r, label);    //循环条件判断失败时跳到这里
    }

    printf("\t#### this is a for statement\n");
}

void semicolon(){
/*检测一个分号*/
    if(sym == ';'){
        nextsym();
    } else{ //需要一个分号
        error(1);
    }
}

int call_state(char *id){
/*入口为一个"("
出口为";"*/

    char exp[MAX_ID_LENTH];
    char fun_name[MAX_ID_LENTH];
    char temp[MAX_ID_LENTH];
    int para_count = 0;
//    nextsym();
//    if(sym == ')'){}
//    else{
//        expression(exp);
//        while (sym == ','){
//            nextsym();
//            expression(exp);
//        }
//    }
    strcpy(fun_name,id);
    do{
        nextsym();
        if(sym == ')'){}
        else{
            expression(exp);
            sprintf(temp,"%d",para_count);
            enter_code(PARAIN,temp,exp,fun_name);
            para_count++;
        }
    }while(sym == ',');

    if(sym == ')'){
        nextsym();
    } else{
        error(1);
    }
    enter_code(CALL,"","",fun_name);
    printf("\t#### this is a function call\n");
    return para_count;
}

void assign_state(){
/*入口为一个"="或"["
出口为";"*/

    char exp[MAX_ID_LENTH];
    char left_ident[MAX_ID_LENTH];  //被赋值的符号
    char temp[MAX_ID_LENTH];
    table_item item;    //标识符的符号表项
    int i;

    strcpy(left_ident,ident);

    i = find(left_ident);
    if(i != -1){
        item = table[i];
    } else{ //标识符未定义
        error(2);
    }

    if(sym == '='){

        if( i!=-1 && !((item.ca == VAR || item.ca == PARA) && item.lenth==0)){ //不是非数组变量
            error(2);
        }
        nextsym();
        expression(exp);
        enter_code(MOV,"",exp,left_ident);
    }
    else if(sym == '['){

        if( i!=-1 && !(item.ca == VAR && item.lenth!=0)){ //不是数组变量
            error(2);
        }

        nextsym();
        expression(exp);
        strcpy(temp,exp);
        if(sym == ']'){
            nextsym();
            if(sym == '='){
                nextsym();
                expression(exp);
                enter_code(WAR,left_ident,temp,exp);
            } else{ //需要一个"="
                error(1);
            }
        } else{ //需要一个"]"
            error(1);
        }
    }
    else{   //需要"=["
        error(1);
    }

    printf("\t#### this is a assignment\n");
}

void scanf_state(){
/*入口为scanf
出口为";"*/

    int i;

    nextsym();
    if(sym == '('){
        nextsym();
    } else{ //需要一个"("
        error(1);
    }

    if(sym == identsym) {

        i = find(ident);
        if(i == -1){    //标识符未定义
            error(2);
        } else if(!((table[i].ca == VAR || table[i].ca == PARA) && table[i].lenth == 0)){
            error(2);   //不是非数组变量
        } else {
            if(table[i].typ == INT) {
                enter_code(SCANI, "", "", ident);
            } else{
                enter_code(SCANC, "", "", ident);
            }
        }

        nextsym();
        while (sym == ','){
            nextsym();
            if (sym == identsym) {

                i = find(ident);
                if(i == -1){    //标识符未定义
                    error(2);
                } else if(!((table[i].ca == VAR || table[i].ca == PARA) && table[i].lenth == 0)){
                    error(2);   //不是非数组变量
                } else {
                    if(table[i].typ == INT) {
                        enter_code(SCANI, "", "", ident);
                    } else{
                        enter_code(SCANC, "", "", ident);
                    }
                }

                nextsym();
            } else if (sym != ')') { //需要一个标识符
                error(1);
            }
        }
    }
    else{   //需要一个标识符
        error(1);
    }

    if(sym == ')'){
        nextsym();
    } else{
        error(1);
    }

    printf("\t#### this is a scanf statement\n");
}

void printf_state(){
/*入口为printf
出口为";"*/

    char exp[MAX_ID_LENTH];
    char temp[MAX_ID_LENTH];
    int strpt;  //待打印字符串在字符串表的位置
    int i;

    nextsym();
    if(sym == '('){
        nextsym();
    } else{ //需要一个"("
        error(1);
    }

    if(sym == stringsym){

        strpt = strp;
        enter_string_table(string_value);
        sprintf(temp,"%d",strpt);
        enter_code(PRINTS,"","",temp);
        nextsym();
        if(sym == ','){
            nextsym();
            expression(exp);
            i = find(exp);
            if(i!=-1 && table[i].typ == CHAR){
                enter_code(PRINTC,"","",exp);
            } else {
                enter_code(PRINTI,"","",exp);
            }
        }
        else if(sym == ')'){}
        else{   //需要",)"
            error(1);
        }
    }else{   //标识符
        expression(exp);
        i = find(exp);
        if(i!=-1 && table[i].typ == CHAR){
            enter_code(PRINTC,"","",exp);
        } else {
            enter_code(PRINTI,"","",exp);
        }
    }

    if(sym == ')'){
        nextsym();
    } else{
        error(1);
    }

    enter_code(PRINTLN,"","","");

    printf("\t#### this is a printf statement\n");
}

void return_state(){
/*入口为return
出口为";"*/

    char exp[MAX_ID_LENTH];

    int has_retval = 0; //有无返回值
    nextsym();
    if(sym == '('){
        nextsym();
        expression(exp);
        has_retval = 1;
        if(sym == ')'){
            nextsym();
        } else{
            error(1);
        }
    }
    if((has_retval && table[global_position].typ==VOID) ||
            (!has_retval && table[global_position].typ!=VOID)){
        //返回值类型不匹配
        error(2);
    }
    if(has_retval){
        enter_code(RETURN,"","",exp);
    } else {
        enter_code(RETURN, "", "", "");
    }
    printf("\t#### this is a return statement\n");
}

int condition(int branch_true){
/*入口为一个条件的第一个symbol
出口为一个条件后面一个symbol*/

    //branch_true 为1时表示条件真跳转，否则条件假跳转
    char exp1[MAX_ID_LENTH];
    char exp2[MAX_ID_LENTH];
    symbol op;  //条件运算符

    expression(exp1);
    if(sym == ltsym || sym == lesym || sym == gtsym ||
            sym == gesym || sym == nesym || sym == eqsym){
        op = sym;
        nextsym();
        expression(exp2);

        if(op == nesym || op == eqsym){   //判断相等或不相等
            if((branch_true && op == eqsym) || (!branch_true && op == nesym)){
                enter_code(BEQ,exp1,exp2,"");
            } else{
                enter_code(BNE,exp1,exp2,"");
            }
        } else {    //判断大小
            new_tempval();
            enter_code(SUB, exp1, exp2, tempval);
            if (branch_true) {  //条件真跳转
                switch (op) {
                    case ltsym: {
                        enter_code(BLT,tempval,"","");
                        break;
                    }
                    case lesym: {
                        enter_code(BLE,tempval,"","");
                        break;
                    }
                    case gtsym: {
                        enter_code(BGT,tempval,"","");
                        break;
                    }
                    case gesym: {
                        enter_code(BGE,tempval,"","");
                        break;
                    }
                    default:{}
                }
            } else {  //条件假跳转
                switch (op) {
                    case ltsym: {
                        enter_code(BGE,tempval,"","");
                        break;
                    }
                    case lesym: {
                        enter_code(BGT,tempval,"","");
                        break;
                    }
                    case gtsym: {
                        enter_code(BLE,tempval,"","");
                        break;
                    }
                    case gesym: {
                        enter_code(BLT,tempval,"","");
                        break;
                    }
                    default:{}
                }
            }
        }
    } else{ //没有条件运算符
        if(branch_true){
            enter_code(BNE,exp1,"0","");
        } else{
            enter_code(BEQ,exp1,"0","");
        }
    }

    printf("\t#### this is a condition\n");
    return qp-1;
}

void factor(char *fname){
/*入口为"/ *"后一个symbol
出口为因子后面一个symbol*/

//    char exp[MAX_ID_LENTH];
    char temp[MAX_ID_LENTH];
    char id_name[MAX_ID_LENTH];
    int isneg = 0;
    int i,j;
    table_item item;

    if(sym == identsym){

        strcpy(id_name,ident);
        i = find(id_name);
        if(i == -1){    //标识符未定义
            error(2);
        } else{
            item = table[i];
        }

        nextsym();
        if(sym == '['){
            if(i != -1){
                if(!(item.ca == VAR && item.lenth!=0)){
                    error(2);   //不是数组变量
                }
            }
            nextsym();
            expression(temp);
            new_tempval();
            enter_code(RAR,id_name,temp,tempval);
            strcpy(fname,tempval);
            if(sym == ']'){
                nextsym();
            } else{ //需要一个"["
                error(1);
            }
        }
        else if(sym == '('){
            if(i != -1){
                if(item.ca != FUNCTION || item.typ == VOID){
                    error(2);   //不是函数或无返回值
                }
            }
            j = call_state(id_name);
            if(i != -1 && fun_table[item.value].para_num!=j){   //参数个数不符
                error(2);
            }

            new_tempval();
            enter_code(RETVAL,"","",tempval);
            strcpy(fname,tempval);
        }
        else {
            if(i != -1){
                if(!(item.ca == CONST || ((table[i].ca == VAR || table[i].ca == PARA) && item.lenth == 0))){
                    error(2);   //不是常量或非数组变量
                }
            }
            if( i!= -1 && item.ca == CONST){    //如果为常数，直接赋值
                sprintf(fname,"%d",item.value);
            } else{
                strcpy(fname, id_name);
            }

        }
    }
    else if(sym == '+' || sym == '-') {
        if(sym == '-'){
            isneg = 1;
        }
        nextsym();
        if(sym == integersym){

            sprintf(temp,"%d",int_value);
            if(isneg){
                new_tempval();
                enter_code(NEG,"",temp,tempval);
                strcpy(temp,tempval);
            }
            strcpy(fname,temp);
            nextsym();
        } else{ //需要一个无符号整数
            error(1);
        }
    }
    else if(sym == integersym || sym == zerosym){
        sprintf(fname,"%d",int_value);
        nextsym();
    }
    else if(sym == charactersym){
        sprintf(fname,"%d",ch_value);
        nextsym();
    }
    else if(sym == '('){
        nextsym();
        expression(temp);
        if(sym == ')'){
            strcpy(fname,temp);
            nextsym();
        } else{ //需要一个(
            error(1);
        }
    }
    else{   //不在因子的头符号集
        error(1);
    }

    printf("\t#### this is a factor\n");
}

void term(char *tname){
/*入口为"+-"后一个symbol
出口为项后面一个symbol*/

    char current[MAX_ID_LENTH]; //处理到当前的变量
    char fname[MAX_ID_LENTH];   //一个因子
    quadop op;

    factor(fname);
    strcpy(current,fname);

    while(sym == '*' || sym == '/'){

        if(sym == '*'){
            op = MUL;
        } else{
            op = DIV;
        }

        nextsym();
        factor(fname);

        new_tempval();
        enter_code(op,current,fname,tempval);
        strcpy(current,tempval);

    }

    printf("\t#### this is a term\n");
    strcpy(tname,current);
}

void expression(char *exp){
/*入口为一个表达式的第一个symbol
出口为一个表达式后面一个symbol*/

    int isneg = 0;
    char tname[MAX_ID_LENTH];
    char current[MAX_ID_LENTH]; //处理到当前的变量
    quadop op;

    if(sym == '+' || sym == '-'){
        if(sym == '-'){
            isneg = 1;
        }
        nextsym();
    }

    term(tname);
    if(isneg){
        new_tempval();
        enter_code(NEG,"",tname,tempval);
        strcpy(current,tempval);
    } else{
        strcpy(current,tname);
    }

    while(sym == '+' || sym == '-'){

        if(sym == '+'){
            op = ADD;
        } else{
            op = SUB;
        }

        nextsym();
        term(tname);

        new_tempval();
        enter_code(op,current,tname,tempval);
        strcpy(current,tempval);
    }
    printf("\t#### this is a expression\n");
    strcpy(exp,current);
}
