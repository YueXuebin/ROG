/*
 *	复活管理器(帮助管理复杂的复活行为)
 */
#ifndef resurgence_helper_h__
#define resurgence_helper_h__

class BattleUnit;
class Dungeon;


class ResurgenceHelper
{
public:
    ResurgenceHelper(BattleUnit* owner);
    ~ResurgenceHelper();

    void EventResurgence(int32 triggerID, int32 delayTime);     // 通知复活
    
    void Update(uint32 curr_tick);

    void OnEnterDungeon(Dungeon* dungeon, uint32 dungeonID);

    void SetResurgenceFullHp();     // 标记下次复活时满血

    void StopResurgence() {m_resurgence = false;}
    void StartResurgence() {m_resurgence = true;}

    int32 GetTesurgenceTime() const {return m_resurgenceTime;}

private:
    void Resurgence();      // 复活操作

    void ResurgenceHp();    // 复活回血

private:
    BattleUnit* m_Owner;

    bool                m_resurgence;               // 是否复活
    bool                m_ResurgenceFullHp;         // 复活时是否满血
    int32               m_resurgenceTime;           // 复活时间
    int32               m_resurgenceTriggerID;      // 复活TriggerID
};


#endif // resurgence_helper_h__
