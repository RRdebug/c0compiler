#include "codeOptimization.h"

//չʾ�����﷨��
void CodeOpt::Show_AST_BY_DFS(shared_ptr<ASTNode> _root, string prefix, bool hasSibling)
{
    cout << prefix << "|����";
    if (_root != nullptr)
        _root->Show();
    cout << "\n";
    if (hasSibling)
        cout << prefix << "|\n";
    else
        cout << prefix << "\n";

    string childPrefix = hasSibling ? prefix + "|       " : prefix + "        ";
    for (size_t i = 0; i < _root->childNode.size(); i++)
    {
        Show_AST_BY_DFS(_root->childNode[i], childPrefix, i + 1 < _root->childNode.size());
    }
}

//�����������������Ż�
void CodeOpt::Do_Optimization(shared_ptr<ASTNode>& node)
{
    if (node == nullptr)
    {
        return;
    }
    node->accept(*this);
    for (auto& child : node->childNode)
    {
        Do_Optimization(child);
    }
}

//����������ʽ��������ԭ���ʽ����
void CodeOpt::DFS_For_Expression(shared_ptr<ASTNode> node)
{
    if (node == nullptr)
    {
        return;
    }
    // ���ȱ���������
    if (node->childNode.size() > 0)
        DFS_For_Expression(node->childNode[0]);
    expList.push_back(node);
    // ������������
    if (node->childNode.size() > 1)
        DFS_For_Expression(node->childNode[1]);
}

//�ͷ��ӽ��ռ䣨����ָ����Զ��ͷţ�
void CodeOpt::Delete_All_Children(std::shared_ptr<ASTNode> node) {
    if (!node) {
        return; // ����ڵ�Ϊ�գ�ֱ�ӷ���
    }
    for (auto child : node->childNode) {
        Delete_All_Children(child);
    }
    node->childNode.clear();
}

//�����ʽ����ת��Ϊ�沨����ʾ��
void CodeOpt::Do_RPN(MyStack<shared_ptr<ASTNode>>& operatorStack, vector<shared_ptr<ASTNode>>& outputVec, int i)
{
    while (true)
    {
        if (operatorStack.empty())
        {
            operatorStack.push(expList[i]);     //��ǰ������ջΪ��ֱ����ջ
            break;
        }
        auto currentOp = dynamic_pointer_cast<Operator>(expList[i]);
        auto stackOp = dynamic_pointer_cast<Operator>(operatorStack.top());
        if (currentOp->Get_Priority() > stackOp->Get_Priority())
        {
            operatorStack.push(expList[i]);     //��ǰ���ȼ�����ջ�����ȼ�������ջ
            break;
        }
        else
        {
            outputVec.push_back(operatorStack.pop());   //����һֱ��ջ��Ԫ�ص�����ֱ��������������������һ��
        }
    }
}

void CodeOpt::Reverse_Polish_Notation()
{
    MyStack<shared_ptr<ASTNode>> operatorStack;     //����ջ�����ɺ�׺���ʽ
    vector<shared_ptr<ASTNode>> outputVec;
    for (int i = 0; i < expList.size(); i++)
    {
        if (i & 1)      //���������洢���ǲ�����
        {
            Do_RPN(operatorStack, outputVec, i);    //����ǲ�������Ҫ���������
        }
        else            //ż�������������
        {   
            outputVec.push_back(expList[i]);        //����ǲ�����ֱ�Ӵ�������
        }
    }

    while (!operatorStack.empty())
    {
        outputVec.push_back(operatorStack.pop());
    }
    expList = outputVec;
}

