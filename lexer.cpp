#include "lexer.h"


// ������һ�� Token
Token Lexer::Next_Token(int nowLines) {
    using namespace token_type;

    m_stateMachine.Reset();    //����״̬��
    position--;                //��������(״̬������ʱ�ܻ�����һ���ַ����ʷ���֮ǰ��ԭ��
    string value = "";         //���ڴ洢��ʶ������������
    size_t start = -1;         //��¼��ʼλ�ã����ڶ�Դ���һ���ֽ��в��
    bool startLock = false;   //����Ƿ��Ѿ����ǿ�ʼ״̬      
    while (true) {
        position++;             //������һ���ַ�

        if (m_stateMachine.Get_Current_State() != state_type::START && !startLock)
        {
            startLock = true;
            start = position - 1;   //��¼��ʼλ�ã����ڶ�Դ���һ���ֽ��в��
        }
        if (start == source.length())   //��������ǰԴ����
        {
            return { EOF_TOKEN, "EOF", nowLines, position };
        }

        switch (m_stateMachine.Get_Current_State())     //���ݵ�ǰ״̬ do something
        {
        case state_type::START:
            m_stateMachine.State_Transition(source[position]);      //�������ʼ״̬����״̬ת��
            break;
        case state_type::IDENTIFY_INTEGER:
            m_stateMachine.State_Transition(source[position]);      //���������ʶ��״̬����״̬ת��
            break;
        case state_type::INTEGER:                                   //����״̬: ʶ������������
            position--;
            value = source.substr(start, position - start);         //��ȡʶ�𵽵�����
            return { NUM, value, nowLines, position };
            break;
        case state_type::IDENTIFY_LETTER:                           //�������ĸʶ��״̬����״̬ת��
            m_stateMachine.State_Transition(source[position]);
            break;
        case state_type::LETTER:                                    //����״̬: ʶ���������ַ���
        {
            position--;
            value = source.substr(start, position - start);         //��ȡʶ�𵽵��ַ���
            auto it = find(keyWord.begin(), keyWord.end(), value);  //����Ƿ�Ϊ�ؼ���
            if (it == keyWord.end())
            {
                return { ID, value, nowLines, position };           //ʶ��Ϊ��ʶ��
            }
            else
            {
                return { Keyword_To_Type(*it), value, nowLines, position };     //ʶ��Ϊ�ؼ���
            }
            break;
        }
        case state_type::IDENTIFY_ZERO:                             //���������0ʶ��״̬����״̬ת��
            m_stateMachine.State_Transition(source[position--]);
            break;
        case state_type::ZERO:                                      //����״̬: ʶ��0
            return { NUM, "0", nowLines, position };
            break;
        case state_type::PLUS:                                      //����״̬: ʶ��"+"
            return { PLUS, "+", nowLines, position };
            break;
        case state_type::MINUS:                                     //����״̬: ʶ��"-"
            return { MINUS, "-", nowLines, position };
            break;
        case state_type::TIMES:                                     //����״̬: ʶ��"*"
            return { TIMES, "*", nowLines, position };
            break;
        case state_type::DIVIDE:                                    //����״̬: ʶ��"/"
            return { DIVIDE, "/", nowLines, position };
            break;
        case state_type::ASSIGN:                                    //����״̬: ʶ��"="
            return { ASSIGN, "=", nowLines, position };
            break;
        case state_type::LPAREN:                                    //����״̬: ʶ��"("
            return { LPAREN, "(", nowLines, position };
            break;
        case state_type::RPAREN:                                    //����״̬: ʶ��")"
            return { RPAREN, ")", nowLines, position };
            break;
        case state_type::LBRACE:                                    //����״̬: ʶ��"{"
            return { LBRACE, "{", nowLines, position };
            break;
        case state_type::RBRACE:                                    //����״̬: ʶ��"}"
            return { RBRACE, "}", nowLines, position };
            break;
        case state_type::SEMICOLON:                                 //����״̬: ʶ��";"
            return { SEMICOLON, ";", nowLines, position };
            break;
        case state_type::COMMA:                                     //����״̬: ʶ��","
            return { COMMA, ",", nowLines, position };
            break;
        case state_type::ILLEGAL_IDENTIFIER:                        //����״̬: ERROR
            return { _ERROR, "��ʶ�����Ϸ�", nowLines, position };
            break;
        case state_type::ILLEGAL_SYMBOL:                            //����״̬: ERROR
            return { _ERROR, "��⵽�Ƿ��ַ�", nowLines, position };
            break;
        case state_type::ILLEGAL_NUM:                               //����״̬: ERROR
            return { _ERROR, "���������ǰ��\"0\"", nowLines, position };
            break;
        default:
            break;
        }
    }

}


