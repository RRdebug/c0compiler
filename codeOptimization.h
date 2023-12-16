/*
    代码优化部分
    包括常量折叠，将表达式转换为逆波兰表示法，死代码消除
*/

#ifndef CODEOPT
#define CODEOPT

#include "parser.h"
#include "redBlackTree.h"
#include "stack.h"

//代码优化类，继承访问者类
class CodeOpt: public Visitor
{
public:
    void Do_Optimization(shared_ptr<ASTNode>& node);    //做代码优化 
    void Show_AST_BY_DFS(shared_ptr<ASTNode> _root, string prefix = "", bool hasSibling = false);
private:
    vector<shared_ptr<ASTNode>> expList;    //处理表达使用的列表

    //检测所有路径下的return，删除return之后的所有语句
    void Check_Return_In_All_Paths(ASTNode& currentNode, ASTNode* parentNode, int index=0);
    
    //将表达式转换为逆波兰表示法
    void Do_RPN(MyStack<shared_ptr<ASTNode>>& operatorStack, vector<shared_ptr<ASTNode>>& outputVec, int i);
    void Reverse_Polish_Notation();

    //常量折叠
    void Const_Determine(int index, bool& breakFlag, bool& isChanged);
    int Const_Folding(int value1, int value2, Operator::Op opType);

    //传入一个根节点，释放所有子节点的空间
    void Delete_All_Children(std::shared_ptr<ASTNode> node);
    
    //遍历表达式，对表达式子树的中序遍历可还原表达式
    void DFS_For_Expression(shared_ptr<ASTNode> node);

    //优化表达式表示形式
    void Opt_Expression(shared_ptr<ASTNode> expRoot, bool is_Pop=true);

    //访问者模式，重写接口，处理相应的结点
    void visit(WriteStatementNode& node) override;
    void visit(LoopStatementNode& node) override;
    void visit(ReturnNode& node) override;
    void visit(VarDefineNode& node) override;
    void visit(AssignStatementNode& node) override;
    void visit(ProgramNode& node) override;
    void visit(FuncBodyNode& node) override;
    void visit(CondStatementNode& node) override;
};

#endif
