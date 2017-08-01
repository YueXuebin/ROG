#ifndef world_map_manager_h__
#define world_map_manager_h__

// 野外Boss管理器

class Player;
class RegularFiledBoss;

class WorldMapManager
{
public:
    WorldMapManager();
    ~WorldMapManager();

    DECLARE_SINGLETON(WorldMapManager)

    void    OnRecv(uint32 msgID, void* data);
    void    OnPlayerRecv(Player* player, uint32 msgID, void* data);

    void    Update(uint32 curr_tick);                           ///< 心跳

    uint16  CanEnterFieldBoss(Player* player);

    void OnSetHookPosReq(Player* player, void* data);                                      //设置挂机点
private:
    //=====================================manager管理消息=====================================================
    void    OnFieldBossInfoReq(void* data);                                                 ///< 野外Boss详细信息请求
    void    SendFieldBossInfoAck(uint16 errcode, uint32 gate_id, uint64 playerID, uint32 dungeon_id);          ///< 野外Boss详细信息响应

    //=====================================规则管理消息=====================================================
    void OnRegularRecv(Player* player, uint32 msgID, void* data);           // 收到规则相关消息
    RegularFiledBoss* GetPlayerRegular(Player* player);                      // 获取玩家当前场景规则

    //=====================================Boss信息=====================================================


    int32   CalWorldLevel();
    int32   GetWorldLevel() {return m_world_level;};
    void    SetWorldLevel(int32 level) {m_world_level = level;};

    void    PlayerIsOnLine(void* data);                        ///玩家是否下线

    int32   m_world_level;                                      ///< 世界等级（等级榜前20为的平均等级）
};

#endif // world_map_manager_h__
