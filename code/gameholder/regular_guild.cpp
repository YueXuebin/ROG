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
// ���캯��
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
// ��������
// ==================================================
RegularGuild::~RegularGuild()
{
    
}

// ==================================================
// ս����λ����
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
// ս����λ�˳�
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
// ��һ�θ���
// ==================================================
void RegularGuild::FirstUpdate()
{
    RegularDungeon::FirstUpdate();
}

// ==================================================
// ÿ��ˢ��
// ==================================================
void RegularGuild::OneSecondUpdate(int32 deltatime)
{
    RegularDungeon::OneSecondUpdate(deltatime);
}

// ==================================================
// �����¼�
// ==================================================
void RegularGuild::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularDungeon::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
}

// ==================================================
// �������
// ==================================================
void RegularGuild::OnDungeonClear(bool success, std::vector<std::string>* arglist)
{
    RegularDungeon::OnDungeonClear(success, arglist);
}

bool RegularGuild::IsGameStart()
{
    if(RegularDungeon::IsGameStart())
        return true;

    // ���κ�һ����ҿ�ʼ��Ϸ�������Կ�ʼ
    if(IsAnyoneStart())
    {
        SetGameStart(true);
    }

    return RegularDungeon::IsGameStart();
}

// ==================================================
// ��俪ʼλ��
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

    CoVec2 randPos = GetScene()->FindValidPositionInCircle(trigger->GetPosition(), trigger->GetRadius(), player->GetRadius()); // trigger��Χ�ڲ���һ�����õ������
    player->SetPosition((int32)randPos.x, (int32)randPos.y);

    player->SetAngle(trigger->GetAngle());

    AddTriggerCounter(player->GetPlayerID(), player->GetGuildID(), triggerID);
}

// ==================================================
// ���ý�ɫ����ʱ�� Pin ��
// ==================================================
void RegularGuild::SetRelivePosition(Player* player)
{
    uint32 triggerID = 0;
    Trigger* trigger = 0;

    if(!player) return;

    // �����4�����ɵ�
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
// ���ÿ�ʼ Pin ��
// ==================================================
void RegularGuild::SetBornTrigger(uint32 beginID, uint32 num)
{
    m_bornBeginID = beginID;
    m_bornIdNum = num;
}

// ==================================================
// �������������
// ==================================================
void RegularGuild::SetReliveTrigger(uint32 beginID, uint32 num)
{
    m_reliveBeginID = beginID;
    m_reliveIdNum   = num;
}

// ==================================================
// ���ݹ���ID��� Pin ��
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

    // ����һ��Ĭ�ϵ�PIN��
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
    // ������ pin ���в��ҽ�ɫ����Ϊ��ɫ���ܻ���ս����;���˻ᣩ
    for(GuildPosVer::iterator pinIt = m_trigers.begin(); pinIt != m_trigers.end(); ++pinIt)
    {
        // ���ÿ��pin���а���ɫID���ң���ɾ��Ϊ 0 �ĵ�
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
