#include "redBlackTree.h"

//构造函数
Symbol::Symbol(string _name, SymbolType sType, string fName, int _line, size_t pos, tree_node::ReturnType rType)
{
    name = _name;
    type = sType;
    funName = fName;
    lines = _line;
    postion = pos;
    retType = rType;
    isAssignment = false;
    isUsed = false;
}

Symbol::Symbol(string _name, SymbolType sType, int _line, size_t pos)
{
    name = _name;
    type = sType;
    funName = "0_GLOBAL";
    lines = _line;
    postion = pos;
    isAssignment = false;
    isUsed = false;
}

//重载比较运算符主要是便于红黑树操作
bool Symbol::operator>(const Symbol& other) const {
    if (this->type != other.type)
    {
        return this->type < other.type;
    }
    else
    {
        if (this->funName != other.funName)
        {
            return this->funName < other.funName;
        }
        else
        {
            return this->name < other.name;
        }
    }
}

//重载比较运算符主要是便于红黑树操作
bool Symbol::operator<(const Symbol& other) const {
    if (this->type != other.type)
    {
        return this->type > other.type;
    }
    else
    {
        if (this->funName != other.funName)
        {
            return this->funName > other.funName;
        }
        else
        {
            return this->name > other.name;
        }
    }
}

//输出符号表
ostream& operator<<(std::ostream& os, const Symbol& symbol) {
    // 设置每列的宽度
    const short nameWidth = 20;
    const short typeWidth = 12;
    const short funNameWidth = 15;
    const short lineColWidth = 5;
    const short returnColWidth = 7;
    const short assColWidth = 6;

    // 打印分隔线(表头和数据之间)
    os << setw(2) << " ";
    os << setfill('-') << setw(nameWidth + typeWidth + funNameWidth + 2 * lineColWidth + 19+ returnColWidth+ assColWidth) << "-" << endl;
    os << setfill(' '); // 重设填充字符为默认的空格

    os <<"  |  "<< left<< std::setw(nameWidth) << symbol.name<< "| " << setw(typeWidth);

    switch (symbol.type) 
    {
    case Symbol::VAR: os << "变量"; break;
    case Symbol::FUNCTION: os << "函数"; break;
    case Symbol::GLOBAL_VAR: os << "全局变量"; break;
    }
    string retOut = (symbol.retType== tree_node::INT)? "int":"void";
    string isAss = (symbol.isAssignment) ? "是" : "否";
    if (symbol.type != Symbol::FUNCTION) retOut = "\/";
    else isAss = "\/";

    os << "| " << setw(funNameWidth) << symbol.funName
        << "| " << setw(lineColWidth) << symbol.lines
        << "| " << setw(lineColWidth) << symbol.postion
        << "| " << setw(returnColWidth) << retOut
        << "| " << setw(assColWidth) << isAss << "   | ";

    return os;
}

//添加符号到符号表
void SymbolTable::Add_Symbol(const Symbol& _symbol)
{
    if (!Is_Symbol_In_Table(_symbol))
    {
        sTable.Insert(_symbol);
    }
    else
    {
        string out = (_symbol.type == Symbol::VAR || _symbol.type == Symbol::GLOBAL_VAR) ? "变量" : "函数";
        cout << "\n(语义)ERROR  |Line:" << _symbol.lines << "(" << _symbol.postion << ")  重复定义" << out << " \"" << _symbol.name << "\"" << endl;
        exit(0);
    }
}

//检测某一符号是否在符号表示，返回布尔值
bool SymbolTable::Is_Symbol_In_Table(const Symbol& _symbol)
{
    if (sTable.Search(_symbol) != nullptr)
    {
        return true;
    }
    return false;
}

//搜索某一符号是否在符号表，返回结点的指针
shared_ptr<RBTreeNode> SymbolTable::Search_Symbol_In_Table(const Symbol& _symbol)
{
    return sTable.Search(_symbol);
}

