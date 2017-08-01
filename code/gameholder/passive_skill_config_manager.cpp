#include "gameholder_pch.h"
#include "passive_skill_config_manager.h"

IMPLEMENT_SINGLETON(PassiveSkillConfigManager)

PassiveSkillConfigManager::PassiveSkillConfigManager(void)
{
	PASSIVE_SKILL_ENTRY::CreateInstance();
    PASSIVE_SKILL_CARD_ENTRY::CreateInstance();
}

PassiveSkillConfigManager::~PassiveSkillConfigManager(void)
{
	PASSIVE_SKILL_ENTRY::DestroyInstance();
    PASSIVE_SKILL_CARD_ENTRY::DestroyInstance();
}

bool PassiveSkillConfigManager::LoadConfig(const char* path)
{
	if (PASSIVE_SKILL_ENTRY::Instance()->LoadConfig(path))
	{
		m_PassiveSkillList = &PASSIVE_SKILL_ENTRY::Instance()->GetStruct();

		for(std::vector<PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT>::const_iterator iter = m_PassiveSkillList->passive_skill_list.begin();
			iter != PASSIVE_SKILL_ENTRY::Instance()->GetStruct().passive_skill_list.end(); iter++)
		{
			m_PassiveSkillConfigMap[iter->id] = &(*iter);
		}
	}

    if (PASSIVE_SKILL_CARD_ENTRY::Instance()->LoadConfig(path))
    {
        const PASSIVE_SKILL_CARD_ROOT_STRUCT & Struct = PASSIVE_SKILL_CARD_ENTRY::Instance()->GetStruct();
        for (std::vector<PASSIVE_SKILL_CARD_ROOT_TPLT_STRUCT>::const_iterator cIter = Struct.tplt_list.begin();
            cIter != Struct.tplt_list.end(); ++cIter)
        {
            if (m_PassiveSkillCardConfigMap.find(cIter->id) != m_PassiveSkillCardConfigMap.end())
            {
                CnWarn("PASSIVE_SKILL_CARD %d!", cIter->id);
                continue;
            }

            m_PassiveSkillCardConfigMap.insert(std::make_pair(cIter->id, &(*cIter)));
        }
    }

	return true;
}

const PASSIVE_SKILL_ROOT_STRUCT* PassiveSkillConfigManager::GetPassiveSkillListConfigData()
{
	return m_PassiveSkillList;
}

const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT* PassiveSkillConfigManager::GetPassiveSkillConfigData(uint32 skillID)
{
	std::map<uint32, const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT*>::const_iterator iter = m_PassiveSkillConfigMap.find(skillID);
	if (iter != m_PassiveSkillConfigMap.end())
		return iter->second;
	else
	{
		CnWarn("passive skill %d config data not exist\n", skillID);
		return NULL;
	}
}

const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_ITEM_STRUCT* PassiveSkillConfigManager::GetPassiveSkillItemConfigData(uint32 skillID, uint32 level)
{
	const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT* pPassiveSkillConfig = GetPassiveSkillConfigData(skillID);
	if (pPassiveSkillConfig)
	{
		for(std::vector<PASSIVE_SKILL_ROOT_PASSIVE_SKILL_ITEM_STRUCT>::const_iterator iter = pPassiveSkillConfig->item_list.begin(); 
			iter != pPassiveSkillConfig->item_list.end(); iter++)
		{
			if(iter->level == level)
			{
				return &(*iter);
			}
		}
	}
	return NULL;
}

const PASSIVE_SKILL_CARD_ROOT_TPLT_STRUCT* PassiveSkillConfigManager::GetPassiveSkillCardConfig(uint32 id)
{
    PassiveSkillCardConfigMap::const_iterator cIter = m_PassiveSkillCardConfigMap.find(id);
    if (m_PassiveSkillCardConfigMap.end() == cIter)
    {
        return NULL;
    }

    return cIter->second;
}
