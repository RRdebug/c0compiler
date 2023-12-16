/*
    �ʷ�������,���ڽ�Դ����ΪToken����
    ʹ�������Զ�����DFA��ʵ�ִʷ�����
*/

#ifndef LEXER
#define LEXER

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

namespace token_type
{
    // ö�����Ͷ����˲�ͬ�Ĵʷ���Ԫ��Token������
    enum TokenTypeEnum
    {
        // ����token
        _ERROR, EOF_TOKEN,
        // �ؼ���
        _INT, _VOID, _MAIN, _IF, _ELSE, _WHILE, _RETURN, _SCANF, _PRINTF,
        // ������������ַ�
        PLUS, MINUS, TIMES, DIVIDE, ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,
        // ��ʶ��������
        ID, NUM
    };

    //�����ֱ�
    const vector<string> keyWord = { "int", "void", "main", "if", "else", "while", "return", "scanf", "printf" };
}

namespace state_type
{
    //״̬����״̬
    enum StateType
    {
        START, IDENTIFY_INTEGER, INTEGER, IDENTIFY_LETTER, LETTER, IDENTIFY_ZERO, ZERO, OPERATE,
        PLUS, MINUS, TIMES, DIVIDE, ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,
        ILLEGAL_IDENTIFIER, ILLEGAL_SYMBOL, ILLEGAL_NUM
    };
}

// �ṹ�嶨���� Token���������ͺ�ֵ��λ����Ϣ
struct Token 
{
    token_type::TokenTypeEnum type;
    string value;
    int s_lines;
    size_t s_position;
};

//״̬��
class StateMachine
{
public:
    StateMachine() :currentState(state_type::START) {}
    state_type::StateType Get_Current_State() { return currentState; }  //��ȡ��ǰ״̬
    void State_Transition(wchar_t _input);                              //״̬ת��
    void Reset() { currentState = state_type::START; }                  //����״̬��
private:
    state_type::StateType currentState;                                 //��ǰ״̬
};

//�ʷ�������
class Lexer {
public:
    Lexer(const string& source) : source(source), position(0) {}
    Token Next_Token(int nowLines);     // ������һ�� Token
private:
    string source;                      // �洢Դ������ַ���
    size_t position;                    // ��ǰ������ַ���λ��
    StateMachine m_stateMachine;        //״̬��
    token_type::TokenTypeEnum Keyword_To_Type(string _input);    //���ؼ����ַ���ת��Ϊ��Ӧ��Token����
};

//�����﷨����������Դ�����У����Token����
vector<Token> Do_Lexer(const vector<string>& sourceCode);           

//��Token����ת��Ϊ�ַ���
string TokenTypeToString(token_type::TokenTypeEnum tokenType);

#endif