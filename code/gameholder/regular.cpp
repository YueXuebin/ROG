#include "gameholder_pch.h"
#include "regular.h"
#include "regular_event.h"
#include "regular_manager.h"
#include "scene.h"
#include "dungeon.h"
#include "trigger.h"
#include "ai_controller.h"
#include "event_define.h"
#include "offline_player.h"
#include "path_data.h"
#include "game_define.h"
#include "player.h"
#include "offline_player_manager.h"
#include "skill_object.h"
#include "battle_unit_connecter.h"
#include "state_define.h"
#include "action_controller.h"
#include "regular_event_start_time.h"
#include "regular_event_end_time.h"
#include "spawn.h"
#include "regular_event_kick_time.h"
#include "game_util.h"
#include "performance_analysis.h"
#include "resurgence_helper.h"
#include "monster.h"
#include "monster_config_manager.h"
#include "ai_group_register.h"
#include "property_set_calculater.h"
#include "soldier.h"
#include "buff_controller.h"
#include "team_manager.h"

CnImplementRTTI(Regular, RegularBase);

Regular::Regular(RegularDoc* pDoc) :
RegularBase(pDoc)
{
    m_doFirstUpdate = false;

    m_monsterSum = 0;
    m_bossSum = 0;
    m_npcSum = 0;
    m_dropObjectSum = 0;

    m_SecondTimer = 0;
    m_TickTime = 0;


    m_canUseHp = true;
    m_canUseMp = true;
    m_canUseDeffence = true;
    m_canUseAttack = true;
    m_canUseSpeed = true;

    m_Level = 1;            // 地图等级,默认1级

    m_AIGroupRegister = CnNew AIGroupRegister(this);
}

Regular::~Regular()
{
    CnDelete m_AIGroupRegister;

    for(RegularEventList::iterator iter = m_LogicEvent.begin(); iter != m_LogicEvent.end(); iter++)
    {
        RegularEvent* pLogicEvent = *iter;
        CnDelete pLogicEvent;
    }
    m_LogicEvent.clear();
}

void Regular::OnCreatureEnter(BattleUnit* creature)
{
}

void Regular::OnCreatureLeave(BattleUnit* creature)
{
}

void Regular::FillStartPostion(Player* player, uint32 triggerID)
{
    // 默认规则，玩家出现在Trigger 1位置处
    Trigger* pFirstTrigger = GetScene()->GetFirstTrigger();
    if(pFirstTrigger)
    {
        player->SetPosition(pFirstTrigger->GetX(), pFirstTrigger->GetY());
        player->SetAngle( pFirstTrigger->GetAngle() );
    }
    else
    {
        CnWarn("scene %d no trigger\n", GetScene()->GetSceneID() );
        player->SetPosition(0, 0);
    }
}

uint32 Regular::GetReliveTriggerID(uint32 groupID)
{
    Trigger* trigger = GetScene()->GetTriggerByID(groupID);
    if(trigger)
    {
        return trigger->GetTriggerId();
    }
    return 0;
}

void Regular::FirstUpdate()
{
    // 创建关卡中的对象
    CreateRegular();
}

RegularEvent* Regular::GetRegularEventByType(int32 type)
{
    // 关卡事件
    for(RegularEventList::iterator it = m_LogicEvent.begin(); it != m_LogicEvent.end(); it++)
    {
        if((*it)->GetType() == type)
        {
            return *it;
        }
    }

    return 0;
}

// 创建关卡中的对象
void Regular::CreateRegular()
{
    // 创建关卡中所有对象
    CreateSceneObjects();

    // 创建关卡规则和逻辑
    TiXmlElement* rootElement = m_pRegularDoc->m_pDoc->FirstChildElement()->ToElement();
    
    // 所有node
    TiXmlNode* designNode = m_pRegularDoc->m_pDoc->FirstChild("design_ex");
    CnAssert(designNode);

    TiXmlElement* designElement = designNode->ToElement();
    CnAssert(designElement);

    ParseDesign(designElement);     // 解析关卡数据

    if(!designNode->FirstChild())
        return;

    bool checkOneRegular = true;
    bool checkOneLogic = true;

    TiXmlElement* nodeElement = designNode->FirstChildElement()->ToElement();
    for(; nodeElement; nodeElement = nodeElement->NextSiblingElement())
    {
        std::string nodeValue = nodeElement->Value();
        if (nodeValue == "regular")         // 规则
        {
            CnAssert(checkOneRegular);
            checkOneRegular = false;

            ParseRegular(nodeElement);      // 子类解析关卡规则
        }
        else if (nodeValue == "logic")      // 逻辑
        {
            CnAssert(checkOneLogic);
            checkOneLogic = false;
            // 解析关卡逻辑
            ParseRegularLogic(nodeElement);
        }
    }
}

