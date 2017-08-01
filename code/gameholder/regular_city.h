#ifndef regular_city_h__
#define regular_city_h__


#include "regular_world.h"

/*
 *	主城(永久地图)
 */
class RegularCity : public RegularWorld
{
public:
    RegularCity(RegularDoc* pDoc);
    virtual ~RegularCity();

    virtual void FirstUpdate();

    virtual void OnCreatureEnter(BattleUnit* creature);
};

#endif // regular_city_h__
