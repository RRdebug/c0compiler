/*
    语义分析器，主要是检测标识符、作用域、类型匹配等方面的内容
    最终生成一张符号表
*/

#ifndef ANALYZER
#define ANLYZER

#include "parser.h"
#include "ast.h"
#include "redBlackTree.h"

//语义分析器，继承访问者类
class SemanticAnalyzer : public Visitor
{
public:
    SemanticAnalyzer()
    {
        nowRetValue = tree_node::ReturnType::_VOID;
        isAlreadRet = true;
        isPass = false;
        nowFunName = "0_GLOBAL";
    }
    void Do_Semantic_Analyzer(shared_ptr<ASTNode> node);
    void Show_Symbol_Table();
private:
    tree_node::ReturnType nowLeftValue;     //当前待匹配的类型
    string nowFunName;                      //当前作用域
    tree_node::ReturnType nowRetValue;      //当前返回值类型
    bool isAlreadRet;                       //检测是否所以路径都返回
    bool isPass;                            //为scanf语句服务，表示是否跳过报错

    //访问者模式，重写处理对应结点的函数
    void visit(WriteStatementNode& node) override;      
    void visit(LoopStatementNode& node) override;
    void visit(ReturnNode& node) override;
    void visit(IdentifierNode& node) override;
    void visit(AssignStatementNode& node) override;
    void visit(FactorNode& node) override;
    void visit(CustomFuncNode& node) override;
    void visit(FuncBodyNode& node) override;
    void visit(CondStatementNode& node) override;
    void visit(ReadStatementNode& node) override;

    //检测是否所有路径都有返回值
    bool Check_Return_In_All_Paths(shared_ptr<ASTNode> node);
};

#endif;
