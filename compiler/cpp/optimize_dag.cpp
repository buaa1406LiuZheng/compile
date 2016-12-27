//
// Created by liuzheng on 2016/12/23.
//

#include "error.h"
#include "optimize.h"
#include "table.h"

extern table_item table[MAX_TABLE_LENTH];
extern quadruples quad_codes[MAX_CODES_LENTH];  //四元式代码表
extern std::vector<quadruples> opt_codes;  //优化后的四元式

dag_node dag[MAX_DAG_NODE];  //dag节点数组
std::map<std::string,int> dmap;   //dag图节点项目表
int dp;     //dag节点数组尾指针

type get_type(std::string id_str){

    if(id_str[0]=='#'){ //临时变量类型为INT
        return INT;
    }
    if(id_str[0]>='0' && id_str[0]<='9'){   //常数返回INT
        return INT;
    }

    char id[MAX_ID_LENTH];

    strcpy(id,id_str.c_str());
    int p = find(id);
    if(p==-1){
        return NONETYPE;
    }
    return table[p].typ;
}
int new_node(std::string id){

    if(dp>=MAX_DAG_NODE){
        fatal_error();
    }
    dag_node node;
    node.op = NONEOP;
    node.left = -1;
    node.right = -1;
    node.typ = get_type(id);
    node.var.insert(id);
    dag[dp++] = node;
    return dp-1;
}

quadruples newquad(){
    quadruples quad;
    quad.op = NONEOP;
    (quad.x)[0]=0;
    (quad.y)[0]=0;
    (quad.r)[0]=0;
    return quad;
}

void remove_var(int nodep,std::string id){
    dag[nodep].var.erase(id);
}

std::string find_name(std::set<std::string> var){
    if(var.empty()){
        return "";
    }

    std::string name;
    std::set<std::string>::iterator i;
    for(i = var.begin();i!=var.end();i++) {
        name = *i;
        if (name[0] != '#') {   //非临时变量
            return name;
        }
    }
    return *(var.begin());
}

void binop(quadruples quad){
    //普通双目操作符(+ - * /)，读数组，写数组，双目branch
    std::map<std::string,int>::iterator itx,ity,itr;
    int xp,yp,rp;
    int findx,findy,findr;
    std::string x,y,r;
    std::string qx,qy,qr="";

    if(quad.op == WAR){
        r = quad.x;
        x = quad.y;
        y = quad.r;
    } else {
        x = quad.x;
        y = quad.y;
        r = quad.r;
    }

    findx = findy = findr = 0;
    rp = -1;

    //找子节点
    itx = dmap.find(x);
    if(itx==dmap.end()){ //原项目表中没有
        xp = new_node(x);    //产生一个新节点
        dmap[x] = xp;   //更新项目表
    } else{
        findx = 1;  //找到节点
        xp = itx->second;    //在项目表中的位置
    }
    qx = find_name(dag[xp].var);

    ity = dmap.find(y);
    if(ity==dmap.end()){ //原项目表中没有
        yp = new_node(y);    //产生一个新节点
        dmap[y] = yp;   //更新项目表
    } else{
        findy = 1;
        yp = ity->second;    //在项目表中的位置
    }
    qy = find_name(dag[yp].var);

    //找有没有相同节点
    if(findx && findy){
        for(int t=0;t<dp;t++){
            dag_node tmp = dag[t];
            if(quad.op == ADD || quad.op == MUL ||
               quad.op == BEQ || quad.op == BNE) {  //加法乘法、判断是否相等可以有交换律
                if (tmp.op == quad.op &&
                        ((tmp.left == xp && tmp.right == yp)||
                        (tmp.left == yp && tmp.right == xp)) ) {
                    findr = 1;
                    rp = t;
                }
            } else{
                if (tmp.op == quad.op &&
                        (tmp.left == xp && tmp.right == yp)) {
                    findr = 1;
                    rp = t;
                }
            }
        }
    }

    //生成r的节点
    if(findr && dag[rp].typ == get_type(r)){  //找到r的节点，且类型正确
        qr = find_name(dag[rp].var);    //找原来节点名字
        dag[rp].var.insert(r);  //在节点中的变量列表中插入
    } else{ //没有找到r的节点
        rp = new_node(r);    //创建新节点
        dag[rp].op = quad.op;
        dag[rp].left = xp;
        dag[rp].right = yp;
    }

    //更新r在项目表的位置
    itr = dmap.find(r);
    if(itr == dmap.end()){  //原来项目表中没有r
        dmap[r] = rp;   //更新项目表，加入r
    } else{ //原来项目表中有r
        int old_rp = dmap[r];
        dmap[r] = rp;
        remove_var(old_rp,r);  //在老节点中的变量列表中删除
    }

    //生成新四元式
    quadruples new_quad = newquad();
    if(quad.op == WAR){   //是跳转或写数组
        new_quad = quad;
    } else{ //其他为赋值类型操作
        if(!qr.empty() &&
                !(quad.op == BEQ || quad.op == BNE)){   //找到了相同值的节点（公共表达式）
            new_quad.op = MOV;
            strcpy(new_quad.r,r.c_str());
            strcpy(new_quad.y,qr.c_str());
        } else{
            new_quad.op = quad.op;
            strcpy(new_quad.x,qx.c_str());
            strcpy(new_quad.y,qy.c_str());
            strcpy(new_quad.r,r.c_str());
        }
    }
    opt_codes.push_back(new_quad);

}

