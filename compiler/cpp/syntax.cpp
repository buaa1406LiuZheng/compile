//
// Created by liuzheng on 2016/11/14.
//

#include <stdio.h>
#include <string.h>
#include "syntax.h"
#include "table.h"
#include "lexical.h"
#include "error.h"

#define STATEMENT_SYM ifsym,dosym,forsym,lbracesym,scanfsym,printfsym,returnsym

extern symbol sym;

extern char ident[MAX_ID_LENTH];
extern int int_value;  // value of an integer constant
extern char ch_value;   // value of a character constant
extern char string_value[MAX_STRING_LENTH];    // value of a string constant
extern type type_value;

extern std::list<quadruples> quad_codes;  //四元式代码表
extern table_item table[MAX_TABLE_LENTH];
extern fun_table_item fun_table[MAX_FUN_TABLE_LENTH];

extern int tp; //符号表头指针
extern int global_position; //函数定义在符号表中的位置
extern int strp;   //字符串常量表头指针
extern int funp;

char tempval[MAX_ID_LENTH]; //临时变量
char label[MAX_ID_LENTH];   //跳转标签

void new_label(){
    static int i=0;
    std::list<quadruples>::iterator tmp;
    tmp = quad_codes.end();
    tmp--;

    if(tmp->op == LABEL){
        strcpy(label,tmp->r);
    } else {
        sprintf(label, "$$%d", i++);
        enter_code(LABEL, "", "", label);
    }
}

void new_tempval(){
    static int i=0;
    sprintf(tempval,"#%d",i++);
}

void programme(){
/*程序解析入口*/

    std::list<quadruples>::iterator tmp;

    int has_err = 0;

    nextsym();
    while(sym == constsym){ // 常量声明
        const_def(1);
    }

    while(sym == intsym || sym == charsym){ // 变量声明
        nextsym();
        if(sym != identsym){ //不是标识符，报错
            std::set<symbol> skipset = {identsym,commasym,semisym,lparensym,
                       voidsym,mainsym};
            error(11);
            skip(skipset);
            if(sym == identsym){
                nextsym();
            }
            else if(sym == voidsym || sym == mainsym){
                break;
            }
        } else {
            nextsym();
        }
        if(sym == lbracketsym || sym == commasym || sym == semisym){
            var_def(1);
        }
        else if(sym == lparensym){    // 函数声明
            fun_def();
            break;
        }
        else{   //需要"[,;("
            std::set<symbol> skipset = {semisym,intsym,charsym,voidsym,mainsym};
            error(39);
            skip(skipset);
            if(sym == semisym){
                nextsym();
            }
        }
    }

    while(sym == intsym || sym == charsym || sym == voidsym){   // 函数声明
        if(sym == voidsym) {
            nextsym();
            if (sym == identsym) {
                nextsym();
                if(sym == lparensym){
                    fun_def();
                } else{ //需要一个(
                    std::set<symbol> skipset = {intsym,charsym,voidsym,mainsym,lbracesym};
                    error(12);
                    skip(skipset);
                    if(sym == lbracesym){
                        nextsym();
                        block();
                    }
                }
            } else if (sym == mainsym) {    // 读到了main
                break;
            } else {   //void后不是标识符也不是main
                std::set<symbol> skipset = {intsym,charsym,voidsym,mainsym,lbracesym};
                error(11);
                skip(skipset);
                if(sym == lbracesym){
                    nextsym();
                    block();
                }
            }
        } else{
            nextsym();
            if (sym == identsym) {
                nextsym();
                fun_def();
            } else {   //int、char后不是标识符
                std::set<symbol> skipset = {intsym,charsym,voidsym,mainsym,lbracesym};
                error(11);
                skip(skipset);
                if(sym == lbracesym){
                    nextsym();
                    block();
                }
            }
        }
    }

    if(sym == mainsym){
        enter("main", FUNCTION, VOID, funp, 1, 0);

        tmp = quad_codes.end();
        tmp--;
        enter_fun_table("main", tmp, tmp, 0);

        symbol sym1, sym2;
        nextsym();
        sym1 = sym;
        nextsym();
        sym2 = sym;
        if(sym1 == lparensym && sym2 == rparensym){
            nextsym();
        } else{ //main后面需要一对括号
            error(14);
            has_err = 1;
        }
        if(sym == lbracesym || has_err){
            nextsym();
        } else{ //需要一个{
            error(15);
            has_err = 1;
        }

        if(has_err){
            std::set<symbol> skipset = {lbracesym};
            skip(skipset);
            nextsym();
        }

        block();

        table[global_position].lenth = tp - global_position-1;
        fun_table[funp-1].begin++;
        tmp = quad_codes.end();
        tmp--;
        fun_table[funp-1].end = tmp;
    } else {    //没有main
        error(26);
    }


}

