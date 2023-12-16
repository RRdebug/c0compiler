#include "parser.h"

using namespace token_type;


// ��ʾ�����﷨�������������������
void Parser::Show_AST_BY_DFS(shared_ptr<ASTNode> _root, string prefix, bool hasSibling)
{
    // ��ʾ��ǰ�ڵ�
    cout << prefix << "|����";
    if (_root != nullptr)
        _root->Show(); 
    cout << "\n";
    // �����Ƿ����ֵܽڵ��������Ƿ��ӡ�ָ���
    if (hasSibling) {
        cout << prefix << "|\n";
    }
    else {
        cout << prefix << "\n";
    }
    // �����ӽڵ��ǰ׺
    string childPrefix = hasSibling ? prefix + "|       " : prefix + "        "; 
    // �����ӽڵ㲢�ݹ���ú�������ʾ����
    for (size_t i = 0; i < _root->childNode.size(); i++)        
    {
        Show_AST_BY_DFS(_root->childNode[i], childPrefix, i + 1 < _root->childNode.size());
    }
}


// �鿴��ǰ��ָ��ƫ������Token
Token Parser::Lookahead(int offset) const
{
    // ���ƫ������Token�б�Χ�ڣ�������Ӧ��Token
    if (nowIndex + offset < tokenList.size())
    {
        return tokenList[nowIndex + offset];
    }
    // ���򷵻ؿ�Token
    return Token();
}


// ƥ��Ԥ�ڵ�Token����
void Parser::Match(TokenTypeEnum expectedType)
{
    // �����ǰ�鿴��Token������Ԥ��ƥ�䣬�ƶ�����һ��Token
    if (Lookahead().type == expectedType)
    {
        nowIndex++; // ����Token
    }
    else
    {
        // �����ƥ�䣬��ӡ������Ϣ
        Print_Error(Lookahead(), expectedType);
    }
}


// ����������ڵ㣬�������ڵ㲢�����������塢���������������
shared_ptr<ASTNode> Parser::Program()
{
    auto root = make_shared<ProgramNode>();
    Optional_Variable_Definitions(root, root);
    Optional_Function_Definitions(root);
    Main_Function(root);
    return root;
}


// ��ѡ�ı����������
void Parser::Optional_Variable_Definitions(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent)
{
    auto varDef = make_shared<VarDefineNode>();
    if (Lookahead().type == _INT && (Lookahead(2).type == COMMA || Lookahead(2).type == SEMICOLON)) //��ǰ���������������Ǳ������Ǻ���
    {
        Variable_Definition(varDef, rootParent);    //��������
    }
    root->childNode.push_back(varDef);              //���Ӹ��ڵ�
}


// ������������
void Parser::Variable_Definition(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent)
{
    Match(_INT);
    Token tmpToken = Lookahead();
    string parentName = " ";

    if (rootParent->Get_type() == tree_node::PROGRAM)       // ���ݸ��ڵ��������ֱ���������������
    {
        parentName = rootParent->childNode.empty() ? "0_GLOBAL" : "0_MAIN";
    }
    else if (rootParent->Get_type() == tree_node::CUSTOMFUNC)
    {
        auto funBody = dynamic_pointer_cast<IdentifierNode>(rootParent->childNode[1]);     //������ָ��ת��Ϊ: �����ʶ����ָ��
        parentName = funBody->Get_Id_Name();
    }
 
    auto vDef = make_shared<IdentifierNode>(tmpToken.value, IdentifierNode::VAR, tmpToken.s_lines, tmpToken.s_position, parentName);   // ������������ڵ�
    Match(ID);
    root->childNode.push_back(move(vDef));              //���Ӹ��ڵ�
    Subsequent_Variable_Definition(root, rootParent);   //�����������
}


