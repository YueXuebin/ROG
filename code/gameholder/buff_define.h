/*
 *	buff的相关定义
 */
#ifndef buff_define_h__
#define buff_define_h__

// buff类型
enum BuffType
{
    BUFF_TYPE_GOOD = 1,         // 增益类
    BUFF_TYPE_BAD,              // 减益类
    BUFF_TYPE_AURA,             // 光环产生的效果buff
    BUFF_TYPE_NORMAL            // 通用类(药水buff,双倍经验等)
};

// buff叠加属性
enum BuffOverlap
{
    BUFF_CAN_UPDATE = 1,            // 可更新
    BUFF_CAN_OVERLAP,               // 可叠加
    BUFF_NO_OVERLAP,                // 不可叠加

};

// buff功能定义
enum BuffFuncType
{
    BUFF_FUNC_STUN = 1,             // 眩晕
    BUFF_FUNC_PROP_SCALE,           // 人物属性按比例增减
    BUFF_FUNC_PROP_CHANGE,          // 人物属性按值增减
    BUFF_FUNC_HEAL,                 // 回复生命值
    BUFF_FUNC_IMMORTAL,             // 免疫物理伤害
    BUFF_FUNC_SHOCK,                // 感电状态
    BUFF_FUNC_DAMAGE,               // 伤害
    BUFF_FUNC_BLEED,                // 流血
    

    BUFF_FUNC_EXP_MUL,              // 获得经验的倍率
    BUFF_FUNC_DROP_QUALITY,         // 物品品质
};


// buff通知定义
const int32     BUFF_ADD = 1;
const int32     BUFF_UPDATE = 2;
const int32     BUFF_DEL = 3;

const int32     BUFF_UPDATE_TIME = 500;        // buff效果每n毫秒作用一次


// 常用buffID
const uint32    BUFF_ID_STUN = 2001;            // 击晕
const uint32    BUFF_ID_IGNITE = 2007;          // 点燃
const uint32    BUFF_ID_CHILL = 2008;           // 冰缓
const uint32    BUFF_ID_FREEZE = 2009;          // 冰冻
const uint32    BUFF_ID_SHOCK = 2010;           // 感电
const uint32    BUFF_ID_BLEED = 2011;           // 流血
const uint32    BUFF_ID_ENDURANCE_CHARGES = 2012;   // 耐力球
const uint32    BUFF_ID_FRENZY_CHARGES = 2013;      // 狂怒球
const uint32    BUFF_ID_POWER_CHARGES = 2014;       // 暴击球


#endif // buff_define_h__
