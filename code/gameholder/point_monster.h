#ifndef point_monster_h__
#define point_monster_h__

#include "point_base.h"
#include "spawn.h"

class SpawnController_Speed;
class SpawnController_MaxNum;

/*
 *	怪点
 *  (对Spawn的封装)
 */
class PointMonster : public PointBase
{
    CnDeclareRTTI
public:
    PointMonster(RegularBuilder* pRogRegular, uint32 CheckPointID);
    virtual ~PointMonster();

    virtual bool Build();
    virtual void Update(uint32 curr_tick);
    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    void SetMonster(uint32 monsterID, int32 num);               // 设置怪和出怪的数量
    void SetMovie(const std::string& movie_name, uint32 movie_time, bool attack_able = true);
    void SetDefaultMovie();                                     // 怪的演绎
    void SetLevel(uint32 level);                                // 设置怪物的等级
    void SetAI(const std::string& aiName);                      
    void SetMax(uint32 max);                                    // 怪的最大数量
    void SetGroup(uint32 group);                                
    void SetDelayTime(int32 time);                              // 触发后延时出怪(ms)
    void SetSpawnSpeed(int32 speed);                            // 出怪速度(几ms出一个)
    void SetTalk(bool canTalk);                                 //
    void SetAlwaysShow(bool alwaysShow);                        //
    void SetAIGroup(uint32 AIGroupId);                          //
    void SetQuality(uint32 quality);                            // 设置怪物品质
    void SetRadius(int32 max_radius, int32 min_radius = 0);     // 出怪范围
    void SetCantMove(bool val);                                 // 怪物不可移动
    void SetAttackable(bool val);                              // 怪物能否被攻击

    void SetLoop(bool loop);                                    // 循环出怪

    void SetTriggerID(int32 TriggerID);                           // 设置触发器触发
    int32 GetTriggerID() {return m_TriggerID;}                    // 获取触发器id

    Spawn* GetSpawn() {return m_pSpawn;}                        // 获取出怪点
    uint32 GetSpawnID() {return m_pSpawn->GetSpawnId();}        // 获取出怪点Id

    uint32 GetMonsterId() { return m_MonsterId;}                  // 获取怪的ID

    MonsterData* GetMonsterData();


protected:
    virtual void OnEventTrigger();


private:
    Spawn*  m_pSpawn;
    SpawnController_Speed*  m_pSpeedController;
    SpawnController_MaxNum* m_pMaxNumController;

    int32   m_TriggerID;                // 触发的TriggerID(进入范围触发)
    int32   m_DelayTime;                // 延迟出怪时间

    bool    m_StartSpawn;               // 开始出怪标记

    uint32  m_MonsterId;                // 怪的ID
};


#endif // point_monster_h__
