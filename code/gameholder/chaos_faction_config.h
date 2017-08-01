#ifndef chaos_faction_config_h__
#define chaos_faction_config_h__

#include "path_data.h"

typedef struct ChaosProp
{
    ChaosProp()
    {
        Clear();
    }

    void Clear()
    {
        type = 0;
        value = 0;
    }

    int32   type;
    real    value;
}ChaosProp;

typedef std::vector<ChaosProp> ChaosPropertyList;

typedef struct ChaosPlayerTmp
{
    void Clear()
    {
        career = 0;
        level = 1;
        skill_level = 1;
        valueTotal = 0;
        propList.clear();
    }

    int8            career;
    int32           level;
    int32           skill_level;
    real            valueTotal;
    ChaosPropertyList    propList;
}ChaosPlayerTmp;

typedef struct ChaosBossTmp
{
    void Clear()
    {
        group = 0;
        spring_id = 0;
        perimeter_id = 0;
        inner_id = 0;
        crystal_id = 0;
        poluo_id = 0;
        ai = "";
        pathData.Clear();
    }

    int8        group;
    uint32      spring_id;
    uint32      perimeter_id;
    uint32      inner_id;
    uint32      crystal_id;
    uint32      poluo_id;
    std::string ai;
    PathData    pathData;
}ChaosBossTmp;

typedef std::map<uint32, real> ChaosBattlepowerMap;

typedef struct ChaosReward 
{
    void Clear()
    {
        rank = 0;
        integral_base = 0;
        integral_begin = 0;
        integral_end = 0;
        alliance_point = 0;
    }

    uint32  rank;
    int32   integral_base;
    int32   integral_begin;
    int32   integral_end;
    int32   alliance_point;
}ChaosReward;

class ChaosFactionConfig
{
public:
    ChaosFactionConfig();
    ~ChaosFactionConfig();

    DECLARE_SINGLETON(ChaosFactionConfig)

    bool LoadConfig(const char* path);

    const ChaosPlayerTmp* GetPlayerProp(int8 career);

    const PropertyList* GetPropAddProByBattlePower(uint32 battlePower);

    real GetScoreByPlayerDead() {return m_scorePlayerDead;}
    real GetScoreByDamageTower() {return m_scoreDamageTower;}
    real GetScoreByPoluoDead() {return m_scorePoluoDead;}

    uint32 GetPoluoKillNum() {return m_poluoKillNum;}
    uint32 GetSkillLevel() {return m_skillLevel;}
    real GetBattlePower() {return m_battlePower;}
    real GetCoefficient() {return m_coefficient;}
    uint32 GetPlayerMax() {return m_player_max;}

    real GetPropByBattlepower(uint32 propType, real battlepower);      // 由战力得到玩家属性

    const ChaosBossTmp* GetBossTmp(int8 groupType);
    const std::map<uint32, ChaosReward>* GetWinReward() {return &m_winRewards;}
    const std::map<uint32, ChaosReward>* GetLoseReward() {return &m_loseRewards;};

    const ChaosReward* GetWinRewardTmpl(int32 rank);
    const ChaosReward* GetLoseRewardTmpl(int32 rank);

    const PropertyList* GetFollowPetProp(uint32 followPetID);
private:
    bool LoadChaosFactionConfig(const char* path);

    bool LoadBattlePower(TiXmlElement* rootEle);
    bool LoadCareers(TiXmlElement* rootEle);
private:
    uint32 m_player_max;

    uint32 m_poluoKillNum;
    uint32 m_skillLevel;
    real m_battlePower;           // 固定战力
    real m_coefficient;           // 战力系数

    std::map<int8, ChaosPlayerTmp>      m_playerPropMap;
    real m_scorePlayerDead;
    real m_scoreDamageTower;
    real m_scorePoluoDead;

    std::map<int8, ChaosBossTmp> m_bossMap;
    std::map<uint32, ChaosReward> m_winRewards;
    std::map<uint32, ChaosReward> m_loseRewards;
    ChaosBattlepowerMap m_chaosBattlepowerMap;

    std::map<uint32, PropertyList>  m_followPetPropMap;
};

#endif // chaos_faction_config_h__
