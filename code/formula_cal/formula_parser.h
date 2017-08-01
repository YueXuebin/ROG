#ifndef FORMULA_PARSER_H
#define FORMULA_PARSER_H

#include "formula_template.h"
#include "token_scan.h"

class FormulaParser
{
public:
    FormulaTemplate Parser(std::string& str);

    void ReadExp();

    void ReadLeftBracket();

    void ReadRightBracket();

    void ReadOperator();

    void FilterRightBrace();

    void ReadIDExp(bool is_positive = true);

    void ReadDigitExp();
private:
    void Clear();

    enum
    {
        kERR_SUCCEED            = 0,
        kERR_FAILED             = 1,
    };

    TokenScan   m_token_scan;       ///< �ַ�����

    int32       m_err_code;         ///< ������

    uint32      m_buff_idx;         ///< �����±�
    std::string m_buff_str;         ///< ��ʽ�ַ���
    std::list<FormulaToken> m_s1;   ///< ��ջ
    std::list<FormulaToken> m_s2;   ///< ��ջ
};

#endif
