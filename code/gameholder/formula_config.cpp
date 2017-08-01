#include "gameholder_pch.h"
#include "formula_config.h"
#include "formula_parser.h"

IMPLEMENT_SINGLETON(FormulaConfig)


FormulaConfig::FormulaConfig()
{

}

FormulaConfig::~FormulaConfig()
{

}

bool FormulaConfig::LoadConfig(const char* path)
{
    FormulaParser formulaParser;
    FormulaTemplate formulaTemplate;
    TiXmlDocument xmlDoc;
    uint32 id = 0;
    std::string calTxt;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load formula file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    for(TiXmlElement* formulaEle = xmlRoot->FirstChildElement(); formulaEle; formulaEle = formulaEle->NextSiblingElement())
    {
        id = Crown::SDAtou(formulaEle->Attribute("id"));
        calTxt = formulaEle->Attribute("cal");
        formulaTemplate = formulaParser.Parser(calTxt);

        if(!formulaTemplate.IsValid())
        {
            CnAssert(false);
            CnWarn("Parser formula failed id:%u\n", id);
            continue;
        }

        if(m_formulas.find(id) == m_formulas.end())
        {
            m_formulas[id] = formulaTemplate;
        }
        else
        {
            CnWarn("Duplicate formula id:%u\n", id);
        }
    }

    return true;
}

const FormulaTemplate*  FormulaConfig::GetFormulaTemplate(uint32 formulaID)
{
    if(m_formulas.find(formulaID) == m_formulas.end())
        return NULL;    // 该公式ID不存在
    return &m_formulas[formulaID];
}
