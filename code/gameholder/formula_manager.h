/*
 *	公式解析管理器
 */
#ifndef formula_manager_h__
#define formula_manager_h__

class BattleUnit;
class Player;

class FormulaManager
{
public:
    FormulaManager();
    ~FormulaManager();

    DECLARE_SINGLETON(FormulaManager)

    bool Init();

    bool GetFormulaParam(const char* param_name, real* val);

    template<typename T> T CallFormulaLua(uint32 formulaID);

    uint32 TestCalc(int32 x, int32 y, int32 z);

    // 计算二级属性
    int32 CalcPlayerSecondProp(uint32 formulaID, int32 strength, int32 agility, int32 constitution, int32 spirit);

    uint32 CalcWorldBossHp(BattleUnit* battleUnit, int32 strong_level);
    uint32 CalcWorldBossAttack(BattleUnit* battleUnit, int32 strong_level);
    uint32 CalcWorldBossDefence(BattleUnit* battleUnit, int32 strong_level);
    int32 CalcWorldBossRewardByDamage(int32 damage);

    uint32 CalcGuildBossHp(BattleUnit* battleUnit, int32 strong_level);
    uint32 CalcGuildBossAttack(BattleUnit* battleUnit, int32 strong_level);
    uint32 CalGuildBossDefence(BattleUnit* battleUnit, int32 strong_level);

    // 虚空跨服boss
    uint32 CalcVoidFieldBossHp(BattleUnit* battleUnit, int32 strong_level);
    uint32 CalcVoidFieldBossAttack(BattleUnit* battleUnit, int32 strong_level);
    uint32 CalcVoidFieldBossDefence(BattleUnit* battleUnit, int32 strong_level);

    // 虚空单服boss
    int32 CalcVoidFieldSoloBossHp(BattleUnit* battleUnit, int32 strong_level);
    int32 CalcVoidFieldSoloBossAttack(BattleUnit* battleUnit, int32 strong_level);
    int32 CalcVoidFieldSoloBossDefence(BattleUnit* battleUnit, int32 strong_level);

    // 计算装备升星秒cd价格
    int32 CalFinishStarCDPrice(int32 minute);

    //计算圣剑升级时的经验
    //表示阶级 level
    uint32 CalcSwordStartExp(int32 level);

    //计算翅膀升级时的经验
    // 表示阶级 level
    //uint32 CalWingStartExp(int32 level);

    //计算跟宠的属性
    uint32 CalFollowpetPro(int32 base,int32 grow,int32 level);

    // 计算升级翅膀所需经验
    //uint32 CalcWingExp(Player* player, uint32 wingLevel);

    // 计算副本得分
    uint32 CalcDungeonPoint(uint32 wasteTime, uint32 totalTime, uint32 maxCombo, uint32 monsterNum, uint32 comboNum);

    // 计算圣光生存战杀怪金钱奖励
    //uint32 CalcCitySurvivalRewardMoney( uint32 killNum );

    // 计算圣光生存战杀怪精魂奖励
    //uint32 CalcCitySurvivalRewardSoul( uint32 killNum );

    // 计算点石成金后的钱
    int32 CalcStoneTurnGold(int32 level);

    // 计算重置个人轮盘的花费
    int32 CalcResetTurnTableCost(int32 times);

    // 计算暴击率
    //real CalcCriticalAttackReductionPercent(int32 CriticalAttackReduction);

    // 计算闪避率
    //real CalcDodgePercent(int32 dodge);

    // 计算吸血率
    //real CalcVampirePercent(int32 vampire);

    // 计算抗吸血率
    //real CalcVampireDefencePercent(int32 vampireDefence);

    // 计算控制减少
    //real CalcControlReductionPercent(int32 control_reduction);

     uint32 CalcRevivalTime(int32 time);

    // ----------------------------------------------------------------------------
    // 战斗计算用公式
    // ----------------------------------------------------------------------------
    // 命中率计算
    real CalcHitRatePro(int32 hit_rate, int32 dodge);

    // 普攻计算
    real CalcNormalAttack(int32 attack, real random, int32 defence, int32 level, int32 damage_add, int32 damage_sub);

    // 技能攻击计算
    real CalcSkillAttack(int32 attack, real random, int32 defence, int32 level, int32 skill_attack, int32 rune_attack, int32 rune_up, int32 rune_defence, int32 damage_add, int32 damage_sub);

    // 物理伤害
    //real CalcPhysicsAttack(int32 baseDamage);

    // 魔法伤害
    //real CalcMagicAttack(int32 skillDamage);

    // 暴击率计算
    real CalcCriAttPro(int32 ct_pro, int32 ct_pro_sub);

    // 暴击伤害计算
    real CalcCriAttDamage(int32 damage, int32 ct_damage, int32 ct_damage_sub);

    // 吸血率计算
    //real CalcCriVampirePro(int32 vampire_pro, int32 vampire_pro_sub);

    // 吸血计算
    //real CalcCriVampire(int32 vampire, int32 vampire_sub);

    //根据战斗力计算pvp的削减百分比
    real CalPvPDamagePercentage(int32 average_battlepower, int32 difference_power);

    // ----------------------------------------------------------------------------
    // 战斗力计算公式
    // ----------------------------------------------------------------------------
    // 战斗力计算
    //real CalcBattlePower(BattlePowerProp& prop);



private:
    FormulaParameter    m_FormulaParameter;
};


#endif // formula_manager_h__
