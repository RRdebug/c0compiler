/*
    �����Ż�����
    ���������۵��������ʽת��Ϊ�沨����ʾ��������������
*/

#ifndef CODEOPT
#define CODEOPT

#include "parser.h"
#include "redBlackTree.h"
#include "stack.h"

//�����Ż��࣬�̳з�������
class CodeOpt: public Visitor
{
public:
    void Do_Optimization(shared_ptr<ASTNode>& node);    //�������Ż� 
    void Show_AST_BY_DFS(shared_ptr<ASTNode> _root, string prefix = "", bool hasSibling = false);
private:
    vector<shared_ptr<ASTNode>> expList;    //������ʹ�õ��б�

    //�������·���µ�return��ɾ��return֮����������
    void Check_Return_In_All_Paths(ASTNode& currentNode, ASTNode* parentNode, int index=0);
    
    //�����ʽת��Ϊ�沨����ʾ��
    void Do_RPN(MyStack<shared_ptr<ASTNode>>& operatorStack, vector<shared_ptr<ASTNode>>& outputVec, int i);
    void Reverse_Polish_Notation();

    //�����۵�
    void Const_Determine(int index, bool& breakFlag, bool& isChanged);
    int Const_Folding(int value1, int value2, Operator::Op opType);

    //����һ�����ڵ㣬�ͷ������ӽڵ�Ŀռ�
    void Delete_All_Children(std::shared_ptr<ASTNode> node);
    
    //�������ʽ���Ա��ʽ��������������ɻ�ԭ���ʽ
    void DFS_For_Expression(shared_ptr<ASTNode> node);

    //�Ż����ʽ��ʾ��ʽ
    void Opt_Expression(shared_ptr<ASTNode> expRoot, bool is_Pop=true);

    //������ģʽ����д�ӿڣ�������Ӧ�Ľ��
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
