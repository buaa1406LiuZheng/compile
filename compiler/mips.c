//
// Created by liuzheng on 2016/11/22.
//

#include <stdio.h>
#include <string.h>
#include "mips.h"
#include "table.h"
#include "error.h"

extern quadruples quad_codes[MAX_CODES_LENTH];
extern fun_table_item fun_table[MAX_FUN_TABLE_LENTH];
extern table_item table[MAX_TABLE_LENTH];  //符号表
extern char string_table[MAX_STRING_CONST_STORAGE_LENTH];  //字符串常量表
extern int funp;
extern int tp;
extern int strp;   //字符串常量表头指针

extern int qp;
extern FILE *fout;

local_val local_table[MAX_LOCAL_VAL];   //局部变量表，存储局部变量以及其在栈的位置
int ltp;    //局部变量表头指针
int offset; //局部变量在活动记录中相对基地址的偏移量

void enter_local(char *id, int position){
    local_val temp;
    if(ltp > MAX_LOCAL_VAL){
        fatal_error();
    } else{
        strcpy(temp.name,id);
        temp.position = position;
        local_table[ltp++] = temp;
    }
}
int find_local(char *id){
    int i;
    for(i=0;i<ltp;i++){
        if(strcmp(id,local_table[i].name) == 0){
            return i;
        }
    }

    return -1;
}

int is_int(char *id){   //判断此标识符是否为立即数
    if(id[0]>='0' && id[0]<='9'){
        return 1;
    }
    return 0;
}

int get_val(char *id, int *is_id_int, int *is_id_global){

    int id_value;
    int i;

    *is_id_int = is_int(id);
    if(*is_id_int){
        sscanf(id,"%d",&id_value);
    } else{
        i = find_local(id);
        if(i == -1){
            *is_id_global = 1;
            id_value = 0;
        } else{
            *is_id_global = 0;
            id_value = local_table[i].position;
        }
    }

    return id_value;
}

void load_val(char *id, int is_id_global, int id_value, char* reg){
    //将变量id放入reg代表的寄存器中，过程使用了$t9
    if(is_id_global){
        fprintf(fout,"la $t9 %s\n",id);
        fprintf(fout,"lw %s 0($t9)\n",reg);
    } else{
        fprintf(fout,"lw %s %d($fp)\n",reg, id_value);
    }
}

void store_val(char *id, int is_id_global, int id_value, char* reg){
    //将寄存器reg的内容存入id代表的内存中，过程中使用了$t9
    int i;
    i = find(id);

    if(is_id_global){
        fprintf(fout,"la $t9 %s\n",id);
        if(table[i].typ == CHAR){
            fprintf(fout,"sb %s 0($t9)\n",reg);
        } else {
            fprintf(fout, "sw %s 0($t9)\n", reg);
        }
    } else{
        if(table[i].typ == CHAR){
            fprintf(fout,"sb %s %d($fp)\n",reg, id_value);
        } else {
            fprintf(fout, "sw %s %d($fp)\n", reg, id_value);
        }
    }
}

void add_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_x_int, is_y_int, is_r_int;
    int is_x_global = 0, is_y_global = 0, is_r_global = 0;
    int x_value, y_value, r_value;

    strcpy(x,quad.x);
    strcpy(y,quad.y);
    strcpy(r,quad.r);

    x_value = get_val(x, &is_x_int, &is_x_global);
    y_value = get_val(y, &is_y_int, &is_y_global);
    r_value = get_val(r, &is_r_int, &is_r_global);

    //载入x, y 并将x+y结果存入$t2
    if(is_x_int && is_y_int){   //都是立即数
        fprintf(fout,"li $t2 %d\n", x_value+y_value);
    } else{
        if(is_x_int){   //x是立即数
            load_val(y,is_y_global,y_value,"$t1");

            fprintf(fout,"addi $t2 $t1 %d\n",x_value);
        } else if(is_y_int){    //y是立即数
            load_val(x,is_x_global,x_value,"$t0");

            fprintf(fout,"addi $t2 $t0 %d\n",y_value);
        } else{ //都不是立即数
            load_val(x,is_x_global,x_value,"$t0");
            load_val(y,is_y_global,y_value,"$t1");
            fprintf(fout,"add $t2 $t0 $t1\n");
        }
    }

    store_val(r, is_r_global, r_value, "$t2");
}

