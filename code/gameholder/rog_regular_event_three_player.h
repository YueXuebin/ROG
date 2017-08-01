#ifndef rog_regular_event_three_player_h__
#define rog_regular_event_three_player_h__

#include "rog_regular_event.h"

/*
 *	三人通关,且无人退出
 */
class ROGRegularEvent_ThreePlayer : public ROGRegularEvent
{
    CnDeclareRTTI
public:
    ROGRegularEvent_ThreePlayer(RegularDungeon* pRegular);
    virtual ~ROGRegularEvent_ThreePlayer();

    virtual void OneSecondUpdate(int32 deltatime);

private:
    int32    m_MaxPlayerNum;
    bool    m_NoOneLeave;
};



#endif // rog_regular_event_three_player_h__
