#include "lexer.h"


// 分析下一个 Token
Token Lexer::Next_Token(int nowLines) {
    using namespace token_type;

    m_stateMachine.Reset();    //重置状态机
    position--;                //索引修正(状态机结束时总会多读入一个字符，故分析之前复原）
    string value = "";         //用于存储标识符或数字序列
    size_t start = -1;         //记录起始位置，用于对源码的一部分进行拆分
    bool startLock = false;   //检测是否已经不是开始状态      
    while (true) {
        position++;             //分析下一个字符

        if (m_stateMachine.Get_Current_State() != state_type::START && !startLock)
        {
            startLock = true;
            start = position - 1;   //记录起始位置，用于对源码的一部分进行拆分
        }
        if (start == source.length())   //结束处理当前源码行
        {
            return { EOF_TOKEN, "EOF", nowLines, position };
        }

        switch (m_stateMachine.Get_Current_State())     //根据当前状态 do something
        {
        case state_type::START:
            m_stateMachine.State_Transition(source[position]);      //如果是起始状态进行状态转换
            break;
        case state_type::IDENTIFY_INTEGER:
            m_stateMachine.State_Transition(source[position]);      //如果是数字识别状态进行状态转换
            break;
        case state_type::INTEGER:                                   //结束状态: 识别到完整的数字
            position--;
            value = source.substr(start, position - start);         //提取识别到的整数
            return { NUM, value, nowLines, position };
            break;
        case state_type::IDENTIFY_LETTER:                           //如果是字母识别状态进行状态转换
            m_stateMachine.State_Transition(source[position]);
            break;
        case state_type::LETTER:                                    //结束状态: 识别到完整的字符串
        {
            position--;
            value = source.substr(start, position - start);         //提取识别到的字符串
            auto it = find(keyWord.begin(), keyWord.end(), value);  //检测是否为关键字
            if (it == keyWord.end())
            {
                return { ID, value, nowLines, position };           //识别为标识符
            }
            else
            {
                return { Keyword_To_Type(*it), value, nowLines, position };     //识别为关键字
            }
            break;
        }
        case state_type::IDENTIFY_ZERO:                             //如果是数字0识别状态进行状态转换
            m_stateMachine.State_Transition(source[position--]);
            break;
        case state_type::ZERO:                                      //结束状态: 识别到0
            return { NUM, "0", nowLines, position };
            break;
        case state_type::PLUS:                                      //结束状态: 识别到"+"
            return { PLUS, "+", nowLines, position };
            break;
        case state_type::MINUS:                                     //结束状态: 识别到"-"
            return { MINUS, "-", nowLines, position };
            break;
        case state_type::TIMES:                                     //结束状态: 识别到"*"
            return { TIMES, "*", nowLines, position };
            break;
        case state_type::DIVIDE:                                    //结束状态: 识别到"/"
            return { DIVIDE, "/", nowLines, position };
            break;
        case state_type::ASSIGN:                                    //结束状态: 识别到"="
            return { ASSIGN, "=", nowLines, position };
            break;
        case state_type::LPAREN:                                    //结束状态: 识别到"("
            return { LPAREN, "(", nowLines, position };
            break;
        case state_type::RPAREN:                                    //结束状态: 识别到")"
            return { RPAREN, ")", nowLines, position };
            break;
        case state_type::LBRACE:                                    //结束状态: 识别到"{"
            return { LBRACE, "{", nowLines, position };
            break;
        case state_type::RBRACE:                                    //结束状态: 识别到"}"
            return { RBRACE, "}", nowLines, position };
            break;
        case state_type::SEMICOLON:                                 //结束状态: 识别到";"
            return { SEMICOLON, ";", nowLines, position };
            break;
        case state_type::COMMA:                                     //结束状态: 识别到","
            return { COMMA, ",", nowLines, position };
            break;
        case state_type::ILLEGAL_IDENTIFIER:                        //结束状态: ERROR
            return { _ERROR, "标识符不合法", nowLines, position };
            break;
        case state_type::ILLEGAL_SYMBOL:                            //结束状态: ERROR
            return { _ERROR, "检测到非法字符", nowLines, position };
            break;
        case state_type::ILLEGAL_NUM:                               //结束状态: ERROR
            return { _ERROR, "不允许包含前导\"0\"", nowLines, position };
            break;
        default:
            break;
        }
    }

}


//将关键字字符串转换为对应的Token类型
token_type::TokenTypeEnum Lexer::Keyword_To_Type(string _input)
{
    using namespace token_type;

    if (_input == "int") return _INT;
    else if (_input == "void") return _VOID;
    else if (_input == "main") return _MAIN;
    else if (_input == "if") return _IF;
    else if (_input == "else") return _ELSE;
    else if (_input == "while") return _WHILE;
    else if (_input == "return") return _RETURN;
    else if (_input == "scanf") return _SCANF;
    else if (_input == "printf") return _PRINTF;
    else return _ERROR;
}