void const_def(int isglobal){
/* 入口symbol为constsym
出口symbol为分号下一个*/

    int is_minus = 0;
    int i;

    int has_err = 0;

    nextsym();
    if(sym == intsym){
        do {
            nextsym();
            if (sym == identsym) {
                nextsym();
                if (sym == equalsym) {
                    nextsym();
                    if (sym == integersym) {

                        i = find(ident);
                        if(i == -1 || table[i].isglobal != isglobal) {
                            enter(ident, CONST, INT, int_value, isglobal, 0);
                        } else{ //重复定义
                            error(100);
                        }

                        nextsym();
                    }
                    else if (sym == zerosym) {

                        i = find(ident);
                        if(i == -1 || table[i].isglobal != isglobal) {
                            enter(ident, CONST, INT, 0, isglobal, 0);
                        } else{ //重复定义
                            error(100);
                        }

                        nextsym();
                    }
                    else if (sym == plussym || sym == minussym) {
                        if (sym == minussym) {
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
                                error(100);
                            }

                            nextsym();
                        } else { //需要一个无符号整数
                            error(22);
                            has_err = 1;
                            break;
                        }
                    }
                    else { //需要一个整数
                        error(21);
                        has_err = 1;
                        break;
                    }
                } else { //需要一个=
                    error(19);
                    has_err = 1;
                    break;
                }
            } else { //需要一个标识符
                error(11);
                has_err = 1;
                break;
            }
        }while (sym == commasym);
    }
    else if(sym == charsym){
        do {
            nextsym();
            if (sym == identsym) {
                nextsym();
                if (sym == equalsym) {
                    nextsym();
                    if (sym == charactersym) {

                        i = find(ident);
                        if(i == -1 || table[i].isglobal != isglobal) {
                            enter(ident, CONST, CHAR, ch_value, isglobal, 0);
                        } else{ //重复定义
                            error(100);
                        }

                        nextsym();
                    } else { //需要一个字符
                        error(24);
                        has_err = 1;
                        break;
                    }
                } else { //需要一个=
                    error(19);
                    has_err = 1;
                    break;
                }
            } else { //需要一个标识符
                error(11);
                has_err = 1;
                break;
            }
        }while (sym == commasym);
    }
    else{   //需要一个int 或 char
        error(23);
        has_err = 1;
    }

    if(has_err){
        if(isglobal) {
            std::set<symbol> skipset = {intsym, charsym, voidsym, constsym, mainsym, semisym};
            skip(skipset);
        } else{
            std::set<symbol> skipset = {STATEMENT_SYM, intsym, charsym, constsym, semisym};
            skip(skipset);
        }
        if(sym == semisym){
            nextsym();
        }
    } else {
        semicolon();
    }

}

