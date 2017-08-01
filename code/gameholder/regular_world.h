#ifndef regular_world_h__
#define regular_world_h__

#include "regular_builder.h"

class SubRegular;

/*
 *	永久型地图规则基类(野外地图)
 */
class RegularWorld : public RegularBuilder
{
    CnDeclareRTTI
public:
    RegularWorld(RegularDoc* pDoc);
    virtual ~RegularWorld();

    virtual bool    IsGameStart() {return true;}        // 永久地图，永远执行逻辑

    virtual void    OneSecondUpdate(int32 deltatime);
    virtual void    OnCreatureEnter(BattleUnit* creature);
    virtual void    OnCreatureLeave(BattleUnit* creatrue);
    virtual void    ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);
    virtual void    Update(uint32 curr_tick);
    

    void AddSubRegular(SubRegular* pSubRegular);
    void RemoveSubRegular(SubRegular* pSubRegular);

private:
    bool HasSubRegular(SubRegular* pSubRegular);

private:
    std::vector<SubRegular*>    m_SubRegulars;          // 子规则列表

};




#endif // regular_world_h__
