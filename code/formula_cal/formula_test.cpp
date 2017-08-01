#include "formula_pch.h"
#include "formula_cal.h"

int main(int argc, char *argv[])
{
    FormulaParser formulaParser;
    FormulaTemplate formulaTmpl;
    FormulaParameter formulaParam;
    std::string a = "a";
    std::string b = "b";
    double result = 0.0;

    // ��ʼ����־
    char logname[20];
    SDSprintf(logname, "formula_cal");
    Crown::ColorLog::CreateInstance();
    Crown::ColorLog::Instance()->Init("log", logname);
    Crown::ColorLog::Instance()->SetLogTime(true);

    // ��ʼ������
    FormulaCal::CreateInstance();

    //formulaParam.AddParam("a", 5);
    //formulaParam.AddParam("b", 6);

    //formulaParam.AddParam("waste_time", 4);
    //formulaParam.AddParam("total_time", 50);
    //formulaParam.AddParam("max_combo", 14);
    //formulaParam.AddParam("monster_num", 22);
    //formulaParam.AddParam("combo_num", 0);

    //std::string formulaStr = "(5+(2.2+1.1*(3+4*6)))";
    //std::string formulaStr = "5+(2.2+1.1*(+3+-4*6))";
    //std::string formulaStr = "(60-(waste_time*60/total_time))+(35*max_combo/monster_num)+(5*2*combo_num/monster_num)";
    //std::string formulaStr = "6 < 5 < 4";
    std::string formulaStr = "0.3 > 0.1 > 1";

    //printf("%d, %d\n", '0', '9');
    //printf("%d, %d, %d, %d, %d\n", '_', 'A', 'Z', 'a', 'z');
    //printf("%d, %d, %d, %d\n", '+', '-', '*', '/');

    //printf("%d, %d\n", '0', '9');
    //printf("%d, %d, %d, %d, %d\n", '_', 'A', 'Z', 'a', 'z');
    //printf("%d, %d, %d, %d\n", '+', '-', '*', '/');

    // ѹ�빫ʽ����
    // ���һ�����㹫ʽ��ģ�壨���ģ�����Ҫ�������٣�
    formulaTmpl = formulaParser.Parser(formulaStr);

    // ������
    result = FormulaCal::Instance()->CalResult(&formulaTmpl, &formulaParam);

    // ���ٵ���
    FormulaCal::DestroyInstance();

    Crown::ColorLog::DestroyInstance();

    return 0;
}
