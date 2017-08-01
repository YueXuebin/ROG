/*
    点石成金管理器
 */
#ifndef stone_turn_gold_manager_h__
#define stone_turn_gold_manager_h__

class Player;

class StoneTurnGoldManager
{
public:
    StoneTurnGoldManager(Player* player);
    ~StoneTurnGoldManager();

    void Clear();

    void OnRecv(uint32 msgID, void* data);

    void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2);   // 接收事件

    void UpdateStoneTurnGoldTimes();

    void OnStoneTurnGold(uint32 type);

private:
    void OnStoneTurnGoldReq(void* data);

    int32 GetStoneHowNum();
    Player* m_player;
};

#endif
// stone_turn_gold_manager_h__
