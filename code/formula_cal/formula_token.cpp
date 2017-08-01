#include "formula_pch.h"
#include "formula_token.h"

void FormulaToken::Clear()
{
    m_errcode       = 0;
    m_priority      = 0;
    m_type          = 0;
    m_digit         = 0.0;
    m_token         = "";
}

void FormulaToken::SetErrCode(int32 val)
{
    m_errcode = val;
}

void FormulaToken::SetPriority(int32 val)
{
    m_priority = val;
}

void FormulaToken::PushChar(int8 val)
{
    m_token.push_back(val);
}
