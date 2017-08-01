/*
 *	副本集(管理同一个副本的多个频道对象)
 */
#ifndef dungeon_set_h__
#define dungeon_set_h__

class Dungeon;
class BattleUnitConnecter;
class DungeonTemplate;
class ChannelManager;
class Player;

class DungeonSet
{
    friend class ChannelManager;
public:
    DungeonSet(uint32 dungeonID, uint32 type, uint32 channel_number);
    ~DungeonSet();

    void Update(uint32 frame_count);

    uint32 GetDungeonID() {return m_dungeonID;}
    uint32 GetType() {return m_type;}
    uint32 GetChannelNumber() {return m_channel_number;}
    void SetChannelNumber(uint32 channel_number) {m_channel_number = channel_number;}
    uint32 GetDungeonNumber() {return m_Dungeons.size();}
    uint32 GetPlayerNum();
    std::vector<Player*> GetPlayerMap();

    Dungeon* CreateDungeon(uint32 channelID);
    Dungeon* GetDungeon(uint32 channel_id);
    std::map<uint32, Dungeon*>& GetDungeons() { return m_Dungeons; }

    void StopAllRegular(bool success);

    BattleUnitConnecter* GetBattleUnitConnecter() { return m_pBattleUnitConnecter; }

    void SetXml(TiXmlHandle scenesXML) { m_SceneXml = scenesXML; }
    TiXmlHandle GetXml() {return m_SceneXml;}

    void SetDungeonTemplate(const DungeonTemplate* val) { m_dungeonTmpl = val; }
    const DungeonTemplate* GetDungeonTemplate() { return m_dungeonTmpl; }

    bool CheckMap();

    void OnDeleteDungeonAck(uint32 channelId, uint16 errCode);

    void SendEvent(uint32 eventId, int32 arg1, int32 arg2, uint32 delayTime);

    // 请求玩家进入该副本后的unionID,(channelID为玩家期望的频道)
    uint32  AskEnterUnionID(Player* player, uint32 sceneID, uint32 channelID);

    ChannelManager*     GetChannelManager() {return m_pChannelManager;}

private:
    ChannelManager*     CreateChannelManager(uint32 dungeonID, uint32 type);

    bool    checkValidChannelID(uint32 channelID);
    uint32  findValidChannelID();

private:
    typedef std::map<uint32, Dungeon*> DungeonMap;          ///< [频道id, 副本对象]
    DungeonMap                  m_Dungeons;                 ///< 所有的副本

    TiXmlHandle                 m_SceneXml;                 ///< xml数据

    const DungeonTemplate*      m_dungeonTmpl;              ///< 副本数据模板

    uint32                      m_dungeonID;                ///< 副本id
    uint32                      m_type;                     ///< 副本类型(主城, 战斗副本)

    uint32                      m_channel_number;           ///< 频道数量

    ChannelManager*             m_pChannelManager;          ///< 频道管理器

    BattleUnitConnecter*        m_pBattleUnitConnecter;
};


#endif // dungeon_set_h__