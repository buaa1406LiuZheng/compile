program pl0 ;  { version 1.0 oct.1989 }
{ PL/0 compiler with code generation }
const norw = 13;          { no. of reserved words }
      txmax = 100;        { length of identifier table }
      nmax = 14;          { max. no. of digits in numbers }
      al = 10;            { length of identifiers }
      amax = 2047;        { maximum address }
      levmax = 3;         { maximum depth of block nesting }
      cxmax = 200;        { size of code array }

type symbol =
     ( nul,ident,number,plus,minus,times,slash,oddsym,eql,neq,lss,
       leq,gtr,geq,lparen,rparen,comma,semicolon,period,becomes,
       beginsym,endsym,ifsym,thensym,whilesym,dosym,callsym,constsym,
       varsym,procsym,readsym,writesym );	{*所有的symbol以助记符方式记录为枚举*}
     alfa = packed array[1..al] of char;	{*标识符名字类型*}
     objecttyp = (constant,variable,prosedure);	{*对象类型枚举*}
     symset = set of symbol;	{*symbol的集合*}
     fct = ( lit,opr,lod,sto,cal,int,jmp,jpc,red,wrt ); { functions }{*Pcode指令枚举*}
     instruction = packed record
                     f : fct;            { function code }{*操作码*}
                     l : 0..levmax;      { level }{*被引用的分程序与说明之间的层次差*}
                     a : 0..amax;        { displacement address }
                   end; {*Pcode指令在程序中的中间存储形式*}
                  {   lit 0, a : load constant a
                      opr 0, a : execute operation a
                      lod l, a : load variable l,a
                      sto l, a : store variable l,a
                      cal l, a : call procedure a at level l
                      int 0, a : increment t-register by a
                      jmp 0, a : jump to a
                      jpc 0, a : jump conditional to a
                      red l, a : read variable l,a
                      wrt 0, 0 : write stack-top
                  }

var   ch : char;      { last character read}{*当前读入字符*}
      sym: symbol;    { last symbol read }{*当前symbol*}
      id : alfa;      { last identifier read }{*当前标识符名*}
      num: integer;   { last number read }{*当前常量值*}
      cc : integer;   { character count }{*当前读取字符在当前行位置*}
      ll : integer;   { line length }{*当前行长度*}
      kk,err: integer;	{*kk为上一个标识符名的长度，err为错误数*}
      cx : integer;   { code allocation index }{*当前生成指令指针*}
      line: array[1..81] of char;	{*缓冲一行代码*}
      a : alfa; {*当前读入的标识符名字缓存*}
      code : array[0..cxmax] of instruction;  {*Pcode指令储存数组*}
      word : array[1..norw] of alfa;	{*保留字表*}
      wsym : array[1..norw] of symbol;	{*保留字对应的symbol*}
      ssym : array[char] of symbol;		{*单个字符的symbol，一个字符一个symbol*}
      mnemonic : array[fct] of
                   packed array[1..5] of char;	{*输出Pcode指令时，每一种指令对应要输出的字符串*}
      declbegsys, statbegsys, facbegsys : symset; {*三个头符号集*}
      table : array[0..txmax] of
                record
                  name : alfa;  {*标识名字*}
                  case kind: objecttyp of {*标识类型*}
                    constant : (val:integer );
                    variable,prosedure: (level,adr: integer )
                end;	{*符号表*}
      fin : text;     { source program file }
      sfile: string;  { source program file name }

procedure error ( n : integer );	{*错误处理过程*} 
  begin
    writeln( '****', ' ':cc-1, '^', n:2 );	{*输出错误信息，'^'指向出错位置，n表示错误类型*}
    err := err+1	{*错误数加1*}
  end; { error }

