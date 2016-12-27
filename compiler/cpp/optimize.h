//
// Created by liuzheng on 2016/12/1.
//

#ifndef COMPILER_OPTIMIZE_H
#define COMPILER_OPTIMIZE_H

#include <vector>
#include <set>
#include <map>
#include <string>
#include <string.h>
#include "table.h"

#define MAX_DAG_NODE 1024

typedef struct {
    std::vector<int> use;
    std::vector<int> def;
} use_def;

typedef struct {
    int begin;   //基本块的第一条语句（入口语句）
    int end;     //基本块的最后一条语句
    std::map<std::string,use_def> var_use_def;
    std::set<std::string> use;
    std::set<std::string> def;
    std::set<std::string> in;
    std::set<std::string> out;
    std::set<int> follow;
} bblock;

typedef struct {
    quadop op;
    int left;
    int right;
    type typ;
    std::set<std::string> var;
} dag_node;

void sreg_alloc(int fp);

void gen_block();
void gen_dag(bblock &block);

std::map<std::string,use_def> gen_use_def();

void print_block();
void print_dag();
void print_optcodes();
void print_optfuncode();

#endif //COMPILER_OPTIMIZE_H
