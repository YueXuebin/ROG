#ifndef regular_maoxian_030_h__
#define regular_maoxian_030_h__

#include "regular_maoxian_base.h"
#include "point_monster.h"

// 冒险地图030
class RegularMaoXian_030 : public RegularMaoXianBase
{
public:
    RegularMaoXian_030(RegularDoc* pDoc);
    virtual ~RegularMaoXian_030();

    virtual void FirstUpdate();
};


#endif // regular_maoxian_030_h__