procedure getsym;	{*词法分析，读取一个symbol*}
var i,j,k : integer;	{*定义临时变量*}
procedure getch;	{*读下一个字符*}
    begin
      if cc = ll  { get character to end of line }{*读到达当前行末*}
      then begin { read next line }{*将下一行存入line*}
             if eof(fin)	{*若文件结束*}
             then begin
                   writeln('program incomplete');	{*程序不完整*}
                   close(fin);	{*关闭fin*}
                   exit;	{*退出编译程序*}
                  end;
             ll := 0;	{*行长度置0*}
             cc := 0;	{*当前字符位置置0*}
             write(cx:4,' ');  { print code address }	{*输出当前指令号*}
             while not eoln(fin) do 	{*将源文件当前行存到行缓冲*}
               begin
                 ll := ll+1;	{*行长度加1*}
                 read(fin,ch);	{*读一个字符*}
                 write(ch);	{*输出这个字符*}
                 line[ll] := ch 	{*将这个字符放入行缓冲*}
               end;
             writeln;	{*换行*}
             readln(fin);	{*读换行*}
             ll := ll+1;	{*行长度加1*}
             line[ll] := ' ' { process end-line }{*行缓冲末置为空*}
           end;
      cc := cc+1;	{*当前读取字符位置加1*}
      ch := line[cc]	{*读取当前字符*}
    end; { getch }
  begin { procedure getsym; }
    while ch = ' ' do	{*跳过空白符*}
      getch;	{*读下一个字符*}
    if ch in ['a'..'z']	{*如果读入的字符为字母*}
      then begin  { identifier of reserved word }
           k := 0;  {*初始化读入标识符长度为0*}
           repeat {*读取完整标识符名*}
             if k < al  {*标识符长度需小于最大标识符长度限制al，若大于al只取前al个字符*}
             then begin
                   k := k+1;  {*标识符长度加1*}
                   a[k] := ch {*读入的字符添加到标识符名*}
                 end;
             getch  {*读下一个字符*}
           until not( ch in ['a'..'z','0'..'9']); {*直到下一个字符不是字母或数字*}
           if k >= kk        { kk : last identifier length }
           then kk := k
           else repeat  {*清除掉上一个标识符名在a中的遗留*}
                  a[kk] := ' ';
                  kk := kk-1
               until kk = k;
           id := a; {*将读取的标识符名存入id*}
           i := 1;  {*i,j分别为二分查找顶、底下标*}
           j := norw;   { binary search reserved word table }
           repeat {*查找id是否为保留字*}
             k := (i+j) div 2;  {*k取中间*}
             if id <= word[k] {*id比第k个word小*}
             then j := k-1; {*置顶下标为k-1*}
             if id >= word[k] {*id比第k个word大*}
             then i := k+1  {*置底下标为k+1*}
           until i > j; {*底下标比顶下标大结束查找*}
           if i-1 > j
           then sym := wsym[k]  {*是保留字，将sym置为此保留字对应symbol*}
           else sym := ident   {*非保留字，将sym置为标识符*}
         end
    else if ch in ['0'..'9']	{*如果读入的字符为数字*}
         then begin  { number }
                k := 0; {*初始化读入数字长度为0*}
                num := 0; {*初始化数字的数值为0*}
                sym := number;  {*置sym为数字*}
                repeat
                  num := 10*num+(ord(ch)-ord('0')); {*更新数值*}
                  k := k+1; {*读入数字长度加1*}
                  getch {*读下一个字符*}
                until not( ch in ['0'..'9']); {*读到下一个非数字为止*}
                if k > nmax {*读入数字长度超过最大限制*}
                then error(30) {*报30号错误*}
              end
    else if ch = ':'	{*如果读入的字符为':'*}
         then begin
               getch;	{*读下一个字符*}
               if ch = '='  {*如果读入的字符为'='*}
               then begin
                     sym := becomes;  {*置sym为赋值号*}
                     getch  {*读下一个字符*}
                   end
               else sym := nul {*置sym为空*}
              end
    else if ch = '<'	{*如果读入的字符为'<'*}
         then begin
                getch;	{*读下一个字符*}
                if ch = '=' {*如果读入的字符为'='*}
                then begin
                       sym := leq;  {*置sym为小于等于号*}
                       getch  {*读下一个字符*}
                     end
                else if ch = '>'  {*如果读入的字符为'>'*}
                     then begin
                           sym := neq;  {*置sym为不等号*}
                           getch  {*读下一个字符*}
                         end
                else sym := lss {*置sym为小于号*}
              end
    else if ch = '>'	{*如果读入的字符为'>'*}
         then begin
                getch;	{*读下一个字符*}
                if ch = '=' {*如果读入的字符为'='*}
                then begin
                       sym := geq;  {*置sym为大于等于号*}
                       getch  {*读下一个字符*}
                     end
                else sym := gtr {*置sym为大于号*}
              end
    else begin 			{*如果读入为其他字符*}
           sym := ssym[ch]; {*将sym置为此字符对应的symbol*}
           getch  {*读下一个字符*}
         end
  end; { getsym }

procedure gen( x: fct; y,z : integer );
  {*生成一个Pcode指令并存入code中*}
  begin
    if cx > cxmax {*若当前指令号大于最大指令数现在*}
    then begin  {*报错并退出*}
           writeln('program too long');
           close(fin);
           exit
         end;
    with code[cx] do
      begin   {*对解析好的当前行代码生成Pcode指令存入code*}
        f := x;
        l := y;
        a := z
      end;
    cx := cx+1  {*指令位置加1*}
  end; { gen }

