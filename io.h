/*
    io相关操作
    ps:其实主要是用来读入源码
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

// 打开文件对话框并获取选中文件的路径
void Open_File_Dialog(std::string& filePath);

// 读取文件内容并进行处理
void Read_File(string filePath, vector<string>& sCode);

//展示源码
void Show_Source_Code(const vector<string>& sourceCode);

//展示词法分析结果
void Show_Lexer(const vector<Token>& tokenList);

//将Token类型转换为字符串
string Get_Token_Type_Name(int tokenType);

//输出一行分隔符
void Print_A_Line(string out);

#endif