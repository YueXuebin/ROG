/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief 加载自动战斗配置信息
******************************************************************************/

#ifndef auto_fight_config_h__
#define auto_fight_config_h__

#include <vector>

struct ItemTemplate;

class AutoFightConfig
{
public:
    AutoFightConfig();
    virtual ~AutoFightConfig();

    DECLARE_SINGLETON(AutoFightConfig)

    void Clear();

    bool LoadConfig(const char* path);

    // HP百分比阀值
    uint32 GetHP() { return m_hp; }

    // 喝红顺序
    uint32 GetHPOrder() { return m_hp_order; }

    // MP百分比阀值
    uint32 GetMP() { return m_mp; }

    // 喝蓝顺序
    uint32 GetMPOrder() { return m_mp_order; }

    // 自动战斗中允许使用的技能标志
    uint32 GetSkills() { return m_skills; }

    // 自动道具复活
    uint32 GetAutoRevival() { return m_auto_revival; }

    // 自动战斗
    uint32 GetAutoFight() { return m_auto_fight; }

    // 自动购买HP药剂
    uint32 GetAutoBuyHPDrug() { return m_auto_buy_hp_drug; }

    // 自动购买MP药剂
    uint32 GetAutoBuyMPDrug() { return m_auto_buy_mp_drug; }

    // 自动购买攻击药剂
    uint32 GetAutoBuyAttackDrug() { return m_auto_buy_attack_drug; }

    // 自动购买防御药剂
    uint32 GetAutoBuyDefenceDrug() { return m_auto_buy_defence_drug; }

    // 自动购买速度药剂
    uint32 GetAutoBuySpeedDrug() { return m_auto_buy_speed_drug; }

    // 自动使用攻击药剂
    uint32 GetAutoUseAttackDrug() { return m_auto_use_attack_drug; }

    // 自动使用防御药剂
    uint32 GetAutoUseDefenceDrug() { return m_auto_use_defence_drug; }

    // 自动使用速度药剂
    uint32 GetAutoUseSpeedDrug() { return m_auto_use_speed_drug; }


private:
    uint32      m_hp;
    uint32      m_hp_order;
    uint32      m_mp;
    uint32      m_mp_order;
    uint32      m_skills;
    uint32      m_auto_revival;
    uint32      m_auto_fight;
    uint32      m_auto_buy_hp_drug;
    uint32      m_auto_buy_mp_drug;
    uint32      m_auto_buy_attack_drug;
    uint32      m_auto_buy_defence_drug;
    uint32      m_auto_buy_speed_drug;
    uint32      m_auto_use_attack_drug;
    uint32      m_auto_use_defence_drug;
    uint32      m_auto_use_speed_drug;
};

#endif