void sub_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_x_int, is_y_int, is_r_int;
    int is_x_global = 0, is_y_global = 0, is_r_global = 0;
    int x_value, y_value, r_value;

    strcpy(x,quad.x);
    strcpy(y,quad.y);
    strcpy(r,quad.r);

    x_value = get_val(x, &is_x_int, &is_x_global);
    y_value = get_val(y, &is_y_int, &is_y_global);
    r_value = get_val(r, &is_r_int, &is_r_global);

    //载入x, y 并将x-y结果存入$t2
    if(is_x_int && is_y_int){   //都是立即数
        fprintf(fout,"li $t2 %d\n", x_value-y_value);
    } else{
        if(is_x_int){   //x是立即数
            fprintf(fout,"li $t0 %d\n",x_value);
            load_val(y,is_y_global,y_value,"$t1");

            fprintf(fout,"sub $t2 $t0 $t1\n");
        } else if(is_y_int){    //y是立即数
            load_val(x,is_x_global,x_value,"$t0");

            fprintf(fout,"addi $t2 $t0 %d\n",-y_value);
        } else{ //都不是立即数
            load_val(x,is_x_global,x_value,"$t0");
            load_val(y,is_y_global,y_value,"$t1");
            fprintf(fout,"sub $t2 $t0 $t1\n");
        }
    }

    store_val(r, is_r_global, r_value, "$t2");
}

void mul_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_x_int, is_y_int, is_r_int;
    int is_x_global = 0, is_y_global = 0, is_r_global = 0;
    int x_value, y_value, r_value;

    strcpy(x,quad.x);
    strcpy(y,quad.y);
    strcpy(r,quad.r);

    x_value = get_val(x, &is_x_int, &is_x_global);
    y_value = get_val(y, &is_y_int, &is_y_global);
    r_value = get_val(r, &is_r_int, &is_r_global);

    //载入x, y 并将x*y结果存入$t2
    if(is_x_int && is_y_int){   //都是立即数
        fprintf(fout,"li $t2 %d\n", x_value*y_value);
    } else{
        if(is_x_int){   //x是立即数
            load_val(y,is_y_global,y_value,"$t1");

            fprintf(fout,"mul $t2 $t1 %d\n",x_value);
        } else if(is_y_int){    //y是立即数
            load_val(x,is_x_global,x_value,"$t0");

            fprintf(fout,"mul $t2 $t0 %d\n",y_value);
        } else{ //都不是立即数
            load_val(x,is_x_global,x_value,"$t0");
            load_val(y,is_y_global,y_value,"$t1");
            fprintf(fout,"mul $t2 $t0 $t1\n");
        }
    }

    store_val(r, is_r_global, r_value, "$t2");
}

void div_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_x_int, is_y_int, is_r_int;
    int is_x_global = 0, is_y_global = 0, is_r_global = 0;
    int x_value, y_value, r_value;

    strcpy(x,quad.x);
    strcpy(y,quad.y);
    strcpy(r,quad.r);

    x_value = get_val(x, &is_x_int, &is_x_global);
    y_value = get_val(y, &is_y_int, &is_y_global);
    r_value = get_val(r, &is_r_int, &is_r_global);

    //载入x, y 并将x/y结果存入$t2
    if(is_x_int && is_y_int){   //都是立即数
        fprintf(fout,"li $t2 %d\n", x_value/y_value);
    } else{
        if(is_x_int){   //x是立即数
            fprintf(fout,"li $t0 %d\n",x_value);
            load_val(y,is_y_global,y_value,"$t1");

            fprintf(fout,"div $t2 $t0 $t1\n");
        } else if(is_y_int){    //y是立即数
            load_val(x,is_x_global,x_value,"$t0");

            fprintf(fout,"div $t2 $t0 %d\n",y_value);
        } else{ //都不是立即数
            load_val(x,is_x_global,x_value,"$t0");
            load_val(y,is_y_global,y_value,"$t1");
            fprintf(fout,"div $t2 $t0 $t1\n");
        }
    }

    store_val(r, is_r_global, r_value, "$t2");
}

