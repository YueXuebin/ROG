#include "gameholder_pch.h"
#include "battle_recorder.h"
#include "game_define.h"
#include "scene.h"
#include "player.h"
#include "event_define.h"
#include "dungeon_score_config.h"
#include "formula_manager.h"

BattleRecorder::BattleRecorder(Player* player) :
m_Player(player)
{
    Clear();
}

BattleRecorder::~BattleRecorder()
{

}

void BattleRecorder::Clear()
{
    m_DungeonTime       = 0;    // 副本游戏时间ms
    m_LastDungeonId     = 0;    // 上一个副本的id

    // 连杀相关
    m_ComboKillCD       = 0;    // 连杀CD
    m_ComboKillCount    = 0;    // 连杀计数
    m_ComboKillMax      = 0;    // 最高连杀数
    m_ComboKillNumber   = 0;    // 连杀次数

    // 被击
    m_OnHitCount        = 0;    // 被击次数

    // 输出
    m_AllAttackDamage   = 0;    // 总输出伤害

    // 吸收精魂数
    m_GainSoulPoint     = 0;    // 吸收精魂数

    // 世界Boss伤害
    m_WorldBossDamage   = 0;

    // 世界Boss排名
    m_WorldBossRank     = 0;
}

void BattleRecorder::onEnterScene( Scene* pScene )
{
    uint32 newDungeonId = DUNGEON_ID(m_Player->GetUnionMapID());
    if(m_LastDungeonId != newDungeonId)
    {
        Reset();
        m_LastDungeonId = newDungeonId;
    }
}

void BattleRecorder::onLeaveScene( Scene* pScene )
{

}

void BattleRecorder::Reset()
{
    m_DungeonTime = 0;      // 副本计时更新

    // 连杀更新
    m_ComboKillCD = 0;
    m_ComboKillCount = 0;
    m_ComboKillMax = 0;
    m_ComboKillNumber = 0;

    // 被击数更新
    m_OnHitCount = 0;

    // 输出清零
    m_AllAttackDamage = 0;

    // 吸收精魂数
    m_GainSoulPoint = 0;
}

void BattleRecorder::Update()
{
    m_DungeonTime += g_ServerUpdateTime;

    // 连杀
    if(m_ComboKillCD > 0)
    {
        m_ComboKillCD -= g_ServerUpdateTime;
    }
    else
    {
        if(m_ComboKillCount > 0)
            m_ComboKillNumber++;        // 完成一次连杀

        m_ComboKillCount = 0;
    }

}

// 输出伤害
void BattleRecorder::DoDamage(BattleUnit* sufferer, int32 damage)
{
    m_AllAttackDamage += damage;
}

void BattleRecorder::OnKill( BattleUnit* sufferer )
{
    m_ComboKillCD += 3000;                  // 每击杀一次,CD增加
    m_ComboKillCD = Crown::SDMin(m_ComboKillCD, COMBO_KILL_CD);

    m_ComboKillCount++;

    if(m_ComboKillMax < m_ComboKillCount)
        m_ComboKillMax = m_ComboKillCount;

    // 通知客户端，连杀
    if(m_ComboKillCount > 1)
    {
        COMBO_KILL_NTF ntf;
        ntf.combo = m_ComboKillCount;           // 当前连杀数
        ntf.combo_max = m_ComboKillMax;         // 最大连杀数
        ntf.combo_cd = m_ComboKillCD;           // 当前连杀CD
        m_Player->SendToGate(MSG_COMBO_KILL_NTF, &ntf);
    }
}

void BattleRecorder::OnDead( BattleUnit* killer )
{

}

void BattleRecorder::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    switch(eventId)
    {
        case BATTLE_EVENT_ONHIT:   // 被攻击
            m_OnHitCount++;
            break;
    }
}

void BattleRecorder::GainSoulPoint( int32 val )
{
    m_GainSoulPoint += val;
}