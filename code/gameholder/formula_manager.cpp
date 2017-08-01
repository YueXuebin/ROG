#include "gameholder_pch.h"
#include "formula_manager.h"
#include "battle_unit.h"
#include "player.h"
#include "formula_config.h"
#include "skill_info_manager.h"
#include "passive_skill_manager.h"
#include "player.h"
#include "lua_engine.h"
#include "config_manager.h"

bool g_UseLuaFormula = false;

IMPLEMENT_SINGLETON(FormulaManager)

FormulaManager::FormulaManager()
{

}

FormulaManager::~FormulaManager()
{

}

bool FormulaManager::Init()
{
#ifdef DEBUG
    // 测试公式参数传递
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("123456", 123);
    LuaCall<int32, int32, int32>(LuaEngine::Instance()->GetLuaContext(), "test1").call(1, 2);

    // 测试公式调用
    TestCalc(10, 20, 30);
#endif

    return true;
}

bool FormulaManager::GetFormulaParam(const char* param_name, real* val)
{
    return m_FormulaParameter.GetParam(param_name, val);
}

template<typename T> T FormulaManager::CallFormulaLua(uint32 formulaID)
{
    char funcName[100];
    SDSprintf(funcName, "f_%d", formulaID);
    T ret = LuaCall<T>(LuaEngine::Instance()->GetLuaContext(), funcName).call();
    return ret;
}

uint32 FormulaManager::TestCalc(int32 x, int32 y, int32 z)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("x", x);
    m_FormulaParameter.AddParam("y", y);
    m_FormulaParameter.AddParam("z", z);

    if(g_UseLuaFormula)
    {
        int32 ret = LuaCall<int32>(LuaEngine::Instance()->GetLuaContext(), "f_1").call();
        return ret;
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(1);
        if(!formulaTmpl)
        {
            CnWarn("formual TestCalc failed!\n");
            return 0;
        }

        uint32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
        return ret;
    }
}

