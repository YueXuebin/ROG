#include "gameholder_pch.h"
#include "regular_dungeon.h"
#include "scene.h"
#include "player.h"
#include "event_define.h"
#include "regular_event.h"
#include "rog_regular_event.h"
#include "regular_event_start_time.h"
#include "regular_event_end_time.h"
#include "dungeon.h"
#include "player.h"
#include "item_manager.h"

CnImplementRTTI(RegularDungeon, RegularBuilder);

RegularDungeon::RegularDungeon( RegularDoc* pDoc ) :
RegularBuilder(pDoc)
{
    m_state = kStateReady;

    m_GameStart = false;

    m_ReadyTime = 0;
    m_RunTime = 0;
    m_EndTime = 0;
    m_LastTime = 0;
    m_CurrentTime = 0;
    m_IsCustomTime = false;

    m_LifeCount = 0;

    m_SucceedEventList.clear();
    m_FailedEventList.clear();
    m_ScoreEventList.clear();
    
    m_EventChanged = false;

}

RegularDungeon::~RegularDungeon()
{
    for(RegularEventList::iterator suIter = m_SucceedEventList.begin(); suIter != m_SucceedEventList.end(); suIter++)
    {
        RegularEvent* pSucceedEvent = *suIter;
        CnDelete pSucceedEvent;
    }
    m_SucceedEventList.clear();

    for(RegularEventList::iterator faIter = m_FailedEventList.begin(); faIter != m_FailedEventList.end(); faIter++)
    {
        RegularEvent* pFailEvent = *faIter;
        CnDelete pFailEvent;
    }
    m_FailedEventList.clear();

    for(RegularEventList::iterator scIter = m_ScoreEventList.begin(); scIter != m_ScoreEventList.end(); scIter++)
    {
        RegularEvent* pScoreEvent = *scIter;
        CnDelete pScoreEvent;
    }
    m_ScoreEventList.clear();
}

void RegularDungeon::ParseRegular(TiXmlElement* RegularElement)
{
    RegularBuilder::ParseRegular(RegularElement);

    // 解析胜利条件
    CnAssert(m_SucceedEventList.size() == 0);
    TiXmlNode* succeedNode = RegularElement->FirstChild("succeed");
    if(succeedNode)
    {
        TiXmlElement* eventElement = succeedNode->FirstChildElement();
        for(; eventElement; eventElement = eventElement->NextSiblingElement())
        {
            std::string nodeValue = eventElement->Value();
            if (nodeValue != "event")
                continue;

            RegularEvent* pSucceedEvent = ParseRegularEvent(eventElement, this);
            if(pSucceedEvent)
                m_SucceedEventList.push_back(pSucceedEvent);
        }
    }

    // 解析失败条件
    CnAssert(m_FailedEventList.size() == 0);
    TiXmlNode* failedNode = RegularElement->FirstChild("fail");
    if(failedNode)
    {
        TiXmlElement* eventElement = failedNode->FirstChildElement();
        for(; eventElement; eventElement = eventElement->NextSiblingElement())
        {
            std::string nodeValue = eventElement->Value();
            if(nodeValue != "event")
                continue;

            RegularEvent* pFailEvent = ParseRegularEvent(eventElement, this);
            if(pFailEvent)
                m_FailedEventList.push_back(pFailEvent);
        }
    }
}
void RegularDungeon::ParseDesign(TiXmlElement* designElement)
{
    RegularBuilder::ParseDesign(designElement);

    // 关卡状态时间
    if(designElement && !m_IsCustomTime)
    {
        m_ReadyTime = 0;
        designElement->Attribute("ready_time", &m_ReadyTime);
        m_RunTime = 0;
        designElement->Attribute("start_time", &m_RunTime);
        m_EndTime = 0;
        designElement->Attribute("end_time", &m_EndTime);
    }

}

void RegularDungeon::AddSucceedEvent(RegularEvent* evt)
{
    m_SucceedEventList.push_back(evt);
}

void RegularDungeon::AddFailedEvent(RegularEvent* evt)
{
    m_FailedEventList.push_back(evt);
}

void RegularDungeon::AddScoreEvent(RegularEvent* evt)
{
    m_ScoreEventList.push_back(evt);
}

void RegularDungeon::FirstUpdate()
{
    RegularBuilder::FirstUpdate();

    StopAI();
}

