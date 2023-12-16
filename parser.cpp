#include "parser.h"

using namespace token_type;


// 显示抽象语法树，采用深度优先搜索
void Parser::Show_AST_BY_DFS(shared_ptr<ASTNode> _root, string prefix, bool hasSibling)
{
    // 显示当前节点
    cout << prefix << "|——";
    if (_root != nullptr)
        _root->Show(); 
    cout << "\n";
    // 根据是否有兄弟节点来决定是否打印分隔符
    if (hasSibling) {
        cout << prefix << "|\n";
    }
    else {
        cout << prefix << "\n";
    }
    // 创建子节点的前缀
    string childPrefix = hasSibling ? prefix + "|       " : prefix + "        "; 
    // 遍历子节点并递归调用函数来显示它们
    for (size_t i = 0; i < _root->childNode.size(); i++)        
    {
        Show_AST_BY_DFS(_root->childNode[i], childPrefix, i + 1 < _root->childNode.size());
    }
}


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


// 匹配预期的Token类型
void Parser::Match(TokenTypeEnum expectedType)
{
    // 如果当前查看的Token类型与预期匹配，移动到下一个Token
    if (Lookahead().type == expectedType)
    {
        nowIndex++; // 消耗Token
    }
    else
    {
        // 如果不匹配，打印错误信息
        Print_Error(Lookahead(), expectedType);
    }
}


// 解析程序入口点，创建根节点并解析变量定义、函数定义和主函数
shared_ptr<ASTNode> Parser::Program()
{
    auto root = make_shared<ProgramNode>();
    Optional_Variable_Definitions(root, root);
    Optional_Function_Definitions(root);
    Main_Function(root);
    return root;
}


// 可选的变量定义解析
void Parser::Optional_Variable_Definitions(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent)
{
    auto varDef = make_shared<VarDefineNode>();
    if (Lookahead().type == _INT && (Lookahead(2).type == COMMA || Lookahead(2).type == SEMICOLON)) //向前看两个符号区分是变量还是函数
    {
        Variable_Definition(varDef, rootParent);    //变量定义
    }
    root->childNode.push_back(varDef);              //连接父节点
}


// 解析变量定义
void Parser::Variable_Definition(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent)
{
    Match(_INT);
    Token tmpToken = Lookahead();
    string parentName = " ";

    if (rootParent->Get_type() == tree_node::PROGRAM)       // 根据父节点类型区分变量所属的作用域
    {
        parentName = rootParent->childNode.empty() ? "0_GLOBAL" : "0_MAIN";
    }
    else if (rootParent->Get_type() == tree_node::CUSTOMFUNC)
    {
        auto funBody = dynamic_pointer_cast<IdentifierNode>(rootParent->childNode[1]);     //将基类指针转换为: 定义标识符类指针
        parentName = funBody->Get_Id_Name();
    }
 
    auto vDef = make_shared<IdentifierNode>(tmpToken.value, IdentifierNode::VAR, tmpToken.s_lines, tmpToken.s_position, parentName);   // 创建变量定义节点
    Match(ID);
    root->childNode.push_back(move(vDef));              //连接父节点
    Subsequent_Variable_Definition(root, rootParent);   //处理后续变量
}