void beq_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_x_int, is_y_int;
    int is_x_global = 0, is_y_global = 0;
    int x_value, y_value;

    strcpy(x,quad.x);
    strcpy(y,quad.y);
    strcpy(r,quad.r);

    x_value = get_val(x, &is_x_int, &is_x_global);
    y_value = get_val(y, &is_y_int, &is_y_global);

    //载入x, y 并将x*y结果存入$t2
    if(is_x_int && is_y_int){   //都是立即数
        if(x_value == y_value){
            fprintf(fout,"j %s\n", r);
        }
    } else{
        if(is_x_int){   //x是立即数
            load_val(y,is_y_global,y_value,"$t1");

            fprintf(fout,"beq $t1 %d %s\n",x_value, r);
        } else if(is_y_int){    //y是立即数
            load_val(x,is_x_global,x_value,"$t0");

            fprintf(fout,"beq $t0 %d %s\n",y_value, r);
        } else{ //都不是立即数
            load_val(x,is_x_global,x_value,"$t0");
            load_val(y,is_y_global,y_value,"$t1");
            fprintf(fout,"beq $t1 $t0 %s\n",r);
        }
    }

}

void bne_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_x_int, is_y_int;
    int is_x_global = 0, is_y_global = 0;
    int x_value, y_value;

    strcpy(x,quad.x);
    strcpy(y,quad.y);
    strcpy(r,quad.r);

    x_value = get_val(x, &is_x_int, &is_x_global);
    y_value = get_val(y, &is_y_int, &is_y_global);

    //载入x, y 并将x*y结果存入$t2
    if(is_x_int && is_y_int){   //都是立即数
        if(x_value != y_value){
            fprintf(fout,"j %s\n", r);
        }
    } else{
        if(is_x_int){   //x是立即数
            load_val(y,is_y_global,y_value,"$t1");

            fprintf(fout,"bne $t1 %d %s\n",x_value, r);
        } else if(is_y_int){    //y是立即数
            load_val(x,is_x_global,x_value,"$t0");

            fprintf(fout,"bne $t0 %d %s\n",y_value, r);
        } else{ //都不是立即数
            load_val(x,is_x_global,x_value,"$t0");
            load_val(y,is_y_global,y_value,"$t1");
            fprintf(fout,"bne $t1 $t0 %s\n",r);
        }
    }

}

void bge_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_x_int;
    int is_x_global = 0;
    int x_value;

    strcpy(x,quad.x);
    strcpy(r,quad.r);

    x_value = get_val(x, &is_x_int, &is_x_global);

    //载入x, y 并将x*y结果存入$t2
    if(is_x_int){   //都是立即数
        if(x_value >= 0){
            fprintf(fout,"j %s\n", r);
        }
    } else{
        load_val(x,is_x_global,x_value,"$t0");
        fprintf(fout,"bgez $t0 %s\n",r);
    }

}

void bgt_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_x_int;
    int is_x_global = 0;
    int x_value;

    strcpy(x,quad.x);
    strcpy(r,quad.r);

    x_value = get_val(x, &is_x_int, &is_x_global);

    //载入x, y 并将x*y结果存入$t2
    if(is_x_int){   //都是立即数
        if(x_value > 0){
            fprintf(fout,"j %s\n", r);
        }
    } else{
        load_val(x,is_x_global,x_value,"$t0");
        fprintf(fout,"bgtz $t0 %s\n",r);
    }

}

