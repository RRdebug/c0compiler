/*
    语法分析器，用于检测语法规则是否合乎预期的语法规则，同时生成一颗抽象语法树
    这部分本来想用LL(1)的，后来发现改写的地方太多，加上语法规则比较简单，就直接用递归下降分析法了
*/

#ifndef PARSER
#define PARSER

#include <vector>
#include <set>
#include <string>
#include "lexer.h"
#include "ast.h"

using namespace std;

// 语法分析器类
class Parser
{
public:
    // 构造函数，接收Token序列
    Parser(const vector<Token>& tlist) : tokenList(tlist), nowIndex(0) {};
    // 进行语法分析并返回抽象语法树的根节点
    shared_ptr<ASTNode> Do_Parser() { return Program(); };
    // 使用深度优先搜索展示抽象语法树
    void Show_AST_BY_DFS(shared_ptr<ASTNode> _root, string prefix = "", bool hasSibling = false);
private:
    vector<Token> tokenList;  // 存储待处理的Token序列
    size_t nowIndex;          // 游标，指示当前处理到的Token位置

    // 返回当前Token或当前Token之后指定偏移量的Token
    Token Lookahead(int offset = 0) const;

    // 检查当前Token是否与预期类型匹配，如果不匹配则抛出异常或错误处理
    void Match(token_type::TokenTypeEnum expectedType);

    // 表达式处理，优先级作为参数
    shared_ptr<ASTNode> Expression(int priority);
    shared_ptr<ASTNode> Subsequent_Expression(shared_ptr<ASTNode> child, int priority);
    shared_ptr<ASTNode> Term(int priority);
    shared_ptr<ASTNode> Subsequent_Term(shared_ptr<ASTNode> child, int priority);
    shared_ptr<ASTNode> Factor(int priority);
    shared_ptr<ASTNode> Function_Call(bool isExp);

    // 递归处理表达式,修正优先级
    void DFS_EXP_Decrease(shared_ptr<ASTNode> node);

    // 错误处理，打印错误信息
    void Print_Error(Token errorToken, token_type::TokenTypeEnum expectedType);

    // 下面是具体处理不同语法结构的方法，每个函数代表一个非终结符，使用向前看两个符号的递归下降分析法，同时构建抽象语法树
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
