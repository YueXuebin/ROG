#ifndef regular_dungeon_h__
#define regular_dungeon_h__

#include "regular_builder.h"


/*
 *	临时副本类型玩法的规则基类
 *  特点：
 *      副本时间状态,副本结算规则
 */
class RegularDungeon : public RegularBuilder
{
    CnDeclareRTTI
public:
    RegularDungeon(RegularDoc* pDoc);
    virtual ~RegularDungeon();

    virtual void    FirstUpdate();
    
    virtual void    Update(uint32 curr_tick);

    virtual void    OneSecondUpdate(int32 deltatime);

    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    virtual void    OnCreatureEnter(BattleUnit* creature);

    bool            IsStop() { return (GetState() >= kStateStop); }

    int32           GetGameTime();

    // 设置关卡的各状态时间(小心使用)
    void            SetAllStateTime(int32 readyTime, int32 runTime, int32 endTime, int32 curTime);

    int32           GetReadyTime() { return m_ReadyTime; }
    void            SetReadyTime(int32 val) { m_ReadyTime = val; }

    int32           GetRunTime() { return m_RunTime; }
    void            SetRunTime(int32 val) { m_RunTime = val; }

    int32           GetEndTime() { return m_EndTime; }
    void            SetEndTime(int32 val) { m_EndTime = val; }

    int32           GetCurrentTime() { return m_CurrentTime; }
    void            SetCurrentTime(int32 curtime);

    int32           GetState() { return m_state; }
    void            SetState(int32 val) { m_state = val; }
    
    void            SetGameStart(bool val) { m_GameStart = val; }
    virtual bool    IsGameStart() {return m_GameStart;}

    std::vector<uint32> GetstartNtfList() {return m_startNtfList;}
    void            AddToNtfList(uint32 mId) {m_startNtfList.push_back(mId);}

    enum
    {
        kStateReady     = 0,    // 准备状态
        kStateStart     = 1,    // 开始状态
        kStateStop      = 2,    // 结束状态
        kStateKick      = 3     // 踢人状态
    };

    void            DungeonClear(bool success, std::vector<std::string>* arglist = NULL);   // 副本通关
    virtual void    OnDungeonClear(bool success, std::vector<std::string>* arglist);        // 副本通关处理(只由DungeonRuled调用)

    virtual void    SceneClear(bool success, std::vector<std::string>* arglist = NULL);     // 场景通关

    void SetLifeCount(int32 value) { m_LifeCount = value; }
    int32 GetLifeCount() { return m_LifeCount; }
    void AddLifeCount(int32 count);
    void SubLifeCount(int32 count);

    void            SetConditionChanged() { m_EventChanged = true; }
    void            NotifyDungeonCondition(Player* player);
    void            NotifyDungeonConditionToAll();

protected:

    virtual void    OnSceneClear(bool success, std::vector<std::string>* arglist = NULL);   // 场景完成处理
    

    virtual void ParseDesign(TiXmlElement* designElement);

    virtual void ParseRegular(TiXmlElement* RegularElement);

    void            SetStartState();
    void            SetStopState();
    void            SetKickState();

    void            LockAllPlayer();
    void            UnlockAllPlayer();
    void            KickAllPlayer();

    void            StartAI();
    void            StopAI();

    void            UpdateNtfList();

    void            NotifyAllPlayers(uint32 msgID, void* data);     // 通知副本中所有玩家

    void            AddSucceedEvent(RegularEvent* evt);             // 胜利条件
    void            AddFailedEvent(RegularEvent* evt);              // 失败条件
    void            AddScoreEvent(RegularEvent* evt);               // 评价条件

    int32           CalcDungeonScore(Player* player, bool success);               // 计算玩家的副本评价

private:
    void            StateUpdate();

    void            FillDungeonCountdown(DUNGEON_READY_COUNTDOWN& sendMsg);
    void            NotifyDungeonCountdown(Player* player);

    void            OnDungeonAddLifeCount(int32 playerID, uint32 lifeCount);

protected:
    bool                m_GameStart;        // 游戏是否开始
    
    /*
     *  单位(秒)
     *           isGameStart
     *                |
     *	状态:         V------ready ------------------ start ------------------- end ------------------- kick
     *  时间：                 |      ready time        |    run time(0为无限)   |   end time(0为无限)    |
     *  事件                                      (start event)              (ent event)             (kick event)
     *  绝对时间               <--m_CurrentTime                                                         
     */
    int32               m_state;            // 游戏状态

    int32               m_ReadyTime;        // 游戏准备时间(秒)
    int32               m_RunTime;          // 游戏时间(0, 表示无限)(秒)
    int32               m_EndTime;          // 游戏结束时间(秒)

    uint32              m_LastTime;         // 上次帧的时间
    int32               m_CurrentTime;      // 关卡从ready开始的时间(绝对时间)(毫秒)
    bool                m_IsCustomTime;     // 是否手动设置关卡时间

    int32               m_LifeCount;        // 副本还有几命

    std::vector<uint32> m_startNtfList;     // 开始通知列表<objectID>

    // 胜利条件
    RegularEventList    m_SucceedEventList;
    // 失败条件
    RegularEventList    m_FailedEventList;
    // 评价条件
    RegularEventList    m_ScoreEventList;

    bool                m_EventChanged;     // 胜利失败条件有更新
    
};


#endif // regular_dungeon_h__
