//
// Created by liuzheng on 2016/12/1.
//

#include "table.h"

extern table_item table[MAX_TABLE_LENTH];  //符号表
extern int tp; //符号表头指针

void sreg_alloc(int fp){
    //fp为待分配函数在符号表中的位置
    int i;
    int reg_num = 0;

    for(i = fp+1;table[i].ca!=FUNCTION && i<tp;i++){
        if(table[i].ca==VAR && table[i].lenth==0){
            if(reg_num<8){
                //符号表中变量的value域置为被分配的全局寄存器，为正值
                reg_num++;
                table[i].value = reg_num;
            }
        }
    }
}