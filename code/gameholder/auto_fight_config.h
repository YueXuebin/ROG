/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief �����Զ�ս��������Ϣ
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

    // HP�ٷֱȷ�ֵ
    uint32 GetHP() { return m_hp; }

    // �Ⱥ�˳��
    uint32 GetHPOrder() { return m_hp_order; }

    // MP�ٷֱȷ�ֵ
    uint32 GetMP() { return m_mp; }

    // ����˳��
    uint32 GetMPOrder() { return m_mp_order; }

    // �Զ�ս��������ʹ�õļ��ܱ�־
    uint32 GetSkills() { return m_skills; }

    // �Զ����߸���
    uint32 GetAutoRevival() { return m_auto_revival; }

    // �Զ�ս��
    uint32 GetAutoFight() { return m_auto_fight; }

    // �Զ�����HPҩ��
    uint32 GetAutoBuyHPDrug() { return m_auto_buy_hp_drug; }

    // �Զ�����MPҩ��
    uint32 GetAutoBuyMPDrug() { return m_auto_buy_mp_drug; }

    // �Զ����򹥻�ҩ��
    uint32 GetAutoBuyAttackDrug() { return m_auto_buy_attack_drug; }

    // �Զ��������ҩ��
    uint32 GetAutoBuyDefenceDrug() { return m_auto_buy_defence_drug; }

    // �Զ������ٶ�ҩ��
    uint32 GetAutoBuySpeedDrug() { return m_auto_buy_speed_drug; }

    // �Զ�ʹ�ù���ҩ��
    uint32 GetAutoUseAttackDrug() { return m_auto_use_attack_drug; }

    // �Զ�ʹ�÷���ҩ��
    uint32 GetAutoUseDefenceDrug() { return m_auto_use_defence_drug; }

    // �Զ�ʹ���ٶ�ҩ��
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
