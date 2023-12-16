/*

    c0��������ʵ�� ,�����ʷ��������﷨��������������������Ż���Ŀ���������

    github���ӣ�https://github.com/RRdebug/c0compiler

*/

#include "lexer.h"
#include "parser.h"
#include "io.h"
#include "semanticAnalyzer.h"
#include "codeOptimization.h"
#include "objectCode.h"

int main() {
    vector<string> sourceCode;  //�洢Դ��ÿһ�е�����
    string filePath;            //Դ���ļ�λ��
    
    cout << "ѡ��Ŀ���ļ�,·����ע���Я������\n" << std::endl;
    Open_File_Dialog(filePath);             //�ֶ�ѡ���ļ�
    if (!filePath.empty())                  //�ļ��Ϸ�
        Read_File(filePath,sourceCode);     //��ȡ����
    else 
        exit(0);

    Show_Source_Code(sourceCode);    //չʾԴ��
    vector<Token> tokenList = Do_Lexer(sourceCode);     //�ʷ�����
    Show_Lexer(tokenList);           //չʾToken

    Parser parser(tokenList);        
    shared_ptr<ASTNode> root = parser.Do_Parser();      //�﷨����
    parser.Show_AST_BY_DFS(root);    //չʾ�����﷨��

    Print_A_Line("�������\n");
    SemanticAnalyzer sAnalyzer;
    sAnalyzer.Do_Semantic_Analyzer(root);   //�������
    sAnalyzer.Show_Symbol_Table();          //չʾ���ű�

    Print_A_Line("�����Ż�:\n\n");
    CodeOpt cOpt;
    cOpt.Do_Optimization(root);      //���д����Ż�
    cout << "���г����۵���������������ɾ��δʹ�õı�����ĳ����﷨��:\n\n";
    cOpt.Show_AST_BY_DFS(root);      //չʾ�Ż���ĳ����﷨��

    Print_A_Line("Ŀ���������:\n\n");
    ObjCodeController objCon;
    objCon.Do_CodeGeneration(root);     //��������
    objCon.Show_Object_Code();
    objCon.OutputToFile("./object.txt");

    return 0;
}
