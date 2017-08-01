#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

class Player;

// ==================================================
/// 频道管理系统(不用了)
// ==================================================
class ChannelManager
{
public:
    ChannelManager();
    ~ChannelManager();

    DECLARE_SINGLETON(ChannelManager)

    void Clear();

    // 请求公会频道号
    uint32 RequestChannelID(uint32 dungeonID, Player* player);

    // 进入组队派别PK副本
    void OnEnterTeamDungeon(uint32 channelID, uint32 dungeonID, Player* player);

    // 离开最对派别PK副本
    void OnExitTeamDungeon(uint32 channelID, uint32 dungeonID, Player* player);

    // 进入公会BOSS副本
    void OnEnterGuildBossDungeon(uint32 channelID, uint32 dungeonID, Player* player);

    // 离开公会BOSS副本
    void OnExitGuildBossDungeon(uint32 channelID, uint32 dungeonID, Player* player);

private:

    class DungeonPlayerCounter;

    // 请求派别OK频道号
    uint32 RequestFactionPKChannelID(DungeonPlayerCounter& dungeonCounter, Player* player);

    // 请求怪物讨伐战频道号
    uint32 RequestCrusadeBossChannelID(DungeonPlayerCounter& dungeonCounter, uint64 guildID);

    // 请求暮光之战频道号
    uint32 RequestTwilightWarChannelID(DungeonPlayerCounter& dungeonCounter, uint64 guildID);

    // ================================================================================

    // 公会人员数量
    class GroupCounter
    {
    public:
        void Clear()
        {
            score       = 0;
            groupID     = 0;
            triggerID   = 0;
            prepareNum  = 0;
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

        int32 GetPlayerNum()
        {
            return (playerIDList.size() + prepareNum);
        }

        int32 GetPrepareNum() { return prepareNum; }

        void IncPrepareNum() { ++prepareNum; }

        void DecPrepareNum()
        {
            CnAssert(prepareNum > 0);
            if(prepareNum > 0) --prepareNum;
        }

        int32  score;                       // 分数
        uint64 groupID;                     // 组ID
        uint32 triggerID;                   // 将要跳转到的 Trigger 的ID
        int32  prepareNum;                  // 将要跳转还未跳转到地图内的角色数量
        std::vector<uint64> playerIDList;   // 已在副本中的角色数量
    };
    typedef std::vector<GroupCounter> GroupCounterVert;

    // 副本成员计数器
    class DungeonPlayerCounter
    {
    public:
        void Clear()
        {
            dungeonID   = 0;
            channelID   = 0;
            playerNum   = 0;
            groupVert.clear();
        }

        int32 GetPlayerNum() { return playerNum; }
        void IncPlayerNum() { ++playerNum; }
        void DecPlayerNum()
        {
            CnAssert(playerNum > 0);
            if(playerNum > 0) --playerNum;
        }

        uint32  dungeonID;
        uint32  channelID;
        int32   playerNum;
        GroupCounterVert groupVert;
    };
    typedef std::vector<DungeonPlayerCounter> DungeonCounterVert;

    // ================================================================================

    DungeonCounterVert m_dungeonCounter;
};

#endif