void RegularDungeon::OneSecondUpdate( int32 deltatime )
{
    RegularBuilder::OneSecondUpdate(deltatime);

    for(RegularEventList::iterator suIter = m_SucceedEventList.begin(); suIter != m_SucceedEventList.end(); suIter++)
    {
        RegularEvent* pSucceedEvent = *suIter;
        pSucceedEvent->OneSecondUpdate(deltatime);
    }
    for(RegularEventList::iterator faIter = m_FailedEventList.begin(); faIter != m_FailedEventList.end(); faIter++)
    {
        RegularEvent* pFailEvent = *faIter;
        pFailEvent->OneSecondUpdate(deltatime);
    }
    for(RegularEventList::iterator scIter = m_ScoreEventList.begin(); scIter != m_ScoreEventList.end(); scIter++)
    {
        RegularEvent* pScoreEvent = *scIter;
        pScoreEvent->OneSecondUpdate(deltatime);
    }

    if (m_EventChanged)
    {
        m_EventChanged = false;
        NotifyDungeonConditionToAll();
    }

    if(!IsStop())
    {
        // 检查胜利条件
        bool succeed = true;
        if(m_SucceedEventList.size() == 0)
            succeed = false;        // 无胜利条件则，不胜利

        for(RegularEventList::iterator suIter = m_SucceedEventList.begin(); suIter != m_SucceedEventList.end(); suIter++)
        {
            RegularEvent* pSucceedEvent = *suIter;
            if(!pSucceedEvent->GetActive())
                succeed = false;
        }

        if(succeed)
        {
            SceneClear(true);        // 场景关卡胜利
        }
    }

    if(!IsStop())
    {
        // 检查失败条件
        bool fail = false;
        for(RegularEventList::iterator faIter = m_FailedEventList.begin(); faIter != m_FailedEventList.end(); faIter++)
        {
            RegularEvent* pFailEvent = *faIter;
            if(pFailEvent->GetActive())
                fail = true;
        }
        if(fail)
        {
            SceneClear(false);       // 场景关卡失败
        }
    }

}
// 计算玩家的通关评价
int32 RegularDungeon::CalcDungeonScore(Player* player, bool success)
{
    if(!success)
        return REGULAR_SCORE_F;

    int32 score = 0;
    for(RegularEventList::iterator scIter = m_ScoreEventList.begin(); scIter != m_ScoreEventList.end(); scIter++)
    {
        RegularEvent* pScoreEvent = *scIter;
        if(pScoreEvent->GetActiveForPlayer(player))
            score++;
    }

    if(score == 2)
        return REGULAR_SCORE_A;
    else if(score == 1)
        return REGULAR_SCORE_B;
    else if(score == 0)
        return REGULAR_SCORE_C;
    else
    {
        CnAssert(false);
        return REGULAR_SCORE_F;
    }
}

// 副本条件通知
void RegularDungeon::NotifyDungeonConditionToAll()
{
    const Scene::PlayerMap& players = m_pScene->GetPlayers();
    for(Scene::PlayerMap::const_iterator playerIt = players.begin(); playerIt != players.end(); playerIt++)
    {
        Player* player = playerIt->second;
        NotifyDungeonCondition(player);
    }
}

void RegularDungeon::NotifyDungeonCondition(Player* player)
{
    CnAssert(player);

    DUNGEON_CONDITION_NTF sendMsg;

    // 胜利条件
    for(RegularEventList::iterator suIter = m_SucceedEventList.begin(); suIter != m_SucceedEventList.end(); suIter++)
    {
        RegularEvent* pEvent = *suIter;
        if(pEvent->IsKindOf(&ROGRegularEvent::ms_RTTI))
        {
            ROGRegularEvent* pSucceedEvent = (ROGRegularEvent*)pEvent;
            if (pSucceedEvent)
            {
                sendMsg.success.push_back(pSucceedEvent->GetCondition(player));
            }
        }
    }

    // 失败条件
    for(RegularEventList::iterator suIter = m_FailedEventList.begin(); suIter != m_FailedEventList.end(); suIter++)
    {
        RegularEvent* pEvent = *suIter;
        if(pEvent->IsKindOf(&ROGRegularEvent::ms_RTTI))
        {
            ROGRegularEvent* pFailEvent = (ROGRegularEvent*)pEvent;
            if (pFailEvent)
            {
                sendMsg.failure.push_back(pFailEvent->GetCondition(player));
            }
        }
    }
    
    // 评价条件
    uint32 scoreNum = 0;
    for(RegularEventList::iterator scIter = m_ScoreEventList.begin(); scIter != m_ScoreEventList.end(); scIter++)
    {
        RegularEvent* pEvent = *scIter;
        if(pEvent->IsKindOf(&ROGRegularEvent::ms_RTTI))
        {
            ROGRegularEvent* pScoreEvent = (ROGRegularEvent*)pEvent;
            if(!pScoreEvent)
                continue;

            if(scoreNum == 0)
                sendMsg.score1.push_back(pScoreEvent->GetCondition(player));
            else if(scoreNum == 1)
                sendMsg.score2.push_back(pScoreEvent->GetCondition(player));

            scoreNum++;
        }
    }

    player->SendToGate(MSG_DUNGEON_CONDITION_NTF, &sendMsg);
}

