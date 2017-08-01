#include "formula_pch.h"
#include "formula_cal.h"

IMPLEMENT_SINGLETON(FormulaCal)

FormulaCal::FormulaCal()
{

}

FormulaCal::~FormulaCal()
{

}

real FormulaCal::CalResult(const FormulaTemplate* formulaTmpl, FormulaParameter* params)
{
    FormulaTemplate tmpl;

    if(!formulaTmpl) return 0.0;

    tmpl = *formulaTmpl;

    if(params)
    {
        tmpl.SetParams(params);
    }

    return Calculate(tmpl);
}

real FormulaCal::CalResult(const FormulaTemplate* formulaTmpl, FormulaObject* aObj, FormulaObject* bObj)
{
    FormulaTemplate tmpl;
    FormulaParameter params;

    if(!formulaTmpl) return 0.0;

    tmpl = *formulaTmpl;

    if(!params.Init(formulaTmpl, aObj, bObj))
    {
        return 0.0;
    }

    tmpl.SetParams(&params);

    return Calculate(tmpl);
}

real FormulaCal::Calculate(FormulaTemplate& formulaTmpl)
{
    FormulaToken t1;
    FormulaToken t2;
    std::vector<FormulaToken> s1;

    for(std::vector<FormulaToken>::iterator it = formulaTmpl.m_stack.begin();
        it != formulaTmpl.m_stack.end(); ++it)
    {
        if(it->m_type == FormulaToken::kTOKEN_TYPE_OPERATOR)
        {
            if(s1.size() < 1)
            {
                return 0.0;
            }
            t1 = s1.back();
            s1.pop_back();

            if(s1.size() < 1)
            {
                return 0.0;
            }
            t2 = s1.back();
            s1.pop_back();

            if(it->m_token == ">")
            {
                if(t2.m_digit <= t1.m_digit)
                {
                    t2.m_digit = t1.m_digit;
                }
            }
            else if(it->m_token == "<")
            {
                if(t2.m_digit >= t1.m_digit)
                {
                    t2.m_digit = t1.m_digit;
                }
            }
            else if(it->m_token == "-")
            {
                t2.m_digit = t2.m_digit - t1.m_digit;
            }
            else if(it->m_token == "+")
            {
                t2.m_digit = t2.m_digit + t1.m_digit;
            }
            else if(it->m_token == "*")
            {
                t2.m_digit = t2.m_digit * t1.m_digit;
            }
            else if(it->m_token == "/")
            {
                if(t1.m_digit == 0.0)
                {
                    t2.m_digit = 0.0;
                }
                else
                {
                    t2.m_digit = t2.m_digit / t1.m_digit;
                }
            }
            else
            {
                CnAssert(false);
            }

            s1.push_back(t2);
        }
        else
        {
            s1.push_back(*it);
        }
    }

    if(s1.size() > 0)
    {
        t1 = s1.back();
        return t1.m_digit;
    }
    else
    {
        CnAssert(false);
        CnWarn("Formula format error\n");
    }

    return 0.0;
}
