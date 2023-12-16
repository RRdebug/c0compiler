/*
    抽象语法树的相关节点定义
    后续语义分析，代码优化，目标代码生成都用到了抽象语法树，
    所以使用了访问者模式分离操作和元素，便于维护和拓展
*/

#ifndef AST
#define AST

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

//AST结点类型和返回值类型
namespace tree_node
{
    enum NodeType
    {
        PROGRAM, VARDEFINE, VARIDENTIFIER, FACTOR, CUSTOMFUNC, RETURNTYPE, FUNCBODY, CONDSTATEMENT, ELSESTATEMENT, STATEMENTSET, LOOPSTATEMENT, RETURN, READSTATEMENT, WRITESTATEMENT, FUNCCALLSTATEMENT, ASSIGNSTATEMENT, OPERATOR, EXP,ASSIST
    };
    enum ReturnType
    {
        _VOID, INT
    };
}

class Visitor;

// AST节点类基类，因为要建树，所以树上所有类型的结点都要继承这个类，使用基类指针来建树
class ASTNode
{
public:
    vector<shared_ptr<ASTNode>> childNode;

    ASTNode() :lines(0), pos(0), m_type(tree_node::PROGRAM) {}  //构造函数，记录一下位置信息，方便报错
    virtual void Show() { cout << "error_show"; };              //接口，用于展示对应结点的信息
    virtual void accept(Visitor& visitor) = 0;                  //接口，访问者设计模式的定式，调用传入访问者的visit方法传递自身引用
    tree_node::NodeType Get_type() const { return m_type; }     //一系列GET方法，
    int Get_Line() const { return lines; }
    size_t Get_Postion() const { return pos; }
protected:
    tree_node::NodeType m_type;    //记录结点类型
    int lines;                     //位置信息
    size_t pos;
};

// 程序节点
class ProgramNode : public ASTNode
{
public:
    ProgramNode() { m_type = tree_node::PROGRAM; }  //每个节点实例化时设置对应的类型
    void Show() override;                           //重写抽象接口，没什么好说的，往后省略这部分的注释
    void accept(Visitor& visitor) override;
};