void ble_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_x_int;
    int is_x_global = 0;
    int x_value;

    strcpy(x,quad.x);
    strcpy(r,quad.r);

    x_value = get_val(x, &is_x_int, &is_x_global);

    //载入x, y 并将x*y结果存入$t2
    if(is_x_int){   //都是立即数
        if(x_value <= 0){
            fprintf(fout,"j %s\n", r);
        }
    } else{
        load_val(x,is_x_global,x_value,"$t0");
        fprintf(fout,"blez $t0 %s\n",r);
    }

}

void blt_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_x_int;
    int is_x_global = 0;
    int x_value;

    strcpy(x,quad.x);
    strcpy(r,quad.r);

    x_value = get_val(x, &is_x_int, &is_x_global);

    //载入x, y 并将x*y结果存入$t2
    if(is_x_int){   //都是立即数
        if(x_value < 0){
            fprintf(fout,"j %s\n", r);
        }
    } else{
        load_val(x,is_x_global,x_value,"$t0");
        fprintf(fout,"bltz $t0 %s\n",r);
    }

}

void jump_gen(quadruples quad){
    char r[MAX_ID_LENTH];

    strcpy(r,quad.r);

    fprintf(fout,"j %s\n", r);
}

void label_gen(quadruples quad){
    fprintf(fout,"%s:\n",quad.r);
}

void rar_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_y_int, is_r_int;
    int is_y_global = 0, is_r_global = 0;
    int y_value, r_value;
    int i, x_position;

    strcpy(x,quad.x);
    strcpy(y,quad.y);
    strcpy(r,quad.r);

    y_value = get_val(y, &is_y_int, &is_y_global);
    r_value = get_val(r, &is_r_int, &is_r_global);

    i = find_local(x);
    x_position = local_table[i].position;   //找到数组x在内存中的地址

    //将x[y]存在$t1
    if(is_y_int){   //y是立即数
        fprintf(fout,"lw $t1 %d($fp)\n",-y_value*4+x_position);
    } else{
        load_val(y,is_y_global,y_value,"$t0");
        fprintf(fout,"sll $t0 $t0 2\n");    //y = y*4
        fprintf(fout,"sub $t0 $fp $t0\n");
        fprintf(fout,"lw $t1 %d($t0)\n",x_position);
    }

    store_val(r, is_r_global, r_value, "$t1");

}

void war_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_y_int, is_r_int;
    int is_y_global = 0, is_r_global = 0;
    int y_value, r_value;
    int i, x_position;

    strcpy(x,quad.x);
    strcpy(y,quad.y);
    strcpy(r,quad.r);

    y_value = get_val(y, &is_y_int, &is_y_global);
    r_value = get_val(r, &is_r_int, &is_r_global);

    i = find_local(x);
    x_position = local_table[i].position;   //找到数组x在内存中的地址

    //将r的值存在$t1
    if(is_r_int){   //r是常数
        fprintf(fout,"li $t1 %d\n",r_value);
    } else{
        load_val(r,is_r_global,r_value,"$t1");
    }

    //将$t1存在x[y]
    if(is_y_int){   //y是立即数
        fprintf(fout,"sw $t1 %d($fp)\n",-y_value*4+x_position);
    } else{
        load_val(y,is_y_global,y_value,"$t0");
        fprintf(fout,"sll $t0 $t0 2\n");    //y = y*4
        fprintf(fout,"sub $t0 $fp $t0\n");
        fprintf(fout,"sw $t1 %d($t0)\n",x_position);
    }

}

void mov_gen(quadruples quad){
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_y_int, is_r_int;
    int is_y_global = 0, is_r_global = 0;
    int y_value, r_value;

    strcpy(y,quad.y);
    strcpy(r,quad.r);

    y_value = get_val(y, &is_y_int, &is_y_global);
    r_value = get_val(r, &is_r_int, &is_r_global);

    //将y的值放到$t0
    if(is_y_int){   //y是立即数
        fprintf(fout,"li $t0 %d\n",y_value);
    } else{
        load_val(y,is_y_global,y_value,"$t0");
    }

    store_val(r, is_r_global, r_value, "$t0");
}

