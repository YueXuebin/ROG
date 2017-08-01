#ifndef regular_guild_h__
#define regular_guild_h__

#include "regular_dungeon.h"

/*
 *	公会战类玩法规则基类
 */
class RegularGuild : public RegularDungeon
{
    CnDeclareRTTI
public:
    RegularGuild(RegularDoc* pDoc);
    virtual ~RegularGuild();

    virtual void    OnCreatureEnter(BattleUnit* creature);

    virtual void    OnCreatureLeave(BattleUnit* creatrue);

    virtual void    FirstUpdate();

    virtual void    OneSecondUpdate(int32 deltatime);

    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    virtual void    OnDungeonClear(bool success, std::vector<std::string>* arglist);

    virtual bool    IsGameStart();

    virtual void    FillStartPostion(Player* player, uint32 triggerID = 0);


protected:
    void            SetRelivePosition(Player* player);                              // 设置角色重生时的 Trigger 点

    void            SetBornTrigger(uint32 beginID, uint32 num);                     // 设置开始 Pin 点

    void            SetReliveTrigger(uint32 beginID, uint32 num);                   // 设置重生点个数

    uint32          GetTriggerID(uint64 guildID);                                   // 获得 Pin 点

    bool            IsValidTriggerID(uint32 pinID);                                 // 检查是否为有效的 Pin

    void            AddTriggerCounter(uint64 playerID, uint64 guildID, uint32 triggerID);   // 添加 Pin 计数

    void            DelTriggerCounter(uint64 playerID);                                     // 删除 Pin 计数

    typedef struct GuildPos
    {
        void Clear()
        {
            guild_id    = 0;
            trigger_id  = 0;
            player_list.clear();
        }
        uint64  guild_id;
        uint32  trigger_id;
        std::vector<uint64> player_list;
    }GuildPos;
    typedef std::vector<GuildPos> GuildPosVer;

protected:
    uint32                  m_bornBeginID;
    uint32                  m_bornIdNum;
    uint32                  m_reliveBeginID;
    uint32                  m_reliveIdNum;
    GuildPosVer             m_trigers;
};

#endif // regular_guild_h__