// 变量定义节点
class VarDefineNode : public ASTNode
{
public:
    VarDefineNode() { m_type = tree_node::VARDEFINE; }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// 变量标识符节点
class IdentifierNode : public ASTNode
{
public:
    enum IdType             //标识符类型：变量/函数
    {
        VAR = 0, FUNCTION
    };
    //构造函数,实例化函数
    IdentifierNode(string id, IdentifierNode::IdType _type, tree_node::ReturnType ret, int line, size_t _pos) :idName(id), type(_type)
    {
        m_type = tree_node::VARIDENTIFIER;
        retType = ret;
        lines = line;
        pos = _pos;
    }
    //构造函数，实例化变量
    IdentifierNode(string id, IdentifierNode::IdType _type, int line, size_t _pos, string fname) :idName(id), type(_type),funName(fname)
    {
        m_type = tree_node::VARIDENTIFIER;
        retType = tree_node::ReturnType::_VOID;
        lines = line;
        pos = _pos;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    string Get_Id_Name() const { return idName; }                       //一系列GET方法
    IdType Get_Id_Type() const { return type; }
    string Get_Function_Name() const { return funName; }
    tree_node::ReturnType Get_Return_Type() const { return retType; }
private:
    string funName;                 //作用域（函数名），特别说明: 全局类型为："0_GLOBAL" , 主函数为: "0_MAIN"
    string idName;                  //标识符名字
    IdType type;                    //标识符类型
    tree_node::ReturnType retType;  //返回值类型
};

// 自定义函数节点
class CustomFuncNode : public ASTNode
{
public:
    CustomFuncNode(int _line, size_t _pos)
    {
        m_type = tree_node::CUSTOMFUNC;
        lines = _line;
        pos = _pos;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// 返回值节点
class ReturnTypeNode : public ASTNode
{
public:
    ReturnTypeNode(int line, size_t _pos, tree_node::ReturnType rt)
    {
        m_type = tree_node::RETURNTYPE;
        lines = line;
        pos = _pos;
        retType = rt;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    tree_node::ReturnType Get_Return_Type()const { return retType; }
private:
    tree_node::ReturnType retType;      //返回值类型
};

class ReturnNode : public ASTNode
{
public:
    ReturnNode(int line, size_t _pos)
    {
        m_type = tree_node::RETURN;
        lines = line;
        pos = _pos;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// 函数体节点
class FuncBodyNode : public ASTNode
{
public:
    FuncBodyNode(bool ism, int _line, size_t _pos) :isMain(ism)
    {
        m_type = tree_node::FUNCBODY;
        lines = _line;
        pos = _pos;
    }
    FuncBodyNode(const FuncBodyNode& node)
    {
        isMain = node.Is_Main();
        *this = node;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    bool Is_Main() const { return isMain; }
private:
    bool isMain;        //抽象语法树中，我没有给主函数单独设置结点，用函数体结点和一个布尔值表示主函数
};

// 条件语句节点
class CondStatementNode : public ASTNode
{
public:
    CondStatementNode() { m_type = tree_node::CONDSTATEMENT; }
    CondStatementNode(const CondStatementNode& node){*this = node;}
    void Show() override;
    void accept(Visitor& visitor) override;
};

// Else语句节点
class ElseStatementNode : public ASTNode
{
public:
    ElseStatementNode() { m_type = tree_node::ELSESTATEMENT; }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// 语句集合节点
class StatementSetNode : public ASTNode
{
public:
    StatementSetNode() { m_type = tree_node::STATEMENTSET; }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// 循环语句节点
class LoopStatementNode : public ASTNode
{
public:
    LoopStatementNode(int _line, size_t _pos) 
    { 
        m_type = tree_node::LOOPSTATEMENT;
        lines = _line;
        pos = _pos;
    }
    LoopStatementNode(const LoopStatementNode& node) { *this = node; }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// 读语句节点
class ReadStatementNode : public ASTNode
{
public:
    ReadStatementNode(string _id, int _line, size_t _pos) :id(_id)
    {
        m_type = tree_node::READSTATEMENT;
        lines = _line;
        pos = _pos;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    string Get_Id() const { return id; }
private:
    string id;  //要存入的标识符名字
};

// 写语句节点
class WriteStatementNode : public ASTNode
{
public:
    WriteStatementNode() { m_type = tree_node::WRITESTATEMENT; }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// 赋值语句节点
class AssignStatementNode : public ASTNode
{
public:
    AssignStatementNode(string _id, int _line, size_t _pos) :id(_id)
    {
        m_type = tree_node::ASSIGNSTATEMENT;
        lines = _line;
        pos = _pos;
    }
    AssignStatementNode(const AssignStatementNode& node)
    {
        *this = node;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    string Get_Id() const { return id; }
private:
    string id;  //左值
};

//因子结点
class FactorNode : public ASTNode
{
public:
    enum FactorType     //因子类型，变量，整数常量，函数
    {
        VAR, INT, FUNCTION
    };
    //构造函数，INT用的
    FactorNode(int v, int _line, size_t _pos) :value(v), id("")
    {
        m_type = tree_node::NodeType::FACTOR;
        facType = INT;
        lines = _line;
        pos = _pos;
        isInExp = true;
    }
    //构造函数，VAR和FUNCTION用的
    FactorNode(string s, bool isVar, int _line, size_t _pos, bool isexp) :id(s), value(0)
    {
        m_type = tree_node::NodeType::FACTOR;
        facType = isVar ? VAR : FUNCTION;
        lines = _line;
        pos = _pos;
        isInExp = isexp;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    FactorType Get_Factor_Type() const { return facType; }  //一系列GET方法
    bool Get_IsInExp() const { return isInExp; }
    string Get_Id_Name() const { return id; }
    int Get_Value() const { return value; }
private:
    int value;              //如果是int，存对应值
    string id;              //如果是函数或者变量,标识符名字s
    FactorType facType;     //因子类型
    bool isInExp;           //判断函数调用是否在表达式中
};

//操作符结点
class Operator : public ASTNode
{
public:
    enum Op     //操作符类型，加减乘除
    {
        ADD, SUBTRACT,      //优先级: 1
        MULTIPLY, DIVIDE    //优先级: 2
    };
    Operator(Op optype, int pri, int _line, size_t _pos) :opType(optype), priority(pri)
    {
        m_type = tree_node::OPERATOR;
        lines = _line;
        pos = _pos;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    void Decrease_Priority() { priority -= 2; }     //降低操作符优先级
    int Get_Priority() { return priority; }         //获取当前优先级
    Op Get_Operator_Type() { return opType; }      
private:
    Op opType;       //操作符类型
    int priority;    //优先级
};

//逆波兰表示法的表达式
class Expression :public ASTNode
{
public:
    Expression(vector<shared_ptr<ASTNode>> v) : expList(v) { m_type = tree_node::NodeType::EXP; }
    vector<shared_ptr<ASTNode>> expList;
    void Show() override;
    void accept(Visitor& visitor) override;
};

//辅助结点，用于代码生成时确定while跳转时机
class AssistNode : public ASTNode   
{
public:
    AssistNode(){ m_type = tree_node::NodeType::ASSIST; }
    void accept(Visitor& visitor) override;
};

//访问者模式抽象类。对于语义分析、代码优化、代码生成的类，只需要继承Vistor类重写对应函数即可实现处理对应结点的逻辑。
class Visitor
{
public:
    virtual void visit(ReturnNode& node) = 0;
    virtual void visit(FuncBodyNode& node) = 0;
    virtual void visit(CondStatementNode& node) = 0;
    virtual void visit(LoopStatementNode& node) = 0;
    virtual void visit(WriteStatementNode& node) = 0;
    virtual void visit(AssignStatementNode& node) = 0;
    virtual void visit(ProgramNode& node) {};
    virtual void visit(VarDefineNode& node) {};
    virtual void visit(IdentifierNode& node) {};
    virtual void visit(CustomFuncNode& node) {};
    virtual void visit(ReturnTypeNode& node) {};
    virtual void visit(ElseStatementNode& node) {};
    virtual void visit(StatementSetNode& node) {};
    virtual void visit(ReadStatementNode& node) {};
    virtual void visit(FactorNode& node) {};
    virtual void visit(Operator& node) {}; 
    virtual void visit(Expression& node) {};
    virtual void visit(AssistNode& node) {};
};

#endif