// ���������ı�������
void Parser::Subsequent_Variable_Definition(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent)
{
    // �����һ��Token�Ƕ��ţ���ʾ�и����������
    if (Lookahead().type == COMMA)
    {
        Match(COMMA);                  // ƥ�䲢���Ķ���Token
        Token tmpToken = Lookahead();  // ��ȡ��ǰToken��Ϊ������

        // ���ݸ��ڵ���������ñ������������������
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

        // ���������ڵ㣬����������Ϣ
        auto vDef = make_shared<IdentifierNode>(tmpToken.value, IdentifierNode::VAR, tmpToken.s_lines, tmpToken.s_position, parentName);
        Match(ID);  // ƥ�䲢���ı�ʶ��Token
        root->childNode.push_back(move(vDef));                      // �������ڵ���ӵ�AST

        Subsequent_Variable_Definition(root, rootParent);           // �ݹ�����Խ��������������
    }
    else if (Lookahead().type == SEMICOLON)
    {
        Match(SEMICOLON);  // ��������ֺţ���ƥ�䲢���ģ���ʾ�����������
    }
    else
    {
        cout << "(�﷨)ERROR: Line: " << Lookahead().s_lines << "(" << Lookahead().s_position << ")  ��ʶ������δ��������" << endl;
        exit(0);
    }
}


// ������ѡ�ĺ�������
void Parser::Optional_Function_Definitions(shared_ptr<ASTNode> root)
{
    // ����һ����������ڵ�
    auto oFunDef = make_shared<CustomFuncNode>(Lookahead().s_lines, Lookahead().s_position);

    // �����ǰToken��_INT����ʾ��һ����������,�����ǰToken��_VOID������һ��Token��ID��ͬ����ʾ��һ����������
    if ((Lookahead().type == _INT)|| (Lookahead().type == _VOID && Lookahead(1).type == ID))
    {
        Function_Definition(oFunDef);           // ����Function_Definition��������
        root->childNode.push_back(oFunDef);     // ��Ӻ����ڵ㵽AST
        Optional_Function_Definitions(root);    // �ݹ�����Խ������ຯ������
    }
}


// ������������
void Parser::Function_Definition(shared_ptr<ASTNode> root)
{
    Token tmpToken = Lookahead();
    tree_node::ReturnType retType = tree_node::ReturnType::_VOID;

    // ���ݲ鿴����һ��Token��ȷ�������ķ�������
    if (Lookahead().type == _INT)
    {
        // �����һ��Token��_INT�������������ͽڵ㲢��ӵ����ڵ�
        auto _return = make_shared<ReturnTypeNode>(tmpToken.s_lines, tmpToken.s_position, tree_node::ReturnType::INT);
        root->childNode.push_back(_return);
        retType = tree_node::ReturnType::INT;
        Match(_INT);
    }
    else if (Lookahead().type == _VOID)
    {
        // �����һ��Token��_VOID��ͬ�������������ͽڵ�
        auto _return = make_shared<ReturnTypeNode>(tmpToken.s_lines, tmpToken.s_position, tree_node::ReturnType::_VOID);
        root->childNode.push_back(_return);
        retType = tree_node::ReturnType::_VOID;
        Match(_VOID);
    }


    // ��ȡ������
    tmpToken = Lookahead();
    auto funBody = make_shared<FuncBodyNode>(false, tmpToken.s_lines, tmpToken.s_position);
    auto vDef = make_shared<IdentifierNode>(tmpToken.value, IdentifierNode::FUNCTION, retType, tmpToken.s_lines, tmpToken.s_position);

    // ���������ͺ�������ӵ����ڵ�
    root->childNode.push_back(vDef);
    root->childNode.push_back(funBody);

    // ƥ�亯�����Ͳ����б������
    Match(ID);
    Match(LPAREN);
    Match(RPAREN);

    // ����������
    Subprogram(funBody, root);
}


// ����������
void Parser::Main_Function(shared_ptr<ASTNode> root)
{
    Token tmpToken = Lookahead();

    // ������������ڵ�
    auto funBody = make_shared<FuncBodyNode>(true, tmpToken.s_lines, tmpToken.s_position);

    // ��ӵ����ڵ�
    root->childNode.push_back(funBody);

    // ƥ��_void, _main�Ͳ����б����ţ�ȷ�����������Ķ���
    Match(_VOID);
    Match(_MAIN);
    Match(LPAREN);
    Match(RPAREN);

    // ������������
    Subprogram(funBody, root);
}


