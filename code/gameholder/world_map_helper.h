/*
 *	野外Boss辅助数据结构
 */
#ifndef world_map_helper_h__
#define world_map_helper_h__

#include "regular_data.h"

// 野外Boss中每个玩家的记录
struct WorldMapPlayerRecord
{
public:
    void Clear()
    {
        player_id     = 0;
        add_exp_time_point  = 0;
        personal_info.Clear();
    }

    uint64                          player_id;        ///< 角色ID
    uint32                          add_exp_time_point;     ///< 下次增加exp时间
    VOID_FIELD_PERSONAL_INFO_NTF    personal_info;    ///< 个人信息通知
};

enum WorldMapBossState
{
    kBOSS_SATE_ALIVE,       //Boss活着
    kBOSS_SATE_DEAD,        //Boss死亡，鞭尸
    kBOSS_SATE_DESTROY,     //Boss销毁，尸体消失
    kBOSS_SATE_NONE,        //Boss从未创建
};

struct WorldMapBossData
{
    void Clear()
    {
        bossID = 0;
        killed = false;
        killerID = 0;
        allDamage = 0;
        bossCreatureID = 0;
        //createTime = 0;
        createTime = 0;
        killer_name = "";
        state = kBOSS_SATE_NONE;
    }

    uint32              bossID;           ///< Boss的ID
    BossData            bossData;         ///< Boss数据
    bool                killed;           ///< 已经被杀
    uint32              killerID;         ///< Boss的击杀者ID
    int32               allDamage;        ///< 对Boss造成的伤害总和
    uint32              bossCreatureID;   ///< BOSS 的实例ID
   // uint32              createTime;       ///< 创建boss时间
    uint32              createTime;         ///< 创建boss时间
    std::string         killer_name;      ///< Boss击杀者
    WorldMapBossState   state;            ///< boss状态（活着，死亡，销毁）
};

#endif // world_map_helper_h__