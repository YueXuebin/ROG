#include "gameholder_pch.h"
#include "ai_group_register.h"
#include "ai_group.h"
#include "regular.h"
#include "creature.h"

AIGroupRegister::AIGroupRegister(Regular * pRegular)
:m_pRegular(pRegular),
m_GroupCounter(1)
{
    CnAssert(m_pRegular);
}

AIGroupRegister::~AIGroupRegister(void)
{
    for(AIGroupMap::iterator iter = m_GroupMap.begin(); iter != m_GroupMap.end(); iter++)
    {
        AIGroup* pAIGroup = iter->second;
        CnAssert(pAIGroup);
        CnDelete pAIGroup;
    }
    m_GroupMap.clear();
}

void AIGroupRegister::Update()
{
    for(AIGroupMap::iterator iter = m_GroupMap.begin(); iter != m_GroupMap.end(); iter++)
    {
        AIGroup* pAIGroup = iter->second;
        pAIGroup->Update();
    }
}

AIGroup* AIGroupRegister::CreateAIGroup(uint32 id)
{
    if(id == 0)
    {
        CnAssert(false);
        return NULL;
    }

    CnAssert(m_GroupMap.find(id) == m_GroupMap.end());

    AIGroup* pNewGroup = CnNew AIGroup(this, id);
    m_GroupMap[id] = pNewGroup;
    return pNewGroup;
}

AIGroup* AIGroupRegister::GetAIGroup(uint32 id)
{
    AIGroupMap::iterator iter = m_GroupMap.find(id);
    if(iter != m_GroupMap.end())
        return iter->second;
    else
        return NULL;
}

AIGroup* AIGroupRegister::GetAIGroupWithCreate(uint32 id)
{
    if(id == 0)
    {
        CnAssert(false);
        return NULL;
    }

    AIGroup* pAIGroup = GetAIGroup(id);
    if(pAIGroup)
    {
        CnAssert(pAIGroup->GetId() == id);
        return pAIGroup;
    }
    else
        return CreateAIGroup(id);

}