// 解析后续的变量定义
void Parser::Subsequent_Variable_Definition(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent)
{
    // 如果下一个Token是逗号，表示有更多变量定义
    if (Lookahead().type == COMMA)
    {
        Match(COMMA);                  // 匹配并消耗逗号Token
        Token tmpToken = Lookahead();  // 获取当前Token作为变量名

        // 根据父节点的类型设置变量定义的作用域名称
        string parentName = " ";
        if (rootParent->Get_type() == tree_node::PROGRAM)
        {
            parentName = rootParent->childNode.empty() ? "0_GLOBAL" : "0_MAIN";
        }
        else if (rootParent->Get_type() == tree_node::CUSTOMFUNC)
        {
            auto funBody = dynamic_pointer_cast<IdentifierNode>(rootParent->childNode[1]);
            parentName = funBody->Get_Id_Name();
        }

        // 创建变量节点，并设置其信息
        auto vDef = make_shared<IdentifierNode>(tmpToken.value, IdentifierNode::VAR, tmpToken.s_lines, tmpToken.s_position, parentName);
        Match(ID);  // 匹配并消耗标识符Token
        root->childNode.push_back(move(vDef));                      // 将变量节点添加到AST

        Subsequent_Variable_Definition(root, rootParent);           // 递归调用以解析更多变量定义
    }
    else if (Lookahead().type == SEMICOLON)
    {
        Match(SEMICOLON);  // 如果遇到分号，则匹配并消耗，表示变量定义结束
    }
    else
    {
        cout << "(语法)ERROR: Line: " << Lookahead().s_lines << "(" << Lookahead().s_position << ")  标识符定义未正常结束" << endl;
        exit(0);
    }
}


// 解析可选的函数定义
void Parser::Optional_Function_Definitions(shared_ptr<ASTNode> root)
{
    // 创建一个函数定义节点
    auto oFunDef = make_shared<CustomFuncNode>(Lookahead().s_lines, Lookahead().s_position);

    // 如果当前Token是_INT，表示有一个函数定义,如果当前Token是_VOID并且下一个Token是ID，同样表示有一个函数定义
    if ((Lookahead().type == _INT)|| (Lookahead().type == _VOID && Lookahead(1).type == ID))
    {
        Function_Definition(oFunDef);           // 调用Function_Definition解析函数
        root->childNode.push_back(oFunDef);     // 添加函数节点到AST
        Optional_Function_Definitions(root);    // 递归调用以解析更多函数定义
    }
}


// 解析函数定义
void Parser::Function_Definition(shared_ptr<ASTNode> root)
{
    Token tmpToken = Lookahead();
    tree_node::ReturnType retType = tree_node::ReturnType::_VOID;

    // 根据查看的下一个Token来确定函数的返回类型
    if (Lookahead().type == _INT)
    {
        // 如果下一个Token是_INT，创建返回类型节点并添加到根节点
        auto _return = make_shared<ReturnTypeNode>(tmpToken.s_lines, tmpToken.s_position, tree_node::ReturnType::INT);
        root->childNode.push_back(_return);
        retType = tree_node::ReturnType::INT;
        Match(_INT);
    }
    else if (Lookahead().type == _VOID)
    {
        // 如果下一个Token是_VOID，同样创建返回类型节点
        auto _return = make_shared<ReturnTypeNode>(tmpToken.s_lines, tmpToken.s_position, tree_node::ReturnType::_VOID);
        root->childNode.push_back(_return);
        retType = tree_node::ReturnType::_VOID;
        Match(_VOID);
    }


    // 获取函数名
    tmpToken = Lookahead();
    auto funBody = make_shared<FuncBodyNode>(false, tmpToken.s_lines, tmpToken.s_position);
    auto vDef = make_shared<IdentifierNode>(tmpToken.value, IdentifierNode::FUNCTION, retType, tmpToken.s_lines, tmpToken.s_position);

    // 将函数名和函数体添加到根节点
    root->childNode.push_back(vDef);
    root->childNode.push_back(funBody);

    // 匹配函数名和参数列表的括号
    Match(ID);
    Match(LPAREN);
    Match(RPAREN);

    // 解析函数体
    Subprogram(funBody, root);
}


// 解析主函数
void Parser::Main_Function(shared_ptr<ASTNode> root)
{
    Token tmpToken = Lookahead();

    // 创建主函数体节点
    auto funBody = make_shared<FuncBodyNode>(true, tmpToken.s_lines, tmpToken.s_position);

    // 添加到根节点
    root->childNode.push_back(funBody);

    // 匹配_void, _main和参数列表括号，确认是主函数的定义
    Match(_VOID);
    Match(_MAIN);
    Match(LPAREN);
    Match(RPAREN);

    // 解析主函数体
    Subprogram(funBody, root);
}


