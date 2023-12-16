/*
    生成目标代码
    生成假想栈式指令


    LIT 0 a	将常数值取到栈顶，a为常数值
    LOD t a	将变量值取到栈顶，a为相对地址，t为层数
    STO t a	将栈顶内容送入某变量单元中，a为相对地址，t为层数
    CAL 0 a	调用函数，a为函数地址
    INT 0 a	在运行栈中为被调用的过程开辟a个单元的数据区
    JMP 0 a	无条件跳转至a地址
    JPC 0 a	条件跳转，当栈顶值为0，则跳转至a地址，否则顺序执行
    ADD 0 0	次栈顶与栈顶相加，退两个栈元素，结果值进栈
    SUB 0 0	次栈顶减去栈顶，退两个栈元素，结果值进栈
    MUL 0 0	次栈顶乘以栈顶，退两个栈元素，结果值进栈
    DIV 0 0	次栈顶除以栈顶，退两个栈元素，结果值进栈
    RED 0 0	从命令行读入一个输入置于栈顶
    WRT 0 0	栈顶值输出至屏幕并换行
    RET 0 0	函数调用结束后,返回调用点并退栈

*/

#ifndef TAC
#define TAC

#include "parser.h"
#include "redBlackTree.h"
#include "stack.h"
#include <fstream>

//指令类
class ObjectCode
{
    friend ostream& operator<<(ostream& os, const ObjectCode& symbol);
public:
    ObjectCode(string op, int l, int s, int i) :operate(op), layer(l), secondAdd(s), index(i){}
    ObjectCode(string op, int l, int i) :operate(op), layer(l), secondAdd(0), index(i){}
    void Set_Second_Address(int sec) { secondAdd = sec; }   //设置指令中第二个参数的值
    string Get_Operate() const { return operate; }
private:
    string operate;     //操作，如：LIT
    int layer;          //层数
    int secondAdd;      //指令中第二个参数
    int index;          //当前指令的索引
};

//目标代码生成类，继承访问者类
class ObjCodeController : public Visitor
{
public:
    ObjCodeController(): nowIndex(0),nowFunction(""){}
    void Do_CodeGeneration(shared_ptr<ASTNode>& node, bool printLock=false); //目标代码生成
    void Show_Object_Code();                    //展示目标代码
    void OutputToFile(const string& filename);  //将目标代码输出到文件
private:
    vector<ObjectCode> objCodeList;      //目标代码序列
    int nowIndex;                        //当前的索引
    //记录if语句跳转位置的栈（因为可能有嵌套if,对于嵌套语句是先进后出，自然想到了用栈处理）
    MyStack<int> indexStackFor_IF;      
    MyStack<int> indexStackFor_While;   //和if一样，处理while语句
    MyStack<int> indexStackFor_JMP;     //while语句要跳两个地方，需要两个栈
    string nowFunction;                 //当前作用域

    //访问者模式，重写对应的处理逻辑
    void visit(ProgramNode& node) override;
    void visit(CustomFuncNode& node) override;
    void visit(ReturnNode& node) override;
    void visit(FuncBodyNode& node) override;
    void visit(CondStatementNode& node) override;
    void visit(ElseStatementNode& node) override;
    void visit(LoopStatementNode& node) override;
    void visit(ReadStatementNode& node)override;
    void visit(WriteStatementNode& node) override;
    void visit(AssignStatementNode& node) override; 
    void visit(FactorNode& node) override;
    void visit(AssistNode& node) override;
    //生成表达式对应的栈式指令
    void Process_Expression(shared_ptr<Expression> exp);
};

#endif
