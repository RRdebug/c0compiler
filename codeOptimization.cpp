#include "codeOptimization.h"

//展示抽象语法树
void CodeOpt::Show_AST_BY_DFS(shared_ptr<ASTNode> _root, string prefix, bool hasSibling)
{
    cout << prefix << "|——";
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

//遍历整颗树做代码优化
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

//中序遍历表达式子树，还原表达式序列
void CodeOpt::DFS_For_Expression(shared_ptr<ASTNode> node)
{
    if (node == nullptr)
    {
        return;
    }
    // 首先遍历左子树
    if (node->childNode.size() > 0)
        DFS_For_Expression(node->childNode[0]);
    expList.push_back(node);
    // 最后遍历右子树
    if (node->childNode.size() > 1)
        DFS_For_Expression(node->childNode[1]);
}

//释放子结点空间（智能指针会自动释放）
void CodeOpt::Delete_All_Children(std::shared_ptr<ASTNode> node) {
    if (!node) {
        return; // 如果节点为空，直接返回
    }
    for (auto child : node->childNode) {
        Delete_All_Children(child);
    }
    node->childNode.clear();
}

//将表达式序列转换为逆波兰表示法
void CodeOpt::Do_RPN(MyStack<shared_ptr<ASTNode>>& operatorStack, vector<shared_ptr<ASTNode>>& outputVec, int i)
{
    while (true)
    {
        if (operatorStack.empty())
        {
            operatorStack.push(expList[i]);     //当前操作符栈为空直接入栈
            break;
        }
        auto currentOp = dynamic_pointer_cast<Operator>(expList[i]);
        auto stackOp = dynamic_pointer_cast<Operator>(operatorStack.top());
        if (currentOp->Get_Priority() > stackOp->Get_Priority())
        {
            operatorStack.push(expList[i]);     //当前优先级大于栈顶优先级，则入栈
            break;
        }
        else
        {
            outputVec.push_back(operatorStack.pop());   //否则一直将栈顶元素弹出，直到满足上述条件的任意一条
        }
    }
}

void CodeOpt::Reverse_Polish_Notation()
{
    MyStack<shared_ptr<ASTNode>> operatorStack;     //借助栈来生成后缀表达式
    vector<shared_ptr<ASTNode>> outputVec;
    for (int i = 0; i < expList.size(); i++)
    {
        if (i & 1)      //奇数索引存储的是操作符
        {
            Do_RPN(operatorStack, outputVec, i);    //如果是操作符需要分情况讨论
        }
        else            //偶数索引存操作数
        {   
            outputVec.push_back(expList[i]);        //如果是操作数直接存入序列
        }
    }

    while (!operatorStack.empty())
    {
        outputVec.push_back(operatorStack.pop());
    }
    expList = outputVec;
}

//常量折叠
void CodeOpt::Const_Determine(int index, bool& breakFlag, bool& isChanged)
{
    auto fac1 = dynamic_pointer_cast<FactorNode>(expList[index]);
    if (index + 2 >= expList.size()) return;    //如果后续没有元素了，直接返回
    auto fac2 = dynamic_pointer_cast<FactorNode>(expList[index + 2]);
    //如果检测到两个连续常量
    if (fac1->Get_Factor_Type() == FactorNode::INT && fac2->Get_Factor_Type() == FactorNode::INT)
    {
        auto myOP = dynamic_pointer_cast<Operator>(expList[index + 1]);
        int myPriority = myOP->Get_Priority();
        //检测左边优先级是否大于当前操作符
        if (index - 1 > 0)
        {
            auto priOp = dynamic_pointer_cast<Operator>(expList[index - 1]);
            if (priOp->Get_Priority() > myPriority) return;
        }
        //检测右边优先级是否大于当前操作符
        if (index + 3 < expList.size())
        {
            auto subOp = dynamic_pointer_cast<Operator>(expList[index + 3]);
            if (subOp->Get_Priority() > myPriority) return;
        }
        //如果当前优先级大于两边，代表可以进行常量折叠
        int newValue = Const_Folding(fac1->Get_Value(), fac2->Get_Value(), myOP->Get_Operator_Type());
        auto valueNode = make_shared<FactorNode>(newValue, fac1->Get_Line(), fac1->Get_Postion());
        //从列表弹出对应的三个元素，结果插入原来的位置
        expList.erase(expList.begin() + index, expList.begin() + index + 3);
        expList.insert(expList.begin() + index, valueNode);
        breakFlag = true;
        isChanged = true;
    }
}


void CodeOpt::Opt_Expression(shared_ptr<ASTNode> expRoot, bool is_Pop)
{
    expList.clear();    //清除辅助列表，处理下一个表达式
    DFS_For_Expression(expRoot);        //处理表达式每个结点
    if (is_Pop)
        expList.pop_back();

    int myPriority = 0;         //当前优先级
    bool breakFlag = false;     //检测不到需要处理的地方就可以跳出循环
    bool isChanged = false;     //检测这次是否进行了常量折叠

    do
    {
        breakFlag = false;
        for (int i = 0; i < expList.size(); i += 2)
        {
            isChanged = false;
            Const_Determine(i, breakFlag, isChanged);   //进行常量折叠
            if (isChanged) i -= 2;
        }
    } while (breakFlag);
}

//常量折叠
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

//如果是写语句，就对表达式进行处理
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

//如果是循环语句，就对表达式进行处理
void CodeOpt::visit(LoopStatementNode& node)
{
    auto lNode = std::make_shared<LoopStatementNode>(node);
    Opt_Expression(lNode->childNode[0], false);
    Delete_All_Children(lNode->childNode[0]);
    Reverse_Polish_Notation();
    auto expNode = make_shared<Expression>(expList);
    node.childNode.erase(node.childNode.begin());
    node.childNode.insert(node.childNode.begin(), expNode);
    //检测条件是否为永假，如果是则删除循环语句所有内容
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

//如果是返回语句，就对表达式进行处理
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

//检测标识符（变量）是否曾经使用过，如果没使用过，则删除对应的声明
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

//如果是赋值语句，就对表达式进行处理
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

//检测标识符（函数）是否曾经使用过，如果没使用过，则删除对应的声明
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

//删除RETURN语句之后的所有内容
void CodeOpt::visit(FuncBodyNode& node)
{
    Check_Return_In_All_Paths(node, &node);
}

//如果是条件语句，就对表达式进行处理
void CodeOpt::visit(CondStatementNode& node)
{
    auto lNode = std::make_shared<CondStatementNode>(node);
    Opt_Expression(lNode->childNode[0], false);
    Delete_All_Children(lNode->childNode[0]);
    Reverse_Polish_Notation();
    auto expNode = make_shared<Expression>(expList);
    node.childNode.erase(node.childNode.begin());
    node.childNode.insert(node.childNode.begin(), expNode);
    //检测当前的条件是否可以判断永真/假
    if (expList.size() == 1)
    {
        auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
        auto factorNode = dynamic_pointer_cast<FactorNode>(expNode->expList[0]);
        //如果表达式为常量
        if (factorNode->Get_Factor_Type() == FactorNode::INT)
        {
            //永假则删除TRUE分支
            if (factorNode->Get_Value() == 0)
            {
                node.childNode[1]->childNode.clear();
            }
            //永真则删除FALSE分支
            else
            {
                node.childNode[2]->childNode.clear();
            }
        }
    }
}

//删除RETURN语句之后的所有内容
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