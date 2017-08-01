// 通天塔系统

#ifndef BABEL_MANAGER_H
#define BABEL_MANAGER_H

class Player;
/*
class BabelManager
{
public:
    BabelManager(Player* player);
    ~BabelManager();

    void Clear();
    void OnRecv(uint32 msgID, void* data);

    void LoadBabelInfo(const BABEL_INFO& info);
    void FillBabelInfo(BABEL_INFO& info);
    BABEL_INFO&    GetBabelInfo() {return m_babel_info;}
    

    void BabelStart(uint32 tier, uint32 stage);         // 开始挑战
    void BabelRaids();                                  // 一键扫荡
    void BabelReset();
    bool CardByBabelReset(int32 num);                   // 至尊魔宫重置

    void OpenNextBabel(int mDungeonId);                 // 开启下一层

    int32 GetMaxTier();                                 // 得到最大层数

    bool CanStart(uint32 tier, uint32 stage);           // 检查是否可以开启

    void CalDailyRefresh();                             // 日常刷新
    void SaveYesterdayData();                           // 保存昨天数据

    void CBResetBabelTrade();                           // 重置至尊魔宫交易回调
    void CBRaidsBabelTrade();                           // 扫荡至尊魔宫交易回调

    void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, const std::string& arg3);

private:
    void OnBabelInfoReq();
    void OnBabelStartReq(void* data);
    void OnBabelResetReq(void* data);
    void OnBabelRaidsReq(void* data);

    // 检查至尊魔宫可挑战魔宫数量
    void CheckBabelNotify();

private:
    Player*             m_player;
    BABEL_INFO          m_babel_info;
};
*/

#endif