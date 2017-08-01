#ifndef pet_h__
#define pet_h__

#include "creature.h"

class Dungeon;

/*
 *	宠物
 */
class Pet : public Creature
{
    CnDeclareRTTI
public:
    Pet(BattleUnit* pMaster);
    virtual ~Pet();

    virtual bool    IsAIRun() {return m_IsAIRun;}

    void    SetMaster(BattleUnit* master) {m_Master = master;}
    BattleUnit* GetMaster() {return m_Master;}

    bool    IsMasterDead();

    void    MasterOnhit( uint32 arg1 );                      // 主人被攻击
    void    MasterAttack( uint32 arg1 );                     // 主人攻击

    void    OnEnterDungeon(Dungeon* dungeon, uint32 dungeonID);

    void    SetAIRun(bool b) {m_IsAIRun = b;}

    bool    IsEnable() const {return m_IsEnable;}
    void    SetEnable(bool b);

private:
    BattleUnit*     m_Master;                               // 战斗宠物的主人

    bool            m_IsAIRun;                              // 是否执行AI

    bool            m_IsEnable;                             // 是否启用
};

#endif // pet_h__
