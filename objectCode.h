/*
    ����Ŀ�����
    ���ɼ���ջʽָ��


    LIT 0 a	������ֵȡ��ջ����aΪ����ֵ
    LOD t a	������ֵȡ��ջ����aΪ��Ե�ַ��tΪ����
    STO t a	��ջ����������ĳ������Ԫ�У�aΪ��Ե�ַ��tΪ����
    CAL 0 a	���ú�����aΪ������ַ
    INT 0 a	������ջ��Ϊ�����õĹ��̿���a����Ԫ��������
    JMP 0 a	��������ת��a��ַ
    JPC 0 a	������ת����ջ��ֵΪ0������ת��a��ַ������˳��ִ��
    ADD 0 0	��ջ����ջ����ӣ�������ջԪ�أ����ֵ��ջ
    SUB 0 0	��ջ����ȥջ����������ջԪ�أ����ֵ��ջ
    MUL 0 0	��ջ������ջ����������ջԪ�أ����ֵ��ջ
    DIV 0 0	��ջ������ջ����������ջԪ�أ����ֵ��ջ
    RED 0 0	�������ж���һ����������ջ��
    WRT 0 0	ջ��ֵ�������Ļ������
    RET 0 0	�������ý�����,���ص��õ㲢��ջ

*/

#ifndef TAC
#define TAC

#include "parser.h"
#include "redBlackTree.h"
#include "stack.h"
#include <fstream>

//ָ����
class ObjectCode
{
    friend ostream& operator<<(ostream& os, const ObjectCode& symbol);
public:
    ObjectCode(string op, int l, int s, int i) :operate(op), layer(l), secondAdd(s), index(i){}
    ObjectCode(string op, int l, int i) :operate(op), layer(l), secondAdd(0), index(i){}
    void Set_Second_Address(int sec) { secondAdd = sec; }   //����ָ���еڶ���������ֵ
    string Get_Operate() const { return operate; }
private:
    string operate;     //�������磺LIT
    int layer;          //����
    int secondAdd;      //ָ���еڶ�������
    int index;          //��ǰָ�������
};

//Ŀ����������࣬�̳з�������
class ObjCodeController : public Visitor
{
public:
    ObjCodeController(): nowIndex(0),nowFunction(""){}
    void Do_CodeGeneration(shared_ptr<ASTNode>& node, bool printLock=false); //Ŀ���������
    void Show_Object_Code();                    //չʾĿ�����
    void OutputToFile(const string& filename);  //��Ŀ�����������ļ�
private:
    vector<ObjectCode> objCodeList;      //Ŀ���������
    int nowIndex;                        //��ǰ������
    //��¼if�����תλ�õ�ջ����Ϊ������Ƕ��if,����Ƕ��������Ƚ��������Ȼ�뵽����ջ����
    MyStack<int> indexStackFor_IF;      
    MyStack<int> indexStackFor_While;   //��ifһ��������while���
    MyStack<int> indexStackFor_JMP;     //while���Ҫ�������ط�����Ҫ����ջ
    string nowFunction;                 //��ǰ������

    //������ģʽ����д��Ӧ�Ĵ����߼�
    void visit(ProgramNode& node) override;
    void visit(CustomFuncNode& node) override;
    void visit(ReturnNode& node) override;
    void visit(FuncBodyNode& node) override;
    void visit(CondStatementNode& node) override;
    void visit(ElseStatementNode& node) override;
    void visit(LoopStatementNode& node) override;
    void visit(ReadStatementNode& node)override;
    void visit(WriteStatementNode& node) override;
    void visit(AssignStatementNode& node) override; 
    void visit(FactorNode& node) override;
    void visit(AssistNode& node) override;
    //���ɱ��ʽ��Ӧ��ջʽָ��
    void Process_Expression(shared_ptr<Expression> exp);
};

#endif
