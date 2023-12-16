/*
    �����﷨������ؽڵ㶨��
    ������������������Ż���Ŀ��������ɶ��õ��˳����﷨����
    ����ʹ���˷�����ģʽ���������Ԫ�أ�����ά������չ
*/

#ifndef AST
#define AST

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

//AST������ͺͷ���ֵ����
namespace tree_node
{
    enum NodeType
    {
        PROGRAM, VARDEFINE, VARIDENTIFIER, FACTOR, CUSTOMFUNC, RETURNTYPE, FUNCBODY, CONDSTATEMENT, ELSESTATEMENT, STATEMENTSET, LOOPSTATEMENT, RETURN, READSTATEMENT, WRITESTATEMENT, FUNCCALLSTATEMENT, ASSIGNSTATEMENT, OPERATOR, EXP,ASSIST
    };
    enum ReturnType
    {
        _VOID, INT
    };
}

class Visitor;

// AST�ڵ�����࣬��ΪҪ���������������������͵Ľ�㶼Ҫ�̳�����࣬ʹ�û���ָ��������
class ASTNode
{
public:
    vector<shared_ptr<ASTNode>> childNode;

    ASTNode() :lines(0), pos(0), m_type(tree_node::PROGRAM) {}  //���캯������¼һ��λ����Ϣ�����㱨��
    virtual void Show() { cout << "error_show"; };              //�ӿڣ�����չʾ��Ӧ������Ϣ
    virtual void accept(Visitor& visitor) = 0;                  //�ӿڣ����������ģʽ�Ķ�ʽ�����ô�������ߵ�visit����������������
    tree_node::NodeType Get_type() const { return m_type; }     //һϵ��GET������
    int Get_Line() const { return lines; }
    size_t Get_Postion() const { return pos; }
protected:
    tree_node::NodeType m_type;    //��¼�������
    int lines;                     //λ����Ϣ
    size_t pos;
};

// ����ڵ�
class ProgramNode : public ASTNode
{
public:
    ProgramNode() { m_type = tree_node::PROGRAM; }  //ÿ���ڵ�ʵ����ʱ���ö�Ӧ������
    void Show() override;                           //��д����ӿڣ�ûʲô��˵�ģ�����ʡ���ⲿ�ֵ�ע��
    void accept(Visitor& visitor) override;
};

