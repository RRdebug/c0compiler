#include "io.h"


// 打开文件对话框并获取选中文件的路径
void Open_File_Dialog(std::string& filePath)
{
    OPENFILENAME ofn; // Windows API 结构体，用于配置文件对话框
    wchar_t szFile[260] = L""; // 用于存储文件路径的宽字符数组

    ZeroMemory(&ofn, sizeof(ofn)); // 初始化结构体
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All Files\0*.*\0"; // 设置文件类型过滤器
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // 设置对话框标志

    if (GetOpenFileName(&ofn) == TRUE)
    {
        wchar_t* wstr = ofn.lpstrFile; // 获取文件路径
        char str[260];
        size_t num;
        if (wcstombs_s(&num, str, sizeof(str), wstr, _TRUNCATE) == 0)
        {
            filePath = str; // 存储文件路径
        }
        else
        {
            std::cerr << "转换宽字符路径到普通字符串失败" << std::endl;
            exit(0);
        }
    }
    else
    {
        std::wcout << L"用户取消了文件选择" << std::endl; // 如果用户取消了选择
    }
}


// 读取文件内容并进行处理
void Read_File(string filePath, vector<string>& sCode)
{
    fstream fin;
    string tempStr;
    fin.open(filePath, ios::in); // 打开文件进行读取
    if (!fin.is_open()) { // 如果文件打开失败
        std::cout << "文件打开失败" << std::endl;
        exit(1); // 终止程序
    }
    while (getline(fin, tempStr)) { // 读取文件的每一行
        sCode.push_back(tempStr); // 将读取的行存储到源代码表
    }
    fin.close(); // 关闭文件
}


//展示源码
void Show_Source_Code(const vector<string>& sourceCode)
{
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
    cout << "\nC0源码：\n" << endl;
    for (int i = 0; i < sourceCode.size(); i++)
    {
        cout << i + 1 << "\t|" << sourceCode[i] << endl;
    }
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
}


//展示词法分析结果
void Show_Lexer(const vector<Token>& tokenList)
{
    cout << "\n基于DFA的词法分析: \n" << endl;
    size_t maxLineLength = 0, maxPositionLength = 0;
    for (const auto& it : tokenList) {          //这个循环用作计算输出长度，为了对齐输出用的
        size_t lineLength = std::to_string(it.s_lines).length();
        size_t positionLength = std::to_string(it.s_position).length();
        maxLineLength = max(maxLineLength, lineLength);
        maxPositionLength = max(maxPositionLength, positionLength);
    }

    for (const auto& it : tokenList) {          //输出Token信息
        std::cout << "Line: " << std::setw(maxLineLength) << std::right << it.s_lines
            << "(" << std::setw(maxPositionLength) << std::right << it.s_position << ")  "
            << std::left << std::setw(20) << it.value
            << "|Token类型：" << Get_Token_Type_Name(it.type) << std::endl;
    }

    cout << "------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "基于递归下降语法分析器的抽象语法树:\n " << endl;;
}


//将Token类型转换为字符串
string Get_Token_Type_Name(int tokenType) {
    using namespace token_type;

    switch (tokenType) {
    case _INT: return "关键字: int";
    case _VOID: return "关键字: void";
    case _MAIN: return "关键字: main";
    case _IF: return "关键字: if";
    case _ELSE: return "关键字: else";
    case _WHILE: return "关键字: while";
    case _RETURN: return "关键字: return";
    case _SCANF: return "关键字: scanf";
    case _PRINTF: return "关键字: printf";
    case PLUS: return "+";
    case MINUS: return "-";
    case TIMES: return "*";
    case DIVIDE: return "/";
    case ASSIGN: return "=";
    case LPAREN: return "(";
    case RPAREN: return ")";
    case LBRACE: return "{";
    case RBRACE: return "}";
    case SEMICOLON: return ";";
    case COMMA: return ",";
    case ID: return "标识符";
    case NUM: return "整数常量";
    case EOF_TOKEN: return "EOF";
    case _ERROR: return "错误";
    default: return "Unknown Token Type";
    }
}


//输出一行分隔符
void Print_A_Line(string out)
{
    cout << "------------------------------------------------------------------------------------------------------------------------\n\n" << out << endl;
}
