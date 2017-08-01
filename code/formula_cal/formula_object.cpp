#include "formula_pch.h"
#include "formula_object.h"

// ==================================================
// 构造函数
// ==================================================
FormulaObject::FormulaObject()
{
    InitFunctionMap();
}

// ==================================================
// 析构函数
// ==================================================
FormulaObject::~FormulaObject()
{
    
}


// ==================================================
// 初始化函数映射表
// ==================================================
void FormulaObject::InitFunctionMap()
{
    RegisterFunction("hp", &FormulaObject::GetHP);
    RegisterFunction("mp", &FormulaObject::GetMP);
    RegisterFunction("max_hp", &FormulaObject::GetMaxHp);
    RegisterFunction("max_mp", &FormulaObject::GetMaxMp);
    RegisterFunction("attack", &FormulaObject::GetArmor);
    RegisterFunction("armor", &FormulaObject::GetArmor);
    RegisterFunction("dodge", &FormulaObject::GetDodge);
}

// ==================================================
// 注册属性函数
// ==================================================
void FormulaObject::RegisterFunction(std::string name, FuncPoint func)
{
    std::map<std::string, FuncPoint>::iterator it;
    it = m_funcMap.find(name);
    if(it != m_funcMap.end())
    {
        CnWarn("Duplicate battle_unit function name: %s\n", name.c_str());
        return;
    }
    m_funcMap[name] = func;
}

// ==================================================
// 根据名称获得函数值
// ==================================================
int32 FormulaObject::GetFunctionValue(std::string name)
{
    std::map<std::string, FuncPoint>::iterator it;
    it = m_funcMap.find(name);
    if(it == m_funcMap.end())
    {
        CnWarn("Can't find battle_unit function name: %s\n", name.c_str());
        return 0;
    }
    return (this->*it->second)();
}