void uniop(quadruples quad){
    //普通单目操作符(取负)，单目branch

    std::map<std::string,int>::iterator ity,itr;
    int yp,rp;
    int findy,findr;
    std::string y,r;
    std::string qy,qr="";

    if(quad.op == BGE || quad.op == BGT
       || quad.op == BLE || quad.op == BLT){
        y = quad.x;
        r = quad.r;
    } else {
        y = quad.y;
        r = quad.r;
    }

    findy = findr = 0;
    rp = -1;

    //找子节点
    ity = dmap.find(y);
    if(ity==dmap.end()){ //原项目表中没有
        yp = new_node(y);    //产生一个新节点
        dmap[y] = yp;   //更新项目表

    } else{
        yp = ity->second;    //在项目表中的位置
        findy = 1;
    }
    qy = find_name(dag[yp].var);

    //找有没有相同节点
    if(findy){
        for(int t=0;t<dp;t++){
            dag_node tmp = dag[t];
            if(quad.op == tmp.op && tmp.left == yp) {
                findr = 1;
                rp = t;
            }
        }
    }

    //生成r的节点
    if(findr && dag[rp].typ == get_type(r)){  //找到r的节点
        qr = find_name(dag[rp].var);    //查找原节点的名字
        dag[rp].var.insert(r);  //在节点中的变量列表中插入
    } else{ //没有找到r的节点
        rp = new_node(r);    //创建新节点
        dag[rp].op = quad.op;
        dag[rp].left = yp;
    }

    //更新r在项目表的位置
    itr = dmap.find(r);
    if(itr == dmap.end()){  //原来项目表中没有r
        dmap[r] = rp;   //更新项目表，加入r
    } else{ //原来项目表中有r
        int old_rp = dmap[r];
        dmap[r] = rp;
        remove_var(old_rp,r); //在老节点中的变量列表中删除
    }

    //生成新四元式
    quadruples new_quad = newquad();
    if(!qr.empty() &&
            !(quad.op == BGE || quad.op == BGT
              || quad.op == BLE || quad.op == BLT)){   //找到了相同值的节点（公共表达式）
        new_quad.op = MOV;
        strcpy(new_quad.r,r.c_str());
        strcpy(new_quad.y,qr.c_str());
    } else{
        new_quad.op = quad.op;
        if(quad.op == BGE || quad.op == BGT
           || quad.op == BLE || quad.op == BLT){
            strcpy(new_quad.x, qy.c_str());
        } else {
            strcpy(new_quad.y, qy.c_str());
        }
        strcpy(new_quad.r,r.c_str());
    }
    opt_codes.push_back(new_quad);
}