void Regular::ParseRegularLogic(TiXmlNode* logicNode)
{
    TiXmlElement* eventElement = logicNode->FirstChildElement();
    for(; eventElement; eventElement = eventElement->NextSiblingElement())
    {
        std::string nodeValue = eventElement->Value();
        if (nodeValue != "event")
            continue;

        RegularEvent* pLogicEvent = ParseRegularEvent(eventElement, this);
        if(pLogicEvent)
            m_LogicEvent.push_back(pLogicEvent);
    }
}

void Regular::Update(uint32 curr_tick)
{
    if(!m_doFirstUpdate)
    {
        m_doFirstUpdate = true;
        FirstUpdate();
    }

    m_TickTime += g_ServerUpdateTime;

    // 每秒更新时间
    if(m_SecondTimer < 1000)
    {
        m_SecondTimer += g_ServerUpdateTime;
    }
    else
    {
        m_SecondTimer -= 1000;
        OneSecondUpdate(1000);
    }

    // AI群Update
    m_AIGroupRegister->Update();
}

void Regular::OneSecondUpdate(int32 deltatime)
{
    // 遍历事件
    for(RegularEventList::iterator loIter = m_LogicEvent.begin(); loIter != m_LogicEvent.end(); loIter++)
    {
        RegularEvent* pLogicEvent = *loIter;
        pLogicEvent->OneSecondUpdate(deltatime);
    }
}

bool Regular::IsEnemy(BattleUnit* attacker, BattleUnit* defencer)
{
    return attacker->GetGroup() != defencer->GetGroup();
}

bool Regular::IsAnyoneStart(int32 num)
{
    int32 count = 0;
    const Scene::PlayerMap& Players = m_pScene->GetPlayers();
    for(Scene::PlayerMap::const_iterator iter = Players.begin(); iter != Players.end(); iter++)
    {
        Player* pPlayer = iter->second;
        if(pPlayer->GetGameStart())
        {
            ++count;
            if(count >= num)
            {
                return true;
            }
        }
    }
    return false;
}

void Regular::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    RegularBase::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    // 关卡事件
    for(RegularEventList::iterator loIter = m_LogicEvent.begin(); loIter != m_LogicEvent.end(); loIter++)
    {
        RegularEvent* pLogicEvent = *loIter;
        pLogicEvent->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
    }
}

void Regular::Damage( const DamageResult& damage, BattleUnit* Attacker, BattleUnit* Defender, SkillObject* pSkill )
{
    Defender->Damage(damage, Attacker, pSkill);   // 受到伤害
}

void Regular::OnResurgence(BattleUnit* pBattleUnit)
{
    // 给个5秒无敌buff
    //pBattleUnit->AddBuff(6034, 5000, 0, pBattleUnit);
}

void Regular::OnDead(BattleUnit* Attacker, BattleUnit* Defender)
{
    if(Attacker)
    {
        if(Attacker->IsKindOf(&Soldier::ms_RTTI))
        {
            Soldier* pSoldier = CnDynamicCast(Soldier, Attacker);
            if (pSoldier->GetMaster())
            {
                Attacker = pSoldier->GetMaster();
            }
        }

        if(Attacker->IsKindOf(&Player::ms_RTTI))
        {
            Player* player = CnDynamicCast(Player, Attacker);
            if(Defender->IsKindOf(&Monster::ms_RTTI))
            {
                // 杀怪经验
                Monster* monster = CnDynamicCast(Monster, Defender);

                KillBonus(player, monster);
            }
        }
    }
}

void Regular::Resurgence(BattleUnit* battleUnit, uint32 triggerID, int32 delayTime)
{
    if(!battleUnit)
    {
        return;
    }

    if(!battleUnit->IsDead())
    {
        CnError("battle unit is not dead!\n");
        return;
    }

    battleUnit->GetResurgenceHelper()->SetResurgenceFullHp();       // 规则性复活都满血

    m_pScene->SendEvent(battleUnit->GetId(), BATTLE_EVENT_RESURGENCE, triggerID, 0, 0, delayTime);
}