// 解析子程序
void Parser::Subprogram(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent)
{
    // 匹配左大括号，表示函数体的开始
    Match(LBRACE);

    // 解析函数体内的可选变量定义
    Optional_Variable_Definitions(root, rootParent);

    // 解析语句序列
    Statement_Sequence(root);

    // 匹配右大括号，表示函数体的结束
    Match(RBRACE);
}


// 解析语句序列
void Parser::Statement_Sequence(shared_ptr<ASTNode> root)
{
    Statement(root);       // 解析单个语句
    More_Statements(root); // 递归解析更多语句
}


// 解析更多的语句，递归调用直到没有更多语句
void Parser::More_Statements(shared_ptr<ASTNode> root)
{
    // First集
    const set<TokenTypeEnum> firstSet = { _IF, _WHILE, LBRACE, ID, _RETURN, _SCANF, _PRINTF, SEMICOLON };

    // 递归解析
    if (firstSet.find(Lookahead().type) != firstSet.end())
    {
        Statement(root);
        More_Statements(root);
    }
}


// 解析单个语句
void Parser::Statement(shared_ptr<ASTNode> root)
{
    // 根据下一个Token的类型决定如何解析语句
    if (Lookahead().type == _IF)
    {
        Conditional_Statement(root);  // 解析条件语句
    }
    else if (Lookahead().type == _WHILE)
    {
        Loop_Statement(root);         // 解析循环语句
    }
    else if (Lookahead().type == LBRACE)
    {
        Match(LBRACE);            // 匹配左大括号
        Statement_Sequence(root); // 解析大括号内的语句序列
        Match(RBRACE);            // 匹配右大括号
    }
    else if (Lookahead().type == ID)
    {
        // 解析函数调用或赋值语句
        if (Lookahead(1).type == LPAREN)
        {
            root->childNode.push_back(Function_Call(false));
        }
        else if (Lookahead(1).type == ASSIGN)
        {
            Assignment_Statement(root);
        }
        else
        {
            cout << "语法分析错误";
        }
    }
    else if (Lookahead().type == _RETURN)
    {
        Return_Statement(root);  // 解析返回语句
    }
    else if (Lookahead().type == _SCANF)
    {
        Read_Statement(root);   // 解析读取语句
    }
    else if (Lookahead().type == _PRINTF)
    {
        Write_Statement(root);  // 解析写入语句
    }
    else if (Lookahead().type == SEMICOLON)
    {
        Match(SEMICOLON);      // 匹配空语句（只有分号）
    }
}


// 解析条件语句（if语句）
void Parser::Conditional_Statement(shared_ptr<ASTNode> root)
{
    // 创建条件语句节点
    auto conStateNode = make_shared<CondStatementNode>();
    root->childNode.push_back(conStateNode);

    // 匹配if关键字和左括号
    Match(_IF);
    Match(LPAREN);

    // 解析条件表达式
    auto exp = Expression(0);
    // 如果条件表达式为空，则报错并退出
    if (exp == nullptr)
    {
        cout << "(语法)ERROR: Line: " << Lookahead().s_lines << "(" << Lookahead().s_position << ")  if语句条件为空\n\n";
        exit(0);
    }
    conStateNode->childNode.push_back(exp); // 将条件表达式添加到条件语句节点

    Match(RPAREN); // 匹配右括号

    // 创建语句集节点，并解析if内部的语句
    auto stateNode = make_shared<StatementSetNode>();
    conStateNode->childNode.push_back(stateNode);
    Statement(stateNode);

    // 解析可选的else语句
    Optional_Else_Statement(conStateNode);
}


// 解析可选的else语句
void Parser::Optional_Else_Statement(shared_ptr<ASTNode> root)
{
    // 创建else语句节点
    auto conNode = make_shared<ElseStatementNode>();
    root->childNode.push_back(conNode);

    // 如果下一个Token是_else，则匹配_else并解析else内部的语句
    if (Lookahead().type == _ELSE)
    {
        Match(_ELSE);
        Statement(conNode);
    }
}


