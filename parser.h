/*
    �﷨�����������ڼ���﷨�����Ƿ�Ϻ�Ԥ�ڵ��﷨����ͬʱ����һ�ų����﷨��
    �ⲿ�ֱ�������LL(1)�ģ��������ָ�д�ĵط�̫�࣬�����﷨����Ƚϼ򵥣���ֱ���õݹ��½���������
*/

#ifndef PARSER
#define PARSER

#include <vector>
#include <set>
#include <string>
#include "lexer.h"
#include "ast.h"

using namespace std;

// �﷨��������
class Parser
{
public:
    // ���캯��������Token����
    Parser(const vector<Token>& tlist) : tokenList(tlist), nowIndex(0) {};
    // �����﷨���������س����﷨���ĸ��ڵ�
    shared_ptr<ASTNode> Do_Parser() { return Program(); };
    // ʹ�������������չʾ�����﷨��
    void Show_AST_BY_DFS(shared_ptr<ASTNode> _root, string prefix = "", bool hasSibling = false);
private:
    vector<Token> tokenList;  // �洢�������Token����
    size_t nowIndex;          // �αָ꣬ʾ��ǰ������Tokenλ��

    // ���ص�ǰToken��ǰToken֮��ָ��ƫ������Token
    Token Lookahead(int offset = 0) const;

    // ��鵱ǰToken�Ƿ���Ԥ������ƥ�䣬�����ƥ�����׳��쳣�������
    void Match(token_type::TokenTypeEnum expectedType);

    // ���ʽ�������ȼ���Ϊ����
    shared_ptr<ASTNode> Expression(int priority);
    shared_ptr<ASTNode> Subsequent_Expression(shared_ptr<ASTNode> child, int priority);
    shared_ptr<ASTNode> Term(int priority);
    shared_ptr<ASTNode> Subsequent_Term(shared_ptr<ASTNode> child, int priority);
    shared_ptr<ASTNode> Factor(int priority);
    shared_ptr<ASTNode> Function_Call(bool isExp);

    // �ݹ鴦����ʽ,�������ȼ�
    void DFS_EXP_Decrease(shared_ptr<ASTNode> node);

    // ��������ӡ������Ϣ
    void Print_Error(Token errorToken, token_type::TokenTypeEnum expectedType);

    // �����Ǿ��崦��ͬ�﷨�ṹ�ķ�����ÿ����������һ�����ս����ʹ����ǰ���������ŵĵݹ��½���������ͬʱ���������﷨��
    shared_ptr<ASTNode> Program();
    void Optional_Variable_Definitions(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent);
    void Optional_Function_Definitions(shared_ptr<ASTNode> root);
    void Variable_Definition(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent);
    void Subsequent_Variable_Definition(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent);
    void Function_Definition(shared_ptr<ASTNode> root);
    void Main_Function(shared_ptr<ASTNode> root);
    void Subprogram(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent);
    void Statement_Sequence(shared_ptr<ASTNode> root);
    void More_Statements(shared_ptr<ASTNode> root);
    void Statement(shared_ptr<ASTNode> root);
    void Conditional_Statement(shared_ptr<ASTNode> root);
    void Optional_Else_Statement(shared_ptr<ASTNode> root);
    void Loop_Statement(shared_ptr<ASTNode> root);
    void Assignment_Statement(shared_ptr<ASTNode> root);
    void Return_Statement(shared_ptr<ASTNode> root);
    void Optional_Expression(shared_ptr<ASTNode> root);
    void Read_Statement(shared_ptr<ASTNode> root);
    void Write_Statement(shared_ptr<ASTNode> root);
};

#endif
