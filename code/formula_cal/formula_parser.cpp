#include "formula_pch.h"
#include "formula_parser.h"

FormulaTemplate FormulaParser::Parser(std::string& str)
{
    FormulaTemplate formulaTemplate;    // ¹«Ê½Ä£°å
    FormulaToken token;                 // ×Ö·û´®
    int8 ch = 0;

    formulaTemplate.Clear();
    m_err_code = kERR_SUCCEED;

    Clear();

    m_buff_str = str;

    ReadExp();

    while(m_s1.size() > 0)
    {
        token = m_s1.back();
        m_s1.pop_back();
        m_s2.push_back(token);
    }

    for(std::list<FormulaToken>::iterator it = m_s2.begin(); it != m_s2.end(); ++it)
    {
        formulaTemplate.PushToken(*it);
    }

    if(m_err_code == kERR_SUCCEED)
    {
        formulaTemplate.SetValid(true);
    }

    return formulaTemplate;
}

void FormulaParser::ReadExp()
{
    int8 ch1 = '\0';
    int8 ch2 = '\0';
    bool is_positive = true;
    int32 err = 0;
    FormulaToken token;

    // ¼ì²é´íÎó×´Ì¬
    if(m_err_code != kERR_SUCCEED)
    {
        CnAssert(false);
        return;
    }

    // ÅÅ³ý¿Õ¸ñ
    m_token_scan.ReadSpace(m_buff_str, m_buff_idx);

    if(m_buff_idx >= m_buff_str.size()) return;

    ch1 = m_buff_str.at(m_buff_idx);

    if('(' == ch1)
    {
        // ¶ÁÈ¡ ( ²Ù×÷·û
        ReadLeftBracket();
        // ¼ì²é´íÎó×´Ì¬
        if(m_err_code != kERR_SUCCEED)
        {
            CnAssert(false);
            return;
        }

        // ÅÅ³ý¿Õ¸ñ
        m_token_scan.ReadSpace(m_buff_str, m_buff_idx);

        // ¶ÁÈ¡²Ù×÷·û
        ReadExp();
        // ¼ì²é´íÎó×´Ì¬
        if(m_err_code != kERR_SUCCEED)
        {
            CnAssert(false);
            return;
        }

        // ÅÅ³ý¿Õ¸ñ
        m_token_scan.ReadSpace(m_buff_str, m_buff_idx);

        // ¶ÁÈ¡ ) ²Ù×÷·û
        ReadRightBracket();
        // ¼ì²é´íÎó×´Ì¬
        if(m_err_code != kERR_SUCCEED)
        {
            CnAssert(false);
            return;
        }

        // ÅÅ³ý¿Õ¸ñ
        m_token_scan.ReadSpace(m_buff_str, m_buff_idx);

        if(m_buff_str.size() > m_buff_idx)
        {
            ch1 = m_buff_str.at(m_buff_idx);
            if(!m_token_scan.IsOperator(ch1))
            {
                return;
            }

            // ¶ÁÈ¡²Ù×÷·û
            ReadOperator();
            // ¼ì²é´íÎó×´Ì¬
            if(m_err_code != kERR_SUCCEED)
            {
                CnAssert(false);
                return;
            }

            // ÅÅ³ý¿Õ¸ñ
            m_token_scan.ReadSpace(m_buff_str, m_buff_idx);

            // ¶ÁÈ¡±í´ïÊ½
            ReadExp();

            // ÅÅ³ý¿Õ¸ñ
            m_token_scan.ReadSpace(m_buff_str, m_buff_idx);
        }
    }
    else if(')' == ch1)
    {
        return;
    }
    else if(('+' == ch1) || ('-' == ch1))
    {
        m_buff_idx++;

        // ´íÎó
        if(m_buff_idx >= m_buff_str.size())
        {
            goto _ERR;
        }

        ch2 = m_buff_str.at(m_buff_idx);
        if(m_token_scan.IsLetter(ch2))
        {
            if('-' == ch1)
            {
                is_positive = false;
            }
            ReadIDExp(is_positive);
        }
        else
        {
            m_buff_idx--;
            ReadDigitExp();
        }
    }
    else if(m_token_scan.IsLetter(ch1))
    {
        ReadIDExp();
    }
    else if(m_token_scan.IsDigit(ch1))
    {
        ReadDigitExp();
    }

    return;

_ERR:
    CnAssert(false);
    m_err_code = kERR_FAILED;
}

