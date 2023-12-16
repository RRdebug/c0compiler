#include "ast.h"

//之后的几个函数都是重写accept，传入对应结点自身
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

//之后的几个函数都是重写show，展示对应结点的信息
void ProgramNode::Show()
{
    cout << "程序根节点";
}


void VarDefineNode::Show()
{
    cout << "变量声明";
}

void IdentifierNode::Show()
{
    string out = (type == IdentifierNode::VAR) ? "变量" : "函数";
    cout << out << "标识符: " << idName;
}

void FactorNode::Show()
{
    if (facType == INT)
    {
        cout << "整数" << value;
    }
    else if (facType == VAR)
    {
        cout << "变量: " << id;
    }
    else if (facType == FUNCTION)
    {
        cout << "函数调用: " << id;
    }
}

void CustomFuncNode::Show()
{
    cout << "自定义函数";
}

void ReturnTypeNode::Show()
{
    string out = (retType == tree_node::ReturnType::_VOID) ? "VOID" : "INT";
    cout << "返回值类型: " << out;
}

void FuncBodyNode::Show()
{
    string out = isMain ? "主" : "";
    cout <<out<< "函数体";
}

void CondStatementNode::Show()
{
    cout << "条件语句";
}

void ElseStatementNode::Show()
{
    cout << "ELSE语句";
}

void LoopStatementNode::Show()
{
    cout << "循环语句";
}

void ReadStatementNode::Show()
{
    cout << "读语句,存入: "<<id;
}

void WriteStatementNode::Show()
{
    cout << "写语句";
}

void AssignStatementNode::Show()
{
    cout << "赋值语句,左值为: "<<id;
}

void StatementSetNode::Show()
{
    cout << "语句序列";
}

void ReturnNode::Show()
{
    cout << "返回语句";
}

void Operator::Show()
{
    if (opType == Operator::ADD)
    {
        cout << " \"+\"  优先级: "<< priority;
    }
    else if (opType == Operator::SUBTRACT)
    {
        cout << " \"-\"  优先级: " << priority;
    }
    else if (opType == Operator::MULTIPLY)
    {
        cout << " \"*\"  优先级: " << priority;
    }
    else if (opType == Operator::DIVIDE)
    {
        cout << " \"/\"  优先级: " << priority;
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
