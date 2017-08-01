#ifndef rog_regular_event_damage_rank_h__
#define rog_regular_event_damage_rank_h__

#include "rog_regular_event.h"

/*
 *	伤害输出排名第一达成
 */
class ROGRegularEvent_DamageRank : public ROGRegularEvent
{
    CnDeclareRTTI
public:
    ROGRegularEvent_DamageRank(RegularDungeon* pRegular);
    virtual ~ROGRegularEvent_DamageRank();

    virtual void OneSecondUpdate(int32 deltatime);

    virtual bool GetActiveForPlayer(Player* player);

private:
    typedef struct
    {
        std::string player_name;
        uint64  player_id;
        uint32  damage;
    } PLAYER_DAMAGE;

private:
    std::vector<PLAYER_DAMAGE>  m_DamageRank;
    uint64      m_FirstPlayerID;

};


#endif // rog_regular_event_damage_rank_h__
