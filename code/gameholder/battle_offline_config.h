/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief º”‘ÿ”Œœ∑≈‰÷√
******************************************************************************/

#ifndef battle_offline_config_h__
#define battle_offline_config_h__

#include "sdu.h"

class OfflinePlayerTemplate
{
public:
    void Clear()
    {
        m_playerID          = 0;
        m_name              = "";
        m_sex               = 0;
        m_career            = 0;
        m_level             = 0;

        m_battle_power      = 0;

        //m_strength.ClearAll();
        //m_agility.ClearAll();
        //m_spirit.ClearAll();
        //m_constitution.ClearAll();
        //m_attack.ClearAll();
        //m_defence.ClearAll();
        //m_fire_defence.ClearAll();
        //m_ice_defence.ClearAll();
        //m_thunder_defence.ClearAll();
        //m_attack_speed.ClearAll();
        //m_move_speed.ClearAll();
        //m_critical_attack.ClearAll();
        //m_hit_rate.ClearAll();
        //m_dodge.ClearAll();
        //m_consume_mp.ClearAll();
        //m_damage_reduction.ClearAll();
        //m_res_hp.ClearAll();
        //m_res_mp.ClearAll();
        //m_max_hp.ClearAll();
        //m_max_mp.ClearAll();


        m_equip_list.clear();
    }

    //void AddProp(uint32 type, struct EquipPropSet* propSet);
    void CalProp();

    uint32      m_playerID;
    std::string m_name;
    int8        m_sex;
    int8        m_career;
    int32       m_level;

    int         m_battle_power;

    //PropUnit    m_strength;
    //PropUnit    m_agility;
    //PropUnit    m_spirit;
    //PropUnit    m_constitution;
    //PropUnit    m_attack;
    //PropUnit    m_defence;
    //PropUnit    m_fire_defence;
    //PropUnit    m_ice_defence;
    //PropUnit    m_thunder_defence;
    //PropUnit    m_attack_speed;
    //PropUnit    m_move_speed;
    //PropUnit    m_critical_attack;
    //PropUnit    m_hit_rate;
    //PropUnit    m_dodge;
    //PropUnit    m_consume_mp;
    //PropUnit    m_damage_reduction;
    //PropUnit    m_res_hp;
    //PropUnit    m_res_mp;
    //PropUnit    m_max_hp;
    //PropUnit    m_max_mp;
    std::vector<uint32> m_equip_list;
};

class BattleOfflineConfig
{
public:
    BattleOfflineConfig();
    virtual ~BattleOfflineConfig();

    DECLARE_SINGLETON(BattleOfflineConfig)

    bool LoadConfig(const char* path);

    OFFLINE_PLAYER_INFO* GetPlayerTemplate(uint32 playerID);

    void GetPlayerTmplList(uint32 battle_power, std::vector<OFFLINE_PLAYER_INFO*>& list);

private:
    typedef std::map<uint32, OFFLINE_PLAYER_INFO> BofPlayerMap;
    typedef std::vector<OFFLINE_PLAYER_INFO*> BofPlayerList;
    typedef std::map<uint32, BofPlayerList> BofPowerSegment;

    BofPlayerMap    m_players;
    BofPowerSegment m_player_segment;
};

#endif
