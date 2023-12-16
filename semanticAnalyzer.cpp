#include "semanticAnalyzer.h"

//���ѣ�����ÿ�����͵Ľڵ㣬�����������
void SemanticAnalyzer::Do_Semantic_Analyzer(shared_ptr<ASTNode> node)
{
    if (node == nullptr)
    {
        return;
    }
    node->accept(*this);
    for (auto& child : node->childNode)
    {
        Do_Semantic_Analyzer(child);
    }
}

void SemanticAnalyzer::Show_Symbol_Table()
{
    SymbolTable::GetInstance()->Show_Table();
}

//��ÿ�����͵ı�ʶ��������ű�
void SemanticAnalyzer::visit(IdentifierNode& node)
{
    shared_ptr<Symbol> tmpSYMBOL;
    if (nowFunName == "0_GLOBAL")   //ȫ�ֱ���
    {
        tmpSYMBOL = make_shared<Symbol>(node.Get_Id_Name(), Symbol::GLOBAL_VAR, node.Get_Line(), node.Get_Postion());
    }
    else if (nowFunName == "0_MAIN")    //������
    {
        tmpSYMBOL = make_shared<Symbol>(node.Get_Id_Name(), Symbol::VAR, "0_MAIN", node.Get_Line(), node.Get_Postion(), tree_node::_VOID);
    }
    else        //�Զ��庯��
    {
        //����
        if (node.Get_Id_Type() == IdentifierNode::VAR)
        {
            tmpSYMBOL = make_shared<Symbol>(node.Get_Id_Name(), Symbol::VAR, nowFunName, node.Get_Line(), node.Get_Postion(), node.Get_Return_Type());
        }
        //����
        else
        {
            tmpSYMBOL = make_shared<Symbol>(node.Get_Id_Name(), Symbol::FUNCTION, nowFunName, node.Get_Line(), node.Get_Postion(), node.Get_Return_Type());
        }
    }
    SymbolTable::GetInstance()->Add_Symbol(*tmpSYMBOL);
}

//����ֵ��䣬������ֵ����
void SemanticAnalyzer::visit(AssignStatementNode& node)
{
    shared_ptr<RBTreeNode> aSymbol = SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id(), nowFunName);
    if (aSymbol)
    {
        nowLeftValue = tree_node::ReturnType::INT;
        aSymbol->Assignment();  //����Ѿ���ֵ�ˣ���ʵҲ��Ҫ��return�����Ҳ�ݹ�������·��
        aSymbol->UseSymbol();   //������ֻ��ֵ����ʹ�ù��������벻��ʲô�õ��ж��߼�������������
    }
    else
    {
        cout << "(����)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion()
            << ")  δ����ı�ʶ��: " << node.Get_Id() << "\n\n";
        exit(0);
    }
    isPass = false;
}

//������ӵı�ʶ���Ƿ�������ǰ��ʹ����
void SemanticAnalyzer::visit(FactorNode& node)
{
    switch (node.Get_Factor_Type())
    {
    case FactorNode::FUNCTION:
    {
        auto rbNode = SymbolTable::GetInstance()->Get_Function_Symbol(node.Get_Id_Name(), node.Get_Line(), node.Get_Postion());
        if (!rbNode) return;
        rbNode->UseSymbol();
        if (node.Get_IsInExp())
        {
            if (nowLeftValue != rbNode->Get_Date().Get_Return_Type())
            {
                cout << "(����)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  �������Ͳ�ƥ��\n\n";
                exit(0);
            }
        }
        break;
    }
    case FactorNode::VAR:
    {
        if (nowLeftValue != tree_node::ReturnType::INT)
        {
            cout << "(����)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  �������Ͳ�ƥ��\n\n";
            exit(0);
        }
        auto rbTreeNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id_Name(), nowFunName);
        if (rbTreeNode == nullptr)
        {
            cout << "(����)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  ���� \"" << node.Get_Id_Name() << "\" δ����\n\n";
            exit(0);
        }
        else
        {
            rbTreeNode->UseSymbol();
            if (!rbTreeNode->Get_Date().Is_Assignment() && !isPass && rbTreeNode->Get_Date().Get_Function_Name()!="0_GLOBAL")
            {
                cout << "(����)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  ���� \"" << node.Get_Id_Name() << "\" û�г�ʼֵ\n\n";
                exit(0);
            }
        }
        break;
    }
    case FactorNode::INT:
    {
        if (nowLeftValue != tree_node::ReturnType::INT)
        {
            cout << "(����)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  �������Ͳ�ƥ��\n\n";
            exit(0);
        }
        break;
    }
    default:
        break;
    }
}

