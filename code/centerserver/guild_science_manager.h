#ifndef guild_science_manager_h__
#define guild_science_manager_h__

#include "player.h"

// ==================================================
/// 公会科技管理系统
// ==================================================
class GuildScienceManager
{
public:
    GuildScienceManager();
    ~GuildScienceManager();

    DECLARE_SINGLETON(GuildScienceManager)

    void OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data);

private:
    void onAdditionPromote(uint64 playerID, void* data);        // 科技提升处理 拦截条件不符合后加经验 转给Game

    void AddGuildAdditionExp(GUILD_ADDITION* additionItem, int32 exp);                          // 添加公会经验

    void SendAdditionPromoteToGame(Player* pPlayer, GUILD_ADDITION_PROMOTE_TO_GAME& sendMsg);   // 科技加成处理完成 转发Game进行个人科技后续处理

    GUILD_ADDITION* GetAdditionItem(uint64 guildID, int32 additionID);      // 得到一项科技加成信息
    std::vector<GUILD_ADDITION>* GetScienceAdditionList(uint64 guildID);    // 科技加成信息
};

#endif // guild_science_manager_h__
