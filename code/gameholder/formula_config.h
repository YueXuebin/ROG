//------------------------------------------------------------------------------
//
//    Copyright (C) DoinGame. All rights reserved.
//
//    File Name:   formula_config.h
//    Author:      Tomren
//    Create Time: 2014-02-10
//    Description: 
//
//    Version history:
//
//
//------------------------------------------------------------------------------
#ifndef formula_config_h__
#define formula_config_h__

#include "formula_template.h"

class FormulaConfig
{
public:
    FormulaConfig();
    ~FormulaConfig();

    DECLARE_SINGLETON(FormulaConfig)

    bool LoadConfig(const char* path);

    const FormulaTemplate*  GetFormulaTemplate(uint32 formulaID);

private:
    std::map<uint32, FormulaTemplate> m_formulas;
};

#endif // formula_config_h__