// ��������ڵ�
class VarDefineNode : public ASTNode
{
public:
    VarDefineNode() { m_type = tree_node::VARDEFINE; }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// ������ʶ���ڵ�
class IdentifierNode : public ASTNode
{
public:
    enum IdType             //��ʶ�����ͣ�����/����
    {
        VAR = 0, FUNCTION
    };
    //���캯��,ʵ��������
    IdentifierNode(string id, IdentifierNode::IdType _type, tree_node::ReturnType ret, int line, size_t _pos) :idName(id), type(_type)
    {
        m_type = tree_node::VARIDENTIFIER;
        retType = ret;
        lines = line;
        pos = _pos;
    }
    //���캯����ʵ��������
    IdentifierNode(string id, IdentifierNode::IdType _type, int line, size_t _pos, string fname) :idName(id), type(_type),funName(fname)
    {
        m_type = tree_node::VARIDENTIFIER;
        retType = tree_node::ReturnType::_VOID;
        lines = line;
        pos = _pos;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    string Get_Id_Name() const { return idName; }                       //һϵ��GET����
    IdType Get_Id_Type() const { return type; }
    string Get_Function_Name() const { return funName; }
    tree_node::ReturnType Get_Return_Type() const { return retType; }
private:
    string funName;                 //�����򣨺����������ر�˵��: ȫ������Ϊ��"0_GLOBAL" , ������Ϊ: "0_MAIN"
    string idName;                  //��ʶ������
    IdType type;                    //��ʶ������
    tree_node::ReturnType retType;  //����ֵ����
};

// �Զ��庯���ڵ�
class CustomFuncNode : public ASTNode
{
public:
    CustomFuncNode(int _line, size_t _pos)
    {
        m_type = tree_node::CUSTOMFUNC;
        lines = _line;
        pos = _pos;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// ����ֵ�ڵ�
class ReturnTypeNode : public ASTNode
{
public:
    ReturnTypeNode(int line, size_t _pos, tree_node::ReturnType rt)
    {
        m_type = tree_node::RETURNTYPE;
        lines = line;
        pos = _pos;
        retType = rt;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    tree_node::ReturnType Get_Return_Type()const { return retType; }
private:
    tree_node::ReturnType retType;      //����ֵ����
};

class ReturnNode : public ASTNode
{
public:
    ReturnNode(int line, size_t _pos)
    {
        m_type = tree_node::RETURN;
        lines = line;
        pos = _pos;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// ������ڵ�
class FuncBodyNode : public ASTNode
{
public:
    FuncBodyNode(bool ism, int _line, size_t _pos) :isMain(ism)
    {
        m_type = tree_node::FUNCBODY;
        lines = _line;
        pos = _pos;
    }
    FuncBodyNode(const FuncBodyNode& node)
    {
        isMain = node.Is_Main();
        *this = node;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    bool Is_Main() const { return isMain; }
private:
    bool isMain;        //�����﷨���У���û�и��������������ý�㣬�ú��������һ������ֵ��ʾ������
};

// �������ڵ�
class CondStatementNode : public ASTNode
{
public:
    CondStatementNode() { m_type = tree_node::CONDSTATEMENT; }
    CondStatementNode(const CondStatementNode& node){*this = node;}
    void Show() override;
    void accept(Visitor& visitor) override;
};

// Else���ڵ�
class ElseStatementNode : public ASTNode
{
public:
    ElseStatementNode() { m_type = tree_node::ELSESTATEMENT; }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// ��伯�Ͻڵ�
class StatementSetNode : public ASTNode
{
public:
    StatementSetNode() { m_type = tree_node::STATEMENTSET; }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// ѭ�����ڵ�
class LoopStatementNode : public ASTNode
{
public:
    LoopStatementNode(int _line, size_t _pos) 
    { 
        m_type = tree_node::LOOPSTATEMENT;
        lines = _line;
        pos = _pos;
    }
    LoopStatementNode(const LoopStatementNode& node) { *this = node; }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// �����ڵ�
class ReadStatementNode : public ASTNode
{
public:
    ReadStatementNode(string _id, int _line, size_t _pos) :id(_id)
    {
        m_type = tree_node::READSTATEMENT;
        lines = _line;
        pos = _pos;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    string Get_Id() const { return id; }
private:
    string id;  //Ҫ����ı�ʶ������
};

// д���ڵ�
class WriteStatementNode : public ASTNode
{
public:
    WriteStatementNode() { m_type = tree_node::WRITESTATEMENT; }
    void Show() override;
    void accept(Visitor& visitor) override;
};

// ��ֵ���ڵ�
class AssignStatementNode : public ASTNode
{
public:
    AssignStatementNode(string _id, int _line, size_t _pos) :id(_id)
    {
        m_type = tree_node::ASSIGNSTATEMENT;
        lines = _line;
        pos = _pos;
    }
    AssignStatementNode(const AssignStatementNode& node)
    {
        *this = node;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    string Get_Id() const { return id; }
private:
    string id;  //��ֵ
};

//���ӽ��
class FactorNode : public ASTNode
{
public:
    enum FactorType     //�������ͣ���������������������
    {
        VAR, INT, FUNCTION
    };
    //���캯����INT�õ�
    FactorNode(int v, int _line, size_t _pos) :value(v), id("")
    {
        m_type = tree_node::NodeType::FACTOR;
        facType = INT;
        lines = _line;
        pos = _pos;
        isInExp = true;
    }
    //���캯����VAR��FUNCTION�õ�
    FactorNode(string s, bool isVar, int _line, size_t _pos, bool isexp) :id(s), value(0)
    {
        m_type = tree_node::NodeType::FACTOR;
        facType = isVar ? VAR : FUNCTION;
        lines = _line;
        pos = _pos;
        isInExp = isexp;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    FactorType Get_Factor_Type() const { return facType; }  //һϵ��GET����
    bool Get_IsInExp() const { return isInExp; }
    string Get_Id_Name() const { return id; }
    int Get_Value() const { return value; }
private:
    int value;              //�����int�����Ӧֵ
    string id;              //����Ǻ������߱���,��ʶ������s
    FactorType facType;     //��������
    bool isInExp;           //�жϺ��������Ƿ��ڱ��ʽ��
};

//���������
class Operator : public ASTNode
{
public:
    enum Op     //���������ͣ��Ӽ��˳�
    {
        ADD, SUBTRACT,      //���ȼ�: 1
        MULTIPLY, DIVIDE    //���ȼ�: 2
    };
    Operator(Op optype, int pri, int _line, size_t _pos) :opType(optype), priority(pri)
    {
        m_type = tree_node::OPERATOR;
        lines = _line;
        pos = _pos;
    }
    void Show() override;
    void accept(Visitor& visitor) override;
    void Decrease_Priority() { priority -= 2; }     //���Ͳ��������ȼ�
    int Get_Priority() { return priority; }         //��ȡ��ǰ���ȼ�
    Op Get_Operator_Type() { return opType; }      
private:
    Op opType;       //����������
    int priority;    //���ȼ�
};

//�沨����ʾ���ı��ʽ
class Expression :public ASTNode
{
public:
    Expression(vector<shared_ptr<ASTNode>> v) : expList(v) { m_type = tree_node::NodeType::EXP; }
    vector<shared_ptr<ASTNode>> expList;
    void Show() override;
    void accept(Visitor& visitor) override;
};

//������㣬���ڴ�������ʱȷ��while��תʱ��
class AssistNode : public ASTNode   
{
public:
    AssistNode(){ m_type = tree_node::NodeType::ASSIST; }
    void accept(Visitor& visitor) override;
};

//������ģʽ�����ࡣ������������������Ż����������ɵ��ֻ࣬��Ҫ�̳�Vistor����д��Ӧ��������ʵ�ִ����Ӧ�����߼���
class Visitor
{
public:
    virtual void visit(ReturnNode& node) = 0;
    virtual void visit(FuncBodyNode& node) = 0;
    virtual void visit(CondStatementNode& node) = 0;
    virtual void visit(LoopStatementNode& node) = 0;
    virtual void visit(WriteStatementNode& node) = 0;
    virtual void visit(AssignStatementNode& node) = 0;
    virtual void visit(ProgramNode& node) {};
    virtual void visit(VarDefineNode& node) {};
    virtual void visit(IdentifierNode& node) {};
    virtual void visit(CustomFuncNode& node) {};
    virtual void visit(ReturnTypeNode& node) {};
    virtual void visit(ElseStatementNode& node) {};
    virtual void visit(StatementSetNode& node) {};
    virtual void visit(ReadStatementNode& node) {};
    virtual void visit(FactorNode& node) {};
    virtual void visit(Operator& node) {}; 
    virtual void visit(Expression& node) {};
    virtual void visit(AssistNode& node) {};
};

#endif