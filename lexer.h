/*
    词法分析器,用于将源码拆分为Token序列
    使用有穷自动机（DFA）实现词法分析
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
    // 枚举类型定义了不同的词法单元（Token）类型
    enum TokenTypeEnum
    {
        // 特殊token
        _ERROR, EOF_TOKEN,
        // 关键字
        _INT, _VOID, _MAIN, _IF, _ELSE, _WHILE, _RETURN, _SCANF, _PRINTF,
        // 运算符和特殊字符
        PLUS, MINUS, TIMES, DIVIDE, ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,
        // 标识符和数字
        ID, NUM
    };

    //保留字表
    const vector<string> keyWord = { "int", "void", "main", "if", "else", "while", "return", "scanf", "printf" };
}

namespace state_type
{
    //状态机的状态
    enum StateType
    {
        START, IDENTIFY_INTEGER, INTEGER, IDENTIFY_LETTER, LETTER, IDENTIFY_ZERO, ZERO, OPERATE,
        PLUS, MINUS, TIMES, DIVIDE, ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,
        ILLEGAL_IDENTIFIER, ILLEGAL_SYMBOL, ILLEGAL_NUM
    };
}

// 结构体定义了 Token，包括类型和值和位置信息
struct Token 
{
    token_type::TokenTypeEnum type;
    string value;
    int s_lines;
    size_t s_position;
};

//状态机
class StateMachine
{
public:
    StateMachine() :currentState(state_type::START) {}
    state_type::StateType Get_Current_State() { return currentState; }  //获取当前状态
    void State_Transition(wchar_t _input);                              //状态转换
    void Reset() { currentState = state_type::START; }                  //重置状态机
private:
    state_type::StateType currentState;                                 //当前状态
};

//词法分析器
class Lexer {
public:
    Lexer(const string& source) : source(source), position(0) {}
    Token Next_Token(int nowLines);     // 分析下一个 Token
private:
    string source;                      // 存储源代码的字符串
    size_t position;                    // 当前处理的字符的位置
    StateMachine m_stateMachine;        //状态机
    token_type::TokenTypeEnum Keyword_To_Type(string _input);    //将关键字字符串转换为对应的Token类型
};

//进行语法分析，传入源码序列，输出Token序列
vector<Token> Do_Lexer(const vector<string>& sourceCode);           

//将Token类型转换为字符串
string TokenTypeToString(token_type::TokenTypeEnum tokenType);

#endif