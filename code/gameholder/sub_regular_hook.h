/*
 *	大地图挂机规则
 */
#ifndef sub_regular_hook_h__
#define sub_regular_hook_h__

#include "sub_regular.h"
#include "world_map_helper.h"

class Player;
class Scene;
class RegularDoc;
class RegularFiledBoss;

class SubRegularHook : public SubRegular
{
public:
    SubRegularHook(Regular* pRegular);
    virtual ~SubRegularHook();

    virtual void    OneSecondUpdate(int32 deltatime);
    virtual void    OnCreatureEnter(BattleUnit* creature);
    virtual void    OnCreatureLeave(BattleUnit* creatrue);
    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);
    virtual void    Update(uint32 curr_tick);
private:
    //=====================================挂机经验=====================================================
    void            RewardHookExp();                                                            ///< 奖励挂机经验
    void            AddPersonalExpInfo(Player* player, VOID_FIELD_PERSONAL_INFO_NTF* info, int32 exp);      ///< 添加挂机经验信息

    //=====================================玩家战斗=====================================================
    void            OnBattleEventAttack(uint32 attackerID, uint32 defencerID);  ///< 接收角色被攻击事件
    void            OnBattleEventKill(uint32 attackerID, uint32 defencerID);    ///< 接收角色死亡事件
    void            AddPersonalKillInfo(Player* attacker, Player* defencer);                                ///< 添加玩家击杀信息

    //=====================================玩家复活=====================================================
    void            SendResurgenceAsk(Player* player);                                          ///< 发送复活询问
    void            OnPlayerResurgenceReq(int32 arg1);                                          ///< 接收复活请求

    //=====================================红名buff=====================================================
    bool            IsRedName(Player* player);                                                  ///< 是否红名
    void            AddPlayerRedNameBuff(Player* player);                                       ///<.添加玩家红名buff
    void            ClearPlayerRedNameBuff(Player* player);                                     ///<.清除玩家红名buff

    //=====================================推送信息=====================================================
    void            PushMessage();                                                           ///< 推送所有信息

    RegularFiledBoss*            m_masterRegular;           ///< 管理者
};

#endif // sub_regular_hook_h__
