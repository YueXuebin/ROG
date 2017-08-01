#include "gameholder_pch.h"
#include "ai_action_container.h"
#include "ai_define.h"
#include "ai_factory.h"
#include "ai_state.h"



AIActionContainer::AIActionContainer() : 
AIContainer(AICONTAINER_TYPE_ACTION),
AIActionBase(AI_ACTION_SET_AI)
{

}

AIActionContainer::~AIActionContainer()
{

}

AIController* AIActionContainer::GetController()
{
    return m_pController;
}

void AIActionContainer::ParseXML( TiXmlElement* pDoc )
{
    AIActionBase::ParseXML(pDoc);

    // 子状态机的AI名
    const char* pName = pDoc->Attribute("aix");
    if(pName)
    {
        std::string fullName = "ai/";
        fullName += pName;
        fullName += ".aix";
        if(!AIContainer::LoadAI(fullName))
        {
            CnAssert(false);
            CnWarn("create ai %s failed\n", fullName.c_str());
        }
    }
    else
    {
        CnAssert(false);
    }
}

void AIActionContainer::Do()
{
    SetAIState(0);
}

void AIActionContainer::Update(int32 deltaTime)
{
    AIContainer::Update(deltaTime);
}
