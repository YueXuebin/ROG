#include "gameholder_pch.h"
#include "lua_function.h"
#include "formula_manager.h"

float GetFormulaParam(const char* paramName)
{
    real val = 0;
    if(!FormulaManager::Instance())
    {
        CnError("FormulaManager not instance\n");
        return 0;
    }
    bool ret = FormulaManager::Instance()->GetFormulaParam(paramName, &val);
    if(!ret)
    {
        CnError("GetFormulaParam %s error\n", paramName);
    }
    return val;
}
