//
// Created by liuzheng on 2016/12/1.
//

#include "optimize.h"

extern table_item table[MAX_TABLE_LENTH];  //符号表
extern quadruples quad_codes[MAX_CODES_LENTH];  //四元式代码表

extern int tp; //符号表头指针

extern fun_table_item fun; //正在被处理的函数

extern std::vector<bblock> basic_blocks;   //所有的基本块

void gen_block(){

    std::map<std::string,std::vector<int>> jump_position_table; //跳转表
    std::map<std::string,int> label_table; //标签位置表
    int i;
    bblock current;  //当前块
    quadruples quad;    //当前四元式

    int new_block = 0;

    basic_blocks.clear();

    current.begin = fun.begin;
    current.follow.clear();

    for(i=fun.begin;i<=fun.end;i++){
        quad = quad_codes[i];
        if(new_block){
            if(quad.op!=LABEL) {
                current.end = i - 1;
                if(quad_codes[i-1].op!=JUMP && quad_codes[i-1].op!=RETURN){ //上一条不是无条件跳转
                    current.follow.insert((int)basic_blocks.size()+1);   //后继基本块为下一块
                }
                basic_blocks.push_back(current);

                current.begin = i;  //下一个基本块的开始
                current.follow.clear();

            }
            new_block = 0;
        }

        switch (quad.op) {
            case NONEOP: case ADD: case SUB: case MUL: case DIV:
            case MOV: case NEG: case RAR: case WAR:
            case RETVAL: case PARAIN:{
                break;
            }
            case BEQ: case BNE: case BGE:
            case BGT: case BLE: case BLT:
            case JUMP: {
                std::map<std::string,int>::iterator it;
                it = label_table.find(quad.r);
                if(it==label_table.end()){  //还没有生成到跳转的位置
                    jump_position_table[quad.r].push_back((int)basic_blocks.size());    //在跳转表加入此块的目标位置
                } else{ //后继基本块设为跳转位置
                    current.follow.insert(it->second);
                }

                new_block = 1;  //下一个四元式将在新的基本块中
                break;
            }
            case LABEL: {   //跳转的目标位置属于入口语句
                if(i!=fun.begin) {
                    current.end = i - 1;    //上一个块的最后一条四元式是跳转的前一条
                    if (quad_codes[i - 1].op != JUMP && quad_codes[i - 1].op != RETURN) { //上一条不是无条件跳转
                        current.follow.insert((int) basic_blocks.size() + 1);   //后继基本块为下一块
                    }
                    basic_blocks.push_back(current);
                }

                std::vector<int> jump_position = jump_position_table[quad.r];
                std::vector<int>::iterator it;
                for(it = jump_position.begin();it!=jump_position.end();it++){
                    basic_blocks[*it].follow.insert((int)basic_blocks.size());
                }

                current.begin = i;  //新的块
                current.follow.clear();
                label_table[quad.r] = (int)basic_blocks.size();
                break;
            }
            case PRINTS: case PRINTI:
            case PRINTC: case PRINTLN: {
                break;
            }
            case SCANI: case SCANC: {
                break;
            }
            case CALL: {
                break;
            }
            case RETURN: {  //返回语句属于跳转
                current.follow.insert(-1);  //-1代表结束块
                new_block = 1;  //下一个四元式将在新的基本块中
                break;
            }
        }
    }
    current.end = i-1;
    current.follow.insert(-1);  //-1代表结束块
    basic_blocks.push_back(current);

    return;
}

void print_block(){
    printf("%-5s\t%-5s\t%-5s\tfollow\n","ord","begin","end");
    for(int i=0;i<basic_blocks.size();i++){
        bblock block = basic_blocks[i];
        printf("%-5d\t%-5d\t%-5d\t",i,block.begin,block.end);
        for(std::set<int>::iterator j = block.follow.begin();j!=block.follow.end();j++){
            printf("%d ",*j);
        }
        printf("\n");
    }
}