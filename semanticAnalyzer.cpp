#include "semanticAnalyzer.h"

//深搜，处理每种类型的节点，进行语义分析
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

//把每种类型的标识符插入符号表
void SemanticAnalyzer::visit(IdentifierNode& node)
{
    shared_ptr<Symbol> tmpSYMBOL;
    if (nowFunName == "0_GLOBAL")   //全局变量
    {
        tmpSYMBOL = make_shared<Symbol>(node.Get_Id_Name(), Symbol::GLOBAL_VAR, node.Get_Line(), node.Get_Postion());
    }
    else if (nowFunName == "0_MAIN")    //主函数
    {
        tmpSYMBOL = make_shared<Symbol>(node.Get_Id_Name(), Symbol::VAR, "0_MAIN", node.Get_Line(), node.Get_Postion(), tree_node::_VOID);
    }
    else        //自定义函数
    {
        //变量
        if (node.Get_Id_Type() == IdentifierNode::VAR)
        {
            tmpSYMBOL = make_shared<Symbol>(node.Get_Id_Name(), Symbol::VAR, nowFunName, node.Get_Line(), node.Get_Postion(), node.Get_Return_Type());
        }
        //函数
        else
        {
            tmpSYMBOL = make_shared<Symbol>(node.Get_Id_Name(), Symbol::FUNCTION, nowFunName, node.Get_Line(), node.Get_Postion(), node.Get_Return_Type());
        }
    }
    SymbolTable::GetInstance()->Add_Symbol(*tmpSYMBOL);
}

//处理赋值语句，更新左值类型
void SemanticAnalyzer::visit(AssignStatementNode& node)
{
    shared_ptr<RBTreeNode> aSymbol = SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id(), nowFunName);
    if (aSymbol)
    {
        nowLeftValue = tree_node::ReturnType::INT;
        aSymbol->Assignment();  //标记已经赋值了，其实也需要向return语句那也递归找所有路径
        aSymbol->UseSymbol();   //理论上只赋值不算使用过，但我想不出什么好的判断逻辑，就先这样吧
    }
    else
    {
        cout << "(语义)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion()
            << ")  未定义的标识符: " << node.Get_Id() << "\n\n";
        exit(0);
    }
    isPass = false;
}

//检测因子的标识符是否在声明前就使用了
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
                cout << "(语义)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  数据类型不匹配\n\n";
                exit(0);
            }
        }
        break;
    }
    case FactorNode::VAR:
    {
        if (nowLeftValue != tree_node::ReturnType::INT)
        {
            cout << "(语义)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  数据类型不匹配\n\n";
            exit(0);
        }
        auto rbTreeNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id_Name(), nowFunName);
        if (rbTreeNode == nullptr)
        {
            cout << "(语义)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  变量 \"" << node.Get_Id_Name() << "\" 未定义\n\n";
            exit(0);
        }
        else
        {
            rbTreeNode->UseSymbol();
            if (!rbTreeNode->Get_Date().Is_Assignment() && !isPass && rbTreeNode->Get_Date().Get_Function_Name()!="0_GLOBAL")
            {
                cout << "(语义)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  变量 \"" << node.Get_Id_Name() << "\" 没有初始值\n\n";
                exit(0);
            }
        }
        break;
    }
    case FactorNode::INT:
    {
        if (nowLeftValue != tree_node::ReturnType::INT)
        {
            cout << "(语义)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  数据类型不匹配\n\n";
            exit(0);
        }
        break;
    }
    default:
        break;
    }
}

//更新当前作用域
void SemanticAnalyzer::visit(FuncBodyNode& node)
{
    if (node.Is_Main())
    {
        nowRetValue = tree_node::ReturnType::_VOID;
        nowFunName = "0_MAIN";
        if (!isAlreadRet)
        {
            cout << "(语义)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  函数未返回任何值\n\n";
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

//更新当前返回值类型
void SemanticAnalyzer::visit(ReturnNode& node)
{
    if (nowRetValue == tree_node::INT&& node.childNode.empty())
    {
        cout << "(语义)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  函数未返回任何值\n\n";
        exit(0);
    }
    nowLeftValue = nowRetValue;
    isPass = false;
}

//更新待匹配的数据类型
void SemanticAnalyzer::visit(LoopStatementNode& node)
{
    nowLeftValue = tree_node::INT;
    isPass = false;
}

//更新待匹配的数据类型
void SemanticAnalyzer::visit(WriteStatementNode& node)
{
    nowLeftValue = tree_node::INT;
    isPass = false;
}

//更新待匹配的数据类型
void SemanticAnalyzer::visit(CondStatementNode& node)
{
    nowLeftValue = tree_node::INT;
    isPass = false;
}

//检测待赋值的标识符是否被定义
void SemanticAnalyzer::visit(ReadStatementNode& node)
{
    shared_ptr<RBTreeNode> aSymbol = SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id(), nowFunName);
    if (aSymbol)
    {
        aSymbol->Assignment();
    }
    if (!SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id(), nowFunName))
    {
        cout << "(语义)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  未定义的标识符: \"" << node.Get_Id() << "\" \n\n";
        exit(0);
    }
    isPass = true;  //标记已经被赋值了，跳过报错
}

//递归检查函数是否在所有分支都返回
bool SemanticAnalyzer::Check_Return_In_All_Paths(shared_ptr<ASTNode> node)
{
    if (node == nullptr) {
        return false;
    }

    if (node->Get_type() == tree_node::RETURN) {
        return true;
    }
    else if (node->Get_type() == tree_node::CONDSTATEMENT) {
        // 检查语句体和else语句体
        shared_ptr<ASTNode> statementBody = node->childNode[1]; // 假定语句体是第二个子节点
        shared_ptr<ASTNode> elseBody = node->childNode[2]; // 假定else语句体是第三个子节点

        bool hasReturnInStatementBody = Check_Return_In_All_Paths(statementBody);
        bool hasReturnInElseBody = elseBody ? Check_Return_In_All_Paths(elseBody) : true; // 如果没有else语句体，则默认为真

        return hasReturnInStatementBody && hasReturnInElseBody;
    }
    else if (node->Get_type() == tree_node::LOOPSTATEMENT)
    {
        return false;
    }
    else {
        // 对于其他类型的节点
        bool hasReturn = false;
        for (shared_ptr<ASTNode> child : node->childNode) {
            hasReturn |= Check_Return_In_All_Paths(child);   //按位或，只要找到返回语句即可
        }
        return hasReturn;
    }
}

//更新作用域，同时检查函数是否返回正确
void SemanticAnalyzer::visit(CustomFuncNode& node)
{
    auto retNode = dynamic_pointer_cast<ReturnTypeNode>(node.childNode[0]);
    auto idNode = dynamic_pointer_cast<IdentifierNode>(node.childNode[1]);
    if (!retNode || !idNode)
    {
        cout << "错误的指针转换(返回值类型)" << endl;
        exit(0);
    }
    nowRetValue = retNode->Get_Return_Type();
    nowFunName = idNode->Get_Id_Name();

    if (!isAlreadRet)
    {
        cout << "(语义)ERROR: Line: " << node.Get_Line() << "(" << node.Get_Postion() << ")  函数未返回任何值\n\n";
        exit(0);
    }
    if (nowRetValue != tree_node::ReturnType::_VOID)
    {
        isAlreadRet = false;
    }
}