#include "redBlackTree.h"

//���캯��
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

//���رȽ��������Ҫ�Ǳ��ں��������
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

//���رȽ��������Ҫ�Ǳ��ں��������
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

//������ű�
ostream& operator<<(std::ostream& os, const Symbol& symbol) {
    // ����ÿ�еĿ��
    const short nameWidth = 20;
    const short typeWidth = 12;
    const short funNameWidth = 15;
    const short lineColWidth = 5;
    const short returnColWidth = 7;
    const short assColWidth = 6;

    // ��ӡ�ָ���(��ͷ������֮��)
    os << setw(2) << " ";
    os << setfill('-') << setw(nameWidth + typeWidth + funNameWidth + 2 * lineColWidth + 19+ returnColWidth+ assColWidth) << "-" << endl;
    os << setfill(' '); // ��������ַ�ΪĬ�ϵĿո�

    os <<"  |  "<< left<< std::setw(nameWidth) << symbol.name<< "| " << setw(typeWidth);

    switch (symbol.type) 
    {
    case Symbol::VAR: os << "����"; break;
    case Symbol::FUNCTION: os << "����"; break;
    case Symbol::GLOBAL_VAR: os << "ȫ�ֱ���"; break;
    }
    string retOut = (symbol.retType== tree_node::INT)? "int":"void";
    string isAss = (symbol.isAssignment) ? "��" : "��";
    if (symbol.type != Symbol::FUNCTION) retOut = "\/";
    else isAss = "\/";

    os << "| " << setw(funNameWidth) << symbol.funName
        << "| " << setw(lineColWidth) << symbol.lines
        << "| " << setw(lineColWidth) << symbol.postion
        << "| " << setw(returnColWidth) << retOut
        << "| " << setw(assColWidth) << isAss << "   | ";

    return os;
}

//��ӷ��ŵ����ű�
void SymbolTable::Add_Symbol(const Symbol& _symbol)
{
    if (!Is_Symbol_In_Table(_symbol))
    {
        sTable.Insert(_symbol);
    }
    else
    {
        string out = (_symbol.type == Symbol::VAR || _symbol.type == Symbol::GLOBAL_VAR) ? "����" : "����";
        cout << "\n(����)ERROR  |Line:" << _symbol.lines << "(" << _symbol.postion << ")  �ظ�����" << out << " \"" << _symbol.name << "\"" << endl;
        exit(0);
    }
}

//���ĳһ�����Ƿ��ڷ��ű�ʾ�����ز���ֵ
bool SymbolTable::Is_Symbol_In_Table(const Symbol& _symbol)
{
    if (sTable.Search(_symbol) != nullptr)
    {
        return true;
    }
    return false;
}

//����ĳһ�����Ƿ��ڷ��ű����ؽ���ָ��
shared_ptr<RBTreeNode> SymbolTable::Search_Symbol_In_Table(const Symbol& _symbol)
{
    return sTable.Search(_symbol);
}

//����ĳһ�����Ƿ��ڷ��ű����ؽ���ָ�룬�������ʵ�ֵģ��ܴ���
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

//����������ʶ��
shared_ptr<RBTreeNode> SymbolTable::Get_Function_Symbol(const string& name,int line, size_t pos) 
{

    auto node = sTable.Search(Symbol(name, Symbol::FUNCTION, name, 0, 0, tree_node::INT));
    if (node != nullptr)
    {
        return node;
    }

    cout << "(����)ERROR: Line: " << line << "(" << pos << ")  δ�ҵ�����: \"" << name << "\" �Ķ���\n\n";
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
    cout  << "���ں�����ķ��ű�: \n\n" << setw(2) << " " << std::setfill('-') << std::setw(nameWidth + typeWidth + funNameWidth + 2 * lineColWidth + 19+ returnColWidth+ assColWidth) << "-" << endl;
    cout<<setfill(' ');
    cout <<"  |  " << left
        << setw(nameWidth) << "    ��ʶ������"
        << "| " << setw(typeWidth) << "   ����"
        << "| " << setw(funNameWidth) << "    ������"
        << "| " << setw(lineColWidth) << " ��"
        << "| " << setw(lineColWidth) << " ��" 
        << "| " << setw(returnColWidth) << "����ֵ" 
        << "| " << setw(assColWidth) << "�Ƿ�ֵ |" << std::endl;
    sTable.In_Order_Traversal();
    cout << setw(2) << " " << setfill('-') << setw(nameWidth + typeWidth + funNameWidth + 2 * lineColWidth +  19+ returnColWidth+ assColWidth) << "-" << endl;
}

