/*
 *	仇恨系统管理器(仅Boss)
 */
#ifndef hatred_manager_h__
#define hatred_manager_h__

class Creature;

// 仇恨信息
struct HatredInfo
{
    void Clear()
    {
        object_id       = 0;
        center_id       = 0;
        player_id       = 0;
        player_name     = "";
        guild_id        = 0;
        damage          = 0;
        battle_power    = 0;
        level           = 0;
    }
    uint32      object_id;      // 对象ID
    uint32      center_id;      // 玩家的CenterID
    uint64      player_id;      // 玩家ID
    uint64      guild_id;       // 公会ID
    int32       damage;         // 伤害值
    int32       battle_power;   // 玩家战斗力
    int32       level;          // 玩家等级
    std::string player_name;    // 玩家名
};

class HatredManager
{
public:
    HatredManager(Creature* creature);
    virtual ~HatredManager();

    void            OnRecv(uint32 msgID, void* data);                               // 消息接收

    void            Update(uint32 frame_count);

    void            OnDamage(const DamageResult& damage, BattleUnit* pAttacker);

    void            AddHatredByPlayer(Player* Attacter, int32 damage);              // 增加某玩家的仇恨记录
    void            AddHatredByCreature(Creature* pCreature, int32 damage);         // 增加某生物的仇恨记录

    void            AddHatredValue(uint32 objectID, int32 damage);                  // 增加仇恨
    void            SubHatredValue(uint32 objectID, int32 damage);                  // 减少仇恨
    void            ClearHatred(uint32 objectID);                                   // 清除仇恨

    HatredInfo*     GetHatredInfo(uint32 objectID);                                 // 获取仇恨信息

    std::vector<HatredInfo*>&       GetSortHatred();                                // 排序仇恨
    std::map<uint32, HatredInfo>&   GetHatredMap(){return m_HatredMap;}

    void            ClearAllHatreList();                                            // 清除仇恨列表
    void            ClearHatredByPlayreID(uint64 playerID);                         // 根据玩家id清出仇恨

private:
    void        AddHatred(uint32 objectID, const HatredInfo& info);                 // 添加仇恨记录

    void        UpdateHatredList();             // 仇恨重排
    void        ReductionHatred();              // 仇恨衰减

    static bool SortHatred(HatredInfo* a, HatredInfo* b);                           // 按由大到小排序

private:
    Creature*       m_Creature;

    std::map<uint32, HatredInfo>        m_HatredMap;        // 仇恨记录表 <objectID, HatredInfo>
    std::vector<HatredInfo*>            m_HatredList;       // 仇恨排名列表

    uint32      m_LastUpdateFrame;
    bool        m_HateDirty;                        // 需要重新排序
};

#endif  // hatred_manager_h__
