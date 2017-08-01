/*
 *	频道管理器
 */
#ifndef channel_manager_h__
#define channel_manager_h__

class DungeonSet;
class Player;

// 频道管理器类型
enum
{
    CHANNEL_DEFAULT = 1,        // 默认临时副本频道类型
    CHANNEL_CITY,               // 城镇频道
    CHANNEL_TEAM,               // 单服组队副本频道
    CHANNEL_CROSS_TEAM,         // 跨服组队副本频道
    CHANNEL_ASHURA,             // 修罗场频道
    CHANNEL_TD,                 // 守卫地宫频道
    CHANNEL_FACTION_PK,         // 两派PK频道
    CHANNEL_WORLD_BOSS,         // 世界BOSS频道
    CHANNEL_CITY_FIGHT,         // 城市保卫战频道
    CHANNEL_CRUSADE_BOSS,       // 争夺公会BOSS
    CHANNEL_TWILIGHT_WAR,       
    CHANNEL_CITY_SURVIVAL,      // 圣光城争夺战
    CHANNEL_CAPTURE_FLAG,       // 夺旗
    CHANNEL_VOID_DREAMLAND,     // 虚空幻境频道
    CHANNEL_MULTISERVER_BATTLE,     // 跨服战斗
    CHANNEL_CHAOS,              // 大乱斗
};

class ChannelManager
{
public:
    ChannelManager(DungeonSet* pDungeonSet);
    virtual ~ChannelManager();

    typedef std::map<uint32, uint32>    ChannelMap;

    virtual uint32 GetType() = 0;

    virtual void Update() = 0;

    /*
     *	进入副本的玩家请求一个频道
     *  channelID!=0为该玩家请求的频道，各管理器根据情况处理
     *  必须返回一个可以进入的频道
     */
    virtual uint32  AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID) = 0;

    // 进入非法副本的流程
    uint32 EnterInvalidChannel();

    virtual void    RegisterChannel(uint32 channelID);
    virtual void    UnregisterChannel(uint32 channelID);

    bool    HasChannelID(uint32 channelID);

    uint32  GetChannelNum() {return m_ChannelIDs.size();}

    // 找一个可用频道
    uint32  findValidChannelID();

    uint32  GetDungeonID();

    virtual void PlayerLeave(uint64 playerID) { }

protected:
    DungeonSet*     m_pDungeonSet;

    ChannelMap      m_ChannelIDs;    // 已经使用的频道 <频道id, 频道id>
};


#endif // channel_manager_h__