void neg_gen(quadruples quad){
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_y_int, is_r_int;
    int is_y_global = 0, is_r_global = 0;
    int y_value, r_value;

    strcpy(y,quad.y);
    strcpy(r,quad.r);

    y_value = get_val(y, &is_y_int, &is_y_global);
    r_value = get_val(r, &is_r_int, &is_r_global);

    //将-y的值放到$t0
    if(is_y_int){   //y是立即数
        fprintf(fout,"li $t0 %d\n",-y_value);
    } else{
        load_val(y,is_y_global,y_value,"$t0");
        fprintf(fout,"neg $t0 $t0\n");
    }

    store_val(r, is_r_global, r_value, "$t0");
}

void print_gen(quadruples quad){
    char r[MAX_ID_LENTH];
    int is_r_int;
    int is_r_global = 0;
    int r_value;

    strcpy(r,quad.r);

    r_value = get_val(r, &is_r_int, &is_r_global);

    if(quad.op == PRINTI){   //写整数
        fprintf(fout,"li $v0 1\n");
    }
    else if(quad.op == PRINTC || quad.op == PRINTLN){ //写字符
        fprintf(fout,"li $v0 11\n");
    }
    else if(quad.op == PRINTS){   //写字符串
        fprintf(fout,"li $v0 4\n");
    }

    if(quad.op == PRINTI || quad.op == PRINTC){
        if(is_r_int){   //r为立即数
            fprintf(fout,"li $a0 %d\n",r_value);
        } else{
            load_val(r,is_r_global,r_value,"$a0");
        }
    }
    else if(quad.op == PRINTS){ //写字符串
        fprintf(fout,"la $a0 $$$\n");
        if(is_r_int){   //r为立即数
            fprintf(fout,"addi $a0 $a0 %d\n",r_value);
        } else{
            load_val(r,is_r_global,r_value,"$t0");
            fprintf(fout,"addi $a0 $a0 $t0\n");
        }
    }
    else{   //回车
        fprintf(fout,"li $a0 %d\n",'\n');
    }

    fprintf(fout,"syscall\n");

}

void scan_gen(quadruples quad){
    char r[MAX_ID_LENTH];
    int is_r_int;
    int is_r_global = 0;
    int r_value;

    strcpy(r,quad.r);

    r_value = get_val(r, &is_r_int, &is_r_global);

    if(quad.op == SCANI){   //读整数
        fprintf(fout,"li $v0 5\n");
    } else{ //读字符
        fprintf(fout,"li $v0 12\n");
    }
    fprintf(fout,"syscall\n");

    store_val(r, is_r_global, r_value, "$v0");

}

void para_gen(quadruples quad){
    char x[MAX_ID_LENTH];
    char y[MAX_ID_LENTH];
    char r[MAX_ID_LENTH];
    int is_y_int;
    int is_y_global = 0;
    int y_value;
    int para_ord;   //是第几个参数
    int para_num;   //函数参数个数
    int i;

    strcpy(x,quad.x);
    strcpy(y,quad.y);
    strcpy(r,quad.r);

    y_value = get_val(y, &is_y_int, &is_y_global);

    sscanf(x,"%d",&para_ord);
    if(para_ord<4){
        if(is_y_int){   //y是立即数
            fprintf(fout,"li $a%d %d\n",para_ord,y_value);
        } else{
            load_val(y,is_y_global,y_value,"$t0");
            fprintf(fout,"move $a%d $t0\n",para_ord);
        }
    } else{
        i = find_global(r);
        i = table[i].value;
        para_num = fun_table[i].para_num;

        if(is_y_int){   //y是立即数
            fprintf(fout,"li $t0 %d\n",y_value);
        } else{
            load_val(y,is_y_global,y_value,"$t0");
        }
        fprintf(fout,"sw $t0 %d($sp)\n",-(para_num-para_ord)*4);
    }

}

