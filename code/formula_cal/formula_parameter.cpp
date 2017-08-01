#include "formula_pch.h"
#include "formula_template.h"
#include "formula_object.h"
#include "formula_token.h"

bool FormulaParameter::Init(const FormulaTemplate* formulaTmpl, FormulaObject* aObject, FormulaObject* bObject)
{
    if(!formulaTmpl) return false;

    for(std::set<std::string>::const_iterator it = formulaTmpl->m_table.begin();
        it != formulaTmpl->m_table.end(); ++it)
    {
        std::vector<std::string> keyValue = Crown::SDSplitStrings(*it, '_');
        if((keyValue.size() == 1) && aObject)
        {
            AddParam(*it, aObject->GetFunctionValue(*it));
        }
        else if(keyValue.size() == 2)
        {
            if((keyValue[0] == "a") && aObject)
            {
                AddParam(*it, aObject->GetFunctionValue(keyValue[1].c_str()));
            }
            else if((keyValue[0] == "b") && bObject)
            {
                AddParam(*it, bObject->GetFunctionValue(keyValue[1].c_str()));
            }
            else
            {
                CnWarn("Formula param format error:%s", it->c_str());
            }
        }
        else
        {
            CnWarn("Formula param format error:%s", it->c_str());
        }
    }

    return true;
}

void FormulaParameter::Clear()
{
    m_params.clear();
}

bool FormulaParameter::GetParam(const std::string& key, real* val)
{
    CalParameterList::iterator iter = m_params.find(key);
    if(iter == m_params.end())
        return false;
    
    if(val)
        (*val) = iter->second.val;
    return true;
}

// 添加参数
void FormulaParameter::AddParam(const std::string& key, int8 val)
{
    CalParameter param;
    param.key = key;
    param.val = val;
    m_params[key] = param;
}

void FormulaParameter::AddParam(const std::string& key, uint8 val)
{
    CalParameter param;
    param.key = key;
    param.val = val;
    m_params[key] = param;
}

void FormulaParameter::AddParam(const std::string& key, int16 val)
{
    CalParameter param;
    param.key = key;
    param.val = val;
    m_params[key] = param;
}

void FormulaParameter::AddParam(const std::string& key, uint16 val)
{
    CalParameter param;
    param.key = key;
    param.val = val;
    m_params[key] = param;
}

void FormulaParameter::AddParam(const std::string& key, int32 val)
{
    CalParameter param;
    param.key = key;
    param.val = (real)val;
    m_params[key] = param;
}

void FormulaParameter::AddParam(const std::string& key, uint32 val)
{
    CalParameter param;
    param.key = key;
    param.val = (real)val;
    m_params[key] = param;
}

//void FormulaParameter::AddParam(const std::string& key, int64 val)
//{
//    CalParameter param;
//    param.key = key;
//    param.val = (real)val;
//    m_params[key] = param;
//}
//
//void FormulaParameter::AddParam(const std::string& key, uint64 val)
//{
//    CalParameter param;
//    param.key = key;
//    param.val = (real)val;
//    m_params[key] = param;
//}

void FormulaParameter::AddParam(const std::string& key, real val)
{
    CalParameter param;
    param.key = key;
    param.val = val;
    m_params[key] = param;
}

//void FormulaParameter::AddParam(const std::string& key, const std::string& val)
//{
//    CalParameter param;
//    param.key = key;
//    param.val = (real)Crown::SDStrtod(val.c_str(), NULL);
//    m_params[key] = param;
//}