void Regular::StopAllSpawm()
{
    Scene::SpawnMap spawnMap = GetScene()->GetSpawn();
    for(Scene::SpawnMap::iterator spawnIt = spawnMap.begin(); spawnIt != spawnMap.end(); ++spawnIt)
    {
        if(spawnIt->second)
        {
            spawnIt->second->SetClearAllMosters();
        }
    }
}

void Regular::KillBonus(Player* pAttacker, Monster* pDefender)
{
    if (!pAttacker || !pDefender)
    {
        CnAssert(false);
        return;
    }

    const MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT* pMonsterStruct = MonsterConfigManager::Instance()->GetMonsterAttributeReviseById(pDefender->GetCreatureId());
    if (!pMonsterStruct)
    {
        CnAssert(false);
        return;
    }
    const MONSTER_ATTRIBUTE_NORM_ROOT_TEMPLATE_STRUCT* pNormStruct = MonsterConfigManager::Instance()->GetMonsterAttributeNormByLevel(pDefender->GetLevel());
    if (!pNormStruct)
    {
        CnAssert(false);
        return;
    }

    // 金币奖励
    int32 nGold = pNormStruct->gold * pMonsterStruct->gold_rate / BASE_RATE_NUM + pMonsterStruct->gold;
    if (nGold > 0)
    {
        pAttacker->AddGamePoint(nGold, 0);

        OBJECT_DROP_GOLD_NTF ntf;
        ntf.drop_object_id = pDefender->GetId();
        ntf.gold = nGold;
        pAttacker->SendToGate(MSG_OBJECT_DROP_GOLD_NTF, &ntf);
    }

    // 经验奖励
    int32 nExp = pNormStruct->exp * pMonsterStruct->exp_rate / BASE_RATE_NUM + pMonsterStruct->exp;

    // 没有组队
    if (!pAttacker->GetTeamID())
    {
        pAttacker->AddKillExp(nExp, pDefender->GetLevel());
    }
    else // 组队经验分享
    {
        if (!TeamManager::Instance()->TeamExpShare(pAttacker->GetTeamID(), nExp, pDefender))
        {
            pAttacker->AddKillExp(nExp, pDefender->GetLevel());
        }
    }
}

void Regular::SendDungeonEvent(Player* player, int32 event_id)
{
    DUNGEON_EVENT_NTF sendMsg;

    if(!player) return;

    sendMsg.event_id = event_id;
    sendMsg.player_name = player->GetName();

    for(Scene::PlayerMap::iterator it = m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
    {
        if(it->second == player) continue;
        it->second->SendToGate(MSG_DUNGEON_EVENT_NTF, &sendMsg);
    }
}

void Regular::CalcMapProp(BattleUnit* battleUnit)
{
    PropertySet& mapPropertySet = battleUnit->GetPropertySetCalculater()->GetMapProp();
    mapPropertySet.Clear();

    if(battleUnit->IsKindOf(&Player::ms_RTTI))
    {
        // 地图等级决定玩家元素抗性基础值
        mapPropertySet.Add(PROP_FIRE_DEFENCE, -m_Level);
        mapPropertySet.Add(PROP_ICE_DEFENCE, -m_Level);
        mapPropertySet.Add(PROP_THUNDER_DEFENCE, -m_Level);
    }
    if(battleUnit->IsKindOf(&Soldier::ms_RTTI))
    {
        // 地图等级决定玩家元素抗性基础值
        mapPropertySet.Add(PROP_FIRE_DEFENCE, -m_Level);
        mapPropertySet.Add(PROP_ICE_DEFENCE, -m_Level);
        mapPropertySet.Add(PROP_THUNDER_DEFENCE, -m_Level);
    }
    else if(battleUnit->IsKindOf(&Monster::ms_RTTI))
    {

    }
}

void Regular::FillSceneInfo(SCENE_INFO_ACK& sceneInfo)
{
    sceneInfo.Clear();

    sceneInfo.channel_id = m_pScene->GetChannelID();
    sceneInfo.map_level = m_Level;
    sceneInfo.player_num = m_pScene->GetDungeon()->GetPlayerNum();

    for(uint32 i=0; i<4; i++)
    {
        sceneInfo.modifier_list.push_back(i);
    }
}