//搜索某一符号是否在符号表，返回结点的指针，中序遍历实现的（很蠢）
shared_ptr<RBTreeNode> SymbolTable::Search_Symbol_In_Table(const string& name, const string& funName) 
{
    vector<shared_ptr<RBTreeNode>> retVec;
    sTable.In_Order_Helper(sTable.root, name, funName, retVec);
    for (const auto& node : retVec)
    {
        Symbol symbol = node->Get_Date();
        if (symbol.type == Symbol::VAR)
        {
            if (symbol.name == name && symbol.funName == funName)
            {
                return node;
            }
        }
        else if (symbol.type == Symbol::GLOBAL_VAR && symbol.name == name)
        {
            return node;
        }
    }
    return nullptr;
}

//搜索函数标识符
shared_ptr<RBTreeNode> SymbolTable::Get_Function_Symbol(const string& name,int line, size_t pos) 
{

    auto node = sTable.Search(Symbol(name, Symbol::FUNCTION, name, 0, 0, tree_node::INT));
    if (node != nullptr)
    {
        return node;
    }

    cout << "(语义)ERROR: Line: " << line << "(" << pos << ")  未找到函数: \"" << name << "\" 的定义\n\n";
    exit(0);
}


void SymbolTable::Show_Table()
{
    const short nameWidth = 20;
    const short typeWidth = 12;
    const short funNameWidth = 15;
    const short lineColWidth = 5;
    const short returnColWidth = 7;
    const short assColWidth = 6;
    cout  << "基于红黑树的符号表: \n\n" << setw(2) << " " << std::setfill('-') << std::setw(nameWidth + typeWidth + funNameWidth + 2 * lineColWidth + 19+ returnColWidth+ assColWidth) << "-" << endl;
    cout<<setfill(' ');
    cout <<"  |  " << left
        << setw(nameWidth) << "    标识符名字"
        << "| " << setw(typeWidth) << "   类型"
        << "| " << setw(funNameWidth) << "    函数名"
        << "| " << setw(lineColWidth) << " 行"
        << "| " << setw(lineColWidth) << " 列" 
        << "| " << setw(returnColWidth) << "返回值" 
        << "| " << setw(assColWidth) << "是否赋值 |" << std::endl;
    sTable.In_Order_Traversal();
    cout << setw(2) << " " << setfill('-') << setw(nameWidth + typeWidth + funNameWidth + 2 * lineColWidth +  19+ returnColWidth+ assColWidth) << "-" << endl;
}

//左旋
void RBTree::Left_Rotate(shared_ptr<RBTreeNode> x) 
{
    // y指向x的右子节点
    shared_ptr<RBTreeNode> y = x->right;

    // 将y的左子树挂载为x的右子树
    x->right = y->left;
    if (y->left != nullptr) 
    {
        y->left->parent = x;
    }

    // 将y提升至x的位置
    y->parent = x->parent;
    if (x->parent == nullptr) 
    {
        this->root = y;
    }
    else if (x == x->parent->left) 
    {
        x->parent->left = y;
    }
    else 
    {
        x->parent->right = y;
    }

    // 将x作为y的左子节点
    y->left = x;
    x->parent = y;
}

// 右旋转函数
void RBTree::Right_Rotate(shared_ptr<RBTreeNode> y) 
{
    // x指向y的左子节点
    shared_ptr<RBTreeNode> x = y->left;

    // 将x的右子树挂载为y的左子树
    y->left = x->right;
    if (x->right != nullptr) 
    {
        x->right->parent = y;
    }

    // 将x提升至y的位置
    x->parent = y->parent;
    if (y->parent == nullptr) 
    {
        this->root = x;
    }
    else if (y == y->parent->right) 
    {
        y->parent->right = x;
    }
    else 
    {
        y->parent->left = x;
    }

    // 将y作为x的右子节点
    x->right = y;
    y->parent = x;
}

