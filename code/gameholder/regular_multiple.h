#ifndef regular_multiple_h__
#define regular_multiple_h__

#include "regular_dungeon.h"

class RegularDoc;


/*
 *	多人玩法规则基类
 */
class RegularMultiple : public RegularDungeon
{
    CnDeclareRTTI
public:
    RegularMultiple(RegularDoc* pDoc);
    virtual ~RegularMultiple();

    virtual bool    IsGameStart();


};


#endif // regular_multiple_h__
