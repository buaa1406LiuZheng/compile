//
// Created by liuzheng on 2016/12/23.
//

#include "optimize.h"

extern std::vector<quadruples> opt_codes;  //优化后的四元式
extern std::vector<bblock> basic_blocks;   //所有的基本块
extern std::map<std::string, std::set<std::string>> conflict_graph;    //冲突图

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

int find_min(std::vector<int> v, int n){
    //找v中大于等于n的最小整数，找不到返回-1
    if(v.size() == 0){
        return -1;
    }

    int i,min=-1;
    for(i=0;i<v.size();i++) {
        if (v[i]>=n) {
            min = v[i];
            break;
        }
    }
    for(;i<v.size();i++){
        if(v[i]>=n && v[i]<min){
            min = v[i];
        }
    }
    return min;
}

std::set<std::string> Union(std::set<std::string> A,std::set<std::string> B){
    std::set<std::string>::iterator it;
    std::set<std::string> result;

    for(it = A.begin();it!=A.end();it++){
        result.insert(*it);
    }
    for(it = B.begin();it!=B.end();it++){
        result.insert(*it);
    }

    return result;
}
std::set<std::string> Difference(std::set<std::string> A,std::set<std::string> B){
    std::set<std::string>::iterator it;
    std::set<std::string> result;

    for(it = A.begin();it!=A.end();it++){
        if(B.find(*it) == B.end()){
            result.insert(*it);
        }
    }

    return result;
}

void active_var_analyze(){
    //活跃变量分析

    //先生成每个块的use集和def集
    for(int i=0;i<basic_blocks.size();i++){
        std::map<std::string,use_def> var_use_def = basic_blocks[i].var_use_def;
        std::map<std::string,use_def>::iterator it;
        for(it = var_use_def.begin();it!=var_use_def.end();it++){
            int min_usep = find_min((it->second).use,0);
            int min_defp = find_min((it->second).def,0);
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

    //活跃变量分析
    std::vector<int> is_inout_change;   //记录各个块的in，out集是否改变
    int has_change; //是否有块的in、out集改变
    for(int i=0;i<basic_blocks.size();i++){
        is_inout_change.push_back(1);
    }

    do{
        for(int i=(int)basic_blocks.size()-1;i>=0;i--){
            std::set<std::string> in;
            std::set<std::string> out;
            bblock block=basic_blocks[i];

            //求解out集
            std::set<int>::iterator it;
            for(it = block.follow.begin();it!=block.follow.end();it++){
                if(*it == -1){  //后继块为出口
                    continue;
                } else{ //后继块不为出口，并上此后继块的in集
                    out = Union(out,basic_blocks[*it].in);
                }
            }
            //求解in集合
            std::set<std::string> tmp;
            tmp = Difference(out,block.def);
            in = Union(block.use,tmp);

            if(block.in.size()==in.size() &&
               block.out.size()==out.size()){
                is_inout_change[i] = 0;
            } else {
                basic_blocks[i].in = in;
                basic_blocks[i].out = out;
            }
        }

        has_change = 0;
        for(int i=0;i<basic_blocks.size();i++){
            if(is_inout_change[i]){
                has_change = 1;
                break;
            }
        }
        for(int i=0;i<basic_blocks.size();i++){
            is_inout_change[i] = 1;
        }
    }while(has_change);
}

int is_active(std::string id,int i, int p){
    //变量id在基本块i的位置p处是否活跃

    bblock block = basic_blocks[i];
    int min_usep;
    int min_defp;

    min_usep = find_min((block.var_use_def)[id].use,p);
    min_defp = find_min((block.var_use_def)[id].def,p);

    if(min_usep == -1 && min_defp == -1){   //无定义无使用
        if(block.out.find(id) == block.out.end()){  //out集没有这个变量（在出口处也不活跃）
            return 0;
        } else{
            return 1;
        }
    } else if(min_usep == -1){  //无使用有定义
        return 0;
    } else if(min_defp == -1){  //无定义有使用
        return 1;
    } else{ //有使用有定义
        if(min_defp<min_usep){  //定义在使用之前
            return 0;
        } else{  //定义在使用之后（或同一条四元式中）
            return 1;
        }
    }
}

void gen_conflict_graph(){
    active_var_analyze();

    conflict_graph.clear();

    std::set<std::string> varset;   //所有非数组局部变量的集合
    for(int i=0;i<basic_blocks.size();i++){
        //varset中加入所有块中有定义或使用的变量
        std::map<std::string,use_def> vartab = basic_blocks[i].var_use_def;
        std::map<std::string,use_def>::iterator it;
        for(it = vartab.begin();it!=vartab.end();it++){
            varset.insert(it->first);
            conflict_graph[it->first].clear();
        }
    }

    for(int i=0;i<basic_blocks.size();i++){
        //遍历所有块中变量的定义点和使用点查看有无冲突
        std::map<std::string,use_def> vartab = basic_blocks[i].var_use_def;
        std::map<std::string,use_def>::iterator it;
        for(it = vartab.begin();it!=vartab.end();it++){
            std::vector<int> use = (it->second).use;
            std::vector<int> def = (it->second).def;
            int p;

//            for(int j=0;j<use.size();j++){
//                p = use[j];
//                std::set<std::string>::iterator varit;
//                for(varit = varset.begin();varit!=varset.end();varit++){
//                    if(((it->first)!=*varit) &&
//                            is_active(*varit,i,p)){ //两个变量冲突
//                        conflict_graph[it->first].insert(*varit);
//                        conflict_graph[*varit].insert(it->first);
//                    }
//                }
//            }
            for(int j=0;j<def.size();j++){
                p = def[j];
                std::set<std::string>::iterator varit;
                for(varit = varset.begin();varit!=varset.end();varit++){
                    if(((it->first)!=*varit) &&
                       is_active(*varit,i,p)){ //两个变量冲突
                        conflict_graph[it->first].insert(*varit);
                        conflict_graph[*varit].insert(it->first);
                    }
                }
            }
        }
    }

}

void print_inout(){

    for(int i=0;i<basic_blocks.size();i++){
        bblock blcok = basic_blocks[i];
        std::set<std::string>::iterator it;

        printf("%d\n",i);
        printf("in:");
        for(it = blcok.in.begin();it!=blcok.in.end();it++){
            printf("%s,",(*it).c_str());
        }
        printf("\n");

        printf("out:");
        for(it = blcok.out.begin();it!=blcok.out.end();it++){
            printf("%s,",(*it).c_str());
        }
        printf("\n");

        printf("use:");
        for(it = blcok.use.begin();it!=blcok.use.end();it++){
            printf("%s,",(*it).c_str());
        }
        printf("\n");

        printf("def:");
        for(it = blcok.def.begin();it!=blcok.def.end();it++){
            printf("%s,",(*it).c_str());
        }
        printf("\n");
    }
}