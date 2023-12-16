/*
    �������������Ҫ�Ǽ���ʶ��������������ƥ��ȷ��������
    ��������һ�ŷ��ű�
*/

#ifndef ANALYZER
#define ANLYZER

#include "parser.h"
#include "ast.h"
#include "redBlackTree.h"

//������������̳з�������
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
    tree_node::ReturnType nowLeftValue;     //��ǰ��ƥ�������
    string nowFunName;                      //��ǰ������
    tree_node::ReturnType nowRetValue;      //��ǰ����ֵ����
    bool isAlreadRet;                       //����Ƿ�����·��������
    bool isPass;                            //Ϊscanf�����񣬱�ʾ�Ƿ���������

    //������ģʽ����д�����Ӧ���ĺ���
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

    //����Ƿ�����·�����з���ֵ
    bool Check_Return_In_All_Paths(shared_ptr<ASTNode> node);
};

#endif;
