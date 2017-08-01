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

    TokenScan   m_token_scan;       ///< ×Ö·û½âÎö

    int32       m_err_code;         ///< ´íÎóÂë

    uint32      m_buff_idx;         ///< »º³åÏÂ±ê
    std::string m_buff_str;         ///< ¹«Ê½×Ö·û´®
    std::list<FormulaToken> m_s1;   ///< ÈëÕ»
    std::list<FormulaToken> m_s2;   ///< ³öÕ»
};

#endif