// 插入后修复红黑树属性的函数
void RBTree::Insert_FixUp(shared_ptr<RBTreeNode> node)
{
    shared_ptr<RBTreeNode> parent, grandParent;

    // 循环直到node的父节点不是红色或node成为根节点
    while ((parent = node->parent) && parent->color == RED) 
    {
        grandParent = parent->parent; // 获取祖父节点

        // 父节点是祖父节点的左子节点的情况
        if (parent == grandParent->left) 
        {
            shared_ptr<RBTreeNode> uncle = grandParent->right; // 获取叔叔节点

            // 叔叔节点存在且为红色：颜色翻转
            if (uncle && uncle->color == RED) 
            {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandParent->color = RED;
                node = grandParent; // 将祖父节点设为当前节点，继续向上修复
            }
            else 
            {
                // 叔叔节点不存在或为黑色
                if (node == parent->right) 
                {
                    // node是右子节点：先对父节点左旋
                    Left_Rotate(parent);
                    node = parent;
                    parent = node->parent;
                }
                // 对祖父节点右旋并调整颜色
                Right_Rotate(grandParent);
                parent->color = BLACK;
                grandParent->color = RED;
            }
        }
        else 
        {
            // 父节点是祖父节点的右子节点的情况，与上面情况对称处理
            shared_ptr<RBTreeNode> uncle = grandParent->left;

            if (uncle && uncle->color == RED) 
            {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandParent->color = RED;
                node = grandParent;
            }
            else 
            {
                if (node == parent->left)
                {
                    Right_Rotate(parent);
                    node = parent;
                    parent = node->parent;
                }
                Left_Rotate(grandParent);
                parent->color = BLACK;
                grandParent->color = RED;
            }
        }
    }

    // 保证根节点为黑色
    this->root->color = BLACK;
}

//插入
void RBTree::Insert(Symbol symbol)
{
    shared_ptr<RBTreeNode> newNode = make_shared<RBTreeNode>(symbol);

    // 初始化用于遍历树的指针
    shared_ptr<RBTreeNode> y = nullptr;
    shared_ptr<RBTreeNode> x = this->root;

    // 遍历树以找到插入新节点的位置
    while (x != nullptr)
    {
        y = x;
        if (newNode->data < x->data)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }

    // 设置新节点的父节点
    newNode->parent = y;
    // 如果父节点为空，说明树为空，新节点成为根节点
    if (y == nullptr)
    {
        this->root = newNode;
    }
    // 否则将新节点作为父节点的左或右子节点
    else if (newNode->data < y->data)
    {
        y->left = newNode;
    }
    else
    {
        y->right = newNode;
    }

    // 新节点初始为红色，然后调用修复函数以维持红黑树性质
    newNode->color = RED;
    Insert_FixUp(newNode);
}

// 查找节点的函数
shared_ptr<RBTreeNode> RBTree::Search(const Symbol& _symbol) 
{
    shared_ptr<RBTreeNode> node = this->root;
    while (node != nullptr) 
    {
        if (_symbol < node->data) 
        {
            node = node->left;
        }
        else if (_symbol > node->data) 
        {
            node = node->right;
        }
        else 
        {
            return node;
        }
    }
    return nullptr;
}

void RBTree::In_Order_Helper(shared_ptr<RBTreeNode> node, const string& name, const string& funName, vector<shared_ptr<RBTreeNode>>& retVec)
{
    if (node != nullptr) 
    {
        In_Order_Helper(node->left, name, funName, retVec);
        if (name == node->data.name)
            retVec.push_back(node);
        In_Order_Helper(node->right, name, funName, retVec);
    }
}

// 中序遍历的辅助函数
void RBTree::In_Order_Helper(shared_ptr<RBTreeNode> node) 
{
    if (node != nullptr) 
    {
        In_Order_Helper(node->left);
        std::cout << node->data << " \n";
        In_Order_Helper(node->right);
    }
}

// 对外公开的中序遍历函数
void RBTree::In_Order_Traversal() 
{
    In_Order_Helper(this->root);
}