// 解析循环语句（while语句）
void Parser::Loop_Statement(shared_ptr<ASTNode> root)
{
    // 创建循环语句节点
    auto conStateNode = make_shared<LoopStatementNode>(Lookahead().s_lines,Lookahead().s_position);
    root->childNode.push_back(conStateNode);

    // 匹配while关键字和左括号
    Match(_WHILE);
    Match(LPAREN);

    // 解析条件表达式
    auto exp = Expression(0);
    // 如果条件表达式为空，则报错并退出
    if (exp == nullptr)
    {
        cout << "(语法)ERROR: Line: " << Lookahead().s_lines << "(" << Lookahead().s_position << ")  while语句条件为空\n\n";
        exit(0);
    }
    conStateNode->childNode.push_back(exp); // 将条件表达式添加到循环语句节点

    Match(RPAREN); // 匹配右括号

    // 创建语句集节点，并解析while内部的语句
    auto stateNode = make_shared<StatementSetNode>();
    conStateNode->childNode.push_back(stateNode);
    Statement(stateNode);
}


// 解析赋值语句
void Parser::Assignment_Statement(shared_ptr<ASTNode> root)
{
    // 创建赋值语句节点
    auto assNode = make_shared<AssignStatementNode>(Lookahead().value, Lookahead().s_lines, Lookahead().s_position);
    root->childNode.push_back(assNode);

    // 匹配变量ID和赋值符号
    Match(ID);
    Match(ASSIGN);

    // 解析赋值表达式
    assNode->childNode.push_back(Expression(0));

    Match(SEMICOLON); // 匹配分号
}


// 解析返回语句
void Parser::Return_Statement(shared_ptr<ASTNode> root)
{
    // 创建返回节点
    auto retNode = make_shared<ReturnNode>(Lookahead().s_lines, Lookahead().s_position);
    root->childNode.push_back(retNode);

    // 匹配return关键字
    Match(_RETURN);

    // 解析可选的返回值表达式
    Optional_Expression(retNode);

    Match(SEMICOLON); // 匹配分号
}


// 解析可选的表达式
void Parser::Optional_Expression(shared_ptr<ASTNode> root)
{
    // First集
    const set<TokenTypeEnum> firstSet = { PLUS, ID, LBRACE, ID, LPAREN, NUM, MINUS };

    // 解析表达式
    if (firstSet.find(Lookahead().type) != firstSet.end())
    {
        // 解析表达式并将其添加到当前节点的子节点中
        root->childNode.push_back(Expression(0));
    }
}


// 解析读取语句（scanf）
void Parser::Read_Statement(shared_ptr<ASTNode> root)
{
    // 匹配scanf关键字、左括号
    Match(_SCANF);
    Match(LPAREN);

    // 创建读取语句节点，并根据下一个Token（变量ID）初始化
    auto readNode = make_shared<ReadStatementNode>(Lookahead().value, Lookahead().s_lines, Lookahead().s_position);
    root->childNode.push_back(readNode); // 将节点添加到当前根节点的子节点中

    // 匹配变量ID、右括号和分号
    Match(ID);
    Match(RPAREN);
    Match(SEMICOLON);
}


// 解析写入语句（printf）
void Parser::Write_Statement(shared_ptr<ASTNode> root)
{
    // 创建写入语句节点
    auto printNode = make_shared<WriteStatementNode>();
    root->childNode.push_back(printNode); // 添加到根节点的子节点中

    // 匹配printf关键字、左括号
    Match(_PRINTF);
    Match(LPAREN);

    // 解析可选的表达式，作为printf的参数
    Optional_Expression(printNode);

    // 匹配右括号和分号
    Match(RPAREN);
    Match(SEMICOLON);
}


// 解析表达式
shared_ptr<ASTNode> Parser::Expression(int priority)
{
    // 首先解析一个项（term），这可能是表达式的一部分
    auto term = Term(priority);
    // 解析随后的表达式部分
    auto subExp = Subsequent_Expression(term, priority);

    // 如果没有随后的表达式部分，返回初始的项；否则返回完整表达式
    if (subExp == nullptr)
    {
        return term;
    }
    else
    {
        return subExp;
    }
}