void RegularDungeon::Update(uint32 curr_tick)
{
    RegularBuilder::Update(curr_tick);

    if(IsGameStart())
    {
        if(m_LastTime == 0)
        {
            m_LastTime = curr_tick;
        }
        else
        {
            if(curr_tick >= m_LastTime)
            {
                m_CurrentTime += (int32)(curr_tick - m_LastTime);
                m_LastTime = curr_tick;
            }
            else
                CnAssert(false);
        }
    }

    StateUpdate();

    UpdateNtfList();
}

void RegularDungeon::StateUpdate()
{
    if(kStateReady == GetState())
    {   // 检查规则是否已开启
        if(m_CurrentTime >= (m_ReadyTime*1000))
        {
            //CnInfo("SetStartState() \n");
            SetStartState();
        }
    }
    if(kStateStart == GetState())
    {   // 开启状态中如果设有停止时间，则检查是否已到停止时间
        if((m_RunTime > 0) && (m_CurrentTime >= ((m_ReadyTime+m_RunTime)*1000)))
        {
            //CnInfo("SetStopState() \n");
            SceneClear(false);
            SetStopState();
        }
    }
    if(kStateStop == GetState())
    {   // 结束状态如果设有强制踢人时间，则踢人
        if((m_EndTime > 0) && (m_CurrentTime >= (m_ReadyTime+m_RunTime+m_EndTime)*1000))
        {
            //CnInfo("SetKickState() \n");
            SetKickState();
        }
    }
    if(kStateKick == GetState())
    {
        KickAllPlayer();
    }
}


void RegularDungeon::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularBuilder::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
    case EVENT_DUNGEON_ADD_LIFE_COUNT_REQ:
        {
            OnDungeonAddLifeCount((uint32)arg1, (uint32)arg2);
        }
        break;
    default:
        break;
    }

    // 胜利条件
    for(RegularEventList::iterator suIter = m_SucceedEventList.begin(); suIter != m_SucceedEventList.end(); suIter++)
    {
        RegularEvent* pSucceedEvent = *suIter;
        pSucceedEvent->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
    }
    // 失败条件
    for(RegularEventList::iterator faIter = m_FailedEventList.begin(); faIter != m_FailedEventList.end(); faIter++)
    {
        RegularEvent* pFailEvent = *faIter;
        pFailEvent->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
    }
    // 评价条件
    for(RegularEventList::iterator scIter = m_ScoreEventList.begin(); scIter != m_ScoreEventList.end(); scIter++)
    {
        RegularEvent* pScoreEvent = *scIter;
        pScoreEvent->ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);
    }
}

void RegularDungeon::UpdateNtfList()
{
    BattleUnit* battleUnit = 0;
    Player* player = 0;

    if(m_startNtfList.size() < 1) return;

    for(std::vector<uint32>::iterator it = m_startNtfList.begin(); it !=  m_startNtfList.end(); )
    {
        battleUnit = m_pScene->GetBattleUnit(*it);
        if(battleUnit && battleUnit->GetObjectType() == OBJ_PLAYER)
        {
            player = (Player*)battleUnit;
            if(player->GetGameStart())
            {
                NotifyDungeonCountdown((Player*)battleUnit);
                it = m_startNtfList.erase(it);
            }
            else
            {
                ++it;
            }
        }
        else
        {
            it = m_startNtfList.erase(it);
        }
    }
}

