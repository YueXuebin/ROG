/*
 *	副本管理器
 */
#ifndef dungeon_manager_h__
#define dungeon_manager_h__

#include "dungeon_data.h"

class Scene;
class Dungeon;
class DungeonSet;
class TiXmlDocument;
class Trigger;

class WorldMap;
class Regular;



class DungeonManager
{
public:
    DungeonManager();
    ~DungeonManager();

    DECLARE_SINGLETON(DungeonManager)

    bool Init();
    bool CreateDungeonSet();

    void Update(uint32 frame_count);

    Dungeon*    GetDungeon(uint32 dungeonID, uint32 channelID);

    Scene*      GetDefaultScene(uint32 dungeonID, uint32 channelID);        // 获得副本的默认场景
    Scene*      GetScene(uint32 sceneUnionID);                              // 获得场景

    void        StopAllRegular(uint32 dungeonID, bool success= true);       // 停掉指定副本的所有规则

    DungeonTemplate* GetDungeonTemplate(uint32 dungeonID);                  // 获得模板信息

    DungeonInfo* GetDungeonInfo(uint32 dungeonID);                          // 获得副本信息

    uint32 GetServerIdByDungeonId(uint32 dungeonID);                        // 获得副本所在服务器id

    Trigger*    GetTrigger(uint32 sceneUnionID, uint32 triggerID);

    DungeonSet* GetDungeonSet(uint32 dungeonId);
    uint32      GetPlayerNum();

    std::string GetDesignFileName(uint32 dungeonId, uint32 sceneId);

    void        SendEvent(uint32 eventId, int32 arg1, int32 arg2, uint32 delayTime=0);

    bool HasDungeonId(uint32 dungeonId);

    bool IsCity(uint32 dungeonID);

    void SendServerDungeonListNtf(uint32 sessionID);

    uint32 RequestJumpDungeonId(uint32 dungeonID);

private:
    bool LoadXml();
    bool LoadDungeonXML();
    bool LoadGameSvrXML();
    bool LoadDungeon(TiXmlElement* xmlEle);
    bool LoadScene(TiXmlElement* xmlEle, DungeonTemplate& dungeonTmpl);
    bool LoadTrigger(TiXmlElement* xmlEle, std::list<Teleport>& triggerList);
    bool LoadGameServer(TiXmlElement* xmlEle);

private:
    typedef std::map<uint32, DungeonTemplate> DungeonTmplMap;               // 副本信息 [副本id, 副本参数]
    typedef std::map<uint32, DungeonInfo> DungeonInfoMap;                   // [副本id, 副本信息]
    typedef std::map<uint32, DungeonSet*> DungeonSetMap;                    // [副本id, 副本集]

    DungeonTmplMap      m_dungeonTmpls;                                     // 副本模板
    DungeonInfoMap      m_dungeonInfos;                                     // 副本信息
    DungeonSetMap       m_dungeonSets;                                      // 副本集

    TiXmlDocument*      m_XmlDoc;                                           // 副本配置文件数据

    std::vector<uint32>     m_CityIDs;                                      // 主城副本的ID列表
};


#endif // dungeon_manager_h__
