#include "gameholder_pch.h"
#include "regular_faction_pk.h"
#include "regular_data.h"
#include "battle_unit.h"
#include "pet.h"
#include "player.h"
#include "scene.h"
#include "trigger.h"
#include "event_define.h"
#include "action_controller.h"
#include "faction_pk_config.h"
#include "faction_manager.h"

RegularFactionPK::RegularFactionPK(RegularDoc* pDoc) :
RegularMultiple(pDoc)
{
    
}

RegularFactionPK::~RegularFactionPK()
{

}

void RegularFactionPK::OnCreatureEnter(BattleUnit* creature)
{
    RegularMultiple::OnCreatureEnter(creature);
    SetGroup(creature);

    if(creature->GetObjectType() == OBJ_PLAYER)
    {
        Player* player = (Player*)creature;
        player->SendtoCenterLog(LOG_ACTIVITY, WriterToLogByActivity(player->GetLevel(), ACTIVITY_TYPE_FACTION_PK, ACTIVITY_INTO));
    }
}

void RegularFactionPK::OnCreatureLeave(BattleUnit* creatrue)
{
    RegularMultiple::OnCreatureLeave(creatrue);
    if(creatrue->GetObjectType() == OBJ_PLAYER)
    {
        Player* player = (Player*)creatrue;
        player->SendtoCenterLog(LOG_ACTIVITY, WriterToLogByActivity(player->GetLevel(), ACTIVITY_TYPE_FACTION_PK, ACTIVITY_LEAVE));

        FactionManager::Instance()->DelPlayer(player);
    }
}

void RegularFactionPK::OneSecondUpdate(int32 deltatime)
{
    RegularMultiple::OneSecondUpdate(deltatime);
}

void RegularFactionPK::FillStartPostion(Player* player, uint32 triggerID)
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

void RegularFactionPK::SetGroup(BattleUnit* creature)
{
    // 设置分组
    uint32 CreatureType = creature->GetObjectType();
    switch(CreatureType)
    {
    case OBJ_PLAYER:
        SetPlayerGroup((Player*)creature);
        break;
    case OBJ_OFFLINE_PLAYER:
        creature->SetGroup(FRIEND_GROUP);
        break;
    case OBJ_PET:
        {
            if(((Pet*)creature)->GetMaster())
                creature->SetGroup( ((Pet*)creature)->GetMaster()->GetGroup() );
        }
        break;
    case OBJ_MONSTER:
        creature->SetGroup(ENEMY_GROUP);
        break;
    case OBJ_BOSS:
        creature->SetGroup(ENEMY_GROUP);
        break;
    //case OBJ_NPC:
    //    creature->SetGroup(FRIEND_GROUP);
    //    break;
    }
}

void RegularFactionPK::SetPlayerGroup(Player* player)
{
    uint64 playerID = player->GetPlayerID();
    int32 playerGroupType = 0;

    int32 blueBattlePower = 0;
    blueBattlePower = CalGroupBattlePowerTotal(FACTION_PK_BLUE);
    int32 redBattlePower = 0;
    redBattlePower = CalGroupBattlePowerTotal(FACTION_PK_RED);

    if(blueBattlePower <= redBattlePower)
    {
        if(GetPlayerCount(FACTION_PK_BLUE) >= 5)
        {
            playerGroupType = FACTION_PK_RED;
        }
        else
        {
            playerGroupType = FACTION_PK_BLUE;
        }
    }
    else
    {
        if(GetPlayerCount(FACTION_PK_RED) >= 5)
        {
            playerGroupType = FACTION_PK_BLUE;
        }
        else
        {
            playerGroupType = FACTION_PK_RED;
        }
    }
    // 派别处理
    player->SetGroup(playerGroupType);
    FactionManager::Instance()->SetPlayerFaction(player, playerGroupType);

    // 设置玩家位置
    JumpToTrigger(player, playerGroupType);
}

bool RegularFactionPK::CanEnter()
{
    // 人数过多
    int32 playerNum = GetScene()->GetPlayerCount();
    CnAssert(playerNum <= 10 && playerNum >= 0);
    if(GetScene()->GetPlayerCount() >= 10)
    {
        return false;
    }

    // 关卡已经结束
    if(IsStop())
    {
        return false;
    }

    return true;
}

