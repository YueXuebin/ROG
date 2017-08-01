#ifndef remana_helper_h__
#define remana_helper_h__

#include "game_define.h"
#include "game_util.h"

class BattleUnit;

/*
 *	魔法恢复器
 */
class ReMpHelper
{
public:
    ReMpHelper(BattleUnit* owner);
    ~ReMpHelper() {}

    void Update();

private:
    BattleUnit* m_owner;
    uint32      m_PeriodTimer;      // 周期计时器
    real        m_AddVal;
};

#endif // remana_helper_h__