//�����۵�
void CodeOpt::Const_Determine(int index, bool& breakFlag, bool& isChanged)
{
    auto fac1 = dynamic_pointer_cast<FactorNode>(expList[index]);
    if (index + 2 >= expList.size()) return;    //�������û��Ԫ���ˣ�ֱ�ӷ���
    auto fac2 = dynamic_pointer_cast<FactorNode>(expList[index + 2]);
    //�����⵽������������
    if (fac1->Get_Factor_Type() == FactorNode::INT && fac2->Get_Factor_Type() == FactorNode::INT)
    {
        auto myOP = dynamic_pointer_cast<Operator>(expList[index + 1]);
        int myPriority = myOP->Get_Priority();
        //���������ȼ��Ƿ���ڵ�ǰ������
        if (index - 1 > 0)
        {
            auto priOp = dynamic_pointer_cast<Operator>(expList[index - 1]);
            if (priOp->Get_Priority() > myPriority) return;
        }
        //����ұ����ȼ��Ƿ���ڵ�ǰ������
        if (index + 3 < expList.size())
        {
            auto subOp = dynamic_pointer_cast<Operator>(expList[index + 3]);
            if (subOp->Get_Priority() > myPriority) return;
        }
        //�����ǰ���ȼ��������ߣ�������Խ��г����۵�
        int newValue = Const_Folding(fac1->Get_Value(), fac2->Get_Value(), myOP->Get_Operator_Type());
        auto valueNode = make_shared<FactorNode>(newValue, fac1->Get_Line(), fac1->Get_Postion());
        //���б�����Ӧ������Ԫ�أ��������ԭ����λ��
        expList.erase(expList.begin() + index, expList.begin() + index + 3);
        expList.insert(expList.begin() + index, valueNode);
        breakFlag = true;
        isChanged = true;
    }
}


void CodeOpt::Opt_Expression(shared_ptr<ASTNode> expRoot, bool is_Pop)
{
    expList.clear();    //��������б�������һ�����ʽ
    DFS_For_Expression(expRoot);        //������ʽÿ�����
    if (is_Pop)
        expList.pop_back();

    int myPriority = 0;         //��ǰ���ȼ�
    bool breakFlag = false;     //��ⲻ����Ҫ����ĵط��Ϳ�������ѭ��
    bool isChanged = false;     //�������Ƿ�����˳����۵�

    do
    {
        breakFlag = false;
        for (int i = 0; i < expList.size(); i += 2)
        {
            isChanged = false;
            Const_Determine(i, breakFlag, isChanged);   //���г����۵�
            if (isChanged) i -= 2;
        }
    } while (breakFlag);
}

//�����۵�
int CodeOpt::Const_Folding(int value1, int value2, Operator::Op opType)
{
    switch (opType)
    {
    case Operator::ADD:
        return value1 + value2;
        break;
    case Operator::SUBTRACT:
        return value1 - value2;
        break;
    case Operator::MULTIPLY:
        return value1 * value2;
        break;
    case Operator::DIVIDE:
        return value1 / value2;
        break;
    default:
        return 0;
        break;
    }
}

//�����д��䣬�ͶԱ��ʽ���д���
void CodeOpt::visit(WriteStatementNode& node)
{
    bool isEmpty = node.childNode.empty();
    auto wNode = std::make_shared<WriteStatementNode>(node);
    Opt_Expression(wNode);
    Delete_All_Children(wNode);
    if (!isEmpty)
    {
        Reverse_Polish_Notation();
        auto expNode = make_shared<Expression>(expList);
        node.childNode.clear();
        node.childNode.push_back(expNode);
    }
}

//�����ѭ����䣬�ͶԱ��ʽ���д���
void CodeOpt::visit(LoopStatementNode& node)
{
    auto lNode = std::make_shared<LoopStatementNode>(node);
    Opt_Expression(lNode->childNode[0], false);
    Delete_All_Children(lNode->childNode[0]);
    Reverse_Polish_Notation();
    auto expNode = make_shared<Expression>(expList);
    node.childNode.erase(node.childNode.begin());
    node.childNode.insert(node.childNode.begin(), expNode);
    //��������Ƿ�Ϊ���٣��������ɾ��ѭ�������������
    if (expList.size() == 1)
    {
        auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
        auto factorNode = dynamic_pointer_cast<FactorNode>(expNode->expList[0]);
        if (factorNode->Get_Factor_Type() == FactorNode::INT)
        {
            if (factorNode->Get_Value() == 0)
            {
                node.childNode[1]->childNode.clear();
            }
        }
    }
    auto assist = make_shared<AssistNode>();
    node.childNode[1]->childNode.push_back(assist);

}

