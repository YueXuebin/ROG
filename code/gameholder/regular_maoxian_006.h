#ifndef regular_maoxian_006_h__
#define regular_maoxian_006_h__

#include "regular_maoxian_base.h"
#include "point_monster.h"

// 冒险地图006
class RegularMaoXian_006 : public RegularMaoXianBase
{
public:
    RegularMaoXian_006(RegularDoc* pDoc);
    virtual ~RegularMaoXian_006();

    virtual void FirstUpdate();
};


#endif // regular_maoxian_006_h__
