#ifndef FORMULA_TEMPLATE_H
#define FORMULA_TEMPLATE_H

#include "formula_token.h"
#include "formula_parameter.h"

class FormulaTemplate
{
public:
    FormulaTemplate();
    ~FormulaTemplate();

    void Clear();

    bool IsValid() { return m_valid; }

    void SetValid(bool val) { m_valid = val; }

    bool SetParams(const FormulaParameter* params);

    void SetParam(const CalParameter& param);

    void PushToken(FormulaToken& token);

    // 有效性
    bool                        m_valid;

    // 关键字表
    std::set<std::string>       m_table;

    // 逆波兰式堆栈
    std::vector<FormulaToken>   m_stack;
};

#endif
