//VIP独享副本
#ifndef __VIP_ONLY_DUNGEON_MANAGER_H__
#define __VIP_ONLY_DUNGEON_MANAGER_H__

class Player;

class VIPOnlyDungeonManager
{
public:
    VIPOnlyDungeonManager(Player *player);

    ~VIPOnlyDungeonManager();

    void ChangeDungeon(bool isFree);

    void LoadVIPOnlyDungeonInfo(VIP_ONLY_DUNGEON_INFO &info);

    void FillVIPOnlyDungeonInfo(VIP_ONLY_DUNGEON_INFO &info);

    void CalDailyRefresh();

    void CalGamePlayTimes();

    void OnRecv(uint32 msgID, void *data);

    void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2);

private:
    void OnMsgVIPOnlyDungeonEnterReq(uint32 msgID, void *data);

    void OnVIPOnlyDungeonEnd(bool isWin);

    void PushVIPOnlyDungeonInfo(uint16 err);

private:
    Player *m_player;
    VIP_ONLY_DUNGEON_INFO  m_vipOnlyDungeonInfo;
};

#endif