void var_def(int isglobal){
/* 入口symbol为int，char（局部变量)或 ",;[" （全局变量）
出口symbol为分号下一个*/

    int i;
    int has_skip = 0;  //在全局变量处理时会预读入了一个标识符符号，需要跳过一次读标识符

    int has_err = 0;

    if(isglobal){
        has_skip = 1;
    }

    do {
        if(!has_skip) {
            nextsym();
            if (sym == identsym) {
                nextsym();
            } else { //不是标识符，报错
                error(11);
                has_err = 1;
                break;
            }
        } else{
            has_skip = 0;
        }

        if (sym == commasym || sym == semisym) {

            i = find(ident);
            if(i == -1 || table[i].isglobal != isglobal) {
                enter(ident, VAR, type_value, 0, isglobal, 0);
            } else{ //重复定义
                error(100);
            }

        }
        else if (sym == lbracketsym) {
            nextsym();
            if(sym == integersym){

                i = find(ident);
                if(i == -1 || table[i].isglobal != isglobal) {
                    enter(ident, VAR, type_value, 0, isglobal, int_value);
                } else{ //重复定义
                    error(100);
                }

                nextsym();
            } else{ //数组大小不是无符号整数
                error(22);
                has_err = 1;
                break;
            }

            if(sym == rbracketsym){
                nextsym();
                if(sym == semisym || sym == commasym){}
                else{   //需要;或,
                    error(39);
                    has_err = 1;
                    break;
                }
            } else{ // 需要一个]
                error(18);
                has_err = 1;
                break;
            }
        }
        else {  //需要;或,或[
            error(39);
            has_err = 1;
            break;
        }
    }while(sym == commasym);

    if(has_err){
        if(isglobal) {
            std::set<symbol> skipset = {intsym, charsym, voidsym, mainsym, semisym};
            skip(skipset);
        } else{
            std::set<symbol> skipset = {STATEMENT_SYM, intsym, charsym, semisym};
            skip(skipset);
        }
        if(sym == semisym){
            nextsym();
        }
    } else {
        semicolon();
    }
    
}

void fun_def(){
/* 入口symbol为"("
出口symbol为"}"下一个*/

    const int max_para = 127;

    std::list<quadruples>::iterator tmp;

    int i;
    int para_count = 0; //参数个数
    char fun_name[MAX_ID_LENTH];

    int has_err = 0;

    i = find(ident);
    if(i == -1) {
        enter(ident, FUNCTION, type_value, funp, 1, 0);
    } else{ //重复定义
        error(100);
        enter("", FUNCTION, type_value, funp, 1, 0);
    }
    strcpy(fun_name,ident);

    do{ //参数声明部分
        nextsym();
        if(sym == intsym || sym == charsym){
            nextsym();
            if(sym == identsym){
                para_count++;
                if(para_count == max_para+1){
                    error(103);
                }
                i = find(ident);
                if(i == -1 || table[i].isglobal) {
                    enter(ident, PARA, type_value, 0, 0, 0);
                } else{ //重复定义
                    error(100);
                }

                nextsym();
            } else{ //需要一个标识符
                error(11);
                has_err = 1;
                break;
            }
        }
        else if(sym == rparensym){}
        else{ //需要int或char或)
            error(13);
            has_err = 1;
            break;
        }
    }while(sym == commasym);
    tmp = quad_codes.end();
    tmp--;
    enter_fun_table(fun_name,tmp,tmp,para_count);

    if(has_err){
        std::set<symbol> skipset = {lbracesym,rbracesym,commasym,rparensym,voidsym,mainsym};
        skip(skipset);
        if(sym!=lbracesym && sym!=rparensym){
            return;
        }
    }

    if(sym == rparensym || has_err){
        nextsym();
    } else{ //需要一个)
        error(13);
    }

    if(sym == lbracesym){
        nextsym();
        //进入函数声明体
        block();
        enter_code(RETURN, "", "", "");

        table[global_position].lenth = tp - global_position-1;
        fun_table[funp-1].begin++;
        tmp = quad_codes.end();
        tmp--;
        fun_table[funp-1].end = tmp;

    } else{ //需要一个{
        std::set<symbol> skipset = {rbracesym, voidsym, mainsym};
        error(15);
        skip(skipset);
        if(sym == rbracesym){
            nextsym();
        }
        return;
    }

    if(sym == rbracesym){
        nextsym();
    } else{ //需要一个}
        error(16);
    }


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
    else if(sym == lbracesym){
        nextsym();
        multi_state();
        nextsym();
    }
    else if(sym == identsym){
        nextsym();
        if(sym == lparensym){

            i = find(ident);
            if(i == -1){    //标识符未定义
                error(101);
            } else if(table[i].ca != FUNCTION){
                error(106);   //不是函数
            }

            j = call_state(ident);
            if(i != -1 && fun_table[table[i].value].para_num!=j){   //参数个数不符
                error(102);
            }
            semicolon();
        }
        else if(sym == equalsym || sym == lbracketsym){
            assign_state();
        }
        else{   //需要"(=["
            std::set<symbol> skipset = {STATEMENT_SYM,
                       rbracesym,semisym};
            error(19);
            skip(skipset);
            if(sym == semisym){
                nextsym();
            }
        }
    }
    else if(sym == scanfsym){
        scanf_state();
    }
    else if(sym == printfsym){
        printf_state();
    }
    else if(sym == returnsym){
        return_state();
    }
    else if(sym == semisym){
        //空语句
        nextsym();
    }
    else{   //符号不在语句头符号集中
        std::set<symbol> skipset = {STATEMENT_SYM,
                    rbracesym,semisym};
        error(31);
        skip(skipset);
        if(sym == semisym){
            nextsym();
        }
    }
}