//���µ�ǰ������
void SemanticAnalyzer::visit(FuncBodyNode& node)
{
    if (node.Is_Main())
    {
        nowRetValue = tree_node::ReturnType::_VOID;
        nowFunName = "0_MAIN";
        if (!isAlreadRet)
        {
            cout << "(����)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  ����δ�����κ�ֵ\n\n";
            exit(0);
        }
    }
    else
    {
        auto funNode = make_shared<FuncBodyNode>(node);
        if (Check_Return_In_All_Paths(funNode))
        {
            isAlreadRet = true;
        }
    }
}

//���µ�ǰ����ֵ����
void SemanticAnalyzer::visit(ReturnNode& node)
{
    if (nowRetValue == tree_node::INT&& node.childNode.empty())
    {
        cout << "(����)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  ����δ�����κ�ֵ\n\n";
        exit(0);
    }
    nowLeftValue = nowRetValue;
    isPass = false;
}

//���´�ƥ�����������
void SemanticAnalyzer::visit(LoopStatementNode& node)
{
    nowLeftValue = tree_node::INT;
    isPass = false;
}

//���´�ƥ�����������
void SemanticAnalyzer::visit(WriteStatementNode& node)
{
    nowLeftValue = tree_node::INT;
    isPass = false;
}

//���´�ƥ�����������
void SemanticAnalyzer::visit(CondStatementNode& node)
{
    nowLeftValue = tree_node::INT;
    isPass = false;
}

//������ֵ�ı�ʶ���Ƿ񱻶���
void SemanticAnalyzer::visit(ReadStatementNode& node)
{
    shared_ptr<RBTreeNode> aSymbol = SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id(), nowFunName);
    if (aSymbol)
    {
        aSymbol->Assignment();
    }
    if (!SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id(), nowFunName))
    {
        cout << "(����)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  δ����ı�ʶ��: \"" << node.Get_Id() << "\" \n\n";
        exit(0);
    }
    isPass = true;  //����Ѿ�����ֵ�ˣ���������
}

//�ݹ��麯���Ƿ������з�֧������
bool SemanticAnalyzer::Check_Return_In_All_Paths(shared_ptr<ASTNode> node)
{
    if (node == nullptr) {
        return false;
    }

    if (node->Get_type() == tree_node::RETURN) {
        return true;
    }
    else if (node->Get_type() == tree_node::CONDSTATEMENT) {
        // ���������else�����
        shared_ptr<ASTNode> statementBody = node->childNode[1]; // �ٶ�������ǵڶ����ӽڵ�
        shared_ptr<ASTNode> elseBody = node->childNode[2]; // �ٶ�else������ǵ������ӽڵ�

        bool hasReturnInStatementBody = Check_Return_In_All_Paths(statementBody);
        bool hasReturnInElseBody = elseBody ? Check_Return_In_All_Paths(elseBody) : true; // ���û��else����壬��Ĭ��Ϊ��

        return hasReturnInStatementBody && hasReturnInElseBody;
    }
    else if (node->Get_type() == tree_node::LOOPSTATEMENT)
    {
        return false;
    }
    else {
        // �����������͵Ľڵ�
        bool hasReturn = false;
        for (shared_ptr<ASTNode> child : node->childNode) {
            hasReturn |= Check_Return_In_All_Paths(child);   //��λ��ֻҪ�ҵ�������伴��
        }
        return hasReturn;
    }
}

//����������ͬʱ��麯���Ƿ񷵻���ȷ
void SemanticAnalyzer::visit(CustomFuncNode& node)
{
    auto retNode = dynamic_pointer_cast<ReturnTypeNode>(node.childNode[0]);
    auto idNode = dynamic_pointer_cast<IdentifierNode>(node.childNode[1]);
    if (!retNode || !idNode)
    {
        cout << "�����ָ��ת��(����ֵ����)" << endl;
        exit(0);
    }
    nowRetValue = retNode->Get_Return_Type();
    nowFunName = idNode->Get_Id_Name();

    if (!isAlreadRet)
    {
        cout << "(����)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  ����δ�����κ�ֵ\n\n";
        exit(0);
    }
    if (nowRetValue != tree_node::ReturnType::_VOID)
    {
        isAlreadRet = false;
    }
}