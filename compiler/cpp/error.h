//
// Created by liuzheng on 2016/11/14.
//

#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <set>
#include "lexical.h"

void error(int n);
void fatal_error();
void skip(std::set<symbol> skipset);

#endif //COMPILER_ERROR_H
