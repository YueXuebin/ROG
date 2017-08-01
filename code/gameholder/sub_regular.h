#ifndef sub_regular_h__
#define sub_regular_h__

class Scene;
class Regular;
class BattleUnit;

/*
 *	永久地图,子规则基类
 */
class SubRegular
{
public:
    SubRegular(Regular* pRegular);
    virtual ~SubRegular();

    virtual void    OneSecondUpdate(int32 deltatime) {}
    virtual void    OnCreatureEnter(BattleUnit* creature) {}
    virtual void    OnCreatureLeave(BattleUnit* creatrue) {}
    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3) {}
    virtual void    Update(uint32 curr_tick) {}

    Regular* GetRegular() {return m_Regular;}
    Scene*  GetScene();


protected:
    Regular*    m_Regular;          // 所属的规则

};

#endif // sub_regular_h__