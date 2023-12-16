# 前言

IMU的编译原理大作业，写一个c0的编译器，其实解释器也写了的，但本项目生成的只是假象的栈式指令，解释器意义不大。重要的还是提供一个写编译器的思路，希望对你有所帮助。本项目完整的实现了一个前后端分离的编译器



**ps:** 符号表那边的实现很蠢，完全按自己感觉来的，~~因为没听课~~，日后可能重构下。



流程为 ：<u>**词法分析 -> 语法分析 -> 语义分析 -> 代码优化 -> 目标代码生成**</u>



1. #### 编译器前端

   - [词法分析](#词法分析)的任务是输入程序源代码输出Token序列

   - [语法分析](#语法分析)的任务是输入Token序列,输出一颗抽象语法树 (AST)

2. #### 编译器后端

   - [语义分析](#语义分析)的任务是输入抽象语法树输出一张符号表

     我这里省略了中间代码生成,因为文法本身比较简单

   - [代码优化](#代码优化)一般是对中间表示或者目标代码进行操作,我这里直接就对抽象语法树进行操作了

   - [代码生成](#目标代码生成)通过抽象语法树和符号表生成目标代码

   

![流程图](https://github.com/RRdebug/c0compiler/blob/main/image/c0.png)



# 文件结构



| 文件               | 描述                   |
| ------------------ | ---------------------- |
| ast.h              | 抽象语法树定义         |
| codeOptimization.h | 代码优化               |
| io.h               | 输入/输出              |
| lexer.h            | 词法分析器             |
| main.cpp           | 程序入口点             |
| objectCode.h       | 目标代码生成           |
| parser.h           | 语法分析器             |
| redBlackTree.h     | 红黑树数据结构、符号表 |
| semanticAnalyzer.h | 语义分析器             |
| stack.h            | 自定义栈               |

# 文法规则

学校给的是BNF，我把他转为了等价的上下文无关文法



1. `P -> OVD OFD MF` (程序 -> 可选变量定义部分 可选自定义函数定义部分 主函数)

2. `OVD -> VD | ε` (可选变量定义部分 -> 变量定义部分 | 空)

3. `OFD -> FD OFD | ε` (可选自定义函数定义部分 -> 函数定义 可选自定义函数定义部分 | 空)

4. `VD -> 'int' id SD` (变量定义部分 -> 'int' 标识符 后续变量定义)

5. `SD -> ',' id SD | ';'` (后续变量定义 -> ',' 标识符 后续变量定义 | ';')

6. `FD -> ('int' id | 'void' id) '(' ')' SP` (函数定义 -> ('int' 标识符 | 'void' 标识符) '()' 分程序)

7. `MF -> 'void main' '(' ')' SP` (主函数 -> 'void main' '()' 分程序)

8. `SP -> '{' OVD SS '}'` (分程序 -> '{' 可选变量定义部分 语句序列 '}')

9. `SS -> S MS` (语句序列 -> 语句 更多语句)

10. `MS -> S MS | ε` (更多语句 -> 语句 更多语句 | 空)

11. `S -> CS | LS | '{' SS '}' | FCS | AS | RET | RS | WS | ';'` (语句 -> 条件语句 | 循环语句 | '{' 语句序列 '}' | 自定义函数调用语句 | 赋值语句 | 返回语句 | 读语句 | 写语句 | ';')

12. `CS -> 'if' '(' E ')' S OES` (条件语句 -> 'if' '(' 表达式 ')' 语句 可选else语句)

13. `OES -> 'else' S | ε` (可选else语句 -> 'else' 语句 | 空)

14. `LS -> 'while' '(' E ')' S` (循环语句 -> 'while' '(' 表达式 ')' 语句)

15. `FCS -> FC ';'` (自定义函数调用语句 -> 自定义函数调用 ';')

16. `AS -> id '=' E ';'` (赋值语句 -> 标识符 '=' 表达式 ';')

17. `RET -> 'return' OE ';'` (返回语句 -> 'return' 可选表达式 ';')

18. `OE -> '(' E ')' | ε` (可选表达式 -> '(' 表达式 ')' | 空)

19. `RS -> 'scanf' '(' id ')' ';'` (读语句 -> 'scanf' '(' 标识符 ')' ';')

20. `WS -> 'printf' '(' OE ')' ';'` (写语句 -> 'printf' '(' 可选表达式 ')' ';')

21. `E -> OP T SE` (表达式 -> 可选前缀 项 后续表达式)

22. `OP -> '+' | '-' | ε` (可选前缀 -> '+' | '-' | 空)

23. `SE -> ('+' | '-') T SE | ε` (后续表达式 -> ('+' | '-') 项 后续表达式 | 空)

24. `T -> F ST` (项 -> 因子 后续项)

25. `ST -> ('*' | '/') F ST | ε` (后续项 -> ('*' | '/') 因子 后续项 | 空)

26. `F -> id | '(' E ')' | 'num' | FC` (因子 -> 标识符 | '(' 表达式 ')' | 数字 | 自定义函数调用)

27. `FC -> id '(' ')'` (自定义函数调用 -> 标识符 '()')

    

# 假象的栈式指令



| 指令  | 参数  | 描述                                               |
| ----- | ----- | -------------------------------------------------- |
| `LIT` | `0 a` | 将常数值取到栈顶，a为常数值                        |
| `LOD` | `t a` | 将变量值取到栈顶，a为相对地址，t为层数             |
| `STO` | `t a` | 将栈顶内容送入某变量单元中，a为相对地址，t为层数   |
| `CAL` | `0 a` | 调用函数，a为函数地址                              |
| `INT` | `0 a` | 在运行栈中为被调用的过程开辟a个单元的数据区        |
| `JMP` | `0 a` | 无条件跳转至a地址                                  |
| `JPC` | `0 a` | 条件跳转，当栈顶值为0，则跳转至a地址，否则顺序执行 |
| `ADD` | `0 0` | 次栈顶与栈顶相加，退两个栈元素，结果值进栈         |
| `SUB` | `0 0` | 次栈顶减去栈顶，退两个栈元素，结果值进栈           |
| `MUL` | `0 0` | 次栈顶乘以栈顶，退两个栈元素，结果值进栈           |
| `DIV` | `0 0` | 次栈顶除以栈顶，退两个栈元素，结果值进栈           |
| `RED` | `0 0` | 从命令行读入一个输入置于栈顶                       |
| `WRT` | `0 0` | 栈顶值输出至屏幕并换行                             |
| `RET` | `0 0` | 函数调用结束后,返回调用点并退栈                    |



# 词法分析

词法分析是程序源代码的第一个阶段，用于将源代码分解为基本的词法单元或词法符号。词法单元是编程语言中的最小语法单元，通常是标识符、关键字、运算符、常量和分隔符等。



词法分析一般是基于有穷状态机（**DFA**）来进行自动的分析的

开始是状态机的起始态 , 剩下的逻辑看图就能明白

![DFA](https://github.com/RRdebug/c0compiler/blob/main/image/DFA.png)



# 语法分析

在编译器中，语法分析是一个核心步骤，负责将词法分析阶段生成的记号（tokens）根据编程语言的规则组织成语法树（Syntax Tree）。这个过程检查源代码是否遵循语言的语法规则，并且在发现语法错误时进行报告。语法分析为编译器的后续步骤，如语义分析和代码生成，提供了结构化的代码表示。



语义分析我采用的是递归下降预测分析法,首先计算每个非终结符的FIRST集和FOLLOW集 (~~其实只算下有冲突的地方就行)~~ ,然后为每个非终结符编写一个程序,从程序入口出发递归的调用就行了

~~本来想手搓一个LR(1)解析器来着,后来发现是我自不量力了,还是老老实实用自动生成工具吧~~



其实核心就两个函数

1. #### Lookahead

   ```c++
   // 查看当前或指定偏移量的Token
   Token Parser::Lookahead(int offset) const
   {
       // 如果偏移量在Token列表范围内，返回相应的Token
       if (nowIndex + offset < tokenList.size())
       {
           return tokenList[nowIndex + offset];
       }
       // 否则返回空Token
       return Token();
   }
   ```

   

2. #### Match

   ```c++
   // 查看当前或指定偏移量的Token
   Token Parser::Lookahead(int offset) const
   {
       // 如果偏移量在Token列表范围内，返回相应的Token
       if (nowIndex + offset < tokenList.size())
       {
           return tokenList[nowIndex + offset];
       }
       // 否则返回空Token
       return Token();
   }
   ```





# 语义分析


在编译器中，语义分析是一个关键的过程，它在语法分析之后进行。语义分析的主要任务是检查源代码的语义一致性，确保符合编程语言的定义。包括以下几个方面：



1. **类型检查**：确保变量和表达式的数据类型是正确的，并符合预期的操作。例如，不允许将整数类型的变量与其他类型的变量相加。
2. **函数和过程调用**：检查函数或过程调用时传递的参数数量和类型是否与声明相匹配。
3. **标识符绑定**：验证所有变量和函数的标识符都已经定义，并且在当前的作用域内是可访问的。
4. **控制流程检查**：如检查每条路径都有返回值。
5. **创建符号表**：编译器在语义分析过程中创建和更新符号表，

语义分析的目的是确保程序不仅语法正确，而且逻辑上也是有意义的，遵循了特定编程语言的规则和约定。



#### 符号表:

符号表是编译器用来存储关于源代码中标识符（如变量名、函数名）信息的数据结构。符号表通常在语法分析和语义分析阶段被创建和填充，并在整个编译过程中被查询和更新

```c++
    /*
        单例模式
        文法比较简单，所以我只用了一张符号表
        为了方便访问符号表，我使用了单例模式
        为了减少复制开销，使用指针传递
    */
    static SymbolTable* GetInstance()   
    {
        static SymbolTable instance;    // 唯一实例
        return &instance;               // 返回指向实例的指针
    }

```



符号表使用**红黑树**存储，保证了插入和查询的高效



# 代码优化

常规的代码优化是生成三地址码或者控制流图之后做的，我这里直接对抽象语法树处理了



用到的技术有：

- 常量折叠：这是编译器将在编译时计算常量表达式的过程，以减少程序运行时的计算量。例如，表达式 `2 + 3` 会被替换为 `5`。

- 死代码消除：移除那些在程序执行过程中永远不会被执行到的代码段。这不仅包括那些因为条件永远不会满足而不会被执行的代码，还包括那些执行了也不会对程序的输出有任何影响的代码。如：

  - 未使用变量删除
  - 未使用的函数删除
  - return语句之后语句删除
  - if和while永远不会走到的分支删除

- 将中缀表达式转换为逆波兰表示法（~~其实这个语法分析就可以转，我写到这里才想到~~）

  

# 目标代码生成

没什么好说的，直接看源码就能懂，注释写的很详细，核心就是维护一个索引，对if和while使用栈处理一下

