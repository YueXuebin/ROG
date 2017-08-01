#ifndef regular_maoxian_230_h__
#define regular_maoxian_230_h__

#include "regular_maoxian_base.h"
#include "point_monster.h"

// 冒险地图230
class RegularMaoXian_230 : public RegularMaoXianBase
{
public:
    RegularMaoXian_230(RegularDoc* pDoc);
    virtual ~RegularMaoXian_230();

    virtual void FirstUpdate();
};


#endif // regular_maoxian_030_h__
