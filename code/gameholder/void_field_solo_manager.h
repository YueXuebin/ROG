#ifndef void_field_solo_manager_h__
#define void_field_solo_manager_h__

// 虚空幻境管理器

class Player;

class VoidFieldSoloManager
{
public:
    VoidFieldSoloManager();
    ~VoidFieldSoloManager();

    DECLARE_SINGLETON(VoidFieldSoloManager)

    void    OnRecv(uint32 msgID, void* data);

    void    Update();                               // 心跳

    void    OnVoidFieldChannelInfoReq(void* data);

    void    OnPlayerChannelInfoReq(Player* player, void* data);

    void    OnSetHookPosReq(Player* player, void* data);        // 虚空幻境中设置挂机点

    uint16  CanEnterVoidField(Player* player);

    bool    IsInBoosScene(Player* player);                     ///< 玩家是否处于珍兽Boss场景中

    uint32  GetOpenTime(int32 mType) {return m_OpenTimeList[mType];}
    void    SetOpenTime(int32 mType);
    bool    IsLastInChannelList(int32 mType, uint32 mChannelId);
    void    AddToChannelList(int32 mType, uint32 mChannelId);
    void    RemoveFromChannelList(int32 mType, uint32 mChannelId);
    void    ClearChannelList(int32 mType);
    bool    CheckLastInChannelListState(int32 mType);    // 检查type表的最后一个频道当前是不是type状态（如：珍兽表的最后一个频道是不是珍兽模式）
    bool    CheckInChannelList(int32 mType, uint32 mChannelId); // 检查是否在当前列表里
    uint32  GetChannelListNum(int32 mType);

private:
    void    SendVoidFieldChannelInfoAck(uint16 errcode, uint32 gate_id, uint64 playerID);

    int32   CalWorldLevel();
    int32   GetWorldLevel() {return m_world_level;};
    void    SetWorldLevel(int32 level) {m_world_level = level;};

    int32   m_world_level;                                      ///< 世界等级（等级榜前20为的平均等级）

    std::vector<uint32>     m_OpenTimeList;                     ///< 各类型开始时间列表
    std::vector<uint32>     m_BossChannelList;
};

#endif // void_field_solo_manager_h__
