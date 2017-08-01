#include "gameholder_pch.h"
#include "regular_world_boss.h"
#include "event_define.h"
#include "scene.h"
#include "game_util.h"
#include "checkpoint.h"
#include "boss.h"
#include "ai_controller.h"
#include "player.h"
#include "trigger.h"
#include "battle_recorder.h"
#include "world_boss_config.h"
#include "player_manager.h"
#include "world_boss_manager.h"
#include "dungeon_set.h"
#include "dungeon_manager.h"
#include "battle_unit_connecter.h"

RegularWorldBoss::RegularWorldBoss( RegularDoc* pDoc ) :
RegularMultiple(pDoc)
{
    m_dungeonID = 0;
    m_KillerID  = 0;
}

RegularWorldBoss::~RegularWorldBoss()
{

}

void RegularWorldBoss::OneSecondUpdate(int32 deltatime)
{
    RegularMultiple::OneSecondUpdate(deltatime);
}

void RegularWorldBoss::FillStartPostion(Player* player, uint32 triggerID)
{
    // 玩家出现在Trigger 1范围内随机位置处
    Trigger* pFirstTrigger = GetScene()->GetFirstTrigger();
    if(pFirstTrigger)
    {
        CoVec2 randPos = GetScene()->FindValidPositionInCircle(pFirstTrigger->GetPosition(), pFirstTrigger->GetRadius(), player->GetRadius()); // trigger范围内查找一个有用的随机点
        player->SetPosition((int32)randPos.x, (int32)randPos.y);

        player->SetAngle( pFirstTrigger->GetAngle() );
    }
    else
    {
        CnWarn("scene %d no trigger\n", GetScene()->GetSceneID() );
        player->SetPosition(0, 0);
    }
}

void RegularWorldBoss::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularMultiple::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
    case BATTLE_EVENT_WORLD_BOSS_DEAD:
        {
            // 世界boss死亡
            uint32 bossCreatureID = arg1;
            m_KillerID = arg2;
            if(m_WorldBossData.creatureId == bossCreatureID)
            {
                DungeonClear(true);
            }
        }
        break;
    }
}

// 开始世界BOSS
void RegularWorldBoss::Start()
{
    
}

// 结束世界BOSS
void RegularWorldBoss::End()
{
    // 世界boss失败
    DungeonClear(false);
}

void RegularWorldBoss::OnDungeonClear(bool success, std::vector<std::string>* arglist)
{
    BattleUnitConnecter* battleConnecter = 0;

    battleConnecter = m_pScene->GetBattleUnitConnecter();
    if(battleConnecter)
    {
        battleConnecter->EndConnecter(success);
    }
}

bool RegularWorldBoss::IsGameStart()
{
    return true;
    /**
    if(RegularMultiple::IsGameStart())
        return true;

    SetGameStart(true);

    return RegularMultiple::IsGameStart();
    */
}

void RegularWorldBoss::OnCreatureEnter( BattleUnit* creature )
{
    RegularMultiple::OnCreatureEnter(creature);

    SetDefaultGroup(creature);

    if(creature->GetObjectType() == OBJ_PLAYER)
    {
        Player* player = (Player*)creature;
        //player->SendTaskEvent(TASK_EVENT_WORLD_BOSS, 0, 0);

        player->SendtoCenterLog(LOG_ACTIVITY, WriterToLogByActivity(player->GetLevel(), ACTIVITY_TYPE_WORLD_BOSS, ACTIVITY_INTO));

        // 活跃度
        player->SendMyselfEvent(LIVENESS_EVENT_TARGET_WORLD_BOSS, 1, 0, 0);
    }
}

void RegularWorldBoss::OnCreatureLeave(BattleUnit* creatrue)
{
    RegularMultiple::OnCreatureLeave(creatrue);

    if (creatrue->GetObjectType() == OBJ_PLAYER)
    {
        Player* player = (Player*)creatrue;
        player->SendtoCenterLog(LOG_ACTIVITY, WriterToLogByActivity(player->GetLevel(), ACTIVITY_TYPE_WORLD_BOSS, ACTIVITY_LEAVE));
    }
}

void RegularWorldBoss::CreateWorldBoss(uint32 creatureID, uint32 dungeonID)
{
    Checkpoint* pCheckPoint = m_pScene->GetCheckPoint(0);

    m_dungeonID = dungeonID;

    if(!pCheckPoint)
        return;

    m_WorldBossData.creatureId = creatureID;
    m_WorldBossData.x = pCheckPoint->GetX();
    m_WorldBossData.y = pCheckPoint->GetY();
    m_WorldBossData.angle = pCheckPoint->GetAngle();
    m_WorldBossData.cantMove = true;
    m_WorldBossData.cantBuff = true;
    m_WorldBossData.bossui = true;
    m_WorldBossData.worldboss = true;
    m_WorldBossData.name = "worldboss_"+Crown::ToString(m_WorldBossData.creatureId);

    Boss* newBoss = AddBoss(m_WorldBossData);

    int32 hp = newBoss->GetProp(PROP_MAXHP);
    if(newBoss == NULL)
    {
        return;
    }

    newBoss->GetAIController()->LoadAI("ai/world_boss_skill.aix");
}
