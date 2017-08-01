/*
 *  公会类玩法的频道管理器(基类)
 */
#ifndef guild_channel_manager_h__
#define guild_channel_manager_h__

#include "channel_manager.h"

class Dungeon;

class GuildChannelManager : public ChannelManager
{
public:
    GuildChannelManager(DungeonSet* pDungeonSet);
    virtual ~GuildChannelManager();

    virtual uint32 AskEnterUnionID( Player* player, uint32 sceneID, uint32 channelID);

    virtual void PlayerLeave(uint64 playerID);

    virtual void SetReadyRunTime(Dungeon* dungeon) = 0;

protected:

    uint32 AskIDInDungeonSet(uint64 groupID, uint64 playerID);

    // 设置组数
    void SetGroupNum(uint32 val) { m_groupNum = val; }

    // 设置每组中的角色数量
    void SetGroupPlayerNum(uint32 val) { m_groupPlayerNum = val; }

    // 组人员数量
    class GroupCounter
    {
    public:
        void Clear()
        {
            score       = 0;
            groupID     = 0;
            triggerID   = 0;
            playerIDList.clear();
        }

        // 检查是否存在此玩家
        bool HasPlayer(uint64 playerID)
        {
            for(std::vector<uint64>::iterator it = playerIDList.begin(); 
                it != playerIDList.end(); ++it)
            {
                if(*it == playerID)
                {
                    return true;
                }
            }
            return false;
        }

        void AddPlayer(uint64 playerID)
        {
            playerIDList.push_back(playerID);
        }

        // 在计数列表中删除玩家
        void DelPlayer(uint64 playerID)
        {
            for(std::vector<uint64>::iterator it = playerIDList.begin(); 
                it != playerIDList.end(); ++it)
            {
                if(*it != playerID) continue;
                playerIDList.erase(it);
                break;
            }
        }

        uint32 GetPlayerNum()
        {
            return playerIDList.size();
        }

        int32  score;                       // 分数
        uint64 groupID;                     // 组ID
        uint32 triggerID;                   // 将要跳转到的 Trigger 的ID
        std::vector<uint64> playerIDList;   // 已在副本中的角色数量
    };
    typedef std::vector<GroupCounter> GroupCounterVert;

    // 频道成员计数器
    class ChannelCounter
    {
    public:
        void Clear()
        {
            channelID   = 0;
            playerNum   = 0;
            groupVert.clear();
        }

        uint32 GetPlayerNum() { return playerNum; }
        void IncPlayerNum() { ++playerNum; }
        void DecPlayerNum()
        {
            CnAssert(playerNum > 0);
            if(playerNum > 0) --playerNum;
        }

        uint32  channelID;
        uint32   playerNum;
        GroupCounterVert groupVert;
    };
    typedef std::vector<ChannelCounter> ChannelCounterVert;

    ChannelCounterVert::iterator GetChannelCounterVert(uint32 channelID);

    uint32              m_groupNum;                 // 公会数(组数)
    uint32              m_groupPlayerNum;           // 每公会最大人数
    ChannelCounterVert  m_channelCounter;
    std::vector<uint32> m_CurrentChannels;
};


#endif // guild_channel_manager_h__