procedure test( s1,s2 :symset; n: integer ); 
  {*测试当前符号是否为当前语法成分合法后继，若不合法进行跳读
    s1是合法后继符号集，s2是其他可以停止跳读的符号集合*}
  begin
    if not ( sym in s1 )  {*若不是合法的语法成分的后继符号，报错*}
    then begin
           error(n); {*报n号错误*}
           s1 := s1+s2; {*将合法符号和停止符号合并*}
           while not( sym in s1) do   {*进行跳读，错误处理时，读取到下一个合法符号后继续编译*}
             getsym {*读取下一个symbol*}
           end
  end; { test }

procedure block( lev,tx : integer; fsys : symset ); 
{*分程序分析处理，lev为分程序的嵌套层数，tx为符号表指针，fsys为后继符号集*}
  var  dx : integer;  { data allocation index }
       tx0: integer;  { initial table index }
       cx0: integer;  { initial code index }

  procedure enter( k : objecttyp );
  {*将常量、变量或过程登记符入号表，k表示被登记的类型*}
    begin  { enter object into table }
      tx := tx+1; {*符号表指针加1*}
      with table[tx] do   {*登记符号表*}
        begin
          name := id; {*登记标识符名*}
          kind := k;  {*登记类型*}
          case k of
            constant : begin  {*如果是常量*}
                      if num > amax {*值超过最大限制*}
                      then begin
                            error(30);  {*报30号错误*}
                            num := 0  {*值置为0*}
                           end;
                      val := num  {*登记常量值*}
                    end;
            variable : begin {*如果是变量*}
                      level := lev; {*登记嵌套层数*}
                      adr := dx;  {*登记地址为当前层变量待登记地址*}
                      dx := dx+1  {*当前层变量待地址加1*}
                    end;
            prosedure: level := lev;  {*如果是过程，登记嵌套层数*}
          end
        end
    end; { enter }

  function position ( id : alfa ): integer; 
  {*查找标识符是否在符号表，id为标识符名*}
    var i : integer;
    begin
      table[0].name := id;
      i := tx;  {*自顶向下查找符号表，若找不到返回0*}
      while table[i].name <> id do
         i := i-1;
      position := i   {*找到返回标识符位置*}
    end;  { position }

  procedure constdeclaration; 
  {*常量声明处理*}
    begin
      if sym = ident  {*若当前符号为标识符*}
      then begin
             getsym;	{*读取下一个symbol*}
             if sym in [eql,becomes]  {*当前符号为等号或赋值号*}
             then begin
                    if sym = becomes  {*如果当前符号是等号*}
                    then error(1);  {*报1号错误*}
                    getsym;	{*读取下一个symbol*}  
                    if sym = number {*如果当前符号是数字*}
                    then begin
                           enter(constant); {*登记这个常量*}
                           getsym {*读取下一个symbol*}
                         end
                    else error(2) {*报2号错误*}
                  end
             else error(3) {*标识符后不是等号，报3号错误*}
           end
      else error(4) {*当前符号不为标识符，报4号错误*}
    end; { constdeclaration }

  procedure vardeclaration; 
  {*变量声明处理*}
    begin
      if sym = ident  {*若当前符号为标识符*}
      then begin
             enter(variable); {*登记此变量*}
             getsym {*读取下一个symbol*}
           end
      else error(4) {*报4号错误*}
    end; { vardeclaration }

  procedure listcode;
  {*打印输出当前分程序生成的Pcode代码*}
    var i : integer;
    begin
      for i := cx0 to cx-1 do   {*cx0为当前分程序的起始行，cx为当前行（终止行）*}
        with code[i] do
          writeln( i:4, mnemonic[f]:7,l:3, a:5) {*将储存在code中的Pcode指令打印输出*}
    end; { listcode }

  procedure statement( fsys : symset );
  {*语句分析处理，这里及下面的fsys都是相应块的后继符号集*}
    var i,cx1,cx2: integer;
    procedure expression( fsys: symset); 
    {*表达式分析处理*}
      var addop : symbol;
      procedure term( fsys : symset); 
      {*项分析处理*} 
        var mulop: symbol ;
        procedure factor( fsys : symset );
        {*因子分析处理*}
          var i : integer;
          begin
            test( facbegsys, fsys, 24 );	{*测试头符号是否为因子头符号集*}
            while sym in facbegsys do 	{*使得因子后紧接着一个因子的错误（漏掉乘除号）能继续编译*}
              begin
                if sym = ident 	{*若当前符号为标识符*}
                then begin
                       i := position(id);	{*查找符号表*}
                       if i= 0	{*若不在符号表中*}
                       then error(11) {*报11号错误*}
                       else	{*在符号表中*}
                         with table[i] do
                           case kind of
                             constant : gen(lit,0,val); {*生成Pcode指令，载入常量的常数值*}
							 variable : gen(lod,lev-level,adr);	{*生成Pcode指令，载入变量*}
							 prosedure: error(21) {*报21号错误*}
                           end;
                       getsym {*读取下一个symbol*}
                     end
                else if sym = number 	{*若当前符号为数字*}
                    then begin
                           if num > amax	{*数值大于最大限制*}
                           then begin
                                  error(30); {*报30号错误*}
                                  num := 0	{*置数值为0*}
                                end;
                           gen(lit,0,num); {*生成Pcode指令，载入此数值代表的常数*}
                           getsym  {*读取下一个symbol*}
                         end
                else if sym = lparen	{*若当前符号为左括号*}
                     then begin
                            getsym;	{*读取下一个symbol*}
                            expression([rparen]+fsys);	{*解析下一个语法项，表达式*}
                            if sym = rparen	{*表达式解析完后，下一个符号是否为右括号*}
                            then getsym  {*读取下一个symbol*}
                            else error(22) {*报22号错误*}
                          end;
                test(fsys,[lparen],23)	{*测试因子的后继符号集*}
              end
          end; { factor }
        begin { procedure term( fsys : symset);   
                var mulop: symbol ;    }
          factor( fsys+[times,slash]);	{*解析因子*}
          while sym in [times,slash] do 	{*右递归，检查下一个是否为乘除号*}
            begin
              mulop := sym;
              getsym;	{*读取下一个symbol*}
              factor( fsys+[times,slash] );
              if mulop = times	{*若为乘号*}
              then gen( opr,0,4 )	{*生成Pcode指令，乘法指令*}
              else gen( opr,0,5)	{*生成Pcode指令，除法指令*}
            end
        end; { term }
      begin { procedure expression( fsys: symset);  
              var addop : symbol; }
        if sym in [plus, minus]	{*第一个符号为正负号*}
        then begin
               addop := sym;	{*记录下这个符号*}
               getsym;	{*读取下一个symbol*}
               term( fsys+[plus,minus]);	{*解析项*}
               if addop = minus	{*若前面符号为负号*}
               then gen(opr,0,1) {*生成Pcode指令，取负*}
             end
        else term( fsys+[plus,minus]);	{*解析项*}
        while sym in [plus,minus] do 	{*右递归，检查下一个是否为加减号*}
          begin
            addop := sym;
            getsym;	{*读取下一个symbol*}
            term( fsys+[plus,minus] );	{*解析项*}
            if addop = plus	{*若为加号*}
            then gen( opr,0,2)	{*生成Pcode指令，加法指令*}
            else gen( opr,0,3)	{*生成Pcode指令，减法指令*}
          end
      end; { expression }

    procedure condition( fsys : symset ); 
    {*条件语句分析处理*}
      var relop : symbol;
      begin
        if sym = oddsym	{*若当前符号为odd*}
        then begin
               getsym;	{*读取下一个symbol*}
               expression(fsys);	{*解析后继的表达式*}
               gen(opr,0,6) {*生成Pcode指令，判断是否为奇数*}
             end
        else begin
             expression( [eql,neq,lss,gtr,leq,geq]+fsys);	{*解析比较的第一个表达式*}
             if not( sym in [eql,neq,lss,leq,gtr,geq])	{*中间若不是比较运算符*}
               then error(20) {*报20号错误*}
               else begin
                      relop := sym;	{*记录比较运算符*}
                      getsym;	{*读取下一个symbol*}
                      expression(fsys);	{*解析比较的第二个表达式*}
                      case relop of	{*根据比较运算符生成相应指令*}
                        eql : gen(opr,0,8); {*生成Pcode指令*}
                        neq : gen(opr,0,9); {*生成Pcode指令*}
                        lss : gen(opr,0,10); {*生成Pcode指令*}
                        geq : gen(opr,0,11); {*生成Pcode指令*}
                        gtr : gen(opr,0,12); {*生成Pcode指令*}
                        leq : gen(opr,0,13); {*生成Pcode指令*}
                      end
                    end
             end
      end; { condition }
    begin { procedure statement( fsys : symset );  
      var i,cx1,cx2: integer; }
      if sym = ident 	{*若当前symbol为标识符，表示是赋值语句*}
      then begin
             i := position(id);	{*查找符号表*}
             if i= 0	{*若不在符号表*}
             then error(11) {*报11号错误*}
             else if table[i].kind <> variable	{*若不是变量*}
                  then begin { giving value to non-variation }
                         error(12); {*报12号错误*}
                         i := 0
                       end;
             getsym;	{*读取下一个symbol*}
             if sym = becomes	{*符号是否为赋值号*}
             then getsym  {*读取下一个symbol*}
             else error(13);  {*报13号错误*}
             expression(fsys);	{*分析赋值号后面的表达式*}
             if i <> 0	{*赋值过程没有出错*}
             then
               with table[i] do
                  gen(sto,lev-level,adr) {*生成Pcode指令，将栈顶值存入变量的地址adr*}
          end
      else if sym = callsym	{*若当前symbol为call，表示过程调用语句*}
      then begin
             getsym;	{*读取下一个symbol*}
             if sym <> ident {*若symbol不是标识符*}
             then error(14) {*报14号错误*}
             else begin
                    i := position(id);	{*查找符号表*}
                    if i = 0	{*若不在符号表*}
                    then error(11) {*报11号错误*}
                    else
                      with table[i] do
                        if kind = prosedure	{*判断call后面标识符是否为过程*}
                        then gen(cal,lev-level,adr) {*生成Pcode指令，调用过程*}
                        else error(15); {*报15号错误*}
                    getsym  {*读取下一个symbol*}
                  end
           end
      else if sym = ifsym	{*若当前symbol为if，表示条件语句*}
           then begin
                  getsym;	{*读取下一个symbol*}
                  condition([thensym,dosym]+fsys);	{*解析关系运算*}
                  if sym = thensym	{*下一个symbol需要为then*}
                  then getsym {*读取下一个symbol*}
                  else error(16); {*报16号错误*}
                  cx1 := cx;	{*记录跳转指令的位置*}
                  gen(jpc,0,0); {*生成Pcode指令，条件跳转*}
                  statement(fsys);	{*解析then后面的语句*}
                  code[cx1].a := cx 	{*将if的跳转指令的跳转位置设置为设置为整条if语句之后*}
                end
      else if sym = beginsym	{*若当前symbol为begin，表示复合语句*}
           then begin
                  getsym;	{*读取下一个symbol*}
                  statement([semicolon,endsym]+fsys);	{*解析语句*}
                  while sym in ([semicolon]+statbegsys) do 	{*右递归*}
                    begin
                      if sym = semicolon	{*语句结尾是否为分号*}
                      then getsym  {*读取下一个symbol*}
                      else error(10);  {*报10号错误*}
                      statement([semicolon,endsym]+fsys)	{*解析下一条语句*}
                    end;
                  if sym = endsym	{*最后一个语法元素是否为end*}
                  then getsym  {*读取下一个symbol*}
                  else error(17) {*报17号错误*}
                end
      else if sym = whilesym	{*若当前symbol为while，表示当循环语句*}
           then begin
                  cx1 := cx;	{*记录循环开始的指令位置*}
                  getsym;	{*读取下一个symbol*}
                  condition([dosym]+fsys);	{*解析循环条件*}
                  cx2 := cx;	{*记录条件跳转指令的位置*}
                  gen(jpc,0,0); {*生成Pcode指令，循环条件为假时跳出循环*}
                  if sym = dosym	{*symbol需要为do*}
                  then getsym {*读取下一个symbol*}
                  else error(18); {*报18号错误*}
                  statement(fsys);	{*解析循环的语句*}
                  gen(jmp,0,cx1); {*生成Pcode指令，一次循环结束后跳回循环头*}
                  code[cx2].a := cx 	{*将条件跳转指令的跳转位置设置为循环结束位置*}
                end
      else if sym = readsym	{*若当前symbol为read，表示读语句*}
           then begin
                  getsym;	{*读取下一个symbol*}
                  if sym = lparen	{*read下一个symbol需为左括号*}
                  then
                    repeat
                      getsym;	{*读取下一个symbol*}
                      if sym = ident {*read内第一个symbol需要为标识符*}
                      then begin
                             i := position(id);	{*差符号表*}
                             if i = 0	{*不在符号表*}
                             then error(11) {*报11号错误*}
                             else if table[i].kind <> variable	{*标识符不是变量*}
                                  then begin
                                         error(12);  {*报12号错误*}
                                         i := 0
                                       end
                                  else with table[i] do
                                        gen(red,lev-level,adr) {*生成Pcode指令，读指令*}
                          end
                      else error(4); {*报4号错误*}
                      getsym;	{*读取下一个symbol*}
                    until sym <> comma	{*右递归*}
                  else error(40);  {*报40号错误*}
                  if sym <> rparen	{*最后一个符号需为右括号*}
                  then error(22);  {*报22号错误*}
                  getsym {*读取下一个symbol*}
                end
      else if sym = writesym	{*若当前symbol为write，表示写语句*}
           then begin
                  getsym;	{*读取下一个symbol*}
                  if sym = lparen	{*write下一个symbol需为左括号*}
                  then begin
                         repeat
                           getsym;	{*读取下一个symbol*}
                           expression([rparen,comma]+fsys);	{*解析读语句里面的表达式*}
                           gen(wrt,0,0); {*生成Pcode指令*}
                         until sym <> comma;	{*右递归*}
                         if sym <> rparen	{*最后一个符号需为右括号*}
                         then error(22);  {*报22号错误*}
                         getsym {*读取下一个symbol*}
                       end
                  else error(40) {*报40号错误*}
                end;
      test(fsys,[],19)	{*测试语句的后继符号集*}
    end; { statement }
  begin  {   procedure block( lev,tx : integer; fsys : symset );   
    var  dx : integer;  /* data allocation index */
    tx0: integer;  /*initial table index */
    cx0: integer;  /* initial code index */              }
    dx := 3;  {*初始数据记录地址（前三个分别留给SL、DL、RA）*}
    tx0 := tx;  {*记录头符号表指针，其实对应的是这个分程序所在符号表中的记录位置*}
    table[tx].adr := cx;  {*将分程序开始地址记入符号表，开始地址的指令是下面生成的跳转指令*}
    gen(jmp,0,0); {*生成Pcode指令，跳过声明部分，跳转到本分程序语句块部分
                  （中间会有过程声明，会生成一些Pcode指令，需要跳过）*}
                  { jump from declaration part to statement part }
    if lev > levmax {*若嵌套层数大于最大限制*}
    then error(32); {*报32号错误*}

    repeat
      if sym = constsym {*若symbol为const，是常量声明部分*}
      then begin
             getsym;	{*读取下一个symbol*}
             repeat
               constdeclaration;  {*解析常量声明*}
               while sym = comma do   {*一个常量声明右递归（逗号隔开）*}
                 begin
                   getsym;	{*读取下一个symbol*}
                   constdeclaration {*解析常量声明*}
                 end;
               if sym = semicolon {*常量定义语句完后需要是分号*}
               then getsym  {*读取下一个symbol*}
               else error(5) {*报5号错误*}
             until sym <> ident {*常量定义语句右递归（分号隔开）*}
           end;
      if sym = varsym {*若symbol为var，是变量声明部分*}
      then begin
             getsym;	{*读取下一个symbol*}
             repeat
               vardeclaration;  {*解析变量声明*}
               while sym = comma do   {*一个变量声明右递归（逗号隔开）*}
                 begin
                   getsym;	{*读取下一个symbol*}
                   vardeclaration {*解析变量声明*}
                 end;
               if sym = semicolon {*变量声明语句完后需要是分号*}
               then getsym  {*读取下一个symbol*}
               else error(5) {*报5号错误*}
             until sym <> ident; {*变量声明语句右递归（分号隔开）*}
           end;
      while sym = procsym do {*若symbol为procedure，是过程声明部分*}
        begin
          getsym;	{*读取下一个symbol*}
          if sym = ident {*procedure保留字后需要是一个标识符*}
          then begin
                 enter(prosedure);  {*登记符号表*}
                 getsym {*读取下一个symbol*}
               end
          else error(4);  {*报4号错误*}
          if sym = semicolon  {*过程声明语句完后需要是分号*}
          then getsym {*读取下一个symbol*}
          else error(5);  {*报5号错误*}
          block(lev+1,tx,[semicolon]+fsys); {*解析过程声明的分程序部分（过程块）*}
                                            {*每次处理下一个子块时，传入的符号表指针都重新从tx开始
                                              tx是局部的，表示当父块符号表指针，字块tx变化不影响父块
                                              表明上一个子块处理完后，上一个子块的符号表都被清除了*}
          if sym = semicolon  {*语句结束后需是分号*}
          then begin
                 getsym;	{*读取下一个symbol*}
                 test( statbegsys+[ident,procsym],fsys,6) {*测试过程声明部分后继符号集是否满足*}
               end
          else error(5) {*报5号错误*}
        end;
      test( statbegsys+[ident],declbegsys,7)  {*测试整个声明部分后继符号集是否满足*}
    until not ( sym in declbegsys );  {*为了使声明顺序颠倒能继续编译*}
    code[table[tx0].adr].a := cx; {*table[tx0].adr是分程序起始地址（一条跳转指令）
                                    将此跳转指令的跳转地址设置为cx，即下面将要解析的语句部分的开始*}
                                  { back enter statement code's start adr. }
    with table[tx0] do
      begin
        adr := cx; { code's start address }{*分程序实际起始地址是下面的语句部分的开始*}
      end;
    cx0 := cx;  {*设置语句部分的起始地址（为了下面listcode时能跳过上面过程声明中产生的Pcode指令）*}
    gen(int,0,dx); {*生成Pcode指令，分配运行栈空间*} { topstack point to operation area }
    statement( [semicolon,endsym]+fsys);  {*解析分程序的语句部分*}
    gen(opr,0,0); {*生成Pcode指令，返回*} { return }
    test( fsys, [],8 ); {*测试后继符号集是否满足*}
    listcode; {*打印出当前分程序块所生成的所以Pcode指令*}
  end { block };

procedure interpret;  
  const stacksize = 500;
  var p,b,t: integer; { program-,base-,topstack-register }
     i : instruction;{ instruction register }
     s : array[1..stacksize] of integer; { data store }{*运行栈*}
  function base( l : integer ): integer;
    {*通过静态链SL求出数据区基地址，l为层次差*}
    var b1 : integer;
    begin { find base l levels down }
      b1 := b;  {*b为当前块基地址*}
      while l > 0 do
        begin
          b1 := s[b1];  {*上一个块的基地址*}
          l := l-1  {*迭代l次*}
        end;
      base := b1  {*返回要找的基地址*}
    end; { base }
  begin  
    writeln( 'START PL/0' );
    t := 0; {*栈指针*}
    b := 1; {*当前块基地址，只想当前块的直接外层基地址*}
    p := 0; {*指令计数器*}
    s[1] := 0;  {*直接外层基地址*}
    s[2] := 0;  {*前驱块基地址*}
    s[3] := 0;  {*返回地址*}
    repeat
      i := code[p]; {*取指令*}
      p := p+1; {*指令计数器加1*}
      with i do
        case f of
          lit : begin {*取常数，将常数a放到栈顶*}
                  t := t+1;
                  s[t]:= a;
              end;
          opr : case a of { operator }{*数据操作运算*}
                  0 : begin { return }{*返回*}
                        t := b-1; {*将栈顶退到当前块基地址的上一个（即将当前块退栈）*}
                        p := s[t+3];  {*指令计数器置为返回地址*}
                        b := s[t+2];  {*当前块基地址置为前驱块基地址*}
                     end;
                  1 : s[t] := -s[t];  {*取负*}
                  2 : begin   {*加法*}
                        t := t-1;
                        s[t] := s[t]+s[t+1]
                     end;
                  3 : begin   {*减法*}
                        t := t-1;
                        s[t] := s[t]-s[t+1]
                     end;
                  4 : begin   {*乘法*}
                        t := t-1;
                        s[t] := s[t]*s[t+1]
                     end;
                  5 : begin   {*除法*}
                        t := t-1;
                        s[t] := s[t]div s[t+1]
                     end;
                  6 : s[t] := ord(odd(s[t]));   {*odd运算*}
                  8 : begin   {*判断相等*}
                        t := t-1;
                        s[t] := ord(s[t]=s[t+1]) 
                    end;
                  9 : begin   {*判断不等*}
                        t := t-1;
                        s[t] := ord(s[t]<>s[t+1])
                     end;
                  10: begin   {*判断小于*}
                        t := t-1;
                        s[t] := ord(s[t]< s[t+1])
                     end;
                  11: begin   {*判断大于等于*}
                        t := t-1;
                        s[t] := ord(s[t] >= s[t+1])
                     end;
                  12: begin   {*判断大于*}
                        t := t-1;
                        s[t] := ord(s[t] > s[t+1])
                     end;
                  13: begin   {*判断小于等于*}
                        t := t-1;
                        s[t] := ord(s[t] <= s[t+1])
                     end;
                end;
          lod : begin   {*载入指令*}
                  t := t+1;
                  s[t] := s[base(l)+a]  {*将前l层的第a个数载入栈顶*}
              end;
          sto : begin   {*存储指令*}
                  s[base(l)+a] := s[t];  { writeln(s[t]); }{*将栈顶存入前l层的第a个数*}
                  t := t-1
              end;
          cal : begin  { generate new block mark }{*调用指令，调用前l层的过程块*}
                  s[t+1] := base(l);{*设置直接外层地址*}
                  s[t+2] := b;  {*前驱块基地址为当前基地址*}
                  s[t+3] := p;  {*返回地址为当前程序计数器值*}
                  b := t+1; {*设置当前基地址*}
                  p := a; {*设置当前程序计数器*}
              end;
          int : t := t+a; {*栈顶加a*}
          jmp : p := a; {*跳转到第a条指令*}
          jpc : begin   {*如果栈顶为0（false），跳转到第a条指令*}
                  if s[t] = 0
                  then p := a;
                  t := t-1;
              end;
          red : begin   {*读操作*}
                  writeln('??:');
                  readln(s[base(l)+a]); 
              end;
          wrt : begin   {*写操作*}
                  writeln(s[t]);
                  t := t+1
              end
        end { with,case }
    until p = 0;  {主程序返回地址为0，p=0说明主程序返回，整个程序执行结束}
    writeln('END PL/0');
  end; { interpret }

begin { main }
  writeln('please input source program file name : ');	{*输出操作提示*}
  readln(sfile);	{*读取源文件文件名*}
  assign(fin,sfile);	{*加源文件名付给fin*}
  reset(fin);	{*打开源文件fin*}
  for ch := 'A' to ';' do
    ssym[ch] := nul;	{*先将所有字符对应的symbol置为nul*}
  word[1] := 'begin        '; word[2] := 'call         ';
  word[3] := 'const        '; word[4] := 'do           ';
  word[5] := 'end          '; word[6] := 'if           ';
  word[7] := 'odd          '; word[8] := 'procedure    ';
  word[9] := 'read         '; word[10]:= 'then         ';
  word[11]:= 'var          '; word[12]:= 'while        ';
  word[13]:= 'write        ';	{*初始化保留字表*}

  wsym[1] := beginsym;      wsym[2] := callsym;
  wsym[3] := constsym;      wsym[4] := dosym;
  wsym[5] := endsym;        wsym[6] := ifsym;
  wsym[7] := oddsym;        wsym[8] := procsym;
  wsym[9] := readsym;       wsym[10]:= thensym;
  wsym[11]:= varsym;        wsym[12]:= whilesym;
  wsym[13]:= writesym;	{*初始化保留字对应的symbol表*}

  ssym['+'] := plus;        ssym['-'] := minus;
  ssym['*'] := times;       ssym['/'] := slash;
  ssym['('] := lparen;      ssym[')'] := rparen;
  ssym['='] := eql;         ssym[','] := comma;
  ssym['.'] := period;
  ssym['<'] := lss;         ssym['>'] := gtr;
  ssym[';'] := semicolon;	{*赋给特殊字符其对应的symbol*}

  mnemonic[lit] := 'LIT  '; mnemonic[opr] := 'OPR  ';
  mnemonic[lod] := 'LOD  '; mnemonic[sto] := 'STO  ';
  mnemonic[cal] := 'CAL  '; mnemonic[int] := 'INT  ';
  mnemonic[jmp] := 'JMP  '; mnemonic[jpc] := 'JPC  ';
  mnemonic[red] := 'RED  '; mnemonic[wrt] := 'WRT  ';	{*初始化Pcode指令输出字符串表*}

  declbegsys := [ constsym, varsym, procsym ];
  statbegsys := [ beginsym, callsym, ifsym, whilesym];
  facbegsys := [ ident, number, lparen ];	{*初始化因子头符号集*}
  err := 0;	{*错误数初始为0*}
  cc := 0;	{*当前读取字符位置初始为0*}
  cx := 0;	{*行号初始为0*}
  ll := 0;	{*行长度初始为0*}
  ch := ' ';	{*当前读入字符初始为空*}
  kk := al;	{*设置当前标识符名长度为最长（表明当前标识符名缓冲区占用满）*}
  getsym;	{*读取下一个symbol*}
  block( 0,0,[period]+declbegsys+statbegsys );	{*进入分程序分析处理过程*}
  if sym <> period	{*最后一个字符不是'.'*}
  then error(9);  {*报9号错误*}
  if err = 0	{*判断编译过程是否出错*}
  then interpret	{*没有出错，解释执行*}
  else write('ERRORS IN PL/0 PROGRAM');	{*有出错，报告程序错误*}
  writeln;	{*换行*}
  close(fin)	{*关闭fin*}
end.                                                     