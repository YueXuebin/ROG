/*
 *	副本相关的结构化数据
 */
#ifndef dungeon_data_h__
#define dungeon_data_h__

#include "scene.h"

class SceneTemplate
{
public:
    void Clear()
    {
        m_scene_id              = 0;
        m_regular_type          = 0;
        m_close                 = 0;
        m_map_file              = "";
        design_file             = "";
        trigger_list.clear();
    }

    uint32                  m_scene_id;
    int32                   m_regular_type;
    int32                   m_close;
    std::string             m_map_file;
    std::string             design_file;
    std::list<Teleport>     trigger_list;
};

class DungeonTemplate
{
public:
    void Clear()
    {
        m_dungeon_id        = 0;
        m_dungeon_type      = 0;
        m_scene_list.clear();
    }

    void GetRaidsMonsterList(std::map<uint32, int32>& monsterCount);        // 获得副本中的怪物列表

    uint32                      m_dungeon_id;       // 副本ID
    int32                       m_dungeon_type;     // 副本类型
    std::list<SceneTemplate>    m_scene_list;       // 场景列表
};

class DungeonInfo
{
public:
    void Clear()
    {
        m_gamesvr_id        = 0;
        m_gamesvr_type      = 0;
        m_dungeon_id        = 0;
        m_max_channel       = 0;
        m_max_player        = 0;
        m_dungeon_tmpl      = 0;
    }

    uint32                  m_gamesvr_id;       // 游戏服务器ID
    int32                   m_gamesvr_type;     // 游戏服务器类型
    uint32                  m_dungeon_id;       // 副本ID
    int32                   m_max_channel;      // 副本最大频道数
    uint32                  m_max_player;       // 最大玩家
    const DungeonTemplate*  m_dungeon_tmpl;     // 副本模板
};

// 根据副本ID，判断副本类型
uint32 GetDungeonClass(uint32 dungeonID);

// 判断可以飞的副本
bool IsDungeonCanFly(uint32 dungeonID);

b2World* MakeBox2DWorld(const std::string& mapFileName);

#endif // dungeon_data_h__
