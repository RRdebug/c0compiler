/*
    ���ű�������ĺܴ��������ʱ����ܻ��ع���
    ����ļ���Ҫ�������ĸ���
    ����: �洢��ʶ���ĸ�����Ϣ
    ���ű�: ʹ�ú�����������ű�
    ������ڵ�
    �������
*/


#ifndef RBTREE
#define RBTREE

#include <iostream>
#include<vector>
#include <iomanip>
#include "ast.h"

using namespace std;

class SymbolTable;
class RBTree;

enum Color { RED, BLACK };  //����������ɫ

class Symbol
{
    friend class RBTree;
    friend class SymbolTable;
    friend ostream& operator<<(ostream& os, const Symbol& symbol);
public:
    enum SymbolType { VAR, FUNCTION, GLOBAL_VAR };  //ERROR: ������ʵд����Щ���⣬ȫ�ֱ�������������������ˣ����Ѿ�д���˲ŷ������⣬��ʱ���ع���
    Symbol(string _name, SymbolType sType, string fName, int _line, size_t pos, tree_node::ReturnType rType);
    Symbol(string _name, SymbolType sType, int _line, size_t pos);
    void Assignment() { isAssignment = true; }          //��־�����ʶ���Ѿ�����ֵ�ˣ��������˵�ģ�
    void UseSymbol() { isUsed = true; }                 //��־ʹ�ù������ʶ���ˣ������Ż��õģ�
    void Set_Index(int _index) { index = _index; }      //�������ջʽָ��洢��λ�õ�ƫ��
    bool Is_Assignment() { return isAssignment; }       //����Ƿ񱻸�ֵ����ʵҲ��GET����
    int Get_Index() const { return index; }             //һϵ�е�GET����
    bool Get_IsUsed() const { return isUsed; }
    string Get_Function_Name() const { return funName; }
    tree_node::ReturnType Get_Return_Type() const { return retType; }
    bool operator>(const Symbol& other) const;  //���رȽ��������Ҫ�Ǳ��ں��������
    bool operator<(const Symbol& other) const;
private:
    string name;        //��ʶ������
    string funName;     //��ʶ�������򣨺�������
    SymbolType type;    //��ʶ������
    tree_node::ReturnType retType;  //����ֵ���� 
    bool isAssignment;  //�Ƿ񱻸���
    bool isUsed;        //�Ƿ�ʹ�ù�
    int index;          //���ɴ���ջʽָ���ƫ����
    int lines;          //��ʶ�����ڵ���
    size_t postion;     //��ʶ�����ڵ���
};

class RBTreeNode {
    friend class RBTree;
public:
    RBTreeNode(Symbol symbol) : data(symbol), left(nullptr), right(nullptr), parent(nullptr), color(RED) {}
    Symbol Get_Date() const { return data; }
    void Assignment() { data.Assignment(); }        //ÿ���ڵ��Ӧһ�����ţ����ö�Ӧ����
    void UseSymbol() { data.UseSymbol(); }
    void Set_Index(int index) { data.Set_Index(index); }
private:
    shared_ptr<RBTreeNode> left, right, parent;     //���������������
    Color color;        //�����ɫ
    Symbol data;        //��Ӧ����
};

class RBTree {
public:
    shared_ptr<RBTreeNode> root;    //��������ڵ�

    RBTree() : root(nullptr) {}


    shared_ptr<RBTreeNode> Search(const Symbol& _symbol);       //����һ������
    //ʹ������������ҷ���
    void In_Order_Helper(shared_ptr<RBTreeNode> node, const string& name, const string& funName, vector<shared_ptr<RBTreeNode>>& retVec);
    void In_Order_Traversal();  //�����������ķ���
    void Insert(Symbol symbol);                         //����
private:
    void In_Order_Helper(shared_ptr<RBTreeNode> node);  //�������
    void Left_Rotate(shared_ptr<RBTreeNode> x);         //����
    void Right_Rotate(shared_ptr<RBTreeNode> y);        //����
    void Insert_FixUp(shared_ptr<RBTreeNode> node);     //������޸���ɫ
};

class SymbolTable
{
public:
    /*
        ����ģʽ
        �ķ��Ƚϼ򵥣�������ֻ����һ�ŷ��ű�
        Ϊ�˷�����ʷ��ű���ʹ���˵���ģʽ
        Ϊ�˼��ٸ��ƿ�����ʹ��ָ�봫��
    */
    static SymbolTable* GetInstance()   
    {
        static SymbolTable instance;    // Ψһʵ��
        return &instance;               // ����ָ��ʵ����ָ��
    }

    void Add_Symbol(const Symbol& _symbol);             //���Ԫ�ص����ű�
    bool Is_Symbol_In_Table(const Symbol& _symbol);     //���ĳһ�����Ƿ��ڷ��ű�ʾ�����ز���ֵ
    shared_ptr<RBTreeNode> Search_Symbol_In_Table(const Symbol& _symbol);       //����ĳһ�����Ƿ��ڷ��ű����ؽ���ָ��
    shared_ptr<RBTreeNode> Search_Symbol_In_Table(const string& name, const string& funName);   //����ĳһ�����Ƿ��ڷ��ű����ؽ���ָ��
    shared_ptr<RBTreeNode> Get_Function_Symbol(const string& name,int line, size_t pos);        //����������ʶ��
    void Show_Table();  //չʾ���ű�

    // ��ֹ���ƺ͸�ֵ
    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

private:
    RBTree sTable;  //���������ű�
    // ˽�й��캯��
    SymbolTable() {}
};

#endif 