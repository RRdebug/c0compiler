#include "ast.h"

//֮��ļ�������������дaccept�������Ӧ�������
void ProgramNode::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void VarDefineNode::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void IdentifierNode::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void CustomFuncNode::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void ReturnTypeNode::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void ReturnNode::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void FuncBodyNode::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void CondStatementNode::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void ElseStatementNode::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void StatementSetNode::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void LoopStatementNode::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void ReadStatementNode::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void WriteStatementNode::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void AssignStatementNode::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void FactorNode::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void Operator::accept(Visitor& visitor) 
{
    visitor.visit(*this);
}

void Expression::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void AssistNode::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

//֮��ļ�������������дshow��չʾ��Ӧ������Ϣ
void ProgramNode::Show()
{
    cout << "������ڵ�";
}


void VarDefineNode::Show()
{
    cout << "��������";
}

void IdentifierNode::Show()
{
    string out = (type == IdentifierNode::VAR) ? "����" : "����";
    cout << out << "��ʶ��: " << idName;
}

void FactorNode::Show()
{
    if (facType == INT)
    {
        cout << "����" << value;
    }
    else if (facType == VAR)
    {
        cout << "����: " << id;
    }
    else if (facType == FUNCTION)
    {
        cout << "��������: " << id;
    }
}

void CustomFuncNode::Show()
{
    cout << "�Զ��庯��";
}

void ReturnTypeNode::Show()
{
    string out = (retType == tree_node::ReturnType::_VOID) ? "VOID" : "INT";
    cout << "����ֵ����: " << out;
}

void FuncBodyNode::Show()
{
    string out = isMain ? "��" : "";
    cout <<out<< "������";
}

void CondStatementNode::Show()
{
    cout << "�������";
}

void ElseStatementNode::Show()
{
    cout << "ELSE���";
}

void LoopStatementNode::Show()
{
    cout << "ѭ�����";
}

void ReadStatementNode::Show()
{
    cout << "�����,����: "<<id;
}

void WriteStatementNode::Show()
{
    cout << "д���";
}

void AssignStatementNode::Show()
{
    cout << "��ֵ���,��ֵΪ: "<<id;
}

void StatementSetNode::Show()
{
    cout << "�������";
}

void ReturnNode::Show()
{
    cout << "�������";
}

void Operator::Show()
{
    if (opType == Operator::ADD)
    {
        cout << " \"+\"  ���ȼ�: "<< priority;
    }
    else if (opType == Operator::SUBTRACT)
    {
        cout << " \"-\"  ���ȼ�: " << priority;
    }
    else if (opType == Operator::MULTIPLY)
    {
        cout << " \"*\"  ���ȼ�: " << priority;
    }
    else if (opType == Operator::DIVIDE)
    {
        cout << " \"/\"  ���ȼ�: " << priority;
    }
}

void Expression::Show()
{
    for (int i=0;i<expList.size();i++)
    {
        if (expList[i]->Get_type() == tree_node::OPERATOR)
        {
            auto opr = dynamic_pointer_cast<Operator>(expList[i]);
            opr->Show();
            cout << "  ";
        }
        else
        {
            auto fac = dynamic_pointer_cast<FactorNode>(expList[i]);
            fac->Show();
            cout << "  ";
        }
    }
}