void assignop(quadruples quad){
    //赋值

    std::map<std::string,int>::iterator ity,itr;
    int yp,rp;
    int findy,findr;
    std::string y,r;
    std::string qy,qr="";

    y = quad.y;
    r = quad.r;

    findr = findy = 0;
    rp = -1;

    //找子节点
    ity = dmap.find(y);
    if(ity==dmap.end()){ //原项目表中没有
        yp = new_node(y);    //产生一个新节点
        dmap[y] = yp;   //更新项目表
    } else{
        yp = ity->second;    //在项目表中的位置
        findy = 1;
    }

    //找有没有相同节点，并生成r的节点
    if(dag[yp].typ == get_type(r)) {    //类型相同
        rp = yp;

        //生成r的节点
        qr = find_name(dag[rp].var);    //查找原节点的名字
        dag[rp].var.insert(r);  //在节点中的变量列表中插入
    } else {

        //找有没有相同节点
        if(findy){
            for(int t=0;t<dp;t++){
                dag_node tmp = dag[t];
                if(quad.op == tmp.op && tmp.left == yp) {
                    findr = 1;
                    rp = t;
                }
            }
        }

        //生成r的节点
        if(findr && dag[rp].typ == get_type(r)){  //找到r的节点
            qr = find_name(dag[rp].var);    //查找原节点的名字
            dag[rp].var.insert(r);  //在节点中的变量列表中插入
        } else{ //没有找到r的节点
            rp = new_node(r);    //创建新节点
            dag[rp].op = quad.op;
            dag[rp].left = yp;
        }
    }

    //更新r在项目表的位置
    itr = dmap.find(r);
    if(itr == dmap.end()){  //原来项目表中没有r
        dmap[r] = rp;   //更新项目表，加入r
    } else{ //原来项目表中有r
        int old_rp = dmap[r];
        dmap[r] = rp;
        remove_var(old_rp,r);  //在老节点中的变量列表中删除
    }

    //生成新四元式
    quadruples new_quad = newquad();
    new_quad.op = quad.op;
    strcpy(new_quad.r,r.c_str());
    if(!qr.empty()){   //找到了相同值的节点（公共表达式）
        strcpy(new_quad.y,qr.c_str());
    } else{
        strcpy(new_quad.y,y.c_str());
    }
    opt_codes.push_back(new_quad);
}

void useop(quadruples quad){
    //print，压参数，函数返回值
    std::map<std::string,int>::iterator itr;
    int rp;
    std::string r;
    std::string qr;

    if(quad.op == PARAIN){
        r = quad.y;
    } else{
        r = quad.r;
    }

    itr = dmap.find(r);
    if(itr==dmap.end()){ //原项目表中没有
        rp = new_node(r);    //产生一个新节点
        dmap[r] = rp;   //更新项目表
    } else{
        rp = itr->second;    //在项目表中的位置
    }
    qr = find_name(dag[rp].var);

    //生成新四元式
    quadruples new_quad = newquad();
    new_quad.op = quad.op;
    if(quad.op == PARAIN){
        strcpy(new_quad.x,quad.x);
        strcpy(new_quad.y,qr.c_str());
        strcpy(new_quad.r,quad.r);
    } else{
        strcpy(new_quad.r,qr.c_str());
    }
    opt_codes.push_back(new_quad);
}

void defop(quadruples quad){
    //scan，取返回值

    std::map<std::string,int>::iterator itr;
    int rp;
    std::string r;

    rp = new_node(r);
    dag[rp].op = quad.op;

    itr = dmap.find(r);
    if(itr == dmap.end()){  //原来项目表中没有r
        dmap[r] = rp;   //更新项目表，加入r
    } else{ //原来项目表中有r
        int old_rp = dmap[r];
        dmap[r] = rp;
        remove_var(old_rp,r);  //在老节点中的变量列表中删除
    }

    //生成新四元式
    opt_codes.push_back(quad);
}

void callop(quadruples quad){
    std::map<std::string,int>::iterator i;
    for(i = dmap.begin();i!=dmap.end();i++){
        std::string id_string = i->first;
        char id[MAX_ID_LENTH];
        strcpy(id,id_string.c_str());
        int p = find(id);
        if(p!=-1 && table[p].isglobal){ //此变量为全局变量，需要清除其在节点表中的记录以及清除其在节点中的记录
            dag[i->second].var.erase(id_string);
            dmap.erase(i);
            i--;
        }
    }

    //生成新四元式
    opt_codes.push_back(quad);
}

