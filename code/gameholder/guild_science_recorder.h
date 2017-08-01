#ifndef guild_science_recorder_h__
#define guild_science_recorder_h__

#include "player.h"

class GuildScienceRecorder
{
public:
    GuildScienceRecorder(Player* pPlayer);
    ~GuildScienceRecorder();

    void OnRecv(uint32 msgID, void* data);

    void LoadGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info);                             // 加载个人公会科技信息
    void FillGuildScienceInfo(GUILD_PLAYER_SCIENCE_INFO& info);                             // 填充个人公会科技信息

    void CalDailyRefresh();                                                                 // 每日刷新

    void AddAdditionPromoteNum(int32 additionID, int32& num);                               // 公会科技对物品获取的加成

    //void CBGuildAdditionPromoteTrade(const std::string& str);                             // 回调公会科技提升
private:
    void onGuildSciencePlayerInfo();                                                        // 科技个人信息请求(进行判断 转发给Cente处理)

    void onAdditionPromoteReq(void* data);                                                  // 科技加成请求
    void PromoteGamePoint(GUILD_PLAYER_ADDITION* playerAdditionItem);                       // 金币加成
    void PromoteMoneyPoint(GUILD_PLAYER_ADDITION* playerAdditionItem);                      // 魔石加成

    void onGuildAdditionPromoteToGame(void* data);                                          // 科技加成回调 (已处理完增加公会经验, 做个人科技信息处理)

    GUILD_PLAYER_ADDITION* GetGuildPlayerAddition(int32 additionID);                        // 个人加成信息
    GUILD_ADDITION* GetGuildAddition(int32 additionID);                                     // 公会加成信息
private:
    Player* m_player;

    std::vector<GUILD_PLAYER_ADDITION>	m_additionList;     // 个人加成信息

    bool    m_lock;                                         // 异步锁
};

#endif // guild_science_recorder_h__
