/*
 *	活动管理
 */
#ifndef activity_manager_h__
#define activity_manager_h__

class Player;

class ActivityManager
{
public:
    ActivityManager();
    ~ActivityManager();

    DECLARE_SINGLETON(ActivityManager)

    void Clear();

    void OnPlayerRecv(Player* player, uint32 msgID, void* data);
    void OnCSRecv(uint32 msgID, void* data);

    void OnServerReady();

    // 向TS请求活动列表
    void ReqTSActivityList();
    // 添加活动
    void AddActivity(ACTIVITY_INFO& activity_info);
    void AddActivity(uint32 activity_id, uint32 activity_type, uint32 activity_state, uint32 start_time, uint32 end_time);
    // 删除活动
    void DelActivity(ACTIVITY_INFO& activity_info);
    void DelActivity(uint32 activity_id);
    // 更新活动(设置某活动开启/关闭)
    void UpdateActivity(ACTIVITY_INFO& activity_info);
    // 关闭活动
    void CloseActivity(uint32 activity_id);
    void CloseActivityByType(uint32 activityType);
    // 开启活动
    void OpenActivity(uint32 activity_id);

    bool IsActivityTypeOpen(uint32 activityType);   // 只用于简单活动

    ACTIVITY_INFO*  GetActivityByType(int32 type, bool checkState=false);
    ACTIVITY_INFO*  GetActivityByID(uint32 id);

private:
    void OnUpdateActivityList(const ACTIVITY_LIST* list);
    void _UpdateActivity(const ACTIVITY_INFO& newinfo, ACTIVITY_INFO* info);
    void _AddActivity(const ACTIVITY_INFO& newinfo);

    ACTIVITY_LIST   m_activitList;
};

#endif // activity_manager_h__