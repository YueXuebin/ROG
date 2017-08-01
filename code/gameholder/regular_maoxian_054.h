#ifndef regular_maoxian_054_h__
#define regular_maoxian_054_h__

#include "regular_maoxian_base.h"
#include "point_monster.h"

// 冒险地图054
class RegularMaoXian_054 : public RegularMaoXianBase
{
public:
    RegularMaoXian_054(RegularDoc* pDoc);
    virtual ~RegularMaoXian_054();

    virtual void FirstUpdate();
};


#endif // regular_maoxian_054_h__
