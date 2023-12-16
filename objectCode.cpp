#include "objectCode.h"

//重载，便于处理输出流（cout,文件操作）
ostream& operator<<(ostream& os, const ObjectCode& objCode)
{
    os << objCode.operate << " " << objCode.layer << " " << objCode.secondAdd << endl;
    return os;
}

//进行目标代码生成
void ObjCodeController::Do_CodeGeneration(shared_ptr<ASTNode>& node,bool printLock)
{
    if (node == nullptr)
    {
        return;
    }
    node->accept(*this);
    for (auto& child : node->childNode)
    {
        Do_CodeGeneration(child,true);
    }
    //最后加一个RET语句保证完整性
    if (!printLock && (objCodeList.back().Get_Operate() != "RET"))
    {
        ObjectCode objCode("RET", 0, 0, nowIndex++);
        objCodeList.push_back(objCode);
    }
}

//展示目标代码
void ObjCodeController::Show_Object_Code()
{
    int i = 0;
    for (const auto& obj : objCodeList)
    {
        cout << i ++ <<"  " << obj;
    }
}


void ObjCodeController::visit(ProgramNode& node)
{
    //指令的第一条为全局区开辟空间，全局区第一个空间存储函数返回值，之后的空间存储变量
    ObjectCode objCodeInt("INT", 0, node.childNode[0]->childNode.size()+1, nowIndex++);
    objCodeList.push_back(objCodeInt);
    for (int i=0;i<node.childNode[0]->childNode.size();i++)
    {
        auto varDef = dynamic_pointer_cast<IdentifierNode>(node.childNode[0]->childNode[i]);
        if (varDef)
        {
            //更新符号表信息
            auto symbolNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(Symbol(varDef->Get_Id_Name(), Symbol::GLOBAL_VAR, 0, 0));
            symbolNode->Set_Index(i + 1);
        }
    }
    //跳转到MAIN函数入口，目前不知道跳到什么地方，先设置为0
    ObjectCode objCode("JMP", 0 ,0, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(CustomFuncNode& node) 
{
    //如果上次函数体没有返回语句，则加一个返回语句，保证能正确执行
    if ((nowFunction != "") && (objCodeList.back().Get_Operate() != "RET"))
    {
        ObjectCode objCode("RET", 0, 0, nowIndex++);
        objCodeList.push_back(objCode);
    }  
    //更新符号表函数的入口位置
    auto idNode = dynamic_pointer_cast<IdentifierNode>(node.childNode[1]);
    auto symbolNode = SymbolTable::GetInstance()->Get_Function_Symbol(idNode->Get_Id_Name(), 0, 0);
    symbolNode->Set_Index(nowIndex);
    nowFunction = dynamic_pointer_cast<IdentifierNode>(node.childNode[1])->Get_Id_Name();
}

void ObjCodeController::visit(ReturnNode& node) 
{
    if (!node.childNode.empty())
    {
        //返回时先处理表达式
        auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
        Process_Expression(expNode);
        //将返回值存到全局区
        ObjectCode objCode("STO", 0, 0, nowIndex++);
        objCodeList.push_back(objCode);
    }
    //返回语句
    ObjectCode objCode("RET", 0, 0, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(FuncBodyNode& node) 
{
    //如果是main函数
    if (node.Is_Main())
    {
        //如果上次函数体没有返回语句，则加一个返回语句，保证能正确执行
        if ((nowFunction != "")&& (objCodeList.back().Get_Operate() != "RET"))
        {
            ObjectCode objCode("RET", 0, 0, nowIndex++);
            objCodeList.push_back(objCode);
        }
        objCodeList[1].Set_Second_Address(nowIndex); //更新第二条语句跳转的位置
        nowFunction = "0_MAIN"; //更新作用域
    }
    //为函数开辟空间
    ObjectCode objCode("INT", 0, node.childNode[0]->childNode.size() + 3, nowIndex++);
    objCodeList.push_back(objCode);
    for (int i = 0; i < node.childNode[0]->childNode.size(); i++)
    {
        auto varDef = dynamic_pointer_cast<IdentifierNode>(node.childNode[0]->childNode[i]);
        if (varDef)
        {
            //更新符号表相关信息
            auto symbolNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(Symbol(varDef->Get_Id_Name(), Symbol::VAR, varDef->Get_Function_Name(), 0, 0, tree_node::INT));
            //函数开辟空间时固定前三个空间为：静态链（SL）、动态链（DL）、返回位置（RA）
            symbolNode->Set_Index(i + 3);
        }
    }
}

void ObjCodeController::visit(CondStatementNode& node)
{
    auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
    //先处理表达式
    Process_Expression(expNode);
    //使用栈记录if语句调用JPC的位置，因为可能有嵌套if,对于嵌套语句是先进后出，自然想到了用栈处理
    indexStackFor_IF.push(nowIndex);
    //目前不确定跳转到哪，后续更改
    ObjectCode objCode("JPC", 0, 0, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(ElseStatementNode& node)
{
   //检测到else语句则更改对应的if语句跳转位置
    objCodeList[indexStackFor_IF.pop()].Set_Second_Address(nowIndex);
}


//while语句需要确定两个跳转位置
//如果表达式值为FALSE跳转的位置（表达式生成之后）
//和循环语句执行完后无条件跳转到while语句开始位置（表达式生成之前），重新判断，以实现循环
void ObjCodeController::visit(LoopStatementNode& node) 
{
    indexStackFor_JMP.push(nowIndex);   //无条件跳转的位置
    auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
    //生成表达式
    Process_Expression(expNode);
    //false分支走的位置
    indexStackFor_While.push(nowIndex);
    ObjectCode objCode("JPC", 0, 0, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(AssistNode& node)
{
    //使用的是先序遍历，我们无法得知while语句合适结束，设置一个辅助结点标志while语句的结束
    ObjectCode objCode("JMP", 0, indexStackFor_JMP.pop(), nowIndex++);
    objCodeList.push_back(objCode);
    objCodeList[indexStackFor_While.pop()].Set_Second_Address(nowIndex);
}

void ObjCodeController::visit(ReadStatementNode& node) 
{
    //从符号表找到对应的标识符
    auto nowSymbol = SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id(), nowFunction);
    //计算层差和偏移量
    int layer = nowSymbol->Get_Date().Get_Function_Name() == "0_GLOBAL" ? 0 : 1;
    int index = nowSymbol->Get_Date().Get_Index();
    //先读入再存储
    ObjectCode objCode1("RED", 0, 0, nowIndex++);
    ObjectCode objCode2("STO", layer, index, nowIndex++);
    objCodeList.push_back(objCode1);
    objCodeList.push_back(objCode2);
}

void ObjCodeController::visit(WriteStatementNode& node) 
{
    if (node.childNode.empty()) return;
    auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
    //处理表达式后写入栈顶元素
    Process_Expression(expNode);
    ObjectCode objCode("WRT", 0, 0, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(AssignStatementNode& node) 
{
    //从符号表找到对应的标识符
    auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
    //处理表达式
    Process_Expression(expNode);
    auto symbolNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id(), nowFunction);
    int layer = symbolNode->Get_Date().Get_Function_Name() == "0_GLOBAL" ? 0 : 1;
    int index = symbolNode->Get_Date().Get_Index();
    ObjectCode objCode("STO", layer, index, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(FactorNode& node)
{
    //这里只处理函数调用即可，其他的类型在其他结点处理过了
    if(node.Get_Factor_Type()==FactorNode::FUNCTION && !node.Get_IsInExp())
    {
        //先查表找到对应函数的入口
        auto symbolNode = SymbolTable::GetInstance()->Get_Function_Symbol(node.Get_Id_Name(), 0, 0);
        int index = symbolNode->Get_Date().Get_Index();
        ObjectCode objCode("CAL", 0, index, nowIndex++);
        objCodeList.push_back(objCode);
    }
}

//处理表达式，因为已经转换为了逆波兰表示法，所以不再需要考虑优先级的问题，单轮遍历即可处理完毕
//逆波兰表示法表达式的计算其实也是使用栈
//当检测到操作数则入栈，检测到操作符则栈顶两个元素退栈，结果入栈
//我们的栈式指令的执行过程也是使用栈，同时指令的操作逻辑恰好匹配对应的规则
//所以直接遇到什么都放到栈顶就行了
void ObjCodeController::Process_Expression(shared_ptr<Expression> expNode)
{
    for (const auto& exp : expNode->expList)
    {
        //如果是因子
        if (exp->Get_type()==tree_node::FACTOR)
        {
            auto fNode = dynamic_pointer_cast<FactorNode>(exp);
            //如果是整数常量
            if(fNode->Get_Factor_Type()== FactorNode::INT)
            {
                ObjectCode objCode1("LIT", 0, fNode->Get_Value(), nowIndex++);
                objCodeList.push_back(objCode1);
            }
            //如果是变量
            else if (fNode->Get_Factor_Type() == FactorNode::VAR)
            {
                //查符号表
                auto symbolNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(fNode->Get_Id_Name(), nowFunction);
                int layer = symbolNode->Get_Date().Get_Function_Name() == "0_GLOBAL" ? 0 : 1;
                int index = symbolNode->Get_Date().Get_Index();
                ObjectCode objCode2("LOD", layer, index, nowIndex++);
                objCodeList.push_back(objCode2);
            }
            //如果是函数调用
            else if (fNode->Get_Factor_Type() == FactorNode::FUNCTION)
            {
                //查符号表
                auto symbolNode = SymbolTable::GetInstance()->Get_Function_Symbol(fNode->Get_Id_Name(), 0, 0);
                int index = symbolNode->Get_Date().Get_Index();
                ObjectCode objCode("CAL", 0, index, nowIndex++);
                objCodeList.push_back(objCode);
                ObjectCode objCode1("LOD", 0, 0, nowIndex++);
                objCodeList.push_back(objCode1);
            }
            //理论上走不到这个分支，兜个底
            else
            {
                throw runtime_error("未知错误");
            }
        }
        //如果是操作符
        else
        {
            //就是检测加减乘除了
            auto OpNode = dynamic_pointer_cast<Operator>(exp);
            if (OpNode->Get_Operator_Type() == Operator::ADD)
            {
                ObjectCode objCode("ADD", 0, 0, nowIndex++);
                objCodeList.push_back(objCode);
            }
            else if (OpNode->Get_Operator_Type() == Operator::SUBTRACT)
            {
                ObjectCode objCode("SUB", 0, 0, nowIndex++);
                objCodeList.push_back(objCode);
            }
            else if (OpNode->Get_Operator_Type() == Operator::MULTIPLY)
            {
                ObjectCode objCode("MUL", 0, 0, nowIndex++);
                objCodeList.push_back(objCode);
            }
            else if (OpNode->Get_Operator_Type() == Operator::DIVIDE)
            {
                ObjectCode objCode("DIV", 0, 0, nowIndex++);
                objCodeList.push_back(objCode);
            }
        }
    }
    
}

//输出目标代码到文件，便于后续解释器处理
void ObjCodeController::OutputToFile(const string& filename) 
{
    ofstream outFile(filename);
    if (!outFile) 
    {
        cerr << "无法打开文件: " << filename << endl;
        return;
    }

    for (const auto& objCode : objCodeList) 
    {
        outFile << objCode; // 使用重载的 << 运算符
    }
    outFile.close();
}