void call_gen(quadruples quad){
    char r[MAX_ID_LENTH];
    int para_num;
    int i;

    strcpy(r,quad.r);

    i = find_global(r);
    i = table[i].value;
    para_num = fun_table[i].para_num;

    if(para_num>4){
        fprintf(fout,"addi $sp $sp %d\n", -4*(para_num-4));
    }
    fprintf(fout, "jal %s\n",r);
    if(para_num>4){
        fprintf(fout,"addi $sp $sp %d\n", 4*(para_num-4));
    }

}

void return_gen(quadruples quad){
    char r[MAX_ID_LENTH];
    int is_r_int;
    int is_r_global = 0;
    int r_value;
    int i;

    strcpy(r,quad.r);

    //存返回值
    if(r[0]!='\0') {
        r_value = get_val(r, &is_r_int, &is_r_global);
        if(is_r_int){
            fprintf(fout,"li $v0 %d\n",r_value);
        } else{
            load_val(r,is_r_global,r_value,"$v0");
        }
    }

    //函数返回
    fprintf(fout,"move $sp $fp\n");
    for(i = 0;i<=7;i++){
        fprintf(fout,"lw $s%d %d($sp)\n",i,-(5+i)*4);
        offset -=4;
    }
    fprintf(fout,"lw $ra %d($sp)\n",-52);
    fprintf(fout,"lw $fp %d($sp)\n",-56);
    fprintf(fout,"jr $ra\n");

}

void retval_gen(quadruples quad){
    char r[MAX_ID_LENTH];
    int is_r_int;
    int is_r_global = 0;
    int r_value;

    strcpy(r,quad.r);

    r_value = get_val(r, &is_r_int, &is_r_global);

    store_val(r, is_r_global, r_value, "$v0");

}

void initialize(){
    int i;
    char name[MAX_ID_LENTH];

    fprintf(fout,".data\n");
    //给全局变量分配空间
    for(i=0;table[i].ca!=FUNCTION;i++){
        if(table[i].ca == CONST){
            continue;
        }

        strcpy(name,table[i].name);
        if(table[i].lenth==0){
            fprintf(fout,"%s: .space %d\n",name, 4);
        } else{
            fprintf(fout,"%s: .space %d\n",name, table[i].lenth*4);
        }
    }

    if(strp!=0) {
        //给字符串常量分配空间
        fprintf(fout, "$$$: .asciiz \"");
        for (i = 0; i < strp; i++) {
            if (string_table[i] == '\0') {
                fprintf(fout, "\\0");
            }
            else if(string_table[i] == '\\'){
                fprintf(fout, "\\\\");
            }
            else {
                fprintf(fout, "%c", string_table[i]);
            }
        }
        fprintf(fout, "\"\n");
    }

    fprintf(fout, "\n.text\nj main\n");
}