void RegularDungeon::NotifyAllPlayers(uint32 msgID, void* data)
{
    Scene* pScene = GetScene();

    const Scene::PlayerMap& players = pScene->GetPlayers();
    for(Scene::PlayerMap::const_iterator playerIt = players.begin(); playerIt != players.end(); playerIt++)
    {
        Player* other = playerIt->second;
        other->SendToGate(msgID, data);
    }
}

void RegularDungeon::OnCreatureEnter(BattleUnit* creature)
{
    RegularBuilder::OnCreatureEnter(creature);

    Player* player = 0;

    if(!creature) return;

    if(OBJ_PLAYER == creature->GetObjectType())
    {
        player = (Player*)creature;

        // 角色进入场景任务触发
        player->SendTaskEvent(TASK_EVENT_CHANGE_DUNGEON, m_pScene->GetDungeonID(), 0);

        if( (GetState() <= kStateReady) || (kStateStop <= GetState() && m_pScene->GetCanClose()) )
        {   // ready状态 或 可结算场景的结束状态,锁定玩家
            player->SetGameLock(false);
        }
        else
        {
            player->SetGameLock(false);
        }

        m_startNtfList.push_back(creature->GetId());
    }
}

int32 RegularDungeon::GetGameTime() 
{
    int32 GameTime = m_CurrentTime - m_ReadyTime*1000;
    if(GameTime < 0)
        GameTime = 0;
    return GameTime;
}

void RegularDungeon::SetCurrentTime(int32 time)
{
    m_CurrentTime = time;
    StateUpdate();
}

void RegularDungeon::SetAllStateTime(int32 readyTime, int32 runTime, int32 endTime, int32 curTime)
{
    m_IsCustomTime = true;
    m_ReadyTime = readyTime;
    m_RunTime = runTime;
    m_EndTime = endTime;
    m_LastTime = 0;
    SetCurrentTime(curTime);
}

void RegularDungeon::SetStartState()
{
    if(GetState() >= kStateStart)
        return;

    //CnInfo("kStateStart\n");

    SetState(kStateStart);

    UnlockAllPlayer();
    StartAI();

    // 触发游戏开始事件
    RegularEvent* regularEvent = GetRegularEventByType(REGULAR_EVENT_START_TIME);
    if(regularEvent)
        regularEvent->SetActive(true);

    OnStart();
}

void RegularDungeon::SetStopState()
{
    if(GetState() >= kStateStop)
        return;

    //CnInfo("kStateStop\n");

    SetState(kStateStop);

    StopAI();   // 副本完成，停止AI

    // 可结算副本才锁定玩家
    //if(m_pScene->GetCanClose())
    //{
    //    LockAllPlayer();
    //}

    // 触发游戏结束事件
    RegularEvent* regularEvent = GetRegularEventByType(REGULAR_EVENT_END_TIME);
    if(regularEvent)
        regularEvent->SetActive(true);
}

void RegularDungeon::SetKickState()
{
    if(GetState() >= kStateKick)
        return;

    //CnInfo("kStateKick\n");

    SetState(kStateKick);

    KickAllPlayer();

    // 触发踢人事件
    RegularEvent* regularEvent = GetRegularEventByType(REGULAR_EVENT_KICK_TIME);
    if(regularEvent)
        regularEvent->SetActive(true);
}