// �����ӳ���
void Parser::Subprogram(shared_ptr<ASTNode> root, shared_ptr<ASTNode> rootParent)
{
    // ƥ��������ţ���ʾ������Ŀ�ʼ
    Match(LBRACE);

    // �����������ڵĿ�ѡ��������
    Optional_Variable_Definitions(root, rootParent);

    // �����������
    Statement_Sequence(root);

    // ƥ���Ҵ����ţ���ʾ������Ľ���
    Match(RBRACE);
}


// �����������
void Parser::Statement_Sequence(shared_ptr<ASTNode> root)
{
    Statement(root);       // �����������
    More_Statements(root); // �ݹ�����������
}


// �����������䣬�ݹ����ֱ��û�и������
void Parser::More_Statements(shared_ptr<ASTNode> root)
{
    // First��
    const set<TokenTypeEnum> firstSet = { _IF, _WHILE, LBRACE, ID, _RETURN, _SCANF, _PRINTF, SEMICOLON };

    // �ݹ����
    if (firstSet.find(Lookahead().type) != firstSet.end())
    {
        Statement(root);
        More_Statements(root);
    }
}


// �����������
void Parser::Statement(shared_ptr<ASTNode> root)
{
    // ������һ��Token�����;�����ν������
    if (Lookahead().type == _IF)
    {
        Conditional_Statement(root);  // �����������
    }
    else if (Lookahead().type == _WHILE)
    {
        Loop_Statement(root);         // ����ѭ�����
    }
    else if (Lookahead().type == LBRACE)
    {
        Match(LBRACE);            // ƥ���������
        Statement_Sequence(root); // �����������ڵ��������
        Match(RBRACE);            // ƥ���Ҵ�����
    }
    else if (Lookahead().type == ID)
    {
        // �����������û�ֵ���
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
            cout << "�﷨��������";
        }
    }
    else if (Lookahead().type == _RETURN)
    {
        Return_Statement(root);  // �����������
    }
    else if (Lookahead().type == _SCANF)
    {
        Read_Statement(root);   // ������ȡ���
    }
    else if (Lookahead().type == _PRINTF)
    {
        Write_Statement(root);  // ����д�����
    }
    else if (Lookahead().type == SEMICOLON)
    {
        Match(SEMICOLON);      // ƥ�����䣨ֻ�зֺţ�
    }
}


// ����������䣨if��䣩
void Parser::Conditional_Statement(shared_ptr<ASTNode> root)
{
    // �����������ڵ�
    auto conStateNode = make_shared<CondStatementNode>();
    root->childNode.push_back(conStateNode);

    // ƥ��if�ؼ��ֺ�������
    Match(_IF);
    Match(LPAREN);

    // �����������ʽ
    auto exp = Expression(0);
    // ����������ʽΪ�գ��򱨴��˳�
    if (exp == nullptr)
    {
        cout << "(�﷨)ERROR: Line: " << Lookahead().s_lines << "(" << Lookahead().s_position << ")  if�������Ϊ��\n\n";
        exit(0);
    }
    conStateNode->childNode.push_back(exp); // ���������ʽ��ӵ��������ڵ�

    Match(RPAREN); // ƥ��������

    // ������伯�ڵ㣬������if�ڲ������
    auto stateNode = make_shared<StatementSetNode>();
    conStateNode->childNode.push_back(stateNode);
    Statement(stateNode);

    // ������ѡ��else���
    Optional_Else_Statement(conStateNode);
}


// ������ѡ��else���
void Parser::Optional_Else_Statement(shared_ptr<ASTNode> root)
{
    // ����else���ڵ�
    auto conNode = make_shared<ElseStatementNode>();
    root->childNode.push_back(conNode);

    // �����һ��Token��_else����ƥ��_else������else�ڲ������
    if (Lookahead().type == _ELSE)
    {
        Match(_ELSE);
        Statement(conNode);
    }
}