// 解析后续的表达式，主要处理加减操作
shared_ptr<ASTNode> Parser::Subsequent_Expression(shared_ptr<ASTNode> child, int priority)
{
    // 检查下一个Token是否是加号
    if (Lookahead().type == PLUS)
    {
        // 创建加法操作符节点
        auto op = make_shared<Operator>(Operator::ADD, 1 + priority, Lookahead().s_lines, Lookahead().s_position);
        Match(PLUS); // 匹配加号

        auto term = Term(priority); // 解析后续的术语
        op->childNode.push_back(child); // 添加当前子节点

        // 递归解析后续表达式
        auto subExp = Subsequent_Expression(term, priority);
        if (subExp != nullptr)
        {
            op->childNode.push_back(subExp);
        }
        else
        {
            op->childNode.push_back(term);
        }
        return op; // 返回加法操作节点
    }
    else if (Lookahead().type == MINUS)
    {
        // 创建减法操作符节点
        auto op = make_shared<Operator>(Operator::SUBTRACT, 1 + priority, Lookahead().s_lines, Lookahead().s_position);
        Match(MINUS); // 匹配减号

        auto term = Term(priority); // 解析后续的术语
        op->childNode.push_back(child); // 添加当前子节点

        // 递归解析后续表达式
        auto subExp = Subsequent_Expression(term, priority);
        if (subExp != nullptr)
        {
            op->childNode.push_back(subExp);
        }
        else
        {
            op->childNode.push_back(term);
        }
        return op; // 返回减法操作节点
    }
    return nullptr; // 如果不是加减操作，返回空
}


// 解析术语，这可能是一个因子或一个包含乘除操作的更复杂表达式
shared_ptr<ASTNode> Parser::Term(int priority)
{
    // 首先解析一个基本因子
    auto factor = Factor(priority);

    // 然后递归解析包含乘除操作的后续术语
    auto subTerm = Subsequent_Term(factor, priority);

    // 如果有后续术语，返回该术语；否则返回基本因子
    if (subTerm == nullptr)
    {
        return factor;
    }
    else
    {
        return subTerm;
    }
}


// 解析后续的术语，主要处理乘除操作
shared_ptr<ASTNode> Parser::Subsequent_Term(shared_ptr<ASTNode> child, int priority)
{
    // 检查下一个Token是否是乘号
    if (Lookahead().type == TIMES)
    {
        // 创建乘法操作符节点
        auto op = make_shared<Operator>(Operator::MULTIPLY, 2 + priority, Lookahead().s_lines, Lookahead().s_position);
        Match(TIMES); // 匹配乘号

        auto factor = Factor(priority); // 解析因子
        op->childNode.push_back(child); // 添加当前子节点

        // 递归解析后续术语
        auto subTerm = Subsequent_Term(factor, priority);
        if (subTerm != nullptr)
        {
            op->childNode.push_back(subTerm);
        }
        else
        {
            op->childNode.push_back(factor);
        }
        return op; // 返回乘法操作节点
    }
    // 检查下一个Token是否是除号
    else if (Lookahead().type == DIVIDE)
    {
        // 创建除法操作符节点
        auto op = make_shared<Operator>(Operator::DIVIDE, 2 + priority, Lookahead().s_lines, Lookahead().s_position);
        Match(DIVIDE); // 匹配除号

        auto factor = Factor(priority); // 解析因子
        op->childNode.push_back(child); // 添加当前子节点

        // 递归解析后续术语
        auto subTerm = Subsequent_Term(factor, priority);
        if (subTerm != nullptr)
        {
            op->childNode.push_back(subTerm);
        }
        else
        {
            op->childNode.push_back(factor);
        }
        return op; // 返回除法操作节点
    }
    return nullptr; // 如果不是乘除操作，返回空
}


