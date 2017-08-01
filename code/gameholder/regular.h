#ifndef regular_h__
#define regular_h__

#include "regular_base.h"
#include "battle_define.h"

class Scene;
class BattleUnit;
class RegularEvent;
class SkillObject;
class Player;
class RegularDoc;
class AIGroupRegister;


/*
 *	游戏的规则引擎(每一个场景对应一个规则对象)
 */
class Regular : public RegularBase
{
    CnDeclareRTTI
public:
    Regular(RegularDoc* pDoc);
    virtual ~Regular();

    virtual void OnCreatureEnter(BattleUnit* creature);
    virtual void OnCreatureLeave(BattleUnit* creatrue);

    virtual void FirstUpdate();
    virtual void OneSecondUpdate(int32 deltatime);

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    virtual void FillStartPostion(Player* player, uint32 triggerID=0);

    virtual uint32 GetReliveTriggerID(uint32 groupID);

    virtual void SetUserData(void* userdata) {}

    void FillSceneInfo(SCENE_INFO_ACK& sceneInfo);

    virtual void OnStart() { }
    virtual void OnStop() { }
    virtual void OnKick() { }

    virtual bool CanEnter() {return true;}           // 能否进入此关卡
    virtual bool CanUseSkill(uint32 skillID = 0) {return true;}
    virtual void SetCanUseHp(bool canUse) {m_canUseHp = canUse;}
    virtual bool CanUseHp() {return m_canUseHp;}
    virtual void SetCanUseMp(bool canUse) {m_canUseMp = canUse;}
    virtual bool CanUseMp() {return m_canUseMp;}
    virtual void SetCanUseAttack(bool canUse) {m_canUseAttack = canUse;}
    virtual bool CanUseAttack() {return m_canUseAttack;}
    virtual void SetCanUseDeffence(bool canUse) {m_canUseDeffence = canUse;}
    virtual bool CanUseDeffence() {return m_canUseDeffence;}
    virtual void SetCanUseSpeed(bool canUse)  {m_canUseSpeed = canUse;}
    virtual bool CanUseSpeed()   {return m_canUseSpeed;}

    int32 GetMapLevel() {return m_Level;}

    virtual bool IsGameStart() { return true; }         // 游戏是否开始

    virtual bool IsEnemy(BattleUnit* attacker, BattleUnit* defencer);

    bool IsAnyoneStart(int32 num=1);                 // 任意玩家开始

    virtual void Update(uint32 curr_tick);

    virtual void    Damage(const DamageResult& damage, BattleUnit* Attacker, BattleUnit* Defender, SkillObject* pSkill);

    virtual void    OnDead(BattleUnit* Attacker, BattleUnit* Defender);

    virtual void OnResurgence(BattleUnit* pBattleUnit);

    void CalcMapProp(BattleUnit* battleUnit);               // 计算玩家的地图属性

    void Resurgence(BattleUnit* battleUnit, uint32 triggerID, int32 delayTime);       // 复活

    void SendDungeonEvent(Player* player, int32 event_id);

    uint32 GetTickTime() {return m_TickTime;}

    AIGroupRegister*    GetAIGroupRegister() {return m_AIGroupRegister;}

protected:
    typedef std::vector<RegularEvent*>  RegularEventList;   // 关卡事件列表对象

    virtual void ParseDesign(TiXmlElement* designElement) {}
    virtual void ParseRegular(TiXmlElement* RegularElement) {}
    RegularEvent* GetRegularEventByType(int32 type);

    void StopAllSpawm();

    void SetMapLevel(int32 level) {m_Level = level;}

    void KillBonus(Player* pAttacker, Monster* pDefender);  // 击杀奖励

private:
    void CreateRegular();                        // 创建关卡配置
    void ParseRegularLogic(TiXmlNode* logicNode);

private:
    bool m_doFirstUpdate;
    uint32 m_SecondTimer;
    uint32 m_TickTime;

    bool m_canUseHp;         // 能否使用血药
    bool m_canUseMp;         // 能否使用蓝药
    bool m_canUseAttack;     // 能否使用攻击药剂
    bool m_canUseDeffence;   // 能否使用防御药剂
    bool m_canUseSpeed;      // 能否使用速度药剂

    int32   m_Level;        // 地图等级

    AIGroupRegister* m_AIGroupRegister;  // AI群组注册器

    RegularEventList m_LogicEvent;
};

#endif // regular_h__