void FormulaParser::ReadLeftBracket()
{
    FormulaToken token;

    // ¶ÁÈ¡×óÀ¨ºÅ
    token = m_token_scan.GetOperator(m_buff_str, m_buff_idx);
    if((token.m_errcode != FormulaToken::kTOKEN_OK) || (token.m_token != "("))
    {
        CnAssert(false);
        m_err_code = kERR_FAILED;
        return;
    }

    m_s1.push_back(token);
}

void FormulaParser::ReadRightBracket()
{
    FormulaToken token;

    // ¶ÁÈ¡ÓÒÀ¨ºÅºó¶ªÆú
    token = m_token_scan.GetOperator(m_buff_str, m_buff_idx);
    if((token.m_errcode != FormulaToken::kTOKEN_OK) || (token.m_token != ")"))
    {
        CnAssert(false);
        m_err_code = kERR_FAILED;
        return;
    }

    while(m_s1.size() > 0)
    {
        token = m_s1.back();
        m_s1.pop_back();

        if(token.m_token != "(")
        {
            m_s2.push_back(token);
        }
        else
        {
            break;
        }
    }
}

void FormulaParser::ReadOperator()
{
    FormulaToken token;
    FormulaToken stackToken;

    token = m_token_scan.GetOperator(m_buff_str, m_buff_idx);
    if(token.m_errcode != token.kTOKEN_OK)
    {
        CnAssert(false);
        m_err_code = kERR_FAILED;
        return;
    }

    // ÅÅ³ý¿Õ¸ñ
    m_token_scan.ReadSpace(m_buff_str, m_buff_idx);

    while(m_s1.size() > 0)
    {
        stackToken = m_s1.back();
        if((stackToken.m_priority >= token.m_priority) &&
           (stackToken.m_token != "("))
        {
            m_s2.push_back(stackToken);
            m_s1.pop_back();
        }
        else
        {
            break;
        }
    }

    m_s1.push_back(token);
}

void FormulaParser::FilterRightBrace()
{
    FormulaToken token;

    while(m_s1.size() > 0)
    {
        token = m_s1.back();
        m_s1.pop_back();
        if(token.m_token == "(")
        {
            break;
        }
        else
        {
            m_s2.push_back(token);
        }
    }
}

void FormulaParser::ReadIDExp(bool is_positive)
{
    int8 ch = '\0';
    FormulaToken token;

    token = m_token_scan.GetID(m_buff_str, m_buff_idx, is_positive);
    m_s2.push_back(token);

    // ÅÅ³ý¿Õ¸ñ
    m_token_scan.ReadSpace(m_buff_str, m_buff_idx);

    if(m_buff_str.size() > m_buff_idx)
    {
        ch = m_buff_str.at(m_buff_idx);
        if(')' == ch)
        {
            return;
        }

        // ¶ÁÈ¡²Ù×÷·û
        ReadOperator();
        // ¼ì²é´íÎó×´Ì¬
        if(m_err_code != kERR_SUCCEED)
        {
            CnAssert(false);
            return;
        }

        // ÅÅ³ý¿Õ¸ñ
        m_token_scan.ReadSpace(m_buff_str, m_buff_idx);

        // ¶ÁÈ¡±í´ïÊ½
        ReadExp();

        // ÅÅ³ý¿Õ¸ñ
        m_token_scan.ReadSpace(m_buff_str, m_buff_idx);
    }
}

void FormulaParser::ReadDigitExp()
{
    int8 ch = '\0';
    FormulaToken token;

    token = m_token_scan.GetDigit(m_buff_str, m_buff_idx);
    m_s2.push_back(token);

    // ÅÅ³ý¿Õ¸ñ
    m_token_scan.ReadSpace(m_buff_str, m_buff_idx);

    if(m_buff_str.size() > m_buff_idx)
    {
        ch = m_buff_str.at(m_buff_idx);
        if(')' == ch)
        {
            return;
        }

        // ¶ÁÈ¡²Ù×÷·û
        ReadOperator();
        // ¼ì²é´íÎó×´Ì¬
        if(m_err_code != kERR_SUCCEED)
        {
            CnAssert(false);
            return;
        }

        // ÅÅ³ý¿Õ¸ñ
        m_token_scan.ReadSpace(m_buff_str, m_buff_idx);

        // ¶ÁÈ¡±í´ïÊ½
        ReadExp();

        // ÅÅ³ý¿Õ¸ñ
        m_token_scan.ReadSpace(m_buff_str, m_buff_idx);
    }
}

void FormulaParser::Clear()
{
    m_buff_idx = 0;
    m_buff_str = "";
    m_err_code = kERR_SUCCEED;
    m_s1.clear();
    m_s2.clear();
}
