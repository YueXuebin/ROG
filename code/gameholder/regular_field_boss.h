/*
 *	野外Boss
 */
#ifndef regular_field_boss_h__
#define regular_field_boss_h__

#include "regular_world.h"
#include "world_map_helper.h"

class Player;
class Scene;
class RegularDoc;
class SubRegular;

class RegularFiledBoss : public RegularWorld
{
    CnDeclareRTTI
public:
    RegularFiledBoss(RegularDoc* pDoc);
    virtual ~RegularFiledBoss();

    virtual void    OneSecondUpdate(int32 deltatime);
    virtual void    OnCreatureEnter(BattleUnit* creature);
    virtual void    OnCreatureLeave(BattleUnit* creatrue);
    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);
    virtual void    Update(uint32 curr_tick);
    virtual bool    IsEnemy(BattleUnit* attacker, BattleUnit* defencer);
    virtual void    FillStartPostion(Player* player, uint32 triggerID = 0);

     void    OnPlayerRecv(Player* player, uint32 msgID, void* data);

    std::map<uint64, WorldMapPlayerRecord>& GetRecordMap() {return m_recordMap;};
    WorldMapPlayerRecord*  GetRecord(Player* player);                          ///< 获得记录
    Player*         GetPlayerByUnitID(uint32 unitID);                           ///< 根据玩家或宠物的ID获取玩家
    uint32          CalPlayerTriggerID(Player* player);                                         ///< 计算TriggerID

    //=====================================规则管理=====================================================
    bool            IsInBossLeakTime();                                         ///< 是否Boss白起侧漏时间
    std::string         GetBossKillerName();                                            ///< Boss击者
    void                ClearBossDamage();                                              //清除boss伤害
    uint32              GetFreshBossTime();                                             //得到boss刷新的时间
private:
    //=====================================玩家管理=====================================================
    void            AddRecord(Player* player);                                  ///< 添加记录
    void            DelRecord(Player* player);                                  ///< 删除记录

    SubRegular*        m_regular_hook; // 挂机规则
    SubRegular*        m_regular_monster; // 刷怪规则
    SubRegular*        m_regular_boss; // Boss规则


    std::map<uint64, WorldMapPlayerRecord>                                     m_recordMap;        ///< 记录映射表
};

#endif // regular_field_boss_h__
