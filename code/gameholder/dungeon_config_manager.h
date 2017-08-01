#ifndef dungeon_config_manager_h__
#define dungeon_config_manager_h__

#include "dungeon_drop_config.h"
#include "dungeon_clearance_bonus_config.h"
#include "dungeon_exp_bonus_config.h"
#include "regular_data.h"
#include "config_reader.h"

/*
 *	关卡相关配置
 */

struct SDungeonClearanceBonusConfig
{
    SDungeonClearanceBonusConfig()
    {
        Clear();
    }

    void Clear()
    {
        pStruct = NULL;
        ClearanceBonus.Clear();
        TwoStarBonus.Clear();
        ThreeStarBonus.Clear();
    }

    const DUNGEON_CLEARANCE_BONUS_ROOT_DUNGEON_STRUCT* pStruct;
    REWARD_TABLE    ClearanceBonus;
    REWARD_TABLE    TwoStarBonus;
    REWARD_TABLE    ThreeStarBonus;
};

struct SDungeonExpBonusConfig
{
    SDungeonExpBonusConfig()
    {
        Clear();
    }

    void Clear()
    {
        pStruct = NULL;
        exp_bonus_1.Clear();
        exp_bonus_2.Clear();
        exp_bonus_3.Clear();
        exp_bonus_4.Clear();
        exp_bonus_5.Clear();
    }

    const DUNGEON_EXP_BONUS_ROOT_DUNGEON_STRUCT* pStruct;
    REWARD_TABLE    exp_bonus_1;
    REWARD_TABLE    exp_bonus_2;
    REWARD_TABLE    exp_bonus_3;
    REWARD_TABLE    exp_bonus_4;
    REWARD_TABLE    exp_bonus_5;
};

typedef std::map<uint32, SDungeonClearanceBonusConfig> DungeonClearanceBonusConfigMap;
typedef std::map<uint32, SDungeonExpBonusConfig> DungeonExpBonusConfigMap;

class DungeonConfigManager
{
public:
    DungeonConfigManager();
    virtual ~DungeonConfigManager();

    DECLARE_SINGLETON(DungeonConfigManager)

public:
    bool LoadConfig(const char* path);

public:
    const DUNGEON_DROP_ROOT_DROP_STRUCT* FindDropByID(uint32 id);
    const SDungeonClearanceBonusConfig* FindClearanceBonusByID(uint32 id);
    const SDungeonExpBonusConfig* FindExpBonusByID(uint32 id);

private:
    std::map<uint32, const DUNGEON_DROP_ROOT_DROP_STRUCT*> m_DungeonDrop;
    DungeonClearanceBonusConfigMap m_DungeonClearanceBonusConfigMap;
    DungeonExpBonusConfigMap m_DungeonExpBonusConfigMap;
};


#endif // dungeon_config_manager_h__