//����
void RBTree::Left_Rotate(shared_ptr<RBTreeNode> x) 
{
    // yָ��x�����ӽڵ�
    shared_ptr<RBTreeNode> y = x->right;

    // ��y������������Ϊx��������
    x->right = y->left;
    if (y->left != nullptr) 
    {
        y->left->parent = x;
    }

    // ��y������x��λ��
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

    // ��x��Ϊy�����ӽڵ�
    y->left = x;
    x->parent = y;
}

// ����ת����
void RBTree::Right_Rotate(shared_ptr<RBTreeNode> y) 
{
    // xָ��y�����ӽڵ�
    shared_ptr<RBTreeNode> x = y->left;

    // ��x������������Ϊy��������
    y->left = x->right;
    if (x->right != nullptr) 
    {
        x->right->parent = y;
    }

    // ��x������y��λ��
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

    // ��y��Ϊx�����ӽڵ�
    x->right = y;
    y->parent = x;
}

// ������޸���������Եĺ���
void RBTree::Insert_FixUp(shared_ptr<RBTreeNode> node)
{
    shared_ptr<RBTreeNode> parent, grandParent;

    // ѭ��ֱ��node�ĸ��ڵ㲻�Ǻ�ɫ��node��Ϊ���ڵ�
    while ((parent = node->parent) && parent->color == RED) 
    {
        grandParent = parent->parent; // ��ȡ�游�ڵ�

        // ���ڵ����游�ڵ�����ӽڵ�����
        if (parent == grandParent->left) 
        {
            shared_ptr<RBTreeNode> uncle = grandParent->right; // ��ȡ����ڵ�

            // ����ڵ������Ϊ��ɫ����ɫ��ת
            if (uncle && uncle->color == RED) 
            {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandParent->color = RED;
                node = grandParent; // ���游�ڵ���Ϊ��ǰ�ڵ㣬���������޸�
            }
            else 
            {
                // ����ڵ㲻���ڻ�Ϊ��ɫ
                if (node == parent->right) 
                {
                    // node�����ӽڵ㣺�ȶԸ��ڵ�����
                    Left_Rotate(parent);
                    node = parent;
                    parent = node->parent;
                }
                // ���游�ڵ�������������ɫ
                Right_Rotate(grandParent);
                parent->color = BLACK;
                grandParent->color = RED;
            }
        }
        else 
        {
            // ���ڵ����游�ڵ�����ӽڵ�����������������Գƴ���
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

    // ��֤���ڵ�Ϊ��ɫ
    this->root->color = BLACK;
}

//����
void RBTree::Insert(Symbol symbol)
{
    shared_ptr<RBTreeNode> newNode = make_shared<RBTreeNode>(symbol);

    // ��ʼ�����ڱ�������ָ��
    shared_ptr<RBTreeNode> y = nullptr;
    shared_ptr<RBTreeNode> x = this->root;

    // ���������ҵ������½ڵ��λ��
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

    // �����½ڵ�ĸ��ڵ�
    newNode->parent = y;
    // ������ڵ�Ϊ�գ�˵����Ϊ�գ��½ڵ��Ϊ���ڵ�
    if (y == nullptr)
    {
        this->root = newNode;
    }
    // �����½ڵ���Ϊ���ڵ��������ӽڵ�
    else if (newNode->data < y->data)
    {
        y->left = newNode;
    }
    else
    {
        y->right = newNode;
    }

    // �½ڵ��ʼΪ��ɫ��Ȼ������޸�������ά�ֺ��������
    newNode->color = RED;
    Insert_FixUp(newNode);
}

// ���ҽڵ�ĺ���
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

// ��������ĸ�������
void RBTree::In_Order_Helper(shared_ptr<RBTreeNode> node) 
{
    if (node != nullptr) 
    {
        In_Order_Helper(node->left);
        std::cout << node->data << " \n";
        In_Order_Helper(node->right);
    }
}

// ���⹫���������������
void RBTree::In_Order_Traversal() 
{
    In_Order_Helper(this->root);
}
