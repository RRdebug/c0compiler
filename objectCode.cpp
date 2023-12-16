#include "objectCode.h"

//���أ����ڴ����������cout,�ļ�������
ostream& operator<<(ostream& os, const ObjectCode& objCode)
{
    os << objCode.operate << " " << objCode.layer << " " << objCode.secondAdd << endl;
    return os;
}

//����Ŀ���������
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
    //����һ��RET��䱣֤������
    if (!printLock && (objCodeList.back().Get_Operate() != "RET"))
    {
        ObjectCode objCode("RET", 0, 0, nowIndex++);
        objCodeList.push_back(objCode);
    }
}

//չʾĿ�����
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
    //ָ��ĵ�һ��Ϊȫ�������ٿռ䣬ȫ������һ���ռ�洢��������ֵ��֮��Ŀռ�洢����
    ObjectCode objCodeInt("INT", 0, node.childNode[0]->childNode.size()+1, nowIndex++);
    objCodeList.push_back(objCodeInt);
    for (int i=0;i<node.childNode[0]->childNode.size();i++)
    {
        auto varDef = dynamic_pointer_cast<IdentifierNode>(node.childNode[0]->childNode[i]);
        if (varDef)
        {
            //���·��ű���Ϣ
            auto symbolNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(Symbol(varDef->Get_Id_Name(), Symbol::GLOBAL_VAR, 0, 0));
            symbolNode->Set_Index(i + 1);
        }
    }
    //��ת��MAIN������ڣ�Ŀǰ��֪������ʲô�ط���������Ϊ0
    ObjectCode objCode("JMP", 0 ,0, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(CustomFuncNode& node) 
{
    //����ϴκ�����û�з�����䣬���һ��������䣬��֤����ȷִ��
    if ((nowFunction != "") && (objCodeList.back().Get_Operate() != "RET"))
    {
        ObjectCode objCode("RET", 0, 0, nowIndex++);
        objCodeList.push_back(objCode);
    }  
    //���·��ű��������λ��
    auto idNode = dynamic_pointer_cast<IdentifierNode>(node.childNode[1]);
    auto symbolNode = SymbolTable::GetInstance()->Get_Function_Symbol(idNode->Get_Id_Name(), 0, 0);
    symbolNode->Set_Index(nowIndex);
    nowFunction = dynamic_pointer_cast<IdentifierNode>(node.childNode[1])->Get_Id_Name();
}

void ObjCodeController::visit(ReturnNode& node) 
{
    if (!node.childNode.empty())
    {
        //����ʱ�ȴ�����ʽ
        auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
        Process_Expression(expNode);
        //������ֵ�浽ȫ����
        ObjectCode objCode("STO", 0, 0, nowIndex++);
        objCodeList.push_back(objCode);
    }
    //�������
    ObjectCode objCode("RET", 0, 0, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(FuncBodyNode& node) 
{
    //�����main����
    if (node.Is_Main())
    {
        //����ϴκ�����û�з�����䣬���һ��������䣬��֤����ȷִ��
        if ((nowFunction != "")&& (objCodeList.back().Get_Operate() != "RET"))
        {
            ObjectCode objCode("RET", 0, 0, nowIndex++);
            objCodeList.push_back(objCode);
        }
        objCodeList[1].Set_Second_Address(nowIndex); //���µڶ��������ת��λ��
        nowFunction = "0_MAIN"; //����������
    }
    //Ϊ�������ٿռ�
    ObjectCode objCode("INT", 0, node.childNode[0]->childNode.size() + 3, nowIndex++);
    objCodeList.push_back(objCode);
    for (int i = 0; i < node.childNode[0]->childNode.size(); i++)
    {
        auto varDef = dynamic_pointer_cast<IdentifierNode>(node.childNode[0]->childNode[i]);
        if (varDef)
        {
            //���·��ű������Ϣ
            auto symbolNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(Symbol(varDef->Get_Id_Name(), Symbol::VAR, varDef->Get_Function_Name(), 0, 0, tree_node::INT));
            //�������ٿռ�ʱ�̶�ǰ�����ռ�Ϊ����̬����SL������̬����DL��������λ�ã�RA��
            symbolNode->Set_Index(i + 3);
        }
    }
}

void ObjCodeController::visit(CondStatementNode& node)
{
    auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
    //�ȴ�����ʽ
    Process_Expression(expNode);
    //ʹ��ջ��¼if������JPC��λ�ã���Ϊ������Ƕ��if,����Ƕ��������Ƚ��������Ȼ�뵽����ջ����
    indexStackFor_IF.push(nowIndex);
    //Ŀǰ��ȷ����ת���ģ���������
    ObjectCode objCode("JPC", 0, 0, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(ElseStatementNode& node)
{
   //��⵽else�������Ķ�Ӧ��if�����תλ��
    objCodeList[indexStackFor_IF.pop()].Set_Second_Address(nowIndex);
}


//while�����Ҫȷ��������תλ��
//������ʽֵΪFALSE��ת��λ�ã����ʽ����֮��
//��ѭ�����ִ�������������ת��while��俪ʼλ�ã����ʽ����֮ǰ���������жϣ���ʵ��ѭ��
void ObjCodeController::visit(LoopStatementNode& node) 
{
    indexStackFor_JMP.push(nowIndex);   //��������ת��λ��
    auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
    //���ɱ��ʽ
    Process_Expression(expNode);
    //false��֧�ߵ�λ��
    indexStackFor_While.push(nowIndex);
    ObjectCode objCode("JPC", 0, 0, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(AssistNode& node)
{
    //ʹ�õ�����������������޷���֪while�����ʽ���������һ����������־while���Ľ���
    ObjectCode objCode("JMP", 0, indexStackFor_JMP.pop(), nowIndex++);
    objCodeList.push_back(objCode);
    objCodeList[indexStackFor_While.pop()].Set_Second_Address(nowIndex);
}

void ObjCodeController::visit(ReadStatementNode& node) 
{
    //�ӷ��ű��ҵ���Ӧ�ı�ʶ��
    auto nowSymbol = SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id(), nowFunction);
    //�������ƫ����
    int layer = nowSymbol->Get_Date().Get_Function_Name() == "0_GLOBAL" ? 0 : 1;
    int index = nowSymbol->Get_Date().Get_Index();
    //�ȶ����ٴ洢
    ObjectCode objCode1("RED", 0, 0, nowIndex++);
    ObjectCode objCode2("STO", layer, index, nowIndex++);
    objCodeList.push_back(objCode1);
    objCodeList.push_back(objCode2);
}

void ObjCodeController::visit(WriteStatementNode& node) 
{
    if (node.childNode.empty()) return;
    auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
    //������ʽ��д��ջ��Ԫ��
    Process_Expression(expNode);
    ObjectCode objCode("WRT", 0, 0, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(AssignStatementNode& node) 
{
    //�ӷ��ű��ҵ���Ӧ�ı�ʶ��
    auto expNode = dynamic_pointer_cast<Expression>(node.childNode[0]);
    //������ʽ
    Process_Expression(expNode);
    auto symbolNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(node.Get_Id(), nowFunction);
    int layer = symbolNode->Get_Date().Get_Function_Name() == "0_GLOBAL" ? 0 : 1;
    int index = symbolNode->Get_Date().Get_Index();
    ObjectCode objCode("STO", layer, index, nowIndex++);
    objCodeList.push_back(objCode);
}

void ObjCodeController::visit(FactorNode& node)
{
    //����ֻ���������ü��ɣ�������������������㴦�����
    if(node.Get_Factor_Type()==FactorNode::FUNCTION && !node.Get_IsInExp())
    {
        //�Ȳ���ҵ���Ӧ���������
        auto symbolNode = SymbolTable::GetInstance()->Get_Function_Symbol(node.Get_Id_Name(), 0, 0);
        int index = symbolNode->Get_Date().Get_Index();
        ObjectCode objCode("CAL", 0, index, nowIndex++);
        objCodeList.push_back(objCode);
    }
}

//������ʽ����Ϊ�Ѿ�ת��Ϊ���沨����ʾ�������Բ�����Ҫ�������ȼ������⣬���ֱ������ɴ������
//�沨����ʾ�����ʽ�ļ�����ʵҲ��ʹ��ջ
//����⵽����������ջ����⵽��������ջ������Ԫ����ջ�������ջ
//���ǵ�ջʽָ���ִ�й���Ҳ��ʹ��ջ��ͬʱָ��Ĳ����߼�ǡ��ƥ���Ӧ�Ĺ���
//����ֱ������ʲô���ŵ�ջ��������
void ObjCodeController::Process_Expression(shared_ptr<Expression> expNode)
{
    for (const auto& exp : expNode->expList)
    {
        //���������
        if (exp->Get_type()==tree_node::FACTOR)
        {
            auto fNode = dynamic_pointer_cast<FactorNode>(exp);
            //�������������
            if(fNode->Get_Factor_Type()== FactorNode::INT)
            {
                ObjectCode objCode1("LIT", 0, fNode->Get_Value(), nowIndex++);
                objCodeList.push_back(objCode1);
            }
            //����Ǳ���
            else if (fNode->Get_Factor_Type() == FactorNode::VAR)
            {
                //����ű�
                auto symbolNode = SymbolTable::GetInstance()->Search_Symbol_In_Table(fNode->Get_Id_Name(), nowFunction);
                int layer = symbolNode->Get_Date().Get_Function_Name() == "0_GLOBAL" ? 0 : 1;
                int index = symbolNode->Get_Date().Get_Index();
                ObjectCode objCode2("LOD", layer, index, nowIndex++);
                objCodeList.push_back(objCode2);
            }
            //����Ǻ�������
            else if (fNode->Get_Factor_Type() == FactorNode::FUNCTION)
            {
                //����ű�
                auto symbolNode = SymbolTable::GetInstance()->Get_Function_Symbol(fNode->Get_Id_Name(), 0, 0);
                int index = symbolNode->Get_Date().Get_Index();
                ObjectCode objCode("CAL", 0, index, nowIndex++);
                objCodeList.push_back(objCode);
                ObjectCode objCode1("LOD", 0, 0, nowIndex++);
                objCodeList.push_back(objCode1);
            }
            //�������߲��������֧��������
            else
            {
                throw runtime_error("δ֪����");
            }
        }
        //����ǲ�����
        else
        {
            //���Ǽ��Ӽ��˳���
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

//���Ŀ����뵽�ļ������ں�������������
void ObjCodeController::OutputToFile(const string& filename) 
{
    ofstream outFile(filename);
    if (!outFile) 
    {
        cerr << "�޷����ļ�: " << filename << endl;
        return;
    }

    for (const auto& objCode : objCodeList) 
    {
        outFile << objCode; // ʹ�����ص� << �����
    }
    outFile.close();
}
