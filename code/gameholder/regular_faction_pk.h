#ifndef regular_faction_pk_h__
#define regular_faction_pk_h__

#include "regular_multiple.h"


// 圣剑争锋
class RegularFactionPK : public RegularMultiple
{
public:
    RegularFactionPK(RegularDoc* pDoc);
    virtual ~RegularFactionPK();

    virtual void    OnCreatureEnter(BattleUnit* creature);
    virtual void    OnCreatureLeave(BattleUnit* creatrue);

    virtual void    OneSecondUpdate(int32 deltatime);

    virtual void    FillStartPostion(Player* player, uint32 triggerID=0);

    virtual bool    CanEnter();

    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    virtual void    OnDungeonClear(bool success, std::vector<std::string>* arglist);

    void            ComboKill(const std::string& attackerName, int32 combo);
    void            StopCombo(const std::string& attackerName, const std::string& deaderName, int32 combo);
private:
    void            AddKillInfo(Player* attacker, Player* deader);

    void            SendBattleInfo(uint32 type, const std::string& attack_name, const std::string& deader_name, int32 combo);
    void            SetGroup(BattleUnit* creature);                     // 设置分组 记录队伍
    void            SetPlayerGroup(Player* player);

    void            RecieveDead(int32 deadID, int32 attackID);          // 接收死亡事件

    int32           CalGroupBattlePowerTotal(uint8 groupType);          // 计算总战斗力
    void            JumpToTrigger(Player* player, int32 triggerID);
    int32           GetPlayerCount(uint8 groupType);
};

#endif // regular_faction_pk_h__
