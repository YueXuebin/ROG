/*
 *	修罗场数据管理器
 */
#ifndef ashura_recorder_h__
#define ashura_recorder_h__

class BattleUnit;
class Player;
#include "ashura_config.h"

class AshuraRecorder
{
public:
    AshuraRecorder(Player* player);
    ~AshuraRecorder();

    void LoadAshuraInfo(const ASHURA_INFO& info);
    void FillAshuraInfo(ASHURA_INFO& info);

    //void SetPlayer(Player* player) {m_player = player;}

    void Update();

    void Clear();

    void AddDeadCount(BattleUnit* Attacker);
    void AddKillCount(BattleUnit* Deader);

    void BattleInfo(BattleUnit* Attacker, BattleUnit* Deader);

    //void CalDailyRefresh();            // 每日刷新

    void OnRecv(uint32 msgID, void* data);
    //int32 GetKillCount();
    //int32 GetDeadCount();
private:
    //void OnAshuraGetAwardReq(void* data);

    //REWARD_TABLE CalAshuraRewardList();         // 计算奖励, 是否标记已领

    //void CalPlayerNumReward(REWARD_TABLE& rewardTable);             // 人头计算
    //void CalNumberReward(const AshRewardList& rewardListTmp, uint32& num, uint32& gamepoint, uint32& soulpoint, uint32& exp);

    //void CalTitleRewardList(REWARD_TABLE& rewardTable);             // 称号计算
    //void CalTitleReward(const std::map<uint32, REWARD_TABLE>& rewardTmp, REWARD_TABLE& rewardTable);

    //void Notify();

private:
    Player*             m_player;

    bool                m_Dirty;

    ASHURA_INFO m_ashura;
};

#endif // ashura_recorder_h__