// 计算二级属性
int32 FormulaManager::CalcPlayerSecondProp(uint32 formulaID, int32 strength, int32 agility, int32 constitution, int32 spirit)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("strength", strength);
    m_FormulaParameter.AddParam("agility", agility);
    m_FormulaParameter.AddParam("constitution", constitution);
    m_FormulaParameter.AddParam("spirit", spirit);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<int32>(formulaID);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(formulaID);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcPlayerSecondProp failed!\n");
            return 0;
        }

        return int32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
    }
}
// 48
int32 FormulaManager::CalFinishStarCDPrice( int32 minute )
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("minute", minute);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<int32>(FORMULA_ID_CALC_FINISH_STAR_CD_PRICE);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_FINISH_STAR_CD_PRICE);
        if(!formulaTmpl)
        {
            CnWarn("formual CalFinishStarCDPrice failed!\n");
            return 0;
        }

        uint32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
        return ret;
    }
}
// 49
uint32 FormulaManager::CalFollowpetPro(int32 base, int32 grow, int32 level)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("base", base);
    m_FormulaParameter.AddParam("grow", grow);
    m_FormulaParameter.AddParam("level", level);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<int32>(FORMULA_ID_FOLLOW_PRO);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_FOLLOW_PRO);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcWingExp failed!\n");
            return 0;
        }

        uint32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
        return ret;
    }
}
// 50
//uint32 FormulaManager::CalcWingExp( Player* player, uint32 wingLevel )
//{
//    m_FormulaParameter.Clear();
//    m_FormulaParameter.AddParam("wing_level", wingLevel);
//
//    if(g_UseLuaFormula)
//    {
//        return CallFormulaLua<int32>(FORMULA_ID_CALC_WING_EXP);
//    }
//    else
//    {
//        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_WING_EXP);
//        if(!formulaTmpl)
//        {
//            CnWarn("formual CalcWingExp failed!\n");
//            return 0;
//        }
//
//        uint32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
//        return ret;
//    }
//}
// 51
uint32 FormulaManager::CalcDungeonPoint( uint32 wasteTime, uint32 totalTime, uint32 maxCombo, uint32 monsterNum, uint32 comboNum )
{
    // monsterNum,totalTime在公式中作为分母,必须非0
    CnAssert(monsterNum != 0);
    CnAssert(totalTime != 0);

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("waste_time", wasteTime);
    m_FormulaParameter.AddParam("total_time", totalTime);
    m_FormulaParameter.AddParam("max_combo", maxCombo);
    m_FormulaParameter.AddParam("monster_num", monsterNum);
    m_FormulaParameter.AddParam("combo_num", comboNum);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<int32>(FORMULA_ID_CALC_DUNGEON_POINT);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_DUNGEON_POINT);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcDungeonPoint failed!\n");
            return 0;
        }

        uint32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
        return ret;
    }
}
// 60
//uint32 FormulaManager::CalcCitySurvivalRewardMoney( uint32 killNum )
//{
//    m_FormulaParameter.Clear();
//    m_FormulaParameter.AddParam("kill_num", killNum);
//
//    if(g_UseLuaFormula)
//    {
//        return CallFormulaLua<int32>(FORMULA_ID_CALC_CITY_SURVIVAL_REWARD_MONEY);
//    }
//    else
//    {
//        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_CITY_SURVIVAL_REWARD_MONEY);
//        if(!formulaTmpl)
//        {
//            CnWarn("formual CalcCitySurvivalRewardMoney failed!\n");
//            return 0;
//        }
//
//        uint32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
//        return ret;
//    }
//}
//// 61
//uint32 FormulaManager::CalcCitySurvivalRewardSoul( uint32 killNum )
//{
//    m_FormulaParameter.Clear();
//    m_FormulaParameter.AddParam("kill_num", killNum);
//    
//    if(g_UseLuaFormula)
//    {
//        return CallFormulaLua<int32>(FORMULA_ID_CALC_CITY_SURVIVAL_REWARD_SOUL);
//    }
//    else
//    {
//        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_CITY_SURVIVAL_REWARD_SOUL);
//        if(!formulaTmpl)
//        {
//            CnWarn("formual CalcCitySurvivalRewardMoney failed!\n");
//            return 0;
//        }
//
//        uint32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
//        return ret;
//    }
//}
// 70
int32 FormulaManager::CalcStoneTurnGold(int32 level)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("level", level);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<int32>(FORMULA_ID_CALC_STONE_TURN_GOLD);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_STONE_TURN_GOLD);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcStoneTurnGold failed!\n");
            return 0;
        }

        int32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
        return ret;
    }
}
// 71
int32 FormulaManager::CalcResetTurnTableCost(int32 times)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("times", times);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<int32>(FORMULA_ID_CALC_RESET_TURNTABLE_COST);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_RESET_TURNTABLE_COST);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcResetTurnTableCost failed!\n");
            return 0;
        }

        int32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
        return ret;
    }
}
// 72
int32 FormulaManager::CalcWorldBossRewardByDamage(int32 damage)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("damage", damage);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<int32>(FORMULA_ID_CALC_WORLD_BOSS_REWARD);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_WORLD_BOSS_REWARD);
        if(!formulaTmpl)
        {
            CnWarn("formual Calworld boss reward failed!\n");
            return 0;
        }

        int32 ret = int32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
        return ret;
    }
}
// 73
uint32 FormulaManager::CalcSwordStartExp(int32 level)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("sword_level", level);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<int32>(FORMULA_ID_CALC_SWORD_EXP);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_SWORD_EXP);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcSwordStartExp failed!\n");
            return 0;
        }

        uint32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
        return ret;
    }
}
// 101
real FormulaManager::CalcHitRatePro(int32 hit_rate, int32 dodge)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("hit_rate", hit_rate);
    m_FormulaParameter.AddParam("dodge", dodge);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<real>(FORMULA_ID_CALC_HIT_RATE);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_HIT_RATE);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcHitRatePercent failed!\n");
            return 0;
        }
        return FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
    }
}
// 103
real FormulaManager::CalcNormalAttack(int32 attack, real random, int32 defence, int32 level, int32 damage_add, int32 damage_sub)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("attack", attack);
    m_FormulaParameter.AddParam("random", random);
    m_FormulaParameter.AddParam("defence", defence);
    m_FormulaParameter.AddParam("level", level);
    m_FormulaParameter.AddParam("damage_add", damage_add);
    m_FormulaParameter.AddParam("damage_sub", damage_sub);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<real>(FORMULA_ID_CALC_NORMAL_ATTACK);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_NORMAL_ATTACK);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcNormalAttackPercent failed!\n");
            return 0;
        }
    
        return FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
    }
}
// 104
real FormulaManager::CalcSkillAttack(int32 attack, real random, int32 defence, int32 level, int32 skill_attack, int32 rune_attack, int32 rune_up, int32 rune_defence, int32 damage_add, int32 damage_sub)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("attack", attack);
    m_FormulaParameter.AddParam("random", random);
    m_FormulaParameter.AddParam("defence", defence);
    m_FormulaParameter.AddParam("level", level);
    m_FormulaParameter.AddParam("skill_attack", skill_attack);
    m_FormulaParameter.AddParam("rune_attack", rune_attack);
    m_FormulaParameter.AddParam("rune_up", rune_up);
    m_FormulaParameter.AddParam("rune_defence", rune_defence);
    m_FormulaParameter.AddParam("damage_add", damage_add);
    m_FormulaParameter.AddParam("damage_sub", damage_sub);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<real>(FORMULA_ID_CALC_SKILL_ATTACK);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_SKILL_ATTACK);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcSkillAttackPercent failed!\n");
            return 0;
        }

        return FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
    }
}
// 105
real FormulaManager::CalcCriAttPro(int32 ct_pro, int32 ct_pro_sub)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("ct_pro", ct_pro);
    m_FormulaParameter.AddParam("ct_pro_sub", ct_pro_sub);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<real>(FORMULA_ID_CALC_CRI_ATT_PRO);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_CRI_ATT_PRO);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcCriAttPro failed!\n");
            return 0;
        }

        return FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
    }
}
// 106
real FormulaManager::CalcCriAttDamage(int32 damage, int32 ct_damage, int32 ct_damage_sub)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("damage", damage);
    m_FormulaParameter.AddParam("ct_damage", ct_damage);
    m_FormulaParameter.AddParam("ct_damage_sub", ct_damage_sub);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<real>(FORMULA_ID_CALC_CRI_ATT_DAM);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_CRI_ATT_DAM);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcCriAttDamage failed!\n");
            return 0;
        }

        return FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
    }
}
// 107
//real FormulaManager::CalcCriVampirePro(int32 vampire_pro, int32 vampire_pro_sub)
//{
//    m_FormulaParameter.Clear();
//    m_FormulaParameter.AddParam("vampire_pro", vampire_pro);
//    m_FormulaParameter.AddParam("vampire_pro_sub", vampire_pro_sub);
//
//    if(g_UseLuaFormula)
//    {
//        return CallFormulaLua<real>(FORMULA_ID_CALC_VAMPIRE_PRO);
//    }
//    else
//    {
//        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_VAMPIRE_PRO);
//        if(!formulaTmpl)
//        {
//            CnWarn("formual CalcCriVampirePro failed!\n");
//            return 0;
//        }
//
//        return FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
//    }
//}
// 108
//real FormulaManager::CalcCriVampire(int32 vampire, int32 vampire_sub)
//{
//    m_FormulaParameter.Clear();
//    m_FormulaParameter.AddParam("vampire", vampire);
//    m_FormulaParameter.AddParam("vampire_sub", vampire_sub);
//
//    if(g_UseLuaFormula)
//    {
//        return CallFormulaLua<real>(FORMULA_ID_CALC_VAMPIRE_NUM);
//    }
//    else
//    {
//        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_VAMPIRE_NUM);
//        if(!formulaTmpl)
//        {
//            CnWarn("formual CalcCriVampire failed!\n");
//            return 0;
//        }
//
//        return FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
//    }
//}
// 109
//real FormulaManager::CalcBattlePower(BattlePowerProp& prop)
//{
//    m_FormulaParameter.Clear();
//
//    m_FormulaParameter.AddParam("hp", prop.hp);
//    m_FormulaParameter.AddParam("mp", prop.mp);
//    m_FormulaParameter.AddParam("attack", prop.attack);
//    m_FormulaParameter.AddParam("skill_attack", prop.skill_attack);
//    m_FormulaParameter.AddParam("defence", prop.defence);
//    m_FormulaParameter.AddParam("dodge", prop.dodge);
//    m_FormulaParameter.AddParam("hit_rate", prop.hit_rate);
//    m_FormulaParameter.AddParam("fire_defence", prop.fire_defence);
//    m_FormulaParameter.AddParam("ice_defence", prop.ice_defence);
//    m_FormulaParameter.AddParam("thunder_defence", prop.thunder_defence);
//    m_FormulaParameter.AddParam("ct_pro", prop.ct_pro);
//    m_FormulaParameter.AddParam("ct_pro_sub", prop.ct_pro_sub);
//    m_FormulaParameter.AddParam("ct_damage", prop.ct_damage);
//    m_FormulaParameter.AddParam("ct_damage_sub", prop.ct_damage_sub);
//
//    m_FormulaParameter.AddParam("level", prop.level);
//
//    m_FormulaParameter.AddParam("q_skill_level", prop.q_skill_level);
//    m_FormulaParameter.AddParam("w_skill_level", prop.w_skill_level);
//    m_FormulaParameter.AddParam("e_skill_level", prop.e_skill_level);
//
//    m_FormulaParameter.AddParam("fire_level", prop.q_rune_level);
//    m_FormulaParameter.AddParam("ice_level", prop.w_rune_level);
//    m_FormulaParameter.AddParam("thunder_level", prop.e_rune_level);
//
//    m_FormulaParameter.AddParam("res_hp", prop.res_hp);
//
//    // 跟宠
//    m_FormulaParameter.AddParam("pet_attack", prop.pet_attack);
//    m_FormulaParameter.AddParam("pet_hit_rate", prop.pet_hit_rate);
//    m_FormulaParameter.AddParam("pet_crit", prop.pet_crit);
//    m_FormulaParameter.AddParam("pet_crit_damage", prop.pet_crit_damage);
//    m_FormulaParameter.AddParam("pet_hit_add_physics", prop.pet_hit_add_physics);
//    m_FormulaParameter.AddParam("pet_hit_add_fire", prop.pet_hit_add_fire);
//    m_FormulaParameter.AddParam("pet_hit_add_ice", prop.pet_hit_add_ice);
//    m_FormulaParameter.AddParam("pet_hit_add_thunder", prop.pet_hit_add_thunder);
//
//    if(g_UseLuaFormula)
//    {
//        return CallFormulaLua<real>(FORMULA_ID_CALC_BATTLE_POWER);
//    }
//    else
//    {
//        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_BATTLE_POWER);
//        if(!formulaTmpl)
//        {
//            CnWarn("formual CalcBattlePower failed!\n");
//            return 0;
//        }
//        return FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
//    }
//}
// 111
//real FormulaManager::CalcPhysicsAttack(int32 baseDamage)
//{
//    const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_PHYSICS_ATTACK);
//    if(!formulaTmpl)
//    {
//        CnWarn("formual CalcAllAttack failed!\n");
//        return 0;
//    }
//
//    FormulaParameter param;
//    param.AddParam("base_damage", baseDamage);
//
//    return (FormulaCal::Instance()->CalResult(formulaTmpl, &param));
//}
//// 112
//real FormulaManager::CalcMagicAttack(int32 skillDamage)
//{
//    const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_MAGIC_ATTACK);
//    if(!formulaTmpl)
//    {
//        CnWarn("formual CalcAllAttack failed!\n");
//        return 0;
//    }
//
//    FormulaParameter param;
//    param.AddParam("skill_damage", skillDamage);
//
//    return (FormulaCal::Instance()->CalResult(formulaTmpl, &param));
//}

