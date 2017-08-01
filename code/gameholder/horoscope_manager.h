#ifndef horoscope_manager_h__
#define horoscope_manager_h__

class Player;
struct ConstellationTemplate;


class HoroscopeManager
{
public:
    HoroscopeManager(Player* player);
    virtual ~HoroscopeManager();

    void Clear();

    void OnRecv(uint32 msgID, void* data);

    void LoadInfo(const HOROSCOPE_INFO& info);
    void FillInfo(HOROSCOPE_INFO& info);

    void CalProp();

    uint32 GetHoroscopeLevel() { return m_horoscope_level; }

    void OnHoroscopeInfoReq();
    void OnHoroscopeLevelUpReq(void* data);

private:
    bool CheckLevel();
    void SendMarQuee();
    void SendHoroscopeInfonAck();
    void SendHoroscopeLevelUpAck(uint16 errcode, int32 horoscope_level, uint32 time);
    void SendHoroscopeLevelUpCmplAck(uint16 errcode, uint8 id, uint8 level, uint32 time);
    // 减去升星物品
    bool SubLeveupObj(const ConstellationTemplate* consTmpl);

    bool Levelup();                                 // 升星
    bool OneLevelup();                              // 一键升星

private:
    Player*     m_player;                           // 角色指针

    int32       m_horoscope_level;                  // 星空图等级
    uint32      m_levelup_id;                       // 正在升级的星空图ID(没有升级则ID为0)
    uint32      m_levelup_time;                     // 正在升级的星空图的结束时间戳
};

#endif
