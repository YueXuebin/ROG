#ifndef passive_skill_config_manager_h__
#define passive_skill_config_manager_h__

#include "passive_skill_config.h"
#include "passive_skill_card_config.h"

typedef std::map<uint32, const PASSIVE_SKILL_CARD_ROOT_TPLT_STRUCT*> PassiveSkillCardConfigMap;

class PassiveSkillConfigManager
{
public:
	PassiveSkillConfigManager(void);
	virtual ~PassiveSkillConfigManager(void);

	DECLARE_SINGLETON(PassiveSkillConfigManager);

	bool LoadConfig(const char* path);

	const PASSIVE_SKILL_ROOT_STRUCT* GetPassiveSkillListConfigData();
	const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT* GetPassiveSkillConfigData(uint32 skillID);
	const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_ITEM_STRUCT* GetPassiveSkillItemConfigData(uint32 skillID, uint32 level);

    const PASSIVE_SKILL_CARD_ROOT_TPLT_STRUCT* GetPassiveSkillCardConfig(uint32 id);

private:
	const PASSIVE_SKILL_ROOT_STRUCT* m_PassiveSkillList;
	std::map<uint32, const PASSIVE_SKILL_ROOT_PASSIVE_SKILL_STRUCT*> m_PassiveSkillConfigMap;	// <id, config data>
    PassiveSkillCardConfigMap m_PassiveSkillCardConfigMap;
};

#endif // passive_skill_manager_h__