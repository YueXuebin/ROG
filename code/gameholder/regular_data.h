/*
 *	关卡用数据
 */
#ifndef regular_data_h__
#define regular_data_h__

#include "path_data.h"
#include "reincarnated_data.h"
#include "state_define.h"


VECTOR3D_INT ParseVector3DInt(const std::string& str);

// 基础属性
class AttributeData
{
public:
    int32 attack;
    int32 hp;
    int32 hpRecovery;
    int32 mp;
    int32 mpRecovery;
    int32 armor;
    int32 moveSpeed;
    int32 attackSpeed;
    int32 castSpeed;
    int32 energyShield;
    int32 hit;
    int32 dodge;
    int32 crit;
    int32 critDamage;
    int32 fireResistance;
    int32 frostResistance;
    int32 lightningResistance;
    int32 attackBlock;
    int32 magicBlock;

    int32 radius;       // 碰撞半径

    AttributeData()
    {
        ClearAttributeData();
    }

    void ClearAttributeData();
};

// 创建怪物用数据
class MonsterData : public AttributeData
{
public:
    //配置数据
    uint32 creatureId;              // 生物Id
    std::string name;
    uint32 creatureType;           // npc还是monster
    int32 x;
    int32 y;
    int32 angle;
    uint32 state;               // 初始状态
    int32 state_arg;            // 初始状态参数
    bool attackable;            // 能被攻击
    bool cantMove;              // 不会移动
    bool cantAngle;             // 不会转向
    bool cantBuff;              // 不受buff
    bool cantHeal;              // 不受加血 (不受技能等加血, 但可以自动回血)
    bool cantTaunt;             // 不受嘲讽
    bool cantPush;              // 不被推动
    int32 group;                // 所在组
    uint32 level;               // 怪物等级
    uint32 quality;             // 怪物品质
    uint32 showflag;            // 显示标记
    // AI
    PathData pathdata;          // 路径数据
    bool pathloop;              // 路径是否循环
    std::string ai;
    uint32  ai_group;           // AI组
    // 演绎
    std::string movie_name;         // 演绎名
    bool    movie_attackable;       // 演绎中是否可以被攻击
    //
    int32 num;                      // 怪物数量(出怪点用)
    bool isAppearNotifyAll;     // 出现/隐藏是否全场景通知(此值不要动态修改)



    MonsterData() : 
    creatureId(0),
    creatureType(0),
    x(0),
    y(0),
    angle(0),
    state(STATE_IDLE),
    state_arg(0),
    cantMove(false),
    cantAngle(false),
    cantBuff(false),
    cantHeal(false),
    cantTaunt(false),
    cantPush(false),
    attackable(true),
    showflag(0),
    pathloop(false),
    group(0),
    level(1),
    quality(QUALITY_WHITE),
    num(1),
    isAppearNotifyAll(false),
    ai_group(0),
    movie_attackable(true)
    {
    }

    bool valid() const;
    void LoadTmplData();
};

MonsterData ParseMonsterAndNpcData(const TiXmlElement* PawnElement);


// 创建Boss用数据
class BossData
{
public:
    //配置数据
    uint32 creatureId;
    int32 x;
    int32 y;
    int32 angle;
    bool attackable;
    bool pathloop;               // 是否路径循环
    bool bossui;
    bool worldboss;              // 是否是世界boss
    int32 group;
    bool cantMove;               // 不会移动
    bool cantAngle;              // 不会转向
    bool cantBuff;               // 不受buff
    bool cantHeal;               // 不受加血 (自受技能等加血, 但可以自动回血)
    bool cantTaunt;              // 不受嘲讽
    bool cantPush;               // 不被推动
    PathData pathdata;
    bool b_corpse;               //是否是鞭尸的状态
    uint32 lifetime;             //boss 的生存时间
    std::string ai;
    std::string name;
    std::string linkedTriggerName;
    ReincarnatedList reincarnate_list;
    bool autoLoadTmpl;
    //模板数据
    uint32 level;
    int32 hp;
    int32 hpRate;
    int32 speed;
    int32 attack;
    int32 hitRate;
    int32 dodge;
    int32 criticalAttack;
    int32 criticalRed;
    int32 criticalDamageAddition;
    int32 criticalDamageRed;
    int32 defence;
    int32 fireDefence;
    int32 iceDefence;
    int32 thunderDefence;
    int32 attackRate;
    int32 radius;
    int32 exp;
    int32 soul;
    int32 gamePoint;

    BossData() : 
    creatureId(0),
    x(0),
    y(0),
    angle(0),
    cantMove(false),
    cantAngle(false),
    cantBuff(false),
    cantPush(false),
    cantHeal(false),
    cantTaunt(false),
    attackable(true),
    pathloop(false),
    bossui(false),
    b_corpse(false),
    lifetime(0),
    group(0),
    autoLoadTmpl(true)
    {
        ClearTmplData();
    }

    bool valid() const;
    void LoadTmplData();
    void ClearTmplData();
};

BossData ParseBossData(const TiXmlElement* BossElement);
void ParseReincarnated(const TiXmlElement* BossElement, ReincarnatedList& list);

// 创建Trigger用数据
class TriggerData
{
public:
    std::string name;
    uint32  triggerId;
    std::string obs_name;   // 物件集名
    VECTOR3D_INT obs_pos;   // 物件集位置
    VECTOR3D_INT pick_size; // 拾取范围
    VECTOR3D_INT pick_pos;  // 拾取范围位置
    uint32  showId;         // 外形配置ID(trigger表中的外形ID)
    int32   x;
    int32   y;
    int32   angle;
    int32   radius;         // 触发半径
    bool    isTouch;        // 是否触摸触发
    int32   funcId;         // 功能ID
    int32   funcType;       // 功能类型(cc/cs)
    std::string funcArgs;   // 功能参数
    
    
    TriggerData() :
    name(""),
    triggerId(0),
    showId(0),
    x(0),
    y(0),
    angle(0),
    radius(0),
    isTouch(false),
    funcId(0),
    funcType(TRIGGER_TYPE_CLIENT_SERVER),
    funcArgs("")
    {}
};

