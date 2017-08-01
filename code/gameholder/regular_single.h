#ifndef regular_single_h__
#define regular_single_h__

#include "regular_dungeon.h"

class RegularDoc;

/*
 *	单人玩法规则基类
 *  特点:
 *      多人加入,难度会改变
 */
class RegularSingle : public RegularDungeon
{
    CnDeclareRTTI
public:
    RegularSingle(RegularDoc* pDoc);
    virtual ~RegularSingle();

    virtual void    FirstUpdate();
    virtual void    OneSecondUpdate(int32 deltatime);
    virtual void    OnCreatureEnter(BattleUnit* creature);
    virtual void    OnCreatureLeave(BattleUnit* creatrue);
    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    virtual void    OnSceneClear(bool success, std::vector<std::string>* arglist);      // 场景完成处理
    virtual void    OnDungeonClear(bool success, std::vector<std::string>* arglist);    // 副本通关处理(只由DungeonRuled调用

    virtual bool    IsGameStart();

    void    OnPlayerEnterDungeonNtf(uint64 playerID, const std::string& playerName);       // 玩家进入副本通知
    void    OnPlayerLeaveDungeonNtf(uint64 playerID, const std::string& playerName);       // 玩家离开副本通知

protected:
    virtual void    ParseRegular(TiXmlElement* RegularElement);

    void    PushDungenGain(Player* player, const REWARD_ITEM& reward, std::vector<REWARD_ITEM>& list);

    virtual int32 CalcDifficultyLevel(uint32 playerNum);                                    // 根据人数计算难度
    virtual int32 GetDifficultLevel() {return m_Difficulty;}

    uint64  GetMasterPlayerID();                                                            // 获得房主ID(第一个进入副本的玩家)

private:
    void    OnDungeonClearNotify(Player* player, bool success, int32 score, uint32 dungeonTime, bool isDiscount, bool hasMaster);

    void    OnChangeDifficulty(uint32 DifficultLevel);

    void    NotifyPlayerEnterDungeon(uint64 playerID, const std::string& playerName, int32 DeltaDifficultLevel);
    void    NotifyPlayerLeaveDungeon(uint64 playerID, const std::string& playerName, int32 DeltaDifficultLevel);

private:
    int32  m_Difficulty;      // 关卡难度(初始为1,人数越多难度越高)

};

#endif // regular_single_h__