void multi_state(){
/*入口为一个语句的第一个symbol
出口为"}"*/
    while(sym != rbracesym){
        statement();
    }

}

void if_state(){
/*入口为if
出口为一条语句后面一个symbol*/

    std::list<quadruples>::iterator branchp;  //if为假的跳转指令地址
    std::list<quadruples>::iterator jumpp;  //有else时，if为真语句后的跳转指令地址

    int has_err=0;

    nextsym();
    if(sym == lparensym){
        nextsym();

        if(condition(0)!=-1) {
            branchp = quad_codes.end();
            branchp--;
            if (sym == rparensym) {
                nextsym();
            } else {
                error(13);
                has_err = 1;
            }
        } else{
            has_err = 1;
        }
    } else{
        error(12);
        has_err = 1;
    }

    if(has_err){
        std::set<symbol> skipset = {STATEMENT_SYM,
                   semisym,rparensym};
        skip(skipset);
        if(sym == semisym || sym == rparensym){
            nextsym();
            return;
        }
    }

    statement();    //if成立的语句
    if(sym == elsesym){ //有else
        enter_code(JUMP,"","","");  //if成立要跳过else的语句
        jumpp = quad_codes.end();
        jumpp--;

        new_label();    //if为假跳到这里
        strcpy(branchp->r,label);

        nextsym();
        statement();    //else部分语句

        new_label();    //if为真跳过else部分语句
        strcpy(jumpp->r,label);
    }
    else{   //没有else，if为假跳到这里
        new_label();
        strcpy(branchp->r,label);
    }


}