TriggerData ParseTriggerData(const TiXmlElement* TriggerElement);

// 创建CheckPoint点用数据
class CheckPointData
{
public:
    std::string name;
    uint32 pinID;
    int32 x;
    int32 y;
    int32 angle;
    int32 radius;

    CheckPointData() :
    name(""),
    pinID(0),
    x(0),
    y(0),
    angle(0),
    radius(0)
    {}
};

CheckPointData ParseCheckPointData(const TiXmlElement* PinElement);


// 怪物出生点的波数据
class SpawnWaveData
{
public:
    int32   delay;
    std::vector<MonsterData>    MonsterDatas;

    SpawnWaveData() :
    delay(0)
    {
    }
};
// 怪物出身点的出怪控制器
class SpawnWaveControlData
{
public:
    uint32  type;
    const TiXmlElement* waveControlXML;

    SpawnWaveControlData() :
    type(SPAWN_CONTROL_NONE),
    waveControlXML(NULL)
    {
    }
};

// 创建怪物出生点用数据
class SpawnData
{
public:
    typedef std::vector<SpawnWaveData> WaveDataList;
    typedef std::vector<SpawnWaveControlData> WaveControlDataList;

    std::string name;
    uint32 spawnId;
    int32 x;
    int32 y;
    int32 angle;
    int32 radius;
    int32 group;      // 所在组(影响其中出生的怪)
    bool loop;       // 是否循环刷怪
    bool no_collison_object;     // 出生的怪是否有碰撞
    bool no_start;   // 默认不开启
    uint32 triggerId;  // 关联的trigger id
    int32 spawnType;  // 出怪类型
    // 刷怪参数
    int32 spawnMax;           // 按数量刷
    int32 spawnPeriod;        // 按周期刷
    int32 spawnCount;
    int32 spawnSecond;
    // 波参数
    WaveDataList WaveDatas;
    // 波控制器
    WaveControlDataList WaveDataControls;
    
    SpawnData() :
    name(""),
    spawnId(0),
    x(0),
    y(0),
    angle(0),
    radius(0),
    spawnType(0),
    group(0),
    loop(false),
    no_collison_object(false),
    no_start(false),
    triggerId(0),
    spawnMax(0),
    spawnPeriod(0),
    spawnCount(0),
    spawnSecond(0)
    {}
};

SpawnData ParseSpawnData(const TiXmlElement* spawnElement);
SpawnWaveData ParseSpawnWaveData(const TiXmlElement* waveElement);
SpawnWaveControlData ParseSpawnWaveControlData(const TiXmlElement* waveElement);

// 创建采集点用数据
class TouchData
{
public:
    std::string name;
    uint32 touchId;
    std::string obs_name;   // 物件集名
    VECTOR3D_INT obs_pos;   // 物件集位置
    VECTOR3D_INT pick_size; // 拾取范围
    VECTOR3D_INT pick_pos;  // 拾取范围位置
    uint32  showId;         // 外形配置ID(trigger表中的外形ID)
    int32 x;
    int32 y;
    int32 angle;
    int32 radius;
    int32 time;       // 采集时间

    TouchData()
    {
        Clear();
    }

    void Clear()
    {
        name = "";
        touchId = 0;
        obs_name = "";
        showId = 0;
        x = 0;
        y = 0;
        angle = 0;
        radius = 0;
        time = 0;
    }
};

TouchData ParseTouchData(const TiXmlElement* patchElement);

// 关卡的结构化数据
class RegularDoc
{
public:
    const CheckPointData* GetPinData(const std::string &name) const;
    const TriggerData* GetTriggerData(const std::string &name) const;
    const SpawnData* GetSpawnData(const std::string &name) const;
    const TouchData* GetTouchData(const std::string &name) const;

    RegularDoc();
    ~RegularDoc();

    void ClearData()
    {
        m_monsterList.clear();
        m_npcList.clear();
        m_bossList.clear();

        m_pinMap.clear();
        m_triggerMap.clear();
        m_spawnMap.clear();
        m_touchMap.clear();

        m_fileName.clear();
        m_ReadyTime = 0;
        m_RunTime = 0;
        m_EndTime = 0;
        m_type = 0;
    }

    std::string m_fileName;
    TiXmlDocument* m_pDoc;         // 只留给规则逻辑用
    int32 m_type;         // (玩法库类型)

    int32 m_ReadyTime;
    int32 m_RunTime;
    int32 m_EndTime;

    typedef std::map<std::string, CheckPointData> CheckPointDataMap;
    typedef std::map<std::string, TriggerData> TriggerDataMap;
    typedef std::map<std::string, SpawnData> SpawnDataMap;
    typedef std::map<std::string, TouchData> TouchDataMap;

    std::vector<MonsterData> m_monsterList;      // 所有怪物
    std::vector<MonsterData> m_npcList;          // 所有npc
    std::vector<BossData> m_bossList;           // 所有boss

    CheckPointDataMap m_pinMap;
    TriggerDataMap m_triggerMap;
    SpawnDataMap m_spawnMap;
    TouchDataMap m_touchMap;
};

struct RandomDropData
{
    int16   firstType;      // 装备、石头、材料、消耗品
    int16   secondType;     // 那个部件的装备--那种石头
    int16   quality;        // 品质

    void Clear()
    {
        firstType = -1;
        secondType = -1;
        quality = -1;
    }
};

#endif // regular_data_h__
