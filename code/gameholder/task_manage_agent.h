#ifndef task_trig_agent_h__
#define task_trig_agent_h__

class RoleTask;
class RoleTaskTarget;
class TaskManager;


/*
 *	任务触发管理器
 */
class TaskManageAgent
{
public:
    TaskManageAgent(TaskManager* pTaskManager);
    ~TaskManageAgent();

    void Clear();

    void SendTaskEvent(uint32 event_id, int32 arg1, int32 arg2, int32 arg3);                                // 发送任务事件
    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3); // 接收任务事件


    // 触发任务
    void OnKillMonster(uint32 monsterID, int32 num, uint32 mapID, bool isRadis=false);      // 杀死怪物触发任务
    void OnSettleDungeon(uint32 dungeonID, bool success, bool raids);                       // 通关副本触发任务
    void OnTaskMicroClient();                                                               // 微端登陆触发任务
    void CheckRideQualityState();                                                           // 获得坐骑触发任务
    void OnExchangeItem(uint32 itemID);                                                     // 兑换物品触发任务
    //

private:
    void OnTriggerTarget(uint32 targetType, uint32 arg1 = 0, uint32 arg2 = 0);    // 触发任务功能

private:
    TaskManager*    m_TaskManager;

};


#endif // task_trig_agent_h__
