/*
 *	离线玩家对象
 */
#ifndef offline_player_h__
#define offline_player_h__

#include "creature.h"

class OfflinePlayer : public Creature
{
    CnDeclareRTTI
public:
    OfflinePlayer();
    virtual ~OfflinePlayer();

    virtual void    Update(uint32 frame_count);

    void SetPorperyByInfo(const OFFLINE_PLAYER_INFO& recordInfo);

    void OfflinePlayerClear();

    const OFFLINE_PLAYER_INFO& GetPlayerInfo() {return m_PlayerInfo;}

    virtual void OnDead(BattleUnit* killer);

    //virtual uint32 GetDefaultSkill();

private:
    OFFLINE_PLAYER_INFO m_PlayerInfo;

};

#endif // offline_player_h__
