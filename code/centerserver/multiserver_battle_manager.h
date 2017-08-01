/*
 *	跨服组队战斗管理
 */
#ifndef multiserver_battle_manager_h__
#define multiserver_battle_manager_h__


class Player;

class MultiserverBattleManager
{
public:
    MultiserverBattleManager(void);
    ~MultiserverBattleManager(void);

    DECLARE_SINGLETON(MultiserverBattleManager)

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

    void onExitGame(Player* pPlayer);

private:
    void RecvMultiserverBattleApplyReq(void* data);             // 跨服战斗报名申请
    void RecvMultiserverBattleQuitReq(void* data);              // 跨服战斗退出申请
    void RecvMultiserverBattleInfoAboutRank(uint64 playerID);   // 得到关于排名的跨服信息

    void RecvWaitAck(void* data);
};

#endif