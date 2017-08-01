#ifndef TOKEN_SCAN_H
#define TOKEN_SCAN_H

#include "formula_token.h"

class TokenScan
{
public:
    TokenScan();
    virtual ~TokenScan();

    bool IsDigit(int8 ch);

    bool IsLetter(int8 ch);

    bool IsOperator(int8 ch);

    void ReadSpace(const std::string& buff_str, uint32& buff_idx);

    FormulaToken GetDigit(const std::string& buff_str, uint32& buff_idx);

    FormulaToken GetOperator(const std::string& buff_str, uint32& buff_idx);

    FormulaToken GetID(const std::string& buff_str, uint32& buff_idx, bool is_positive = true);
};

#endif