void RegularDungeon::LockAllPlayer()
{
    if(!m_pScene) return;

    //CnInfo("LockAllPlayer\n");

    // 通知players
    for (Scene::PlayerMap::iterator it= m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
    {
        if(it->second)
            it->second->SetGameLock(true);
    }
}

void RegularDungeon::UnlockAllPlayer()
{
    if(!m_pScene) return;

    //CnInfo("UnlockAllPlayer\n");

    // 通知players
    for (Scene::PlayerMap::iterator it = m_pScene->GetPlayers().begin(); it != m_pScene->GetPlayers().end(); ++it)
    {
        if(it->second)
            it->second->SetGameLock(false);
    }
}

void RegularDungeon::KickAllPlayer()
{
    CHANGE_DUNGEON_REQ sendMsg;

    if(!m_pScene) return;

    // 踢掉副本中的所有玩家（这里只是强制玩家发了离开副本消息，在一定时间内玩家可能还与副本挂钩）
    std::map<uint32, Player*>& players = m_pScene->GetPlayers();
    for(std::map<uint32, Player*>::iterator playerIt = players.begin(); playerIt != players.end(); ++playerIt)
    {
        playerIt->second->OnRecv(MSG_CHANGE_DUNGEON_REQ, &sendMsg);
    }
}

void RegularDungeon::StartAI()
{
    if(!m_pScene)
        return;
    m_pScene->SetCanUseAI(true);
}

void RegularDungeon::StopAI()
{
    if(!m_pScene)
        return;
    m_pScene->SetCanUseAI(false);
}
// 场景通关
void RegularDungeon::SceneClear(bool success, std::vector<std::string>* arglist)
{
    if(IsStop())
        return;

    SetStopState();

    OnSceneClear(success, arglist);
}
// 场景通关处理
void RegularDungeon::OnSceneClear(bool success, std::vector<std::string>* arglist)
{
    // 场景通关成功
    if(success)
    {
        // 如果是可结算场景，则触发副本通关
        if(m_pScene->GetCanClose() || m_pScene->GetDungeon()->GetInvalide())        // 如果是非法副本也触发通关
        {
            DungeonClear(success, arglist);
        }
    }
    // 场景通关失败
    else
    {   // 一个场景通关失败，副本就通关失败
        DungeonClear(success, arglist);
    }

}
// 副本通关
void RegularDungeon::DungeonClear(bool success, std::vector<std::string>* arglist)
{
    if(success)
        CnAssert(m_pScene->GetCanClose() || m_pScene->GetDungeon()->GetInvalide());      // 可结算副本才触发胜利通关
    GetScene()->GetDungeon()->DungeonClear(success, arglist);      // 副本通关
}
// 副本通关处理
void RegularDungeon::OnDungeonClear(bool success, std::vector<std::string>* arglist)
{

}

void RegularDungeon::FillDungeonCountdown(DUNGEON_READY_COUNTDOWN& sendMsg)
{
    int32 current_time = m_CurrentTime / 1000;
    int32 ready_time = m_ReadyTime - current_time;
    int32 run_time = 0;
    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    if(ready_time < 0)
    {
        ready_time = 0;
    }

    if(m_ReadyTime > current_time)
    {
        run_time = m_RunTime;
    }
    else
    {
        run_time = m_ReadyTime + m_RunTime - current_time;
        if(run_time < 0)
        {
            run_time = 0;
        }
    }

    sendMsg.ready_time = now + ready_time;
    sendMsg.run_time = now + run_time;
    sendMsg.current_time = current_time;
}
// 通知副本计时
void RegularDungeon::NotifyDungeonCountdown(Player* player)
{
    DUNGEON_READY_COUNTDOWN sendMsg;
    RegularEvent_StartTime* startEvent = 0;
    RegularEvent_EndTime*   endEvent = 0;

    if(!player) return;

    FillDungeonCountdown(sendMsg);

    player->SendToGate(MSG_DUNGEON_READY_COUNTDOWN_NTF, &sendMsg);
}


void RegularDungeon::OnDungeonAddLifeCount(int32 playerID, uint32 lifeCount)
{
    if (lifeCount <= 0)
    {
        CnWarn("invalid lifeCount in regular_dungeon");
        return;
    }

    Player* player = 0;
    player = GetScene()->GetPlayerByObjectId(playerID);
    if(!player)
        return;

    ItemBase* pItemBase = player->GetItemManager()->GetPlaceItemById(PLACE_BAG, 13301);
    if (!pItemBase || pItemBase->GetNum() < (int32)lifeCount)
    {
        CnWarn("Add life count consume item not enough"); // 药水不足
        return;
    }

    player->GetItemManager()->RemoveItemNum(REASON_ITEM_SUB_ADD_LIFE, lifeCount, pItemBase->GetPlace(), pItemBase->GetPos());

    AddLifeCount(lifeCount);
}

void RegularDungeon::AddLifeCount(int32 count)
{
    m_LifeCount += count;   

    NotifyDungeonConditionToAll();
}

void RegularDungeon::SubLifeCount(int32 count)
{
    m_LifeCount -= count;

    CnAssert(m_LifeCount >= 0);

    NotifyDungeonConditionToAll();
}
