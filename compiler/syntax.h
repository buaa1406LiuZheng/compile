//
// Created by liuzheng on 2016/11/12.
//

#ifndef COMPILER_SYNTAX_H
#define COMPILER_SYNTAX_H

void programme();
void const_def(int isglobal);
void var_def(int skipIdent);
void fun_def();
void block();
void statement();
void multi_state();
void if_state();
void do_while_state();
void for_state();
void semicolon();
int call_state(char *id);
void assign_state();
void scanf_state();
void printf_state();
void return_state();
int condition(int branch_true);
void expression(char *exp);
//void term();
//void factor();
//void new_temp();

#endif //COMPILER_SYNTAX_H
