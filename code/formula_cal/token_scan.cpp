#include "formula_pch.h"
#include "token_scan.h"
#include "formula_token.h"

TokenScan::TokenScan()
{

}

TokenScan::~TokenScan()
{

}

bool TokenScan::IsDigit(int8 ch)
{
    if(('0' <= ch) && (ch <='9'))
    {
        return true;
    }
    return false;
}

bool TokenScan::IsLetter(int8 ch)
{
    if(('_' == ch) || 
       (('A' <= ch) && (ch <='Z')) ||
       (('a' <= ch) && (ch <='z')))
    {
        return true;
    }
    return false;
}

bool TokenScan::IsOperator(int8 ch)
{
    if(('+' == ch) || ('-' == ch) ||
       ('*' == ch) || ('/' == ch) ||
       ('>' == ch) || ('<' == ch))
    {
        return true;
    }
    return false;
}

// ==================================================
// 过滤空格
// ==================================================
void TokenScan::ReadSpace(const std::string& buff_str, uint32& buff_idx)
{
    int8 ch = '\0';

    while(buff_str.size() > buff_idx)
    {
        ch = buff_str.at(buff_idx);
        if((' ' == ch) || ('\t' == ch))
        {
            buff_idx++;
            continue;
        }
        else
        {
            break;
        }
    }
}

// ==================================================
// 获得数字
// ==================================================
FormulaToken TokenScan::GetDigit(const std::string& buff_str, uint32& buff_idx)
{
    FormulaToken token;
    int32 state = 0;
    int8 ch = '\0';

    token.Clear();

    // 0,1,2,3,4 数字识别
    // 5 识别结束
    // 6 错误状态
    // ============================================================
    while((buff_idx < buff_str.size()) && (state < 5))
    {
        ch = buff_str.at(buff_idx);

        if(0 == state)
        {
            if((ch == '+') || (ch =='-'))
            {
                state = 1;
                token.PushChar(ch);
                buff_idx++;
            }
            else if(IsDigit(ch))
            {
                state = 2;
                token.PushChar(ch);
                buff_idx++;
            }
            else
            {
                state = 6;
            }
        }
        else if(1 == state)
        {
            if(IsDigit(ch))
            {
                state = 2;
                token.PushChar(ch);
                buff_idx++;
            }
            else
            {
                state = 6;
            }
        }
        else if(2 == state)
        {
            if(IsDigit(ch))
            {
                state = 2;
                token.PushChar(ch);
                buff_idx++;
            }
            else if('.' == ch)
            {
                state = 3;
                token.PushChar(ch);
                buff_idx++;
            }
            else
            {
                state = 5;
            }
        }
        else if(3 == state)
        {
            if(IsDigit(ch))
            {
                state = 4;
                token.PushChar(ch);
                buff_idx++;
            }
            else
            {
                state = 6;
            }
        }
        else if(4 == state)
        {
            if(IsDigit(ch))
            {
                token.PushChar(ch);
                buff_idx++;
            }
            else
            {
                state = 5;
            }
        }
        else
        {
            state = 6;
        }
    }

    if((state == 2) || (state == 4))
    {
        state = 5;
    }

    // 如果正常结束，则将字符串转换为数字
    if(state == 5)
    {
        token.SetErrCode(FormulaToken::kTOKEN_OK);
        token.m_type = FormulaToken::kTOKEN_TYPE_DIGIT;
        token.m_digit = (real)Crown::SDStrtod(token.m_token.c_str(), NULL);
    }
    else
    {
        CnAssert(false);
        token.SetErrCode(FormulaToken::kTOKEN_ERR);
    }

    return token;
}

// ==================================================
// 获得操作符
// ==================================================
FormulaToken TokenScan::GetOperator(const std::string& buff_str, uint32& buff_idx)
{
    FormulaToken token;
    int32 state = 0;
    int8 ch = '\0';

    token.Clear();

    if(buff_idx >= buff_str.size())
    {
        CnAssert(false);
        token.SetErrCode(FormulaToken::kTOKEN_ERR);
        return token;
    }

    ch = buff_str.at(buff_idx);

    switch(ch)
    {
    case '>':
    case '<':
        token.PushChar(ch);
        token.SetPriority(1);
        state = 1;
        buff_idx++;
        break;
    case '+':
    case '-':
        token.PushChar(ch);
        token.SetPriority(2);
        state = 1;
        buff_idx++;
        break;
    case '*':
    case '/':
        token.PushChar(ch);
        token.SetPriority(3);
        state = 1;
        buff_idx++;
        break;
    case '(':
    case ')':
        token.PushChar(ch);
        token.SetPriority(4);
        state = 1;
        buff_idx++;
        break;
    default:
        state = 2;
        break;
    }

    if(state == 1)
    {
        token.SetErrCode(FormulaToken::kTOKEN_OK);
    }
    else
    {
        CnAssert(false);
        token.SetErrCode(FormulaToken::kTOKEN_ERR);
    }

    token.m_type = FormulaToken::kTOKEN_TYPE_OPERATOR;

    return token;
}

// ==================================================
// 获得自定义变量标记
// ==================================================
FormulaToken TokenScan::GetID(const std::string& buff_str, uint32& buff_idx, bool is_positive)
{
    FormulaToken token;
    int32 state = 0;
    int8 ch = '\0';

    token.Clear();

    // 0,1,2
    while((buff_idx < buff_str.size()) && (state <= 1))
    {
        ch = buff_str.at(buff_idx);

        if(0 == state)
        {
            if(IsLetter(ch))
            {
                state = 1;
                token.PushChar(ch);
                buff_idx++;
            }
            else
            {
                state = 3;
            }
        }
        else if(1 == state)
        {
            if(IsLetter(ch))
            {
                state = 1;
                token.PushChar(ch);
                buff_idx++;
            }
            else
            {
                state = 2;
            }
        }
    }

    // 如果正常结束，则将字符串转换为数字
    if((state == 1) || (state == 2))
    {
        token.SetErrCode(FormulaToken::kTOKEN_OK);
        token.m_positive = is_positive;
    }
    else
    {
        CnAssert(false);
        token.SetErrCode(FormulaToken::kTOKEN_ERR);
    }

    token.m_type = FormulaToken::kTOKEN_TYPE_ID;

    return token;
}
