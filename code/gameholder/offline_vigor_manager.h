#ifndef offline_vigor_manager_h_
#define offline_vigor_manager_h_

class Player;

class OfflineVigorManager
{
public:
    OfflineVigorManager(Player* player);
    ~OfflineVigorManager(void);

    void OnRecv(uint32 msgID, void* data);

    void FillInfo(OFF_LINE_VIGOR_INFO& info);
    void LoadInfo(const OFF_LINE_VIGOR_INFO& info);

    void CalOffLineVigorTime();                                                 //计算下线累计时间

    void CalDailyRefresh();                                                     //离线体力每天只能领一次

    void BuyOffLineVigor(int32 point);                                          // 花费魔石购买的离线体力

    OFF_LINE_VIGOR_INFO& GetInfo() {return m_off_line_vigor_info;}

private:
    void Init(void* data);

    void GetOffLineVigor(void* data);

    void SendOfflineVigorAck();

    void CaloffVigorByTime();                                                   //根据累计时间计算体力点

    Player*                     m_player;
    OFF_LINE_VIGOR_INFO         m_off_line_vigor_info;
};
#endif // offline_vigor_manager_h
