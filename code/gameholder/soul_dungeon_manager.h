#ifndef soul_dungeon_manager_h_
#define soul_dungeon_manager_h_

class Player;
struct DungeonGainTemplate;


/*
 *	异界地图副本管理
 */
class SoulDungeonManager
{
public:
    SoulDungeonManager(Player* player);
    ~SoulDungeonManager(void);

    void Clear();

    void Refresh();

    SOUL_DUNGEON_INFO& GetSoulDungeonInfo() { return m_soul_dungeon_info; }

    void FillSoulDungeonInfo(SOUL_DUNGEON_INFO& info) const;
    void LoadSoulDungeonInfo(const SOUL_DUNGEON_INFO& info);

    void OnRecv(uint32 msgID, void * data);

    // 请求护魂副本信息
    void OnRaidsSoulDungeonInfoReq();

    // 请求护魂副本开始
    void OnRaidsSoulDungeonStartReq(void* data);

    // 请求护魂副本结束
    void OnRaidsSoulDungeonCmplReq(void* data);

    // 请求护魂副本停止
    void OnRaidsSoulDungeonStopReq();

    // 护魂副本买CD请求
    void OnRaidsSoulDungeonBuyCDReq();

    // 护魂副本购买次数请求
    void OnRaidsSoulDungeonBuyTimesReq(void* data);

    // 添加护魂副本的次数
    void AddProtectSoulNum(const std::string& txt);

    // 请求扫荡完成秒CD
    void OnProtectSoulSubTime(uint32 cd_time);

    // 获得要扫荡的副本的副本次数
    int32   GetRaidsDungeonTimes(uint32 dungeonID);

    // 扫荡次数减1
    void    DecRaidsDungeonTimes(uint32 dungeonID);

    // 获得扫荡副本ID
    uint32  GetRaidsDungeonID();

    // 发送护魂副本信息
    void    SendSoulDungeonTimesList();

    bool CardBySoulDungeonTime(int32 dungeonID, int32 num);

private:
    bool    IsValidRaidsID(uint32 id);                  // 检查是否为有效的ID

    void    ClearRaidsDungeonList();                    // 清除副本扫荡列表

    int32   GetRaidsDungeonTimes();                     // 获得扫荡副本的剩余次数
    void    SetRaidsDungeonTimes(int32 val);            // 设置扫荡副本的剩余次数

    int32   GetRaidsDungeonTotalTimes();                // 获得扫房副本总次数
    void    SetRaidsDungeonTotalTimes(int32 val);       // 设置扫房副本总次数

    uint32  GetRaidsDungeonEndTime();                   // 获得扫荡副本的结束时间
    void    SetRaidsDungeonEndTime(uint32 val);         // 设置扫荡副本的结束时间

    int32   GetDungeonPurTimes(uint32 dungeon_id);              // 副本已购买次数
    void    AddDungeonPurTimes(uint32 dungeon_id, int times);   // 副本购买次数增加

    void    AddProtectSoulTimes(uint32 dungeon_id, int32 times);                            // 添加护魂副本次数

    void    SendRaidsSoulDungeonStartAck(uint16 errCode, int32 raids_times,                 // 发送扫荡副本应答
                        int32 total_raids_times, uint32 left_time, uint32 total_time);

    void    SendRaidsSoulDungeonCmplAck(uint16 errCode, int32 raids_times, 
                        int32 total_raids_times, uint32 left_time,  uint32 total_time);     // 发送扫荡副本完成

    void    SendRaidsSoulDungeonStopAck();                 // 发送扫荡副本停止

private:
    Player*             m_player;
    SOUL_DUNGEON_INFO   m_soul_dungeon_info;
};

#endif