void do_while_state(){
/*入口为do
出口为")"后面一个symbol*/

    std::list<quadruples>::iterator branchp;    //while条件跳转指令地址

    char label_save[MAX_ID_LENTH];  //保存跳转label

    int has_err = 0;

    new_label();
    strcpy(label_save,label);

    nextsym();
    statement();

    if(sym == whilesym) {
        nextsym();
        if(sym == lparensym){
            nextsym();


            if(condition(1) != -1) {
                branchp = quad_codes.end();
                branchp--;
                strcpy(branchp->r, label_save);
            } else{
                has_err = 1;
            }
        } else{
            error(12);
            has_err = 1;
        }
    } else{ //需要一个while
        error(28);
        has_err = 1;
    }

    if(has_err){
        std::set<symbol> skipset = {STATEMENT_SYM,
                   semisym,rparensym};
        skip(skipset);
        if(sym == semisym || sym == rparensym){
            nextsym();
        }
        return;
    }

    if(sym == rparensym){
        nextsym();
    } else{
        std::set<symbol> skipset = {STATEMENT_SYM, semisym};
        error(13);
        skip(skipset);
        if(sym == semisym){
            nextsym();
        }
    }


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
    std::list<quadruples>::iterator branchp;
    int i;
    int has_err = 1;

    nextsym();
    if(sym == lparensym){
        nextsym();
        if(sym == identsym){

            strcpy(id1,ident);
            i = find(id1);
            if(i == -1){    //标识符未定义
                error(101);
            }

            nextsym();
            if(sym == equalsym){
                nextsym();
                if(expression(exp)!=-1) {
                    enter_code(MOV, "", exp, id1);

                    if (sym == semisym) {

                        new_label();    //循环结束无条件跳转位置
                        strcpy(label_save, label);

                        nextsym();
                        if (condition(0) != -1) {
                            branchp = quad_codes.end();
                            branchp--;
                            if (sym == semisym) {
                                nextsym();
                                if (sym == identsym) {

                                    strcpy(id2, ident);
                                    i = find(id2);
                                    if (i == -1) {    //标识符未定义
                                        error(101);
                                    } else if (strcmp(id1, id2) != 0) {  //此标识符与循环变量不是同一个
                                        error(150);
                                    }

                                    nextsym();
                                    if (sym == equalsym) {
                                        nextsym();
                                        if (sym == identsym) {

                                            strcpy(id3, ident);
                                            i = find(id3);
                                            if (i == -1) {    //标识符未定义
                                                error(101);
                                            } else if (strcmp(id1, id3) != 0) {  //此标识符与循环变量不是同一个
                                                error(150);
                                            }

                                            nextsym();
                                            if (sym == plussym || sym == minussym) {

                                                if (sym == plussym) {
                                                    isinc = 1;
                                                } else {
                                                    isinc = 0;
                                                }

                                                nextsym();
                                                if (sym == integersym) {
                                                    sprintf(step, "%d", int_value);
                                                    nextsym();
                                                    if (sym == rparensym) {
                                                        nextsym();
                                                        has_err = 0;
                                                    } else { //需要一个rparensym
                                                        error(13);
                                                    }
                                                } else { //需要一个无符号整数
                                                    error(22);
                                                }
                                            } else { //需要"+-"
                                                error(25);
                                            }
                                        } else { //需要一个标识符
                                            error(11);
                                        }
                                    } else { //需要一个"="
                                        error(19);
                                    }
                                } else { //需要一个标识符
                                    error(11);
                                }
                            } else { //需要一个";"
                                error(39);
                            }
                        }
                    } else { //需要一个";"
                        error(39);
                    }
                }
            } else{ //需要一个"="
                error(19);
            }
        } else{ //需要一个标识符
            error(11);
        }
    } else{ //需要一个lparensym
        error(12);
    }

    if(has_err){
        std::set<symbol> skipset = {STATEMENT_SYM,
                                      semisym,rparensym};
        skip(skipset);
        if(sym == semisym || sym == rparensym){
            nextsym();
            return;
        }
    }

    statement();
    if(isinc){  //增加步长
        enter_code(ADD,id3,step,id2);
    } else{
        enter_code(SUB,id3,step,id2);
    }
    enter_code(JUMP,"","",label_save);   //跳回判断处

    if(!has_err) {  //没有语法错误生成了跳转语句
        new_label();
        strcpy(branchp->r, label);    //循环条件判断失败时跳到这里
    }


}

void semicolon(){
/*检测一个分号*/
    if(sym == semisym){
        nextsym();
    } else{ //需要一个分号
        error(39);
    }
}