//------------------------公会BOSS----------------------------------------------
// 201
uint32 FormulaManager::CalcGuildBossHp(BattleUnit* battleUnit, int32 strong_level)
{
    int32 initHp = battleUnit->GetProp(PROP_MAXHP);

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_hp", initHp);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongHp = initHp;

    if(g_UseLuaFormula)
    {
        strongHp = CallFormulaLua<int32>(FORMULA_ID_GUILD_BOSS_HP);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_GUILD_BOSS_HP);
        if(!formulaTmpl)
        {
            CnWarn("formual cal guild boos hp failed!\n");
            return 0;
        }
    
        strongHp = (uint32)FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
    }

    strongHp = Crown::SDMin(strongHp, (uint32)MAX_HP_LEN);
    return strongHp;
}
// 202
uint32 FormulaManager::CalcGuildBossAttack(BattleUnit* battleUnit, int32 strong_level)
{
    int32 initAttack = battleUnit->GetProp(PROP_ATTACK);

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_attack", initAttack);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongAttack = initAttack;

    if(g_UseLuaFormula)
    {
        strongAttack = CallFormulaLua<int32>(FORMULA_ID_GUILD_BOSS_ATTACK);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_GUILD_BOSS_ATTACK);
        if(!formulaTmpl)
        {
            CnWarn("formual guildboss attack failed!\n");
            return 0;
        }

        strongAttack = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
    }

    strongAttack = Crown::SDMin(strongAttack, (uint32)MAX_HP_LEN);
    return strongAttack;
}
// 203
uint32 FormulaManager::CalGuildBossDefence(BattleUnit* battleUnit, int32 strong_level)
{
    int32 initDefence = battleUnit->GetProp(PROP_ARMOR);

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_deffense", initDefence);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongDefence = initDefence;

    if(g_UseLuaFormula)
    {
        strongDefence = CallFormulaLua<int32>(FORMULA_ID_GUILD_BOSS_DEFENCE);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_GUILD_BOSS_DEFENCE);
        if(!formulaTmpl)
        {
            CnWarn("formual cal guild boos defence failed!\n");
            return 0;
        }
        
        strongDefence = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
    }

    strongDefence = Crown::SDMin(strongDefence, (uint32)MAX_HP_LEN);
    return strongDefence;
}
//------------------------世界BOSS----------------------------------------------
// 204
uint32 FormulaManager::CalcWorldBossHp(BattleUnit* battleUnit, int32 strong_level )
{
    int32 initHp = battleUnit->GetProp(PROP_MAXHP);     // 必须使用GetPropertyValue

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_hp", initHp);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongHp = initHp;

    if(g_UseLuaFormula)
    {
        strongHp = CallFormulaLua<int32>(FORMULA_ID_WORLD_BOSS_HP);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_WORLD_BOSS_HP);
        if(!formulaTmpl)
        {
            CnWarn("formual cal world boos hp failed!\n");
            return 0;
        }

        strongHp = (uint32)FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
    }

    strongHp = Crown::SDMin(strongHp, (uint32)MAX_HP_LEN);
    return strongHp;
}
// 205
uint32 FormulaManager::CalcWorldBossAttack(BattleUnit* battleUnit, int32 strong_level )
{
    int32 initAttack = battleUnit->GetProp(PROP_ATTACK);

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_attack", initAttack);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongAttack = initAttack;

    if(g_UseLuaFormula)
    {
        strongAttack = CallFormulaLua<int32>(FORMULA_ID_WORLD_BOSS_ATTACK);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_WORLD_BOSS_ATTACK);
        if(!formulaTmpl)
        {
            CnWarn("formual cal world boos attack failed!\n");
            return 0;
        }
        
        strongAttack = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
    }

    strongAttack = Crown::SDMin(strongAttack, (uint32)MAX_HP_LEN);
    return strongAttack;
}
// 206
uint32 FormulaManager::CalcWorldBossDefence(BattleUnit* battleUnit, int32 strong_level)
{
    int32 initDefence = battleUnit->GetProp(PROP_ARMOR);

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_deffense", initDefence);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongDefence = initDefence;

    if(g_UseLuaFormula)
    {
        strongDefence = CallFormulaLua<int32>(FORMULA_ID_WORLD_BOSS_DEFENCE);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_WORLD_BOSS_DEFENCE);
        if(!formulaTmpl)
        {
            CnWarn("formual cal world boss defence failed!\n");
            return 0;
        }
        
        strongDefence = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
    }

    strongDefence = Crown::SDMin(strongDefence, (uint32)MAX_HP_LEN);
    return strongDefence;
}
// 207
real FormulaManager::CalPvPDamagePercentage(int32 average_battlepower, int32 difference_power)
{
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("aver_battle_power", average_battlepower);
    m_FormulaParameter.AddParam("difference_power", difference_power);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<real>(FORMULA_ID_PVP_CUT_DOWN_PER);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_PVP_CUT_DOWN_PER);
        if(!formulaTmpl)
        {
            CnWarn("formual Calworldboss failed!\n");
            return 0;
        }

        return FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
    }
}
//------------------------虚空跨服BOSS----------------------------------------------
// 208
uint32 FormulaManager::CalcVoidFieldBossHp(BattleUnit* battleUnit, int32 strong_level)
{
    int32 initHp = battleUnit->GetProp(PROP_MAXHP);     // 必须使用GetPropertyValue

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_hp", initHp);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongHp = initHp;

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<int32>(FORMULA_ID_VOID_FIELD_BOSS_HP);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_VOID_FIELD_BOSS_HP);
        if(!formulaTmpl)
        {
            CnWarn("formual cal void field boos hp failed!\n");
            return 0;
        }

        strongHp = (uint32)FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
    }

    strongHp = Crown::SDMin(strongHp, (uint32)MAX_HP_LEN);
    return strongHp;
}
// 209
uint32 FormulaManager::CalcVoidFieldBossAttack(BattleUnit* battleUnit, int32 strong_level)
{
    int32 initAttack = battleUnit->GetProp(PROP_ATTACK);

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_attack", initAttack);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongAttack = initAttack;

    if(g_UseLuaFormula)
    {
        strongAttack = CallFormulaLua<int32>(FORMULA_ID_VOID_FIELD_BOSS_ATTACK);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_VOID_FIELD_BOSS_ATTACK);
        if(!formulaTmpl)
        {
            CnWarn("formual cal void field boos attack failed!\n");
            return 0;
        }
        
        strongAttack = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
    }

    strongAttack = Crown::SDMin(strongAttack, (uint32)MAX_HP_LEN);
    return strongAttack;
}
// 210
uint32 FormulaManager::CalcVoidFieldBossDefence(BattleUnit* battleUnit, int32 strong_level)
{
    int32 initDefence = battleUnit->GetProp(PROP_ARMOR);

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_deffense", initDefence);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongDefence = initDefence;

    if(g_UseLuaFormula)
    {
        strongDefence = CallFormulaLua<int32>(FORMULA_ID_VOID_FIELD_BOSS_DEFENCE);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_VOID_FIELD_BOSS_DEFENCE);
        if(!formulaTmpl)
        {
            CnWarn("formual cal void field boos defence failed!\n");
            return 0;
        }

        strongDefence = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
    }
    strongDefence = Crown::SDMin(strongDefence, (uint32)MAX_HP_LEN);
    return strongDefence;
}

