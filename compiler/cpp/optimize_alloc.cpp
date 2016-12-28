//
// Created by liuzheng on 2016/12/27.
//

#include <stack>
#include "optimize.h"

#define MAX_REG_NUM 8

extern table_item table[MAX_TABLE_LENTH];  //符号表
extern int tp; //符号表头指针

extern std::map<std::string, std::set<std::string>> conflict_graph;    //冲突图

void sreg_alloc(int fp){
    //fp为待分配函数在符号表中的位置
    int i;
    int reg_num = 0;

    for(i = fp+1;table[i].ca!=FUNCTION && i<tp;i++){
        if(reg_num<MAX_REG_NUM){
            if(table[i].ca==VAR && table[i].lenth==0){
                //符号表中变量的value域置为被分配的全局寄存器，为正值
                reg_num++;
                table[i].value = reg_num;
            }
        } else{
            break;
        }
    }
}

int find_node(std::map<std::string, std::set<std::string>> color_graph, std::string &node){
    //找有没有节点数小于MAX_REG_NUM的点，有返回1，将节点保存在node中；没有返回0，将边数最大点保存在node

    std::map<std::string, std::set<std::string>>::iterator git;
    int max_edge = -1;
    std::string max_edge_node;
    for(git = color_graph.begin();git!=color_graph.end();git++){
        int edge_num = (int)(git->second).size();
        if(edge_num<MAX_REG_NUM){
            node = git->first;
            return 1;
        } else{
            if(edge_num>max_edge){
                max_edge = edge_num;
                max_edge_node = git->first;
            }
        }
    }

    node = max_edge_node;
    return 0;
}

void remove_node(std::map<std::string, std::set<std::string>> &color_graph, std::string node){
    //在冲突图中删掉结点node
    std::set<std::string> neighbor = color_graph[node]; //此节点的邻居节点
    std::set<std::string>::iterator nit;

    //从邻居节点中将此节点删除
    for(nit = neighbor.begin();nit!=neighbor.end();nit++){
        color_graph[*nit].erase(node);
    }
    color_graph.erase(node);
}

void alloc_reg(std::map<std::string, int> &reg_alloc_tab, std::string id){
    int reg_num;
    for(reg_num=1;reg_num<=MAX_REG_NUM;reg_num++){
        std::set<std::string> neighbor = conflict_graph[id];
        std::set<std::string>::iterator nit;

        //找邻居节点是否分配了这号寄存器
        for(nit = neighbor.begin();nit!=neighbor.end();nit++){
            if(reg_alloc_tab.find(*nit)==reg_alloc_tab.end()){  //此邻居还没有被分配寄存器
                continue;
            }
            if(reg_alloc_tab[*nit]==reg_num){   //这号寄存器已被某个邻居节点分配
                break;
            }
        }
        if(nit==neighbor.end()){    //这号寄存器没有被邻居节点分配
            reg_alloc_tab[id]=reg_num;
            return;
        }
    }
}


void sreg_alloc_opt(int fp){
    //fp为待分配函数在符号表中的位置
    gen_conflict_graph();
    print_conflict_graph();

    std::map<std::string, std::set<std::string>> color_graph = conflict_graph;  //用于图着色的冲突图副本

    std::string node;
    std::stack<std::string> alloc_stack;    //被分配寄存器的变量栈

    while(!color_graph.empty()){    //着色图中还有节点
        if(find_node(color_graph,node)){
            alloc_stack.push(node);
        }
        remove_node(color_graph,node);
    }

    std::map<std::string, int> reg_alloc_tab;   //寄存器分配表
    while(!alloc_stack.empty()){
        std::string id;
        id = alloc_stack.top();
        alloc_stack.pop();

        alloc_reg(reg_alloc_tab,id);
    }

    for(int i = fp+1;table[i].ca!=FUNCTION && i<tp;i++){
        std::string id = table[i].name;
        if(reg_alloc_tab.find(id) != reg_alloc_tab.end()){  //此变量被分配了寄存器
            table[i].value = reg_alloc_tab[id];
        }
    }
}

void print_conflict_graph(){
    std::map<std::string, std::set<std::string>>::iterator cgit;
    for(cgit=conflict_graph.begin();cgit!=conflict_graph.end();cgit++){
        printf("%s:",(cgit->first).c_str());
        std::set<std::string> neighbor = cgit->second;
        std::set<std::string>::iterator nit;
        for(nit = neighbor.begin();nit!=neighbor.end();nit++){
            printf("%s,",(*nit).c_str());
        }
        printf("\n");
    }
}