int call_state(char *id){
/*入口为一个"("
出口为";"*/

    const int max_para = 127;

    char exp[MAX_ID_LENTH];
    char fun_name[MAX_ID_LENTH];
    char temp[MAX_ID_LENTH];
    char para[max_para][MAX_ID_LENTH];
    int para_count = 0;
    int i;

    strcpy(fun_name,id);
    do{
        nextsym();
        if(sym == rparensym){}
        else{
            if(expression(exp)==-1){
                std::set<symbol> skipset = {STATEMENT_SYM,
                semisym,rparensym};
                skip(skipset);
                if(sym == semisym){
                    nextsym();
                    return para_count;
                } else if(sym == rparensym){
                    continue;
                } else{
                    return para_count;
                }
            }
            if(para_count<max_para){
                strcpy(para[para_count],exp);
            }
            para_count++;
        }
    }while(sym == commasym);

    for(i=0;i<para_count;i++){
        sprintf(temp,"%d",i);
        enter_code(PARAIN,temp,para[i],fun_name);
    }

    if(sym == rparensym){
        nextsym();
    } else{
        std::set<symbol> skipset = {STATEMENT_SYM, semisym};
        error(13);
        skip(skipset);
        if(sym == semisym){
            nextsym();
        }
    }

    enter_code(CALL,"","",fun_name);
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

    int has_err = 1;

    strcpy(left_ident,ident);

    i = find(left_ident);
    if(i != -1){
        item = table[i];
    } else{ //标识符未定义
        error(101);
    }

    if(sym == equalsym){

        if( i!=-1 && !((item.ca == VAR || item.ca == PARA) && item.lenth==0)){ //不是非数组变量
            error(105);
        }
        nextsym();
        if(expression(exp)!=-1) {
            enter_code(MOV, "", exp, left_ident);
            has_err = 0;
        }
    }
    else if(sym == lbracketsym){

        if( i!=-1 && !(item.ca == VAR && item.lenth!=0)){ //不是数组变量
            error(104);
        }

        nextsym();

        if(expression(exp)!=-1) {
            strcpy(temp, exp);
            if (sym == rbracketsym) {
                nextsym();
                if (sym == equalsym) {
                    nextsym();
                    if(expression(exp)!=-1) {
                        enter_code(WAR, left_ident, temp, exp);
                        has_err = 0;
                    }
                } else { //需要一个"="
                    error(19);
                }
            } else { //需要一个"]"
                error(18);
            }
        }
    }
    else{   //需要"=["
        error(19);
    }

    if(has_err){
        std::set<symbol> skipset = {STATEMENT_SYM,
                   semisym};
        skip(skipset);
        if(sym == semisym){
            nextsym();
        }
    } else{
        semicolon();
    }

}

void scanf_state(){
/*入口为scanf
出口为";"*/

    int i;

    int has_err = 0;

    nextsym();
    if(sym == lparensym){
        nextsym();
        if(sym == identsym) {

            i = find(ident);
            if(i == -1){    //标识符未定义
                error(101);
            } else if(!((table[i].ca == VAR || table[i].ca == PARA) && table[i].lenth == 0)){
                error(105);   //不是非数组变量
            } else {
                if(table[i].typ == INT) {
                    enter_code(SCANI, "", "", ident);
                } else{
                    enter_code(SCANC, "", "", ident);
                }
            }

            nextsym();
            while (sym == commasym){
                nextsym();
                if (sym == identsym) {

                    i = find(ident);
                    if(i == -1){    //标识符未定义
                        error(101);
                    } else if(!((table[i].ca == VAR || table[i].ca == PARA) && table[i].lenth == 0)){
                        error(105);   //不是非数组变量
                    } else {
                        if(table[i].typ == INT) {
                            enter_code(SCANI, "", "", ident);
                        } else{
                            enter_code(SCANC, "", "", ident);
                        }
                    }

                    nextsym();
                } else { //需要一个标识符
                    error(11);
                    has_err = 1;
                }
            }
        }
        else{   //需要一个标识符
            error(11);
            has_err = 1;
        }
    } else{ //需要一个"("
        error(12);
        has_err = 1;
    }

    if(has_err){
        std::set<symbol> skipset = {STATEMENT_SYM,semisym};
        skip(skipset);
        if(sym == semisym){
            nextsym();
        }
    } else {
        if (sym == rparensym) {
            nextsym();
            semicolon();
        } else {
            std::set<symbol> skipset = {STATEMENT_SYM,semisym};
            error(13);
            skip(skipset);
            if(sym == semisym){
                nextsym();
            }
        }
    }
}