//状态转换
void StateMachine::State_Transition(wchar_t _input)
{
    using namespace state_type;

    switch (currentState)
    {
    case START:                                 //起始状态
        if (_input >= '1' && _input <= '9')     //检测到数字转换为数字识别状态
            currentState = IDENTIFY_INTEGER;
        else if (iswspace(_input))              //忽略空格，回车，制表
            currentState = currentState;
        else if (_input == '0')                 //检测到数字0转换为数字0识别状态
            currentState = IDENTIFY_ZERO;
        else if ((_input >= 'a' && _input <= 'z') || (_input >= 'A' && _input <= 'Z')|| _input=='_')    //检测到字母或下划线转换为字符识别状态
            currentState = IDENTIFY_LETTER;
        else if (_input == '+')                 //剩下就是检测一些操作符之类的了，就省略注释了
            currentState = PLUS;
        else if (_input == '-')
            currentState = MINUS;
        else if (_input == '*')
            currentState = TIMES;
        else if (_input == '/')
            currentState = DIVIDE;
        else if (_input == '=')
            currentState = ASSIGN;
        else if (_input == '(')
            currentState = LPAREN;
        else if (_input == ')')
            currentState = RPAREN;
        else if (_input == '{')
            currentState = LBRACE;
        else if (_input == '}')
            currentState = RBRACE;
        else if (_input == ';')
            currentState = SEMICOLON;
        else if (_input == ',')
            currentState = COMMA;
        else
            currentState = ILLEGAL_SYMBOL;     //其他就是不合法字符了
        break;
    case IDENTIFY_INTEGER:                              //检测数字状态
        if (iswdigit(_input))                           //如果还是数字则状态不变
            currentState = currentState;
        else if (iswalpha(_input) || _input == '_')     //如果是字符就是非法标识符
            currentState = ILLEGAL_IDENTIFIER;
        else
            currentState = INTEGER;                     //剩下的就代表数字检测完毕
        break;
    case IDENTIFY_LETTER:                               //检测字符状态
        if ((_input >= 'a' && _input <= 'z') || (_input >= 'A' && _input <= 'Z') || _input == '_' || iswdigit(_input))      //检测标识符是否合法
            currentState = currentState;
        else                                            //剩下的就代表字符检测完毕
            currentState = LETTER;
        break;
    case IDENTIFY_ZERO:                                 //检测数字 0 的状态
        if (iswdigit(_input))
            currentState = ILLEGAL_NUM;                 //如果之后还有数字则代表有前导0 ，我懒得处理就直接报错了
        else if (iswalpha(_input) || _input == '_')     //有字符就是非法标识符
            currentState = ILLEGAL_IDENTIFIER;     
        else                                            //剩下的就代表检测到了数字 0
            currentState = ZERO;
        break;
    default:
        cout << "状态机发生未知错误" << endl;
        exit(0);
    }
}


//进行词法分析，传入源码序列，输出Token序列
vector<Token> Do_Lexer(const vector<string>& sourceCode)
{
    vector<Token> tokenList;                        //声明一个存放Token的数组
    for (int i = 0; i < sourceCode.size(); i++)     //处理每一行源码
    {
        Lexer lexer(sourceCode[i]);                 //处理每一行源码
        Token token = lexer.Next_Token(i + 1);      //获取第一个Token

        while (token.type != token_type::EOF_TOKEN)     //如果当前Token不为EOF，则继续
        {
            if (token.type == token_type::_ERROR)       //词法分析报错
            {
                cout << "\n\n(词法)ERROR Line: " << i + 1 << "(" << token.s_position << ")  " << token.value << "\n" << endl;
                exit(0);
            }
            tokenList.push_back(token);                 //没问题就把检测到的Token加到数组里
            token = lexer.Next_Token(i + 1);            //继续分析下一个Token
        }
    }
    return tokenList;           //返回Token序列
}


//将Token类型转换为字符串
string TokenTypeToString(token_type::TokenTypeEnum tokenType)
{
    using namespace token_type;

    switch (tokenType)
    {
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
    case _ERROR: return "error";
    case EOF_TOKEN: return "EOF_TOKEN";
    case _INT: return "int";
    case _VOID: return "void";
    case _MAIN: return "main";
    case _IF: return "if";
    case _ELSE: return "else";
    case _WHILE: return "while";
    case _RETURN: return "return";
    case _SCANF: return "scanf";
    case _PRINTF: return "printf";
    case ID: return "标识符";
    case NUM: return "整数";
    default: return "UNKNOWN";
    }
}