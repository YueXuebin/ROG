#ifndef center_handler_helper_h__
#define center_handler_helper_h__

/*
 *	CenterServer游戏协议处理
 */
class CenterHandlerHelper
{
public:
    CenterHandlerHelper(CenterHandler* centerHandler);
    ~CenterHandlerHelper();

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata);
private:
    void RecvKickoutGame(uint64 playerID, void* data);                      // 收到踢出游戏的消息

    void RecvPlayerPropList(uint64 playerID, void* data);                   // 接收角色属性信息

    void RecvRefreshAOfflinePlayerAck(uint64 playerID, uint32 msgID, void* data);   // 刷新离线战斗角色列表
    void RecvOfflinePlayerRecordAck(uint32 msgID, void* data);                      // 获得离线玩家数据

    void RecvChangePlayerNameAck(uint64 playerID, uint32 msgID, void* data);        // 修改角色名称应答
    void RecvFriendInfoNtf(uint64 playerID, uint32 msgID, void* data);              // 好友信息
    void RecvFriendAddSCAck(uint64 playerID, uint32 msgID, void* data);             // 添加好友

    void RecvTeamMsg(uint64 playerID, uint32 msgID, void* data);                    // 接收队伍信息
    void RecvAnswerMsg(uint64 playerID, uint32 msgID, void* data);                  // 接收答题信息

    void RecvSaveLevelActivityAck(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);     // 接收存档等级活动应答
    void RecvSaveSpecialActivityAck(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);   // 接收存档特殊活动应答

    void RecvMailInfoNtf(uint64 playerID, uint32 msgID, void* data);                // 接收邮件信息
    void RecvMailDelExpiredMailNtf(uint64 playerID, uint32 msgID, void* data);      // 删除过期邮件

    void RecvGuildMsg(uint64 playerID, uint32 msgID, void* data);                   // 接收公会信息
    void RecvGuildScienceMsg(uint64 playerID, uint32 msgID, void* data);            // 接收公会科技信息

    void RecvWorldBossMsg(uint64 playerID, uint32 msgID, void* data);               // 接受世界BOSS信息
    
    void RecvActivityListNtf(uint64 playerID, uint32 msgID, void* data);            // 接收活动开始信息

    void RecvAdventureMsg(uint64 playerID, uint32 msgID, void* data);               // 获得运镖消息
    void RecvMallMsg(uint64 playerID, uint32 msgID, void* data);                    // 接收购买数据

    void RecvRankAck(uint64 playerID, uint32 msgID, void* data);                    // 获得排名应答
    void RecvActiveCodeMsg(uint64 playerID, uint32 msgID, void* data);              // 获得激活码消息

    void OnActiveCodeUseAck(uint64 playerID, void* data);                           // 使用激活码应答

    void OnSetRight(uint64 playerID, uint32 msgID, void* data);                     // 设置禁言标识

    void RecvSaveDayInvestmentAck(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);     // 接收存档日常投资应答
    void RecvSaveLevelInvestmentAck(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);   // 接收存档等级投资应答

    void RecvPlayerRechargeInfoNtf(uint64 playerID, uint32 msgID, void* data);      // 接收充值通知

private:
    CenterHandler*  m_CenterHandler;
};


#endif // center_handler_helper_h__
