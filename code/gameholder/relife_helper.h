#ifndef relife_helper_h__
#define relife_helper_h__

#include "game_define.h"
#include "game_util.h"

class BattleUnit;

/*
 *	生命恢复器(每秒回血可能小于1)
 */
class ReHpHelper
{
public:
    ReHpHelper(BattleUnit* owner);
    ~ReHpHelper() {}

    void Update();

private:
    BattleUnit* m_owner;
    uint32      m_PeriodTimer;      // 周期计时器
    real        m_AddVal;
};

#endif // relife_helper_h__
