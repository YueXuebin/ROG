#ifndef reenergyshield_helper_h__
#define reenergyshield_helper_h__

class BattleUnit;

/*
 *	能量盾恢复器
 *  不被攻击后n秒开始恢复，n秒恢复满
 */
class ReEnergyShieldHelper
{
public:
    ReEnergyShieldHelper(BattleUnit* owner);
    ~ReEnergyShieldHelper() {}

    void Update();

    void OnDamage();

private:
    BattleUnit* m_owner;
    uint32      m_PeriodTimer;      // 周期计时器
    real        m_AddVal;
    int32       m_DamageTimer;

};



#endif // reenergyshield_helper_h__