// ����ѭ����䣨while��䣩
void Parser::Loop_Statement(shared_ptr<ASTNode> root)
{
    // ����ѭ�����ڵ�
    auto conStateNode = make_shared<LoopStatementNode>(Lookahead().s_lines,Lookahead().s_position);
    root->childNode.push_back(conStateNode);

    // ƥ��while�ؼ��ֺ�������
    Match(_WHILE);
    Match(LPAREN);

    // �����������ʽ
    auto exp = Expression(0);
    // ����������ʽΪ�գ��򱨴��˳�
    if (exp == nullptr)
    {
        cout << "(�﷨)ERROR: Line: " << Lookahead().s_lines << "(" << Lookahead().s_position << ")  while�������Ϊ��\n\n";
        exit(0);
    }
    conStateNode->childNode.push_back(exp); // ���������ʽ��ӵ�ѭ�����ڵ�

    Match(RPAREN); // ƥ��������

    // ������伯�ڵ㣬������while�ڲ������
    auto stateNode = make_shared<StatementSetNode>();
    conStateNode->childNode.push_back(stateNode);
    Statement(stateNode);
}


// ������ֵ���
void Parser::Assignment_Statement(shared_ptr<ASTNode> root)
{
    // ������ֵ���ڵ�
    auto assNode = make_shared<AssignStatementNode>(Lookahead().value, Lookahead().s_lines, Lookahead().s_position);
    root->childNode.push_back(assNode);

    // ƥ�����ID�͸�ֵ����
    Match(ID);
    Match(ASSIGN);

    // ������ֵ���ʽ
    assNode->childNode.push_back(Expression(0));

    Match(SEMICOLON); // ƥ��ֺ�
}


// �����������
void Parser::Return_Statement(shared_ptr<ASTNode> root)
{
    // �������ؽڵ�
    auto retNode = make_shared<ReturnNode>(Lookahead().s_lines, Lookahead().s_position);
    root->childNode.push_back(retNode);

    // ƥ��return�ؼ���
    Match(_RETURN);

    // ������ѡ�ķ���ֵ���ʽ
    Optional_Expression(retNode);

    Match(SEMICOLON); // ƥ��ֺ�
}


// ������ѡ�ı��ʽ
void Parser::Optional_Expression(shared_ptr<ASTNode> root)
{
    // First��
    const set<TokenTypeEnum> firstSet = { PLUS, ID, LBRACE, ID, LPAREN, NUM, MINUS };

    // �������ʽ
    if (firstSet.find(Lookahead().type) != firstSet.end())
    {
        // �������ʽ��������ӵ���ǰ�ڵ���ӽڵ���
        root->childNode.push_back(Expression(0));
    }
}


// ������ȡ��䣨scanf��
void Parser::Read_Statement(shared_ptr<ASTNode> root)
{
    // ƥ��scanf�ؼ��֡�������
    Match(_SCANF);
    Match(LPAREN);

    // ������ȡ���ڵ㣬��������һ��Token������ID����ʼ��
    auto readNode = make_shared<ReadStatementNode>(Lookahead().value, Lookahead().s_lines, Lookahead().s_position);
    root->childNode.push_back(readNode); // ���ڵ���ӵ���ǰ���ڵ���ӽڵ���

    // ƥ�����ID�������źͷֺ�
    Match(ID);
    Match(RPAREN);
    Match(SEMICOLON);
}


// ����д����䣨printf��
void Parser::Write_Statement(shared_ptr<ASTNode> root)
{
    // ����д�����ڵ�
    auto printNode = make_shared<WriteStatementNode>();
    root->childNode.push_back(printNode); // ��ӵ����ڵ���ӽڵ���

    // ƥ��printf�ؼ��֡�������
    Match(_PRINTF);
    Match(LPAREN);

    // ������ѡ�ı��ʽ����Ϊprintf�Ĳ���
    Optional_Expression(printNode);

    // ƥ�������źͷֺ�
    Match(RPAREN);
    Match(SEMICOLON);
}


// �������ʽ
shared_ptr<ASTNode> Parser::Expression(int priority)
{
    // ���Ƚ���һ���term����������Ǳ��ʽ��һ����
    auto term = Term(priority);
    // �������ı��ʽ����
    auto subExp = Subsequent_Expression(term, priority);

    // ���û�����ı��ʽ���֣����س�ʼ������򷵻��������ʽ
    if (subExp == nullptr)
    {
        return term;
    }
    else
    {
        return subExp;
    }
}