void printf_state(){
/*入口为printf
出口为";"*/

    char exp[MAX_ID_LENTH];
    char temp[MAX_ID_LENTH];
    int strpt;  //待打印字符串在字符串表的位置
    int ischar;

    int has_err = 0;

    nextsym();
    if(sym == lparensym){
        nextsym();
        if(sym == nullsym){
            return;
        }
        if(sym == stringsym){

            strpt = strp;
            enter_string_table(string_value);
            sprintf(temp,"%d",strpt);
            enter_code(PRINTS,"","",temp);
            nextsym();
            if(sym == commasym){
                nextsym();
                ischar = expression(exp);
                if(ischar!=-1) {
                    if (ischar) {   //刚读到一个字符
                        enter_code(PRINTC, "", "", exp);
                    } else {
                        enter_code(PRINTI, "", "", exp);
                    }
                } else{ //表达式解析出错
                    has_err = 1;
                }
            }
            else if(sym == rparensym){}
            else{   //需要",)"
                error(13);
                has_err = 1;
            }
        }else{   //标识符
            ischar = expression(exp);
            if(ischar!=-1) {
                if (ischar) {   //刚读到一个字符
                    enter_code(PRINTC, "", "", exp);
                } else {
                    enter_code(PRINTI, "", "", exp);
                }
            } else{ //表达式解析出错
                has_err = 1;
            }
        }
    } else{ //需要一个"("
        error(12);
        has_err = 1;
    }

    if(has_err){
        std::set<symbol> skipset = {STATEMENT_SYM,semisym};
        skip(skipset);
        if(sym == semisym){
            nextsym();
        }
    } else {
        if (sym == rparensym) {
            nextsym();
            semicolon();
        } else {
            std::set<symbol> skipset = {STATEMENT_SYM,semisym};
            error(13);
            skip(skipset);
            if(sym == semisym){
                nextsym();
            }
        }
    }

    enter_code(PRINTLN,"","","");


}

void return_state(){
/*入口为return
出口为";"*/

    char exp[MAX_ID_LENTH];
    int has_retval = 0; //有无返回值

    int has_err = 0;

    nextsym();
    if(sym == lparensym){
        nextsym();
        if(expression(exp)!=-1) {
            has_retval = 1;
            if (sym == rparensym) {
                nextsym();
            } else {
                error(13);
                has_err = 1;
            }
        } else{
            has_err = 1;
        }
    }

    if(has_err){
        std::set<symbol> skipset = {STATEMENT_SYM,semisym};
        skip(skipset);
        if(sym == semisym){
            nextsym();
        }
        return;
    }

    //返回值类型不匹配
    if(has_retval && table[global_position].typ==VOID){
        error(108);
    }
    if(!has_retval && table[global_position].typ!=VOID){
        error(109);
    }
    if(has_retval){
        enter_code(RETURN,"","",exp);
    } else {
        enter_code(RETURN, "", "", "");
    }
    semicolon();
}

