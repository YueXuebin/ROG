#ifndef skill_config_manager_h__
#define skill_config_manager_h__

#include "skill_config.h"
#include "buff_config.h"
#include "skill_levelup_config.h"
#include "rune_config.h"

/*
 *	技能相关配置xml
 */
class SkillConfigManager
{
public:
    SkillConfigManager();
    virtual ~SkillConfigManager();

    DECLARE_SINGLETON(SkillConfigManager)

    bool LoadConfig(const char* path);

    const SKILL_ROOT_SKILL_STRUCT* GetSkillConfigData(uint32 skillID);
    const BUFF_ROOT_BUFF_STRUCT* GetBuffConfigData(uint32 buffID);
    uint32  GetSkillGrowByLevelQuality(uint32 level, uint32 quality, int32* result_exp=NULL);       // 根据技能等级和品质获取成长率
    int32   GetSkillExpByLevelQuality(int32 level, uint32 quality);

    const RUNE_ROOT_RUNE_STRUCT* GetRuneConfigData(uint32 runeID);

    int32   GetSkillMinLevel();
    int32   GetSkillMaxLevel();

private:
    std::map<uint32, const SKILL_ROOT_SKILL_STRUCT*>    m_SkillConfigMap;       // <skill id, skill config data>
    std::map<uint32, const BUFF_ROOT_BUFF_STRUCT*>      m_BuffConfigMap;        // <skill id, skill config data>
    std::map<uint32, const SKILL_LEVELUP_ROOT_SKILL_VALUE_STRUCT*>  m_SkillLevelUpConfigMap;    // <level, level config data>

    std::map<uint32, const RUNE_ROOT_RUNE_STRUCT*>      m_RuneConfigMap;             // <rune id, rune config data>
};


#endif // skill_config_manager_h__
