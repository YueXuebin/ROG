#include "formula_pch.h"
#include "formula_template.h"

FormulaTemplate::FormulaTemplate()
{
    
}

FormulaTemplate::~FormulaTemplate()
{
    
}

void FormulaTemplate::Clear()
{
    m_valid = false;
    m_table.clear();
    m_stack.clear();
}

bool FormulaTemplate::SetParams(const FormulaParameter* params)
{
    // 初始化所有ID的数值
    for(std::vector<FormulaToken>::iterator it = m_stack.begin();
            it != m_stack.end(); ++it)
    {
        // 初始化 ID 的关键字，操作符和数字不需要重新初始化
        if(it->m_type == FormulaToken::kTOKEN_TYPE_ID)
        {
            it->m_errcode = FormulaToken::kTOKEN_ERR;   // 通过 ERR 区分之后是否被初始化
            it->m_digit = 0.0;                          // 初始化 0.0 为默认数值
        }
    }

    if(params)
    {
        // 如果有参数列表
        for(FormulaParameter::CalParameterList::const_iterator paramIt = params->m_params.begin();
            paramIt != params->m_params.end(); ++paramIt)
        {
            SetParam(paramIt->second);
        }
    }

    // 检查是否所有的参数都被初始化
    for(std::vector<FormulaToken>::iterator it = m_stack.begin();
        it != m_stack.end(); ++it)
    {
        // 检查到有未被初始化的参数(检查Token的Err状态)
        if((it->m_type == FormulaToken::kTOKEN_TYPE_ID) && 
           (it->m_errcode == FormulaToken::kTOKEN_ERR))
        {
            CnAssert(false);
        }
    }

    return true;
}

void FormulaTemplate::SetParam(const CalParameter& param)
{
    for(std::vector<FormulaToken>::iterator tokenIt = m_stack.begin();
        tokenIt != m_stack.end(); ++tokenIt)
    {
        // 初始化 ID 的关键字，操作符和数字不需要重新初始化
        if((tokenIt->m_type == FormulaToken::kTOKEN_TYPE_ID) && (tokenIt->m_token == param.key))
        {
            if(tokenIt->m_errcode != FormulaToken::kTOKEN_ERR)
            {
                CnAssert(false);
                // 打印错误信息，初始化多次
                CnWarn("FormulaToken multiple initialize id:%s", param.key.c_str());
            }
            else
            {
                tokenIt->m_errcode = FormulaToken::kTOKEN_OK;
                tokenIt->m_digit = param.val;
                // 如果ID被标记有负号，则对原参数值再做负号操作
                if(!tokenIt->m_positive)
                {
                    tokenIt->m_digit = -tokenIt->m_digit;
                }
            }
        }
    }
}

void FormulaTemplate::PushToken(FormulaToken& token)
{
    // 向表中插入不存在的ID
    if((token.m_type == FormulaToken::kTOKEN_TYPE_ID) &&
       (m_table.find(token.m_token) == m_table.end()))
    {
        m_table.insert(token.m_token);
    }
    // 顺序压入堆栈
    m_stack.push_back(token);
}
