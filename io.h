/*
    io��ز���
    ps:��ʵ��Ҫ����������Դ��
*/


#ifndef IO
#define IO

#include <fstream>
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip> 
#include <algorithm>
#include "lexer.h"

using namespace std;

// ���ļ��Ի��򲢻�ȡѡ���ļ���·��
void Open_File_Dialog(std::string& filePath);

// ��ȡ�ļ����ݲ����д���
void Read_File(string filePath, vector<string>& sCode);

//չʾԴ��
void Show_Source_Code(const vector<string>& sourceCode);

//չʾ�ʷ��������
void Show_Lexer(const vector<Token>& tokenList);

//��Token����ת��Ϊ�ַ���
string Get_Token_Type_Name(int tokenType);

//���һ�зָ���
void Print_A_Line(string out);

#endif