//------------------------虚空单服BOSS----------------------------------------------
// 211
int32 FormulaManager::CalcVoidFieldSoloBossHp(BattleUnit* battleUnit, int32 strong_level)
{
    int32 initHp = battleUnit->GetProp(PROP_MAXHP);     // 必须使用GetPropertyValue

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_hp", initHp);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongHp = initHp;

    if(g_UseLuaFormula)
    {
        strongHp = CallFormulaLua<int32>(FORMULA_ID_VOID_FIELD_SOLO_BOSS_HP);
    }
    else
    {
    
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_VOID_FIELD_SOLO_BOSS_HP);
        if(!formulaTmpl)
        {
            CnWarn("formual cal void field solo boos hp failed!\n");
            return 0;
        }

        strongHp = (uint32)FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter);
    }

    strongHp = Crown::SDMin(strongHp, (uint32)MAX_HP_LEN);
    return strongHp;
}
// 212
int32 FormulaManager::CalcVoidFieldSoloBossAttack(BattleUnit* battleUnit, int32 strong_level)
{
    int32 initAttack = battleUnit->GetProp(PROP_ATTACK);

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_attack", initAttack);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongAttack = initAttack;

    if(g_UseLuaFormula)
    {
        strongAttack = CallFormulaLua<int32>(FORMULA_ID_VOID_FIELD_SOLO_BOSS_ATTACK);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_VOID_FIELD_SOLO_BOSS_ATTACK);
        if(!formulaTmpl)
        {
            CnWarn("formual cal void field solo boos attack failed!\n");
            return 0;
        }
    
        strongAttack = int32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
    }

    strongAttack = Crown::SDMin(strongAttack, (uint32)MAX_HP_LEN);
    return strongAttack;
}
// 213
int32 FormulaManager::CalcVoidFieldSoloBossDefence(BattleUnit* battleUnit, int32 strong_level)
{
    int32 initDefence = battleUnit->GetProp(PROP_ARMOR);

    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("boss_deffense", initDefence);
    m_FormulaParameter.AddParam("leveup", strong_level);

    uint32 strongDefence = initDefence;

    if(g_UseLuaFormula)
    {
        strongDefence = CallFormulaLua<int32>(FORMULA_ID_VOID_FIELD_SOLO_BOSS_DEFENCE);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_VOID_FIELD_SOLO_BOSS_DEFENCE);
        if(!formulaTmpl)
        {
            CnWarn("formual cal void field solo boos defence failed!\n");
            return 0;
        }
        
        strongDefence = int32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
    }
    strongDefence = Crown::SDMin(strongDefence, (uint32)MAX_HP_LEN);
    return strongDefence;
}

