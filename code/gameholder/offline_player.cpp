#include "gameholder_pch.h"
#include "offline_player.h"
#include "relife_helper.h"
#include "scene.h"
#include "event_define.h"

CnImplementRTTI(OfflinePlayer, Creature);

OfflinePlayer::OfflinePlayer() :
Creature(OBJ_OFFLINE_PLAYER)
{
    OfflinePlayerClear();
}

OfflinePlayer::~OfflinePlayer()
{
}

void OfflinePlayer::SetPorperyByInfo( const OFFLINE_PLAYER_INFO& recordInfo )
{
    //SetMaxHp(recordInfo.max_hp);
   // SetMaxMp(recordInfo.max_mp);
    SetHP(GetMaxHp());
    SetMP(GetMaxMp());

    //SetHpRecovery(recordInfo.res_hp);
    //SetMpRecovery(recordInfo.res_mp);

    //SetAttack(recordInfo.attack);
    //SetArmor(recordInfo.defence);
    //SetFireResistance(recordInfo.fire_defence);
    //SetIceResistance(recordInfo.ice_defence);
    //SetLightningResistance(recordInfo.thunder_defence);

    //SetAttackSpeed(recordInfo.attack_speed);
    //SetMoveSpeed(recordInfo.move_speed);
    //SetSpeed(700);

    //SetCrit(recordInfo.critical_attack);
    //SetCritDamage(recordInfo.cri_att_damage_add);
    //SetHit(recordInfo.hit_rate);
    //SetDodge(recordInfo.dodge);

    m_PlayerInfo = recordInfo;

    if(m_PlayerInfo.sex == 0)
    {
        if(Crown::SDRandom(100) <= 50)
            m_PlayerInfo.sex = 1;       // 男性
        else
            m_PlayerInfo.sex = 2;       // 女性
    }
    SetLevel(recordInfo.level);
}

void OfflinePlayer::OfflinePlayerClear()
{

}

void OfflinePlayer::Update(uint32 frame_count)
{
    Creature::Update(frame_count);
}

void OfflinePlayer::OnDead(BattleUnit* killer)
{
    Creature::OnDead(killer);

//    // 宠物死亡
//    GetPetController()->KillAllPet();

    GetScene()->SendEvent(0, BATTLE_EVENT_OFFLINE_PLAYER_DEAD, 0, 0, 0);
}

//uint32 OfflinePlayer::GetDefaultSkill()
//{
//    uint32 career = m_PlayerInfo.career;
//    if(career == CAREER_WARRIOR)
//        return WARRIOR_DEFAULT_SKILLID;
//    else if(career == CAREER_MAGE)
//        return MAGE_DEFAULT_SKILLID;
//    else
//        return PRIEST_DEFAULT_SKILLID;
//}
