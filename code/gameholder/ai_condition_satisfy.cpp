#include "gameholder_pch.h"
#include "ai_condition_satisfy.h"
#include "ai_define.h"


AIConditionSatisfy::AIConditionSatisfy() :
AIConditionBase(AI_CONDITION_SATISFY)
{
    m_pro = 100;
}

AIConditionSatisfy::~AIConditionSatisfy()
{

}

void AIConditionSatisfy::ParseXML( TiXmlElement* pDoc )
{
    AIConditionBase::ParseXML(pDoc);

    pDoc->QueryIntAttribute("pro", &m_pro);
}

bool AIConditionSatisfy::IsSatisfy()
{
    int32 random = Crown::SDRandom(100);
    if (random < m_pro)
    {
        return true;
    }
    return false;
}
