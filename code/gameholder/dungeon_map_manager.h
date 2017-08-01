#ifndef dungeon_map_manager_h__
#define dungeon_map_manager_h__

class Player;

/*
 * trigger 配置数据
 */
class MapTriggerTemplate
{
public:
    void Clear()
    {
        m_sourceTriggerID = 0;
        m_targetDungeonID = 0;
        m_targetSceneID = 0;
        m_targetTriggerID = 0;
    }

    uint32  m_sourceTriggerID;
    uint32  m_targetDungeonID;
    uint32  m_targetSceneID;
    uint32  m_targetTriggerID;
};

/*
 * scene 配置数据
 */
class MapSceneTemplate
{
public:
    void Clear()
    {
        m_scene_id              = 0;
        trigger_list.clear();
    }

    uint32                  m_scene_id;
    std::list<MapTriggerTemplate>     trigger_list;    // 跳转触发器列表
};

/*
 * dungeon 配置数据
 */
class MapDungeonTemplate
{
public:
    void Clear()
    {
        m_dungeon_id        = 0;
        m_scene_list.clear();
    }

    uint32                      m_dungeon_id;       ///< 副本ID
    std::list<MapSceneTemplate>    m_scene_list;       ///< 场景列表
};

/*
 *	 大地图的副本链接数据
 */
class DungeonMapManager
{
public:
    DungeonMapManager();
    ~DungeonMapManager();

    DECLARE_SINGLETON(DungeonMapManager)

    bool Init();

    // 通过trigger获取跳转地图
    bool AskTriggerJumpUnionID(Player* player, uint32 triggerID, uint32& targetUnionID, uint32& targetTriggerID);

private:
    bool LoadDungeonXML();
    bool LoadDungeon(TiXmlElement* xmlEle);
    bool LoadScene(TiXmlElement* xmlEle, MapDungeonTemplate& dungeonTmpl);
    bool LoadTrigger(TiXmlElement* xmlEle, std::list<MapTriggerTemplate>& triggerList);

    const MapTriggerTemplate* GetTrigger(uint32 dungeonID, uint32 sceneID, uint32 triggerID);

    typedef std::map<uint32, MapDungeonTemplate> MapDungeonTemplateMap;   ///< 副本配置[副本id，副本模板]
    MapDungeonTemplateMap m_dungeonTemplateMap;                           ///< 所有副本配置信息

    TiXmlDocument*      m_XmlDoc;                                           ///< 副本配置文件数据
};


#endif // dungeon_map_manager_h__
