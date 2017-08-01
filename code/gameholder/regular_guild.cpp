#include "gameholder_pch.h"
#include "regular_guild.h"
#include "regular_data.h"
#include "battle_unit.h"
#include "pet.h"
#include "player.h"
#include "scene.h"
#include "trigger.h"
#include "event_define.h"
#include "action_controller.h"
#include "checkpoint.h"
#include "channel_manager.h"
#include "dungeon.h"
#include "dungeon_set.h"


CnImplementRTTI(RegularGuild, RegularDungeon);
// ==================================================
// 构造函数
// ==================================================
RegularGuild::RegularGuild(RegularDoc* pDoc) :
RegularDungeon(pDoc)
{
    m_bornBeginID       = 0;
    m_bornIdNum         = 0;
    m_reliveBeginID     = 0;
    m_reliveIdNum       = 0;
}

// ==================================================
// 析构函数
// ==================================================
RegularGuild::~RegularGuild()
{
    
}

// ==================================================
// 战斗单位进入
// ==================================================
void RegularGuild::OnCreatureEnter(BattleUnit* creature)
{
    RegularDungeon::OnCreatureEnter(creature);

    if(creature->GetObjectType() == OBJ_PLAYER)
    {
        
    }
    else if(creature->GetObjectType() == OBJ_PET)
    {
        Player* player = (Player*)((Pet*)creature)->GetMaster();
        if(player)
        {
            creature->SetGroup(player->GetGroup());
        }
    }
}

// ==================================================
// 战斗单位退出
// ==================================================
void RegularGuild::OnCreatureLeave(BattleUnit* creature)
{
    RegularDungeon::OnCreatureLeave(creature);

    if(creature->GetObjectType() == OBJ_PLAYER)
    {
        ChannelManager* channelManager = m_pScene->GetDungeon()->GetDungeonSet()->GetChannelManager();
        Player* player = (Player*)creature;
        channelManager->PlayerLeave(player->GetPlayerID());

        DelTriggerCounter(((Player*)creature)->GetPlayerID());
    }
}

// ==================================================
// 第一次更新
// ==================================================
void RegularGuild::FirstUpdate()
{
    RegularDungeon::FirstUpdate();
}

// ==================================================
// 每秒刷新
// ==================================================
void RegularGuild::OneSecondUpdate(int32 deltatime)
{
    RegularDungeon::OneSecondUpdate(deltatime);
}

// ==================================================
// 接收事件
// ==================================================
void RegularGuild::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularDungeon::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
}

// ==================================================
// 清除副本
// ==================================================
void RegularGuild::OnDungeonClear(bool success, std::vector<std::string>* arglist)
{
    RegularDungeon::OnDungeonClear(success, arglist);
}

bool RegularGuild::IsGameStart()
{
    if(RegularDungeon::IsGameStart())
        return true;

    // 有任何一个玩家开始游戏，即可以开始
    if(IsAnyoneStart())
    {
        SetGameStart(true);
    }

    return RegularDungeon::IsGameStart();
}

// ==================================================
// 填充开始位置
// ==================================================
void RegularGuild::FillStartPostion(Player* player, uint32 triggerID)
{
    Trigger* trigger = 0;

    if(!player) return;

    triggerID = GetTriggerID(player->GetGuildID());

    trigger = m_pScene->GetTriggerByID(triggerID);
    if(!trigger)
    {
        CnAssert(false);
        return;
    }

    player->SetGroup(triggerID);

    CoVec2 randPos = GetScene()->FindValidPositionInCircle(trigger->GetPosition(), trigger->GetRadius(), player->GetRadius()); // trigger范围内查找一个有用的随机点
    player->SetPosition((int32)randPos.x, (int32)randPos.y);

    player->SetAngle(trigger->GetAngle());

    AddTriggerCounter(player->GetPlayerID(), player->GetGuildID(), triggerID);
}

// ==================================================
// 设置角色重生时的 Pin 点
// ==================================================
void RegularGuild::SetRelivePosition(Player* player)
{
    uint32 triggerID = 0;
    Trigger* trigger = 0;

    if(!player) return;

    // 随机后4个生成点
    if(m_reliveBeginID)
    {
        triggerID = Crown::SDRandom(m_reliveBeginID);
    }

    triggerID += m_reliveIdNum;

    trigger = m_pScene->GetTriggerByID(triggerID);
    if(!trigger)
    {
        CnAssert(false);
        return;
    }

    player->GetActionController()->JumpTo(trigger->GetX(), trigger->GetY());
    player->GetActionController()->AngleTo(trigger->GetAngle());
}

// ==================================================
// 设置开始 Pin 点
// ==================================================
void RegularGuild::SetBornTrigger(uint32 beginID, uint32 num)
{
    m_bornBeginID = beginID;
    m_bornIdNum = num;
}

// ==================================================
// 设置重生点个数
// ==================================================
void RegularGuild::SetReliveTrigger(uint32 beginID, uint32 num)
{
    m_reliveBeginID = beginID;
    m_reliveIdNum   = num;
}

// ==================================================
// 根据公会ID获得 Pin 点
// ==================================================
uint32 RegularGuild::GetTriggerID(uint64 guildID)
{
    for(GuildPosVer::iterator it = m_trigers.begin(); it != m_trigers.end(); ++it)
    {
        if(it->guild_id == guildID)
        {
            return it->trigger_id;
        }
    }

    for(uint32 i=m_bornBeginID; i<=m_bornIdNum; ++i)
    {
        if(IsValidTriggerID(i))
        {
            return i;
        }
    }

    // 分配一个默认的PIN点
    return m_bornBeginID;
}

bool RegularGuild::IsValidTriggerID(uint32 pinID)
{
    for(GuildPosVer::iterator it = m_trigers.begin(); it != m_trigers.end(); ++it)
    {
        if(it->trigger_id == pinID)
        {
            return false;
        }
    }

    return true;
}

void RegularGuild::AddTriggerCounter(uint64 playerID, uint64 guildID, uint32 triggerID)
{
    GuildPos guildPos;

    for(GuildPosVer::iterator it = m_trigers.begin(); it != m_trigers.end(); ++it)
    {
        if(it->trigger_id == triggerID)
        {
            it->player_list.push_back(playerID);
            return;
        }
    }

    guildPos.Clear();
    guildPos.guild_id = guildID;
    guildPos.trigger_id = triggerID;
    guildPos.player_list.push_back(playerID);
    m_trigers.push_back(guildPos);
}

void RegularGuild::DelTriggerCounter(uint64 playerID)
{
    // 在所有 pin 点中查找角色（因为角色可能会在战斗中途被退会）
    for(GuildPosVer::iterator pinIt = m_trigers.begin(); pinIt != m_trigers.end(); ++pinIt)
    {
        // 检查每个pin点中按角色ID查找，并删除为 0 的点
        for(std::vector<uint64>::iterator playerIt = pinIt->player_list.begin(); 
            playerIt != pinIt->player_list.end(); ++playerIt)
        {
            if(*playerIt != playerID) continue;

            pinIt->player_list.erase(playerIt);

            if(pinIt->player_list.size() < 1)
            {
                m_trigers.erase(pinIt);
            }
            return;
        }
    }
}