void fun_initialize(fun_table_item fun){
    //每个函数前的压栈等等预操作

    int i,j;
    int fptr;    //函数在符号表的位置
    int para_offset;    //参数的偏移
    int tempval_begin;  //临时变量在局部变量表中的起始地址
    quadruples temp_code;

    fprintf(fout,"\n%s:\n",fun.name);

    //3个参数压栈
    for(i = 3;i>=0;i--){
        fprintf(fout,"sw $a%d %d($sp)\n",i,offset);
        offset -=4;
    }

    //压栈需要保存的寄存器
    for(i = 0;i<=7;i++){
        fprintf(fout,"sw $s%d %d($sp)\n",i,offset);
        offset -=4;
    }
    fprintf(fout,"sw $ra %d($sp)\n",offset);
    fprintf(fout,"sw $fp %d($sp)\n",offset-4);
    offset -=8;

    //初始化局部变量表
    fptr = find_global(fun.name);   //找到函数名在符号表中的位置
    para_offset = -16;  //第一个参数偏移为-16
    //先初始化参数和局部变量的地址
    for(i = fptr+1; table[i].ca!=FUNCTION && i<tp;i++){
        if(table[i].ca == PARA){
            //分配参数的位置
            enter_local(table[i].name,para_offset);
            para_offset = para_offset+4;
        }
        else if(table[i].ca == CONST){
            continue;
        }
        else{
            enter_local(table[i].name,offset);
            if(table[i].lenth == 0){
                offset -= 4;
            } else{
                offset -= table[i].lenth*4;
            }
        }
    }
    tempval_begin = ltp;
    //在初始化临时变量的地址
    for(i = fun.begin;i<=fun.end;i++){
        temp_code = quad_codes[i];
        if(temp_code.r[0]=='#'){
            for(j = tempval_begin;j<ltp;j++){
                if(strcmp(temp_code.r, local_table[j].name) == 0){
                    break;
                }
            }
            if(j == ltp){
                enter_local(temp_code.r,offset);
                offset -= 4;
            }
        }
        if(temp_code.x[0]=='#'){
            for(j = tempval_begin;j<ltp;j++){
                if(strcmp(temp_code.x, local_table[j].name) == 0){
                    break;
                }
            }
            if(j == ltp){
                enter_local(temp_code.x,offset);
                offset -= 4;
            }
        }
        if(temp_code.y[0]=='#'){
            for(j = tempval_begin;j<ltp;j++){
                if(strcmp(temp_code.y, local_table[j].name) == 0){
                    break;
                }
            }
            if(j == ltp){
                enter_local(temp_code.y,offset);
                offset -= 4;
            }
        }
    }

    fprintf(fout, "move $fp $sp\n");
    fprintf(fout, "addiu $sp $sp %d\n\n",offset+4);


}

void mips_gen(){
    int i,j;
    quadruples quad;
    fun_table_item fun; //被处理的函数

    initialize();

    for(i=0;i<funp;i++) {

        offset = -4;     //第一个栈空间偏移为-4
        ltp = 0;    //清空局部变量表
        fun = fun_table[i];
        fun_initialize(fun);
        print_local_table();

        for (j = fun.begin; j <= fun.end; j++) {
            quad = quad_codes[j];
            switch (quad.op) {
                case NONEOP: {
                    break;
                }
                case ADD: {
                    add_gen(quad);
                    break;
                }
                case SUB: {
                    sub_gen(quad);
                    break;
                }
                case MUL: {
                    mul_gen(quad);
                    break;
                }
                case DIV: {
                    div_gen(quad);
                    break;
                }
                case BEQ: {
                    beq_gen(quad);
                    break;
                }
                case BNE: {
                    bne_gen(quad);
                    break;
                }
                case BGE: {
                    bge_gen(quad);
                    break;
                }
                case BGT: {
                    bgt_gen(quad);
                    break;
                }
                case BLE: {
                    ble_gen(quad);
                    break;
                }
                case BLT: {
                    blt_gen(quad);
                    break;
                }
                case JUMP: {
                    jump_gen(quad);
                    break;
                }
                case LABEL: {
                    label_gen(quad);
                    break;
                }
                case RAR: {
                    rar_gen(quad);
                    break;
                }
                case WAR: {
                    war_gen(quad);
                    break;
                }
                case MOV: {
                    mov_gen(quad);
                    break;
                }
                case NEG: {
                    neg_gen(quad);
                    break;
                }
                case PRINTS:
                case PRINTI:
                case PRINTC:
                case PRINTLN: {
                    print_gen(quad);
                    break;
                }
                case SCANI:
                case SCANC:{
                    scan_gen(quad);
                    break;
                }
                case PARAIN: {
                    para_gen(quad);
                    break;
                }
                case CALL: {
                    call_gen(quad);
                    break;
                }
                case RETURN: {
                    return_gen(quad);
                    break;
                }
                case RETVAL: {
                    retval_gen(quad);
                    break;
                }
            }
        }

    }
}

void print_local_table(){
    int i;
    local_val t;

    printf("\n%-15s\t%-5s\n","name","position");
    for(i=0;i<ltp;i++){
        t = local_table[i];
        printf("%-15s\t%-5d\n",
               t.name,t.position);
    }
}