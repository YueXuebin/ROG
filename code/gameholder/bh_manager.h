/*
 *	勇敢的心
 */
#ifndef BH_MANAGER_H
#define BH_MANAGER_H

class Player;

class BhManager
{
public:
    BhManager(Player* player);
    ~BhManager();
    void Clear();

    void LoadBhInfo(const BH_INFO& info);
    void FillBhInfo(BH_INFO& info);

    void OnRecv(uint32 msgID, void* data);

    void CalDailyRefresh();         // 每日刷新

    int32 GetEnterTimes();          // 获取次数

    void CalBhProp();               // 计算勇敢的心属性

    int32 DungeonEnd(bool bWin);    // 通关

private:
    void OnBhInfoReq();
    void OnBhSelectTypeReq(void* data);
    void OnBhStartReq(void* data);
    void OnBhRedeemReq(void* data);
    void OnBhRewardReq(void* data);
    void OnBhSelectTypeCB(int32 err, int32 type);

private:
    Player*             m_player;
    BH_INFO             m_bh_info;
};

#endif