// ���������ı��ʽ����Ҫ����Ӽ�����
shared_ptr<ASTNode> Parser::Subsequent_Expression(shared_ptr<ASTNode> child, int priority)
{
    // �����һ��Token�Ƿ��ǼӺ�
    if (Lookahead().type == PLUS)
    {
        // �����ӷ��������ڵ�
        auto op = make_shared<Operator>(Operator::ADD, 1 + priority, Lookahead().s_lines, Lookahead().s_position);
        Match(PLUS); // ƥ��Ӻ�

        auto term = Term(priority); // ��������������
        op->childNode.push_back(child); // ��ӵ�ǰ�ӽڵ�

        // �ݹ�����������ʽ
        auto subExp = Subsequent_Expression(term, priority);
        if (subExp != nullptr)
        {
            op->childNode.push_back(subExp);
        }
        else
        {
            op->childNode.push_back(term);
        }
        return op; // ���ؼӷ������ڵ�
    }
    else if (Lookahead().type == MINUS)
    {
        // ���������������ڵ�
        auto op = make_shared<Operator>(Operator::SUBTRACT, 1 + priority, Lookahead().s_lines, Lookahead().s_position);
        Match(MINUS); // ƥ�����

        auto term = Term(priority); // ��������������
        op->childNode.push_back(child); // ��ӵ�ǰ�ӽڵ�

        // �ݹ�����������ʽ
        auto subExp = Subsequent_Expression(term, priority);
        if (subExp != nullptr)
        {
            op->childNode.push_back(subExp);
        }
        else
        {
            op->childNode.push_back(term);
        }
        return op; // ���ؼ��������ڵ�
    }
    return nullptr; // ������ǼӼ����������ؿ�
}


// ��������������һ�����ӻ�һ�������˳������ĸ����ӱ��ʽ
shared_ptr<ASTNode> Parser::Term(int priority)
{
    // ���Ƚ���һ����������
    auto factor = Factor(priority);

    // Ȼ��ݹ���������˳������ĺ�������
    auto subTerm = Subsequent_Term(factor, priority);

    // ����к���������ظ�������򷵻ػ�������
    if (subTerm == nullptr)
    {
        return factor;
    }
    else
    {
        return subTerm;
    }
}


// ���������������Ҫ����˳�����
shared_ptr<ASTNode> Parser::Subsequent_Term(shared_ptr<ASTNode> child, int priority)
{
    // �����һ��Token�Ƿ��ǳ˺�
    if (Lookahead().type == TIMES)
    {
        // �����˷��������ڵ�
        auto op = make_shared<Operator>(Operator::MULTIPLY, 2 + priority, Lookahead().s_lines, Lookahead().s_position);
        Match(TIMES); // ƥ��˺�

        auto factor = Factor(priority); // ��������
        op->childNode.push_back(child); // ��ӵ�ǰ�ӽڵ�

        // �ݹ������������
        auto subTerm = Subsequent_Term(factor, priority);
        if (subTerm != nullptr)
        {
            op->childNode.push_back(subTerm);
        }
        else
        {
            op->childNode.push_back(factor);
        }
        return op; // ���س˷������ڵ�
    }
    // �����һ��Token�Ƿ��ǳ���
    else if (Lookahead().type == DIVIDE)
    {
        // ���������������ڵ�
        auto op = make_shared<Operator>(Operator::DIVIDE, 2 + priority, Lookahead().s_lines, Lookahead().s_position);
        Match(DIVIDE); // ƥ�����

        auto factor = Factor(priority); // ��������
        op->childNode.push_back(child); // ��ӵ�ǰ�ӽڵ�

        // �ݹ������������
        auto subTerm = Subsequent_Term(factor, priority);
        if (subTerm != nullptr)
        {
            op->childNode.push_back(subTerm);
        }
        else
        {
            op->childNode.push_back(factor);
        }
        return op; // ���س��������ڵ�
    }
    return nullptr; // ������ǳ˳����������ؿ�
}