//���ؼ����ַ���ת��Ϊ��Ӧ��Token����
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


//״̬ת��
void StateMachine::State_Transition(wchar_t _input)
{
    using namespace state_type;

    switch (currentState)
    {
    case START:                                 //��ʼ״̬
        if (_input >= '1' && _input <= '9')     //��⵽����ת��Ϊ����ʶ��״̬
            currentState = IDENTIFY_INTEGER;
        else if (iswspace(_input))              //���Կո񣬻س����Ʊ�
            currentState = currentState;
        else if (_input == '0')                 //��⵽����0ת��Ϊ����0ʶ��״̬
            currentState = IDENTIFY_ZERO;
        else if ((_input >= 'a' && _input <= 'z') || (_input >= 'A' && _input <= 'Z')|| _input=='_')    //��⵽��ĸ���»���ת��Ϊ�ַ�ʶ��״̬
            currentState = IDENTIFY_LETTER;
        else if (_input == '+')                 //ʣ�¾��Ǽ��һЩ������֮����ˣ���ʡ��ע����
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
            currentState = ILLEGAL_SYMBOL;     //�������ǲ��Ϸ��ַ���
        break;
    case IDENTIFY_INTEGER:                              //�������״̬
        if (iswdigit(_input))                           //�������������״̬����
            currentState = currentState;
        else if (iswalpha(_input) || _input == '_')     //������ַ����ǷǷ���ʶ��
            currentState = ILLEGAL_IDENTIFIER;
        else
            currentState = INTEGER;                     //ʣ�µľʹ������ּ�����
        break;
    case IDENTIFY_LETTER:                               //����ַ�״̬
        if ((_input >= 'a' && _input <= 'z') || (_input >= 'A' && _input <= 'Z') || _input == '_' || iswdigit(_input))      //����ʶ���Ƿ�Ϸ�
            currentState = currentState;
        else                                            //ʣ�µľʹ����ַ�������
            currentState = LETTER;
        break;
    case IDENTIFY_ZERO:                                 //������� 0 ��״̬
        if (iswdigit(_input))
            currentState = ILLEGAL_NUM;                 //���֮���������������ǰ��0 �������ô����ֱ�ӱ�����
        else if (iswalpha(_input) || _input == '_')     //���ַ����ǷǷ���ʶ��
            currentState = ILLEGAL_IDENTIFIER;     
        else                                            //ʣ�µľʹ����⵽������ 0
            currentState = ZERO;
        break;
    default:
        cout << "״̬������δ֪����" << endl;
        exit(0);
    }
}


//���дʷ�����������Դ�����У����Token����
vector<Token> Do_Lexer(const vector<string>& sourceCode)
{
    vector<Token> tokenList;                        //����һ�����Token������
    for (int i = 0; i < sourceCode.size(); i++)     //����ÿһ��Դ��
    {
        Lexer lexer(sourceCode[i]);                 //����ÿһ��Դ��
        Token token = lexer.Next_Token(i + 1);      //��ȡ��һ��Token

        while (token.type != token_type::EOF_TOKEN)     //�����ǰToken��ΪEOF�������
        {
            if (token.type == token_type::_ERROR)       //�ʷ���������
            {
                cout << "\n\n(�ʷ�)ERROR Line: " << i + 1 << "(" << token.s_position << ")  " << token.value << "\n" << endl;
                exit(0);
            }
            tokenList.push_back(token);                 //û����ͰѼ�⵽��Token�ӵ�������
            token = lexer.Next_Token(i + 1);            //����������һ��Token
        }
    }
    return tokenList;           //����Token����
}


//��Token����ת��Ϊ�ַ���
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
    case ID: return "��ʶ��";
    case NUM: return "����";
    default: return "UNKNOWN";
    }
}