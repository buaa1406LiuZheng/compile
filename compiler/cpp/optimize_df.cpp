//
// Created by liuzheng on 2016/12/23.
//

#include "optimize.h"

extern std::vector<quadruples> opt_codes;  //优化后的四元式
extern std::vector<bblock> basic_blocks;   //所有的基本块

void add_use(std::map<std::string,use_def> &vartab, std::string id, int usep){
    if(!(id[0]>='0' && id[0]<='9')){    //不是数字
        vartab[id].use.push_back(usep);
    }
}

void add_def(std::map<std::string,use_def> &vartab, std::string id, int defp){
    vartab[id].def.push_back(defp);
}

std::map<std::string,use_def> gen_use_def(){
    std::map<std::string,use_def> vartab;
    for(int i=0;i<opt_codes.size();i++){
        quadruples quad = opt_codes[i];
        switch (quad.op) {
            case NONEOP: {
                break;
            }
            case ADD: case SUB: case MUL: case DIV:{
                add_use(vartab,quad.x,i);
                add_use(vartab,quad.y,i);
                add_def(vartab,quad.r,i);
                break;
            }
            case NEG: {
                add_use(vartab,quad.y,i);
                add_def(vartab,quad.r,i);
                break;
            }
            case MOV:{
                add_use(vartab,quad.y,i);
                add_def(vartab,quad.r,i);
                break;
            }
            case RAR:{
                add_use(vartab,quad.y,i);
                add_def(vartab,quad.r,i);
                break;
            }
            case WAR:{
                add_use(vartab,quad.r,i);
                add_use(vartab,quad.y,i);
                break;
            }
            case RETVAL:{
                add_def(vartab,quad.r,i);
                break;
            }
            case PARAIN:{
                add_use(vartab,quad.y,i);
                break;
            }
            case BEQ: case BNE:{
                add_use(vartab,quad.x,i);
                add_use(vartab,quad.y,i);
                break;
            }
            case BGE:case BGT: case BLE: case BLT:{
                add_use(vartab,quad.x,i);
                break;
            }
            case JUMP: {
                break;
            }
            case LABEL: {
                break;
            }
            case PRINTS: case PRINTI:
            case PRINTC:{
                add_use(vartab,quad.r,i);
                break;
            }
            case PRINTLN: {
                break;
            }
            case SCANI: case SCANC: {
                add_def(vartab,quad.r,i);
                break;
            }
            case CALL: {
                break;
            }
            case RETURN: {
                if((quad.r)[0]!='\0'){  //无返回值
                    add_use(vartab,quad.r,i);
                }
                break;
            }
        }
    }

    return vartab;
}

int find_min(std::vector<int> v){

    if(v.size() == 0){
        return -1;
    }

    int min = v[0];
    for(int i=0;i<v.size();i++){
        if(v[i]<min){
            min = v[i];
        }
    }
    return min;
}
void active_var_analyze(){
    //活跃变量分析

    //先生成每个块的use集和def集
    for(int i=0;i<basic_blocks.size();i++){
        std::map<std::string,use_def> var_use_def = basic_blocks[i].var_use_def;
        std::map<std::string,use_def>::iterator it;
        for(it = var_use_def.begin();it!=var_use_def.end();it++){
            int min_usep = find_min((it->second).use);
            int min_defp = find_min((it->second).def);
            if(min_usep == -1 && min_defp == -1){   //无定义无使用
                continue;
            } else if(min_usep == -1){  //无使用有定义
                basic_blocks[i].def.insert(it->first);
            } else if(min_defp == -1){  //无定义有使用
                basic_blocks[i].use.insert(it->first);
            } else{ //有使用有定义
                if(min_defp<min_usep){  //定义在使用之前
                    basic_blocks[i].def.insert(it->first);
                } else{  //定义在使用之后（或同一条四元式中）
                    basic_blocks[i].use.insert(it->first);
                }
            }
        }
    }
}