int condition(int branch_true){
/*入口为一个条件的第一个symbol
出口为一个条件后面一个symbol*/

    //branch_true 为1时表示条件真跳转，否则条件假跳转
    char exp1[MAX_ID_LENTH];
    char exp2[MAX_ID_LENTH];
    symbol op;  //条件运算符

    if(expression(exp1)==-1){
        return -1;
    }
    if(sym == ltsym || sym == lesym || sym == gtsym ||
            sym == gesym || sym == nesym || sym == eqsym){
        op = sym;
        nextsym();
        if(expression(exp2)==-1){
            return -1;
        }

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

    return 0;
}

int factor(char *fname){
/*入口为"/ *"后一个symbol
出口为因子后面一个symbol*/

//    char exp[MAX_ID_LENTH];
    char temp[MAX_ID_LENTH];
    char id_name[MAX_ID_LENTH];
    int isneg = 0;
    int i,j;
    table_item item;
    int ischar = 0;

    int has_err = 0;

    if(sym == identsym){

        strcpy(id_name,ident);
        i = find(id_name);
        if(i == -1){    //标识符未定义
            error(101);
        } else{
            item = table[i];
        }

        nextsym();
        if(sym == lbracketsym){ //数组
            if(i != -1){
                if(!(item.ca == VAR && item.lenth!=0)){
                    error(104);   //不是数组变量
                }
                if(item.typ == CHAR){   //字符数组
                    ischar = 1;
                }
            }
            nextsym();
            if(expression(temp)==-1){
                return -1;
            }
            new_tempval();
            enter_code(RAR,id_name,temp,tempval);
            strcpy(fname,tempval);
            if(sym == rbracketsym){
                nextsym();
            } else{ //需要一个"]"
                error(18);
                has_err = 1;
            }
        }
        else if(sym == lparensym){    //函数调用
            if(i != -1){
                if(item.ca != FUNCTION || item.typ == VOID){
                    error(107);   //不是函数或无返回值
                }
                if(item.typ == CHAR){   //函数返回值为char
                    ischar = 1;
                }
            }
            j = call_state(id_name);
            if(i != -1 && fun_table[item.value].para_num!=j){   //参数个数不符
                error(102);
            }

            new_tempval();
            enter_code(RETVAL,"","",tempval);
            strcpy(fname,tempval);
        }
        else {  //常量或变量
            if(i != -1){
                if(!(item.ca == CONST || ((table[i].ca == VAR || table[i].ca == PARA) && item.lenth == 0))){
                    error(110);   //不是常量或非数组变量
                }
            }
            if( i!= -1 && item.ca == CONST){    //如果为常数，直接赋值
                sprintf(fname,"%d",item.value);
                if(item.typ == CHAR){   //字符常量
                    ischar = 1;
                }
            } else{
                strcpy(fname, id_name);
                if(item.typ == CHAR){   //字符变量
                    ischar = 1;
                }
            }

        }
    }
    else if(sym == plussym || sym == minussym) {
        if(sym == minussym){
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
            error(22);
            has_err = 1;
        }
    }
    else if(sym == integersym || sym == zerosym){
        sprintf(fname,"%d",int_value);
        nextsym();
    }
    else if(sym == charactersym){
        ischar = 1;
        sprintf(fname,"%d",ch_value);
        nextsym();
    }
    else if(sym == lparensym){
        nextsym();
        ischar = expression(temp);
        if(ischar == -1){
            return -1;
        }
        if(sym == rparensym){
            strcpy(fname,temp);
            nextsym();
        } else{ //需要一个(
            error(12);
            has_err = 1;
        }
    }
    else{   //不在因子的头符号集
        error(32);
        has_err = 1;
    }

    if(has_err){
        return -1;
    }
    return ischar;
}

int term(char *tname){
/*入口为"+-"后一个symbol
出口为项后面一个symbol*/

    char current[MAX_ID_LENTH]; //处理到当前的变量
    char fname[MAX_ID_LENTH];   //一个因子
    quadop op;
    int ischar;

    ischar = factor(fname);
    if(ischar == -1){
        return -1;
    }

    strcpy(current,fname);

    while(sym == mulsym || sym == divsym){

        ischar = 0;
        if(sym == mulsym){
            op = MUL;
        } else{
            op = DIV;
        }

        nextsym();
        if(factor(fname)==-1){
            return -1;
        }

        new_tempval();
        enter_code(op,current,fname,tempval);
        strcpy(current,tempval);

    }


    strcpy(tname,current);

    return ischar;
}

int expression(char *exp){
/*入口为一个表达式的第一个symbol
出口为一个表达式后面一个symbol*/

    int isneg = 0;
    char tname[MAX_ID_LENTH];
    char current[MAX_ID_LENTH]; //处理到当前的变量
    quadop op;
    int ischar;

    if(sym == plussym || sym == minussym){
        if(sym == minussym){
            isneg = 1;
        }
        nextsym();
    }

    ischar = term(tname);
    if(ischar == -1){
        return -1;
    }

    if(isneg){
        ischar = 0;
        new_tempval();
        enter_code(NEG,"",tname,tempval);
        strcpy(current,tempval);
    } else{
        strcpy(current,tname);
    }

    while(sym == plussym || sym == minussym){
        ischar = 0;
        if(sym == plussym){
            op = ADD;
        } else{
            op = SUB;
        }

        nextsym();
        if(term(tname)==-1){
            return -1;
        }

        new_tempval();
        enter_code(op,current,tname,tempval);
        strcpy(current,tempval);
    }

    strcpy(exp,current);

    return ischar;
}
