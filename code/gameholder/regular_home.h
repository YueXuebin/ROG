#ifndef regular_home_h__
#define regular_home_h__

#include "regular_single.h"

/*
 *	领地地图
 */
class RegularHome : public RegularSingle
{
public:
    RegularHome(RegularDoc* pDoc);
    virtual ~RegularHome();

    virtual void FirstUpdate();

    virtual void OnCreatureEnter(BattleUnit* creature);
};



#endif // regular_home_h__