//����Ƿ�����䣬�ͶԱ��ʽ���д���
void CodeOpt::visit(ReturnNode& node)
{
    bool isEmpty = node.childNode.empty();
    auto rNode = std::make_shared<ReturnNode>(node);
    Opt_Expression(rNode);
    Delete_All_Children(rNode);
    if (!isEmpty)
    {
        Reverse_Polish_Notation();
        auto expNode = make_shared<Expression>(expList);
        node.childNode.clear();
        node.childNode.push_back(expNode);
    }
}

//����ʶ�����������Ƿ�����ʹ�ù������ûʹ�ù�����ɾ����Ӧ������
void CodeOpt::visit(VarDefineNode& node)
{
    for (int i = 0; i < node.childNode.size(); i++)
    {
        auto varDefNode = static_pointer_cast<IdentifierNode>(node.childNode[i]);
        auto symbolNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(varDefNode->Get_Id_Name(), varDefNode->Get_Function_Name());
        if (!symbolNode->Get_Date().Get_IsUsed())
        {
            node.childNode.erase(node.childNode.begin() + i--);
        }
    }

}

//����Ǹ�ֵ��䣬�ͶԱ��ʽ���д���
void CodeOpt::visit(AssignStatementNode& node)
{
    auto assignNodePtr = std::make_shared<AssignStatementNode>(node);
    Opt_Expression(assignNodePtr);
    Delete_All_Children(assignNodePtr);
    Reverse_Polish_Notation();
    auto expNode = make_shared<Expression>(expList);
    node.childNode.clear();
    node.childNode.push_back(expNode);
}

//����ʶ�����������Ƿ�����ʹ�ù������ûʹ�ù�����ɾ����Ӧ������
void CodeOpt::visit(ProgramNode& node)
{
    for (int i = 1; i < node.childNode.size() - 1; i++)
    {
        string name = static_pointer_cast<IdentifierNode>(node.childNode[i]->childNode[1])->Get_Id_Name();
        auto symbolNode = SymbolTable::GetInstance()->Get_Function_Symbol(name, 0, 0);
        if (!symbolNode->Get_Date().Get_IsUsed())
        {
            node.childNode.erase(node.childNode.begin() + i--);
        }
    }
}

//ɾ��RETURN���֮�����������
void CodeOpt::visit(FuncBodyNode& node)
{
    Check_Return_In_All_Paths(node, &node);
}

//�����������䣬�ͶԱ��ʽ���д���
void CodeOpt::visit(CondStatementNode& node)
{
    auto lNode = std::make_shared<CondStatementNode>(node);
    Opt_Expression(lNode->childNode[0], false);
    Delete_All_Children(lNode->childNode[0]);
    Reverse_Polish_Notation();
    auto expNode = make_shared<Expression>(expList);
    node.childNode.erase(node.childNode.begin());
    node.childNode.insert(node.childNode.begin(), expNode);
    //��⵱ǰ�������Ƿ�����ж�����/��
    if (expList.size() == 1)
    {
        auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
        auto factorNode = dynamic_pointer_cast<FactorNode>(expNode->expList[0]);
        //������ʽΪ����
        if (factorNode->Get_Factor_Type() == FactorNode::INT)
        {
            //������ɾ��TRUE��֧
            if (factorNode->Get_Value() == 0)
            {
                node.childNode[1]->childNode.clear();
            }
            //������ɾ��FALSE��֧
            else
            {
                node.childNode[2]->childNode.clear();
            }
        }
    }
}

//ɾ��RETURN���֮�����������
void CodeOpt::Check_Return_In_All_Paths(ASTNode& currentNode, ASTNode* parentNode, int index)
{
    if (currentNode.Get_type() == tree_node::RETURN)
    {
        if (parentNode != nullptr && index != parentNode->childNode.size() - 1)
        {
            parentNode->childNode.erase(parentNode->childNode.begin() + index + 1, parentNode->childNode.end());
        }
        return;
    }

    for (int i = 0; i < currentNode.childNode.size(); ++i) {

        Check_Return_In_All_Paths(*currentNode.childNode[i], &currentNode, i);
    }
}