uint32 FormulaManager::CalcRevivalTime( int32 time )
{
    if(!ConfigManager::Instance()->IsTW())
    {
        return time;
    }
    m_FormulaParameter.Clear();
    m_FormulaParameter.AddParam("time", time);

    if(g_UseLuaFormula)
    {
        return CallFormulaLua<int32>(FORMULA_ID_CALC_REVIVAL_TIME);
    }
    else
    {
        const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_REVIVAL_TIME);
        if(!formulaTmpl)
        {
            CnWarn("formual CalcRevivalTime failed!\n");
            return 0;
        }

        uint32 ret = uint32( FormulaCal::Instance()->CalResult(formulaTmpl, &m_FormulaParameter) );
        return ret;
    }
}



//uint32 FormulaManager::CalWingStartExp(int32 level)
//{
//    real exp = level* 5.0f;
//    return (int32)exp;
//}

//real FormulaManager::CalcCriticalAttackReductionPercent(int32 CriticalAttackReduction)
//{
//    const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_CRITICAL_ATTACK_REDUCTION_PERCENT);
//    if(!formulaTmpl)
//    {
//        CnWarn("formual CalcCriticalAttackReductionPercent failed!\n");
//        return 0;
//    }
//
//    FormulaParameter param;
//    param.AddParam("critical_attack_reduction", CriticalAttackReduction);
//
//    return FormulaCal::Instance()->CalResult(formulaTmpl, &param);
//}
//
//real FormulaManager::CalcVampirePercent(int32 vampire)
//{
//    const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_VAMPIRE);
//    if(!formulaTmpl)
//    {
//        CnWarn("formual CalcVampire failed!\n");
//        return 0;
//    }
//
//    FormulaParameter param;
//    param.AddParam("vampire", vampire);
//
//    return FormulaCal::Instance()->CalResult(formulaTmpl, &param);
//}
//
//real FormulaManager::CalcVampireDefencePercent(int32 vampireDefence)
//{
//    const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_VAMPIRE_DEFENCE);
//    if(!formulaTmpl)
//    {
//        CnWarn("formual CalcVampireDefence failed!\n");
//        return 0;
//    }
//
//    FormulaParameter param;
//    param.AddParam("vampire_defence", vampireDefence);
//
//    return FormulaCal::Instance()->CalResult(formulaTmpl, &param);
//}
//
//real FormulaManager::CalcControlReductionPercent(int32 control_reduction)
//{
//    const FormulaTemplate* formulaTmpl = FormulaConfig::Instance()->GetFormulaTemplate(FORMULA_ID_CALC_CONTROL_REDUCTION);
//    if(!formulaTmpl)
//    {
//        CnWarn("formual CalcControlReduction failed!\n");
//        return 0;
//    }
//
//    FormulaParameter param;
//    param.AddParam("control_reduction", control_reduction);
//
//    return FormulaCal::Instance()->CalResult(formulaTmpl, &param);
//}





