/*

    c0编译器的实现 ,包括词法分析，语法分析，语义分析，代码优化，目标代码生成

    github链接：https://github.com/RRdebug/c0compiler

*/

#include "lexer.h"
#include "parser.h"
#include "io.h"
#include "semanticAnalyzer.h"
#include "codeOptimization.h"
#include "objectCode.h"

int main() {
    vector<string> sourceCode;  //存储源码每一行的数组
    string filePath;            //源码文件位置
    
    cout << "选择目标文件,路径中注意别携带中文\n" << std::endl;
    Open_File_Dialog(filePath);             //手动选择文件
    if (!filePath.empty())                  //文件合法
        Read_File(filePath,sourceCode);     //读取内容
    else 
        exit(0);

    Show_Source_Code(sourceCode);    //展示源码
    vector<Token> tokenList = Do_Lexer(sourceCode);     //词法分析
    Show_Lexer(tokenList);           //展示Token

    Parser parser(tokenList);        
    shared_ptr<ASTNode> root = parser.Do_Parser();      //语法分析
    parser.Show_AST_BY_DFS(root);    //展示抽象语法树

    Print_A_Line("语义分析\n");
    SemanticAnalyzer sAnalyzer;
    sAnalyzer.Do_Semantic_Analyzer(root);   //语义分析
    sAnalyzer.Show_Symbol_Table();          //展示符号表

    Print_A_Line("代码优化:\n\n");
    CodeOpt cOpt;
    cOpt.Do_Optimization(root);      //进行代码优化
    cout << "进行常量折叠、死代码消除、删除未使用的变量后的抽象语法树:\n\n";
    cOpt.Show_AST_BY_DFS(root);      //展示优化后的抽象语法树

    Print_A_Line("目标代码生成:\n\n");
    ObjCodeController objCon;
    objCon.Do_CodeGeneration(root);     //代码生成
    objCon.Show_Object_Code();
    objCon.OutputToFile("./object.txt");

    return 0;
}