void gen_dag(bblock &block){

    opt_codes.clear();
    dmap.clear();
    dp = 0;

    for(int i=block.begin;i<=block.end;i++){
        quadruples quad = quad_codes[i];
        switch (quad.op) {
            case NONEOP: {
                break;
            }
            case ADD: case SUB: case MUL: case DIV:{
                binop(quad);
                break;
            }
            case NEG: {
                uniop(quad);
                break;
            }
            case MOV:{
                assignop(quad);
                break;
            }
            case RAR:{
                binop(quad);
                break;
            }
            case WAR:{
                binop(quad);
                break;
            }
            case RETVAL:{
                defop(quad);
                break;
            }
            case PARAIN:{
                useop(quad);
                break;
            }
            case BEQ: case BNE:{
                binop(quad);
                break;
            }
            case BGE:case BGT: case BLE: case BLT:{
                uniop(quad);
                break;
            }
            case JUMP: {
                opt_codes.push_back(quad);
                break;
            }
            case LABEL: {
                opt_codes.push_back(quad);
                break;
            }
            case PRINTS: case PRINTI:
            case PRINTC:{
                useop(quad);
                break;
            }
            case PRINTLN: {
                opt_codes.push_back(quad);
                break;
            }
            case SCANI: case SCANC: {
                defop(quad);
                break;
            }
            case CALL: {
                callop(quad);
                break;
            }
            case RETURN: {
                if((quad.r)[0]=='\0'){  //无返回值
                    opt_codes.push_back(quad);
                } else{
                    useop(quad);
                }
                break;
            }
        }
    }
    print_optcodes();

    std::map<std::string,use_def> var_use_def = gen_use_def();
    std::map<std::string,use_def>::iterator i;
    for(i = var_use_def.begin();i!=var_use_def.end();i++){
        std::string id = i->first;
        if(id[0]=='#'){ //临时变量
            use_def tmp = i->second;
            if(tmp.use.size()==0){  //临时变量没有使用
                for(int j=0;j<tmp.def.size();j++){
                    int defp = (tmp.def)[j];
                    opt_codes[defp].op = NONEOP;
                    (opt_codes[defp].x)[0] = '\0';
                    (opt_codes[defp].y)[0] = '\0';
                    (opt_codes[defp].r)[0] = '\0';
                }
            }
        }
    }

    for(i = var_use_def.begin();i!=var_use_def.end();i++){
        char id[MAX_ID_LENTH];
        strcpy(id,(i->first).c_str());
        if(id[0]=='#') { //临时变量
            continue;
        } else{
            int p = find(id);
            if(!(table[p].isglobal || table[p].ca == PARA)){
                block.var_use_def[i->first] = i->second;
            }
        }
    }

    for(i = block.var_use_def.begin();i!=block.var_use_def.end();i++){
        printf("%s\n",(i->first).c_str());
        printf("use:");
        for(int j=0;j<(i->second).use.size();j++){
            printf("%d ",((i->second).use)[j]);
        }
        printf("\ndef:");
        for(int j=0;j<(i->second).def.size();j++){
            printf("%d ",((i->second).def)[j]);
        }
        printf("\n");
    }
}

void print_dag(){
    printf("ord\tl\tr\top\n");
    for(int i=0;i<dp;i++){
        dag_node node = dag[i];
        printf("%d\t%d\t%d\t%d\t",i,node.left,node.right,node.op);
        for(std::set<std::string>::iterator j = node.var.begin();
                j!=node.var.end();j++){
            printf("%s ",(*j).c_str());
        }
        printf("\n");
    }

    printf("%-10s\tp\n","name");
    for(std::map<std::string,int>::iterator i = dmap.begin();
            i!=dmap.end();i++){
        printf("%-10s\t%d\n",(i->first).c_str(),i->second);
    }
}

void print_optcodes(){
    const char quarop_string[][20] = {
            "NONEOP", "ADD", "SUB", "MUL", "DIV",
            "BEQ", "BNE", "BGE", "BGT", "BLE", "BLT",
            "JUMP", "LABEL", "RAR", "WAR",
            "MOV", "NEG",
            "PRINTS", "PRINTI", "PRINTC", "PRINTLN",
            "SCANI", "SCANC",
            "PARAIN", "CALL", "RETURN", "RETVAL"
    };

    printf("\nblock after dag optimize:\n");
    printf("%-5s\t%-5s\t%-5s\t%-7s\t%-5s\n","ord","r","x","op","y");
    for(int i=0;i<opt_codes.size();i++){
        quadruples t = opt_codes[i];
        char s[30];
        strcpy(s,quarop_string[t.op]);
        printf("%-5d\t%-5s\t%-5s\t%-7s\t%-5s\n",
               i, t.r, t.x, s, t.y);
    }
}