void RegularFactionPK::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularMultiple::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
    switch(eventId)
    {
    case BATTLE_EVENT_PLAYER_DEAD:
        RecieveDead(arg1, arg2);
        break;
    default:
        break;
    }
}

void RegularFactionPK::OnDungeonClear(bool success, std::vector<std::string>* arglist)
{

}

int32 RegularFactionPK::CalGroupBattlePowerTotal(uint8 groupType)
{
    Scene::PlayerMap& playerMap = GetScene()->GetPlayers();

    int32 battlePower = 0;
    for(Scene::PlayerMap::iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); ++playerIt)
    {
        if(!playerIt->second)
        {
            continue;
        }

        if(playerIt->second->GetGroup() == groupType)
        {
            battlePower += playerIt->second->GetBattlePower();
        }
    }

    return battlePower;
}


void RegularFactionPK::RecieveDead(int32 deadID, int32 attackID)
{
    // 死亡者必须为玩家
    BattleUnit* deader = GetScene()->GetBattleUnit(deadID);
    if(!deader)
    {
        return;
    }
    if(deader->GetObjectType() != OBJ_PLAYER)
    {
        return;
    }

    BattleUnit* attacker = GetScene()->GetBattleUnit(attackID);
    if(!attacker)
    {
        return;
    }

    //当攻击者是玩家时
    if(attacker->GetObjectType() == OBJ_PLAYER)
    {
        AddKillInfo((Player*)attacker, (Player*)deader);
    }
    // 当攻击者是战宠时
    else if(attacker->GetObjectType() == OBJ_PET)
    {
        BattleUnit* attackerMaster = ((Pet*)attacker)->GetMaster();
        if(!attackerMaster)
        {
            return;
        }

        if(attackerMaster->GetObjectType() != OBJ_PLAYER)
        {
            return;
        }

        AddKillInfo((Player*)attackerMaster, (Player*)deader);
    }
}

void RegularFactionPK::JumpToTrigger(Player* player, int32 triggerID)
{
    if(!player)
    {
        return;
    }
    // 设置玩家位置
    Trigger* trigger = GetScene()->GetTriggerByID(triggerID);
    if(!trigger)
    {
        CnError("trigger id:%d is not exist!\n", triggerID);
        return;
    }
    player->GetActionController()->JumpTo(trigger->GetX(), trigger->GetY());
}

int32 RegularFactionPK::GetPlayerCount(uint8 groupType)
{
    int32 playerNum = 0;
    Scene::PlayerMap& playerMap = GetScene()->GetPlayers();
    for(Scene::PlayerMap::iterator playerIt = playerMap.begin(); playerIt != playerMap.end(); ++playerIt)
    {
        if(playerIt->second->GetGroup() == groupType)
        {
            playerNum++;
        }
    }

    return playerNum;
}

void RegularFactionPK::AddKillInfo(Player* attacker, Player* deader)
{
    if(attacker == NULL || deader == NULL)
    {
        return;
    }

    SendBattleInfo(FACTION_PK_TYPE_KILL, attacker->GetName(), deader->GetName(), 0);
    FactionManager::Instance()->CalScore((Player*)attacker, (Player*)deader, this);
}

void RegularFactionPK::ComboKill(const std::string& attackerName, int32 combo)
{
    SendBattleInfo(FACTION_PK_TYPE_COMBO, attackerName, "", combo);
}

void RegularFactionPK::StopCombo(const std::string& attackerName, const std::string& deaderName, int32 combo)
{
    SendBattleInfo(FACTION_PK_TYPE_STOP_COMBO, attackerName, deaderName, combo);
}

void RegularFactionPK::SendBattleInfo(uint32 type, const std::string& attack_name, const std::string& deader_name, int32 combo)
{
    FACTION_PK_BATTLE_NTF sendMsg;
    sendMsg.type = type;
    sendMsg.attack_name = attack_name;
    sendMsg.deader_name = deader_name;
    sendMsg.combo = combo;
    GetScene()->NotifyMessageToAll(MSG_FACTION_PK_BATTLE_NTF, &sendMsg);
}



