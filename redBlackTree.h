/*
    符号表这边做的很蠢，如果有时间可能会重构下
    这个文件主要声明了四个类
    符号: 存储标识符的各种信息
    符号表: 使用红黑树构建符号表
    红黑树节点
    红黑树类
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

enum Color { RED, BLACK };  //红黑树结点颜色

class Symbol
{
    friend class RBTree;
    friend class SymbolTable;
    friend ostream& operator<<(ostream& os, const Symbol& symbol);
public:
    enum SymbolType { VAR, FUNCTION, GLOBAL_VAR };  //ERROR: 这里其实写的有些问题，全局变量靠作用域就能区分了，但已经写完了才发现问题，有时间重构吧
    Symbol(string _name, SymbolType sType, string fName, int _line, size_t pos, tree_node::ReturnType rType);
    Symbol(string _name, SymbolType sType, int _line, size_t pos);
    void Assignment() { isAssignment = true; }          //标志这个标识符已经被赋值了（语义分析说的）
    void UseSymbol() { isUsed = true; }                 //标志使用过这个标识符了（代码优化用的）
    void Set_Index(int _index) { index = _index; }      //最后生成栈式指令存储的位置的偏移
    bool Is_Assignment() { return isAssignment; }       //检测是否被赋值，其实也是GET方法
    int Get_Index() const { return index; }             //一系列的GET方法
    bool Get_IsUsed() const { return isUsed; }
    string Get_Function_Name() const { return funName; }
    tree_node::ReturnType Get_Return_Type() const { return retType; }
    bool operator>(const Symbol& other) const;  //重载比较运算符主要是便于红黑树操作
    bool operator<(const Symbol& other) const;
private:
    string name;        //标识符名字
    string funName;     //标识符作用域（函数名）
    SymbolType type;    //标识符类型
    tree_node::ReturnType retType;  //返回值类型 
    bool isAssignment;  //是否被辅助
    bool isUsed;        //是否被使用过
    int index;          //生成代码栈式指令的偏移量
    int lines;          //标识符所在的行
    size_t postion;     //标识符所在的列
};

class RBTreeNode {
    friend class RBTree;
public:
    RBTreeNode(Symbol symbol) : data(symbol), left(nullptr), right(nullptr), parent(nullptr), color(RED) {}
    Symbol Get_Date() const { return data; }
    void Assignment() { data.Assignment(); }        //每个节点对应一个符号，调用对应方法
    void UseSymbol() { data.UseSymbol(); }
    void Set_Index(int index) { data.Set_Index(index); }
private:
    shared_ptr<RBTreeNode> left, right, parent;     //左右子树，父结点
    Color color;        //结点颜色
    Symbol data;        //对应符号
};

class RBTree {
public:
    shared_ptr<RBTreeNode> root;    //红黑树根节点

    RBTree() : root(nullptr) {}


    shared_ptr<RBTreeNode> Search(const Symbol& _symbol);       //搜索一个符号
    //使用中序遍历查找符号
    void In_Order_Helper(shared_ptr<RBTreeNode> node, const string& name, const string& funName, vector<shared_ptr<RBTreeNode>>& retVec);
    void In_Order_Traversal();  //中序遍历对外的方法
    void Insert(Symbol symbol);                         //插入
private:
    void In_Order_Helper(shared_ptr<RBTreeNode> node);  //中序遍历
    void Left_Rotate(shared_ptr<RBTreeNode> x);         //左旋
    void Right_Rotate(shared_ptr<RBTreeNode> y);        //右旋
    void Insert_FixUp(shared_ptr<RBTreeNode> node);     //插入后修复颜色
};

class SymbolTable
{
public:
    /*
        单例模式
        文法比较简单，所以我只用了一张符号表
        为了方便访问符号表，我使用了单例模式
        为了减少复制开销，使用指针传递
    */
    static SymbolTable* GetInstance()   
    {
        static SymbolTable instance;    // 唯一实例
        return &instance;               // 返回指向实例的指针
    }

    void Add_Symbol(const Symbol& _symbol);             //添加元素到符号表
    bool Is_Symbol_In_Table(const Symbol& _symbol);     //检测某一符号是否在符号表示，返回布尔值
    shared_ptr<RBTreeNode> Search_Symbol_In_Table(const Symbol& _symbol);       //搜索某一符号是否在符号表，返回结点的指针
    shared_ptr<RBTreeNode> Search_Symbol_In_Table(const string& name, const string& funName);   //搜索某一符号是否在符号表，返回结点的指针
    shared_ptr<RBTreeNode> Get_Function_Symbol(const string& name,int line, size_t pos);        //搜索函数标识符
    void Show_Table();  //展示符号表

    // 禁止复制和赋值
    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

private:
    RBTree sTable;  //红黑树存符号表
    // 私有构造函数
    SymbolTable() {}
};

#endif 