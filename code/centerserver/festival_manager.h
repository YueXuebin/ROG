/*
 *   活动管理器
 */
#ifndef festival_manager_h__
#define festival_manager_h__

class FestivalManager
{
public:
    FestivalManager();
    ~FestivalManager();

    DECLARE_SINGLETON(FestivalManager)

    void SetFestivalList(const TASK_FESTIVAL_TEMPLATE_LIST& list);              // 设置活动数据

    const TASK_FESTIVAL_TEMPLATE* GetFestivalTemplate(uint32 id);               // 获取活动模板

    void SendToAllGateSvrFestivalList();                                        // 通知所有Gate活动信息

    void SendToAllGameSvrFestivalList();                                        // 通知所有Game活动信息

    void SendFestivalListToGameSvr(uint32 sessionID);                           // 通知游戏服务器所有活动信息

private:

    TASK_FESTIVAL_TEMPLATE_LIST     m_festivalList;                             // 活动列表
};

#endif // festival_manager_h__
