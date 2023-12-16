#include "io.h"


// ���ļ��Ի��򲢻�ȡѡ���ļ���·��
void Open_File_Dialog(std::string& filePath)
{
    OPENFILENAME ofn; // Windows API �ṹ�壬���������ļ��Ի���
    wchar_t szFile[260] = L""; // ���ڴ洢�ļ�·���Ŀ��ַ�����

    ZeroMemory(&ofn, sizeof(ofn)); // ��ʼ���ṹ��
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All Files\0*.*\0"; // �����ļ����͹�����
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // ���öԻ����־

    if (GetOpenFileName(&ofn) == TRUE)
    {
        wchar_t* wstr = ofn.lpstrFile; // ��ȡ�ļ�·��
        char str[260];
        size_t num;
        if (wcstombs_s(&num, str, sizeof(str), wstr, _TRUNCATE) == 0)
        {
            filePath = str; // �洢�ļ�·��
        }
        else
        {
            std::cerr << "ת�����ַ�·������ͨ�ַ���ʧ��" << std::endl;
            exit(0);
        }
    }
    else
    {
        std::wcout << L"�û�ȡ�����ļ�ѡ��" << std::endl; // ����û�ȡ����ѡ��
    }
}


// ��ȡ�ļ����ݲ����д���
void Read_File(string filePath, vector<string>& sCode)
{
    fstream fin;
    string tempStr;
    fin.open(filePath, ios::in); // ���ļ����ж�ȡ
    if (!fin.is_open()) { // ����ļ���ʧ��
        std::cout << "�ļ���ʧ��" << std::endl;
        exit(1); // ��ֹ����
    }
    while (getline(fin, tempStr)) { // ��ȡ�ļ���ÿһ��
        sCode.push_back(tempStr); // ����ȡ���д洢��Դ�����
    }
    fin.close(); // �ر��ļ�
}


//չʾԴ��
void Show_Source_Code(const vector<string>& sourceCode)
{
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
    cout << "\nC0Դ�룺\n" << endl;
    for (int i = 0; i < sourceCode.size(); i++)
    {
        cout << i + 1 << "\t|" << sourceCode[i] << endl;
    }
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
}


//չʾ�ʷ��������
void Show_Lexer(const vector<Token>& tokenList)
{
    cout << "\n����DFA�Ĵʷ�����: \n" << endl;
    size_t maxLineLength = 0, maxPositionLength = 0;
    for (const auto& it : tokenList) {          //���ѭ����������������ȣ�Ϊ�˶�������õ�
        size_t lineLength = std::to_string(it.s_lines).length();
        size_t positionLength = std::to_string(it.s_position).length();
        maxLineLength = max(maxLineLength, lineLength);
        maxPositionLength = max(maxPositionLength, positionLength);
    }

    for (const auto& it : tokenList) {          //���Token��Ϣ
        std::cout << "Line: " << std::setw(maxLineLength) << std::right << it.s_lines
            << "(" << std::setw(maxPositionLength) << std::right << it.s_position << ")  "
            << std::left << std::setw(20) << it.value
            << "|Token���ͣ�" << Get_Token_Type_Name(it.type) << std::endl;
    }

    cout << "------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "���ڵݹ��½��﷨�������ĳ����﷨��:\n " << endl;;
}


//��Token����ת��Ϊ�ַ���
string Get_Token_Type_Name(int tokenType) {
    using namespace token_type;

    switch (tokenType) {
    case _INT: return "�ؼ���: int";
    case _VOID: return "�ؼ���: void";
    case _MAIN: return "�ؼ���: main";
    case _IF: return "�ؼ���: if";
    case _ELSE: return "�ؼ���: else";
    case _WHILE: return "�ؼ���: while";
    case _RETURN: return "�ؼ���: return";
    case _SCANF: return "�ؼ���: scanf";
    case _PRINTF: return "�ؼ���: printf";
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
    case ID: return "��ʶ��";
    case NUM: return "��������";
    case EOF_TOKEN: return "EOF";
    case _ERROR: return "����";
    default: return "Unknown Token Type";
    }
}


//���һ�зָ���
void Print_A_Line(string out)
{
    cout << "------------------------------------------------------------------------------------------------------------------------\n\n" << out << endl;
}
