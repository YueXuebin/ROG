#ifndef rog_regular_event_combo_kill_h__
#define rog_regular_event_combo_kill_h__

#include "rog_regular_event.h"

/*
 *	连杀数
 *  m_param1: 需要达到的连杀数
 */
class ROGRegularEvent_ComboKill : public ROGRegularEvent
{
    CnDeclareRTTI
public:
    ROGRegularEvent_ComboKill(RegularDungeon* pRegular);
    virtual ~ROGRegularEvent_ComboKill();

    virtual void OneSecondUpdate(int32 deltatime);

    virtual bool GetActiveForPlayer(Player* player);
    

    void SetComboKill(int32 num) {m_param1 = num;}
    int32 GetComboKill() {return m_param1;}

private:
    void SetActiveForPlayer(Player* player);

private:
    std::vector<uint64>  m_ActivePlayerIDs;

};


#endif // rog_regular_event_combo_kill_h__