// 解析因子，因子可以是变量、数字或括号内的表达式
shared_ptr<ASTNode> Parser::Factor(int priority)
{
    // 检查因子是否是一个标识符（变量或函数调用）
    if (Lookahead().type == ID)
    {
        // 定义可以出现在标识符之后的Token类型集合
        const set<TokenTypeEnum> firstSet = { PLUS, TIMES, DIVIDE, RPAREN, SEMICOLON, MINUS };

        // 如果标识符后面跟着左括号，它是一个函数调用
        if (Lookahead(1).type == LPAREN)
        {
            return Function_Call(true);
        }
        // 否则，它是一个变量
        else if (firstSet.find(Lookahead(1).type) != firstSet.end())
        {
            auto varNode = make_shared<FactorNode>(Lookahead().value, true, Lookahead().s_lines, Lookahead().s_position, true);
            Match(ID);
            return varNode;
        }
    }
    // 检查因子是否是括号内的表达式
    else if (Lookahead().type == LPAREN)
    {
        bool isEMpty = Lookahead(-1).type == LPAREN;
        Match(LPAREN);
        auto exp = Expression(priority + 2);
        Match(RPAREN);
        isEMpty &= Lookahead().type == RPAREN;  //按位与，检测是否是无效括号
        if (isEMpty)
        {
            DFS_EXP_Decrease(exp); // 降低表达式中操作符的优先级
        }
        return exp;
    }
    // 检查因子是否是数字
    else if (Lookahead().type == NUM)
    {
        auto numNode = make_shared<FactorNode>(stoi(Lookahead().value), Lookahead().s_lines, Lookahead().s_position);
        Match(NUM);
        return numNode;
    }
    // 检查因子是否是负数
    else if (Lookahead().type == MINUS)
    {
        Match(MINUS);
        auto numNode = make_shared<FactorNode>((-stoi(Lookahead().value)), Lookahead().s_lines, Lookahead().s_position);
        Match(NUM);
        return numNode;
    }
    else
    {
        cout << "(语法)ERROR: Line: " << Lookahead().s_lines << "(" << Lookahead().s_position << ")  非法表达式" << endl;
        exit(0);
    }
    return nullptr; // 如果不是上述类型，返回空
}


// 解析函数调用
shared_ptr<ASTNode> Parser::Function_Call(bool isExp)
{
    // 创建函数调用节点，初始化它使用当前Token的值
    auto funNode = make_shared<FactorNode>(Lookahead().value, false, Lookahead().s_lines, Lookahead().s_position, isExp);

    // 匹配函数名（ID）、左括号和右括号
    Match(ID);
    Match(LPAREN);
    Match(RPAREN);

    // 返回函数调用节点
    return funNode;
}


// 递归减少表达式树中所有操作符节点的优先级
void Parser::DFS_EXP_Decrease(shared_ptr<ASTNode> node)
{
    // 如果节点为空，直接返回
    if (node == nullptr) return;

    // 如果节点是操作符类型
    if (node->Get_type() == tree_node::OPERATOR)
    {
        // 尝试将节点转换为操作符节点
        auto tmpNode = dynamic_pointer_cast<Operator>(node);
        if (tmpNode)
        {
            // 减少操作符的优先级
            tmpNode->Decrease_Priority();
        }
    }

    // 对所有子节点递归执行同样的操作
    for (shared_ptr<ASTNode> child : node->childNode)
    {
        DFS_EXP_Decrease(child);
    }
}


// 打印语法错误信息并退出程序
void Parser::Print_Error(Token errorToken, TokenTypeEnum expectedType)
{
    // 如果错误Token的行号和位置未定义，修正位置
    if (errorToken.s_lines == 0 && errorToken.s_position == 0)
    {
        int maxLine = tokenList[tokenList.size() - 1].s_lines + 1;
        errorToken.s_lines = maxLine;
        errorToken.s_position = 1;
    }

    // 打印错误信息，包括错误位置、预期Token类型和实际Token
    cout << "\n(语法)ERROR  |Line:" << errorToken.s_lines << "(" << errorToken.s_position
        << ")" << " 期待有一个 \"" << TokenTypeToString(expectedType) << "\"\t|但却识别到了 \"" << errorToken.value << "\"\n" << endl;

    exit(0); // 退出程序
}