// �������ӣ����ӿ����Ǳ��������ֻ������ڵı��ʽ
shared_ptr<ASTNode> Parser::Factor(int priority)
{
    // ��������Ƿ���һ����ʶ���������������ã�
    if (Lookahead().type == ID)
    {
        // ������Գ����ڱ�ʶ��֮���Token���ͼ���
        const set<TokenTypeEnum> firstSet = { PLUS, TIMES, DIVIDE, RPAREN, SEMICOLON, MINUS };

        // �����ʶ��������������ţ�����һ����������
        if (Lookahead(1).type == LPAREN)
        {
            return Function_Call(true);
        }
        // ��������һ������
        else if (firstSet.find(Lookahead(1).type) != firstSet.end())
        {
            auto varNode = make_shared<FactorNode>(Lookahead().value, true, Lookahead().s_lines, Lookahead().s_position, true);
            Match(ID);
            return varNode;
        }
    }
    // ��������Ƿ��������ڵı��ʽ
    else if (Lookahead().type == LPAREN)
    {
        bool isEMpty = Lookahead(-1).type == LPAREN;
        Match(LPAREN);
        auto exp = Expression(priority + 2);
        Match(RPAREN);
        isEMpty &= Lookahead().type == RPAREN;  //��λ�룬����Ƿ�����Ч����
        if (isEMpty)
        {
            DFS_EXP_Decrease(exp); // ���ͱ��ʽ�в����������ȼ�
        }
        return exp;
    }
    // ��������Ƿ�������
    else if (Lookahead().type == NUM)
    {
        auto numNode = make_shared<FactorNode>(stoi(Lookahead().value), Lookahead().s_lines, Lookahead().s_position);
        Match(NUM);
        return numNode;
    }
    // ��������Ƿ��Ǹ���
    else if (Lookahead().type == MINUS)
    {
        Match(MINUS);
        auto numNode = make_shared<FactorNode>((-stoi(Lookahead().value)), Lookahead().s_lines, Lookahead().s_position);
        Match(NUM);
        return numNode;
    }
    else
    {
        cout << "(�﷨)ERROR: Line: " << Lookahead().s_lines << "(" << Lookahead().s_position << ")  �Ƿ����ʽ" << endl;
        exit(0);
    }
    return nullptr; // ��������������ͣ����ؿ�
}


// ������������
shared_ptr<ASTNode> Parser::Function_Call(bool isExp)
{
    // �����������ýڵ㣬��ʼ����ʹ�õ�ǰToken��ֵ
    auto funNode = make_shared<FactorNode>(Lookahead().value, false, Lookahead().s_lines, Lookahead().s_position, isExp);

    // ƥ�亯������ID���������ź�������
    Match(ID);
    Match(LPAREN);
    Match(RPAREN);

    // ���غ������ýڵ�
    return funNode;
}


// �ݹ���ٱ��ʽ�������в������ڵ�����ȼ�
void Parser::DFS_EXP_Decrease(shared_ptr<ASTNode> node)
{
    // ����ڵ�Ϊ�գ�ֱ�ӷ���
    if (node == nullptr) return;

    // ����ڵ��ǲ���������
    if (node->Get_type() == tree_node::OPERATOR)
    {
        // ���Խ��ڵ�ת��Ϊ�������ڵ�
        auto tmpNode = dynamic_pointer_cast<Operator>(node);
        if (tmpNode)
        {
            // ���ٲ����������ȼ�
            tmpNode->Decrease_Priority();
        }
    }

    // �������ӽڵ�ݹ�ִ��ͬ���Ĳ���
    for (shared_ptr<ASTNode> child : node->childNode)
    {
        DFS_EXP_Decrease(child);
    }
}


// ��ӡ�﷨������Ϣ���˳�����
void Parser::Print_Error(Token errorToken, TokenTypeEnum expectedType)
{
    // �������Token���кź�λ��δ���壬����λ��
    if (errorToken.s_lines == 0 && errorToken.s_position == 0)
    {
        int maxLine = tokenList[tokenList.size() - 1].s_lines + 1;
        errorToken.s_lines = maxLine;
        errorToken.s_position = 1;
    }

    // ��ӡ������Ϣ����������λ�á�Ԥ��Token���ͺ�ʵ��Token
    cout << "\n(�﷨)ERROR  |Line:" << errorToken.s_lines << "(" << errorToken.s_position
        << ")" << " �ڴ���һ�� \"" << TokenTypeToString(expectedType) << "\"\t|��ȴʶ���� \"" << errorToken.value << "\"\n" << endl;

    exit(0); // �˳�����
}
