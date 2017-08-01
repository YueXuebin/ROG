#include "gameholder_pch.h"
#include "regular_data.h"
#include "regular_manager.h"
#include "monster_config_manager.h"


VECTOR3D_INT ParseVector3DInt(const std::string& str)
{
    VECTOR3D_INT vector3D;
    std::vector<std::string> vpos = Crown::SDSplitStrings(str, ' ');
    vector3D.x = atoi(vpos[0].c_str());
    vector3D.y = atoi(vpos[1].c_str());
    vector3D.z = atoi(vpos[2].c_str());
    return vector3D;
}

void AttributeData::ClearAttributeData()
{
    attack = 0;
    hp = 0;
    hpRecovery = 0;
    mp = 0;
    mpRecovery = 0;
    armor = 0;
    moveSpeed = 0;
    attackSpeed = 0;
    castSpeed = 0;
    energyShield = 0;
    hit = 0;
    dodge = 0;
    crit = 0;
    critDamage = 0;
    fireResistance = 0;
    frostResistance = 0;
    lightningResistance = 0;
    attackBlock = 0;
    magicBlock = 0;

    radius = PHYSICAL_RADIUS;       // 默认碰撞半径
}

bool MonsterData::valid() const
{
    return MonsterConfigManager::Instance()->GetMonsterAttributeReviseById(creatureId) != NULL;
}

void MonsterData::LoadTmplData()
{
    // 获取标准怪物属性
    const MONSTER_ATTRIBUTE_NORM_ROOT_TEMPLATE_STRUCT * pNorm = MonsterConfigManager::Instance()->GetMonsterAttributeNormByLevel(level);
    if (!pNorm)
    {
        CnWarn("MonsterData::LoadTmplData MONSTER_ATTRIBUTE_NORM level=%d not exist!\n", level);
        return;
    }
    // 获取怪物修正
    const MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT * pRevise = MonsterConfigManager::Instance()->GetMonsterAttributeReviseById(creatureId);
    if (!pRevise)
    {
        CnWarn("MonsterData::LoadTmplData MONSTER_ATTRIBUTE_REVISE id=%d not exist!\n", creatureId);
        return;
    }

    // 计算怪物数值
    attack = pNorm->attack * pRevise->attack_rate / BASE_RATE_NUM + pRevise->attack;
    hp = pNorm->hp * pRevise->hp_rate / BASE_RATE_NUM + pRevise->hp;
    hpRecovery = pNorm->hp_recovery * pRevise->hp_recovery_rate / BASE_RATE_NUM + pRevise->hp_recovery;
    mp = pNorm->mp * pRevise->mp_rate / BASE_RATE_NUM + pRevise->mp;
    mpRecovery = pNorm->mp_recovery * pRevise->mp_recovery_rate / BASE_RATE_NUM + pRevise->mp_recovery;
    armor = pNorm->armor * pRevise->armor_rate / BASE_RATE_NUM + pRevise->armor;
    moveSpeed = pNorm->move_speed * pRevise->move_speed_rate / BASE_RATE_NUM + pRevise->move_speed;
    attackSpeed = pNorm->attack_speed * pRevise->attack_speed_rate / BASE_RATE_NUM + pRevise->attack_speed;
    castSpeed = pNorm->cast_speed * pRevise->cast_speed_rate / BASE_RATE_NUM + pRevise->cast_speed;
    energyShield = pNorm->energy_shield * pRevise->energy_shield_rate / BASE_RATE_NUM + pRevise->energy_shield;
    hit = pNorm->hit * pRevise->hit_rate / BASE_RATE_NUM + pRevise->hit;
    dodge = pNorm->dodge * pRevise->dodge_rate / BASE_RATE_NUM + pRevise->dodge;
    crit = pNorm->crit * pRevise->crit_rate / BASE_RATE_NUM + pRevise->crit;
    critDamage = pNorm->crit_damage * pRevise->crit_damage_rate / BASE_RATE_NUM + pRevise->crit_damage;
    fireResistance = pNorm->fire_resistance * pRevise->fire_resistance_rate / BASE_RATE_NUM + pRevise->fire_resistance;
    frostResistance = pNorm->frost_resistance * pRevise->frost_resistance_rate / BASE_RATE_NUM + pRevise->frost_resistance;
    lightningResistance = pNorm->lightning_resistance * pRevise->lightning_resistance_rate / BASE_RATE_NUM + pRevise->lightning_resistance;
    attackBlock = pNorm->attack_block * pRevise->attack_block_rate / BASE_RATE_NUM + pRevise->attack_block;
    magicBlock = pNorm->magic_block * pRevise->magic_block_rate / BASE_RATE_NUM + pRevise->magic_block;

    
    ai = pRevise->ai;               // 怪物AI
    radius = pRevise->radius;       // 怪物半径
}

MonsterData ParseMonsterAndNpcData(const TiXmlElement* pawnElement)
{
    MonsterData data;

    data.creatureId = 0;
    pawnElement->QueryUnsignedAttribute("sid", &data.creatureId);

    data.name = pawnElement->Attribute("name");

    int32 v;
    pawnElement->QueryIntAttribute("v", &v);

    data.creatureType = 0;
    pawnElement->QueryUnsignedAttribute("type", &data.creatureType);

    const char* posChar = pawnElement->Attribute("pos");
    if(posChar)
    {
        std::string pos = posChar;
        std::vector<std::string> vpos;
        vpos = Crown::SDSplitStrings(pos, ' ');

        data.x = atoi(vpos[0].c_str());
        data.y = atoi(vpos[1].c_str());
    }
    else
    {
        data.x = 0;
        data.y = 0;
    }

    data.angle = 0;
    pawnElement->QueryIntAttribute("angle", &data.angle);

    int32 cantmove = 0;
    pawnElement->QueryIntAttribute("cantmove", &cantmove);
    data.cantMove = cantmove;

    int32 cantangle = 0;
    pawnElement->QueryIntAttribute("cantangle", &cantangle);
    data.cantAngle = cantangle;

    int32 cantbuff = 0;
    pawnElement->QueryIntAttribute("cantbuff", &cantbuff);
    data.cantBuff = cantbuff;

    int32 cantheal = 0;
    pawnElement->QueryIntAttribute("cantheal", &cantheal);
    data.cantHeal = cantheal;

    int32 canttount = 0;
    pawnElement->QueryIntAttribute("canttount", &canttount);
    data.cantTaunt = canttount;

    int32 cantpush = 0;
    pawnElement->QueryIntAttribute("cantpush", &cantpush);
    data.cantPush = cantpush;

    int32 attackable = -1;
    pawnElement->QueryIntAttribute("attackable", &attackable);
    data.attackable = attackable;

    // 兼容上一版本
    if(attackable == -1)
    {
        if(data.creatureType == 1)
            data.attackable = true;         // 怪一定能被攻击
        else
            data.attackable = false;        // npc一定不能被攻击
    }
    else
        data.attackable = attackable;

    int32 group = 0;
    pawnElement->QueryIntAttribute("group", &group);
    data.group = group;

    // 是否为随机ID怪
    int32 isrand = 0;
    pawnElement->QueryIntAttribute("isrand", &isrand);

    int32 minlvl = 0;
    pawnElement->QueryIntAttribute("minlvl", &minlvl);

    int32 maxlvl = 0;
    pawnElement->QueryIntAttribute("maxlvl", &maxlvl);

    // 解析路径
    const char* pathString = pawnElement->Attribute("path");
    data.pathdata = ParsePath(pathString);

    int32 pathloop = 0;
    pawnElement->QueryIntAttribute("pathloop", &pathloop);
    data.pathloop = pathloop;

    // 解析AI
    const char* aiString = pawnElement->Attribute("ai");
    if(aiString && Crown::SDStrlen(aiString) > 0)
        data.ai = aiString;

    return data;
}

bool BossData::valid() const
{
    //return BossConfig::Instance()->GetBossTemplate(creatureId) != NULL;
    return NULL;
}

void BossData::LoadTmplData()
{
    //const BossTemplate *tmpl = BossConfig::Instance()->GetBossTemplate(creatureId);
    //if (!tmpl)
    //{
    //    CnWarn("BossData::LoadTmplData %d not exist\n", creatureId);
    //    ClearTmplData();
    //    return;
    //}
    //level = tmpl->level;
    //hp = tmpl->hp;
    //hpRate = tmpl->hp_rate;
    //speed = tmpl->speed;
    //attack = tmpl->attack;
    //hitRate = tmpl->hit_rate;
    //dodge = tmpl->dodge;
    //criticalAttack = tmpl->critical_attack;
    //criticalRed = tmpl->critical_red;
    //criticalDamageAddition = tmpl->critical_damage_addition;
    //criticalDamageRed = tmpl->critical_damage_red;
    //defence = tmpl->defence;
    //fireDefence = tmpl->fire_defence;
    //iceDefence = tmpl->ice_defence;
    //thunderDefence = tmpl->thunder_defence;
    //attackRate = tmpl->attack_rate;
    //radius = tmpl->radius;
    //exp = tmpl->exp;
    //soul = tmpl->soul;
    //gamePoint = tmpl->game_point;
}

void BossData::ClearTmplData()
{
    level = 0;
    hp = 0;
    hpRate = 0;
    speed = 0;
    attack = 0;
    hitRate = 0;
    dodge = 0;
    criticalAttack = 0;
    criticalRed = 0;
    criticalDamageAddition = 0;
    criticalDamageRed = 0;
    defence = 0;
    fireDefence = 0;
    iceDefence = 0;
    thunderDefence = 0;
    attackRate = 0;
    radius = 0;
    exp = 0;
    soul = 0;
    gamePoint = 0;
}

BossData ParseBossData(const TiXmlElement* BossElement)
{
    BossData data;

    BossElement->QueryUnsignedAttribute("sid", &data.creatureId);

    data.name = BossElement->Attribute("name");

    const char* posChar = BossElement->Attribute("pos");
    if(posChar)
    {
        std::string pos = posChar;
        std::vector<std::string> vpos;
        vpos = Crown::SDSplitStrings(pos, ' ');
    
        data.x = atoi(vpos[0].c_str());
        data.y = atoi(vpos[1].c_str());
    }
    else
    {
        data.x = 0;
        data.y = 0;
    }

    data.angle = 0;
    BossElement->QueryIntAttribute("angle", &data.angle);

    data.group = 0;
    BossElement->QueryIntAttribute("group", &data.group);

    int32 cantmove = 0;
    BossElement->QueryIntAttribute("cantmove", &cantmove);
    data.cantMove = cantmove;

    int32 cantangle = 0;
    BossElement->QueryIntAttribute("cantangle", &cantangle);
    data.cantAngle = cantangle;

    int32 cantbuff = 0;
    BossElement->QueryIntAttribute("cantbuff", &cantbuff);
    data.cantBuff = cantbuff;

    int32 cantheal = 0;
    BossElement->QueryIntAttribute("cantheal", &cantheal);
    data.cantHeal = cantheal;

    int32 canttaunt = 0;
    BossElement->QueryIntAttribute("canttaunt", &canttaunt);
    data.cantTaunt = canttaunt;

    int32 cantpush = 0;
    BossElement->QueryIntAttribute("cantpush", &cantpush);
    data.cantPush = cantpush;

    int32 attackable = 1;
    BossElement->QueryIntAttribute("attackable", &attackable);
    data.attackable = attackable == 1;

    int32 bossui = 0;
    BossElement->QueryIntAttribute("bossui", &bossui);
    data.bossui = bossui;

    int32 worldboss = 0;
    BossElement->QueryIntAttribute("worldboss", &worldboss);
    data.worldboss = worldboss;

    // 解析路径
    const char* pathString = BossElement->Attribute("path");
    data.pathdata = ParsePath(pathString);

    int32 pathloop = 0;
    BossElement->QueryIntAttribute("pathloop", &pathloop);
    data.pathloop = pathloop;

    // 解析AI
    const char* aiString = BossElement->Attribute("ai");
    if(aiString && Crown::SDStrlen(aiString) > 0)
        data.ai = aiString;

    // 解析boss的转生链
    ParseReincarnated(BossElement, data.reincarnate_list);

    return data;
}

void ParseReincarnated(const TiXmlElement* BossElement, ReincarnatedList& list)
{
    list.Clear();

    if(!BossElement->FirstChildElement())
        return;

    const TiXmlElement* ReincarnatedElement = BossElement->FirstChildElement()->ToElement();
    for(; ReincarnatedElement; ReincarnatedElement = ReincarnatedElement->NextSiblingElement())
    {
        std::string nodeValue = ReincarnatedElement->Value();
        if (nodeValue == "reincarnated")
        {
            uint32 bossid = 0;
            ReincarnatedElement->QueryUnsignedAttribute("bossid", &bossid);
            int32 num = 0;
            ReincarnatedElement->QueryIntAttribute("num", &num);

            const char* aiString = ReincarnatedElement->Attribute("ai");

            if(bossid != 0 && num != 0)
            {
                ReincarnatedData data;
                data.m_BossId = bossid;
                data.m_Num = num;

                if(aiString && Crown::SDStrlen(aiString) > 0)
                    data.m_AIName = aiString;

                list.Add(data);
            }
        }
    }
}
// 解析触发器数据
TriggerData ParseTriggerData(const TiXmlElement* TriggerElement)
{
    TriggerData data;

    // 名字
    data.name = TriggerElement->Attribute("name");

    // trigger ID
    data.triggerId = 0;
    TriggerElement->QueryUnsignedAttribute("tid", &data.triggerId);

    // 位置
    std::string triggerPos = TriggerElement->Attribute("pos");
    VECTOR3D_INT vector3D = ParseVector3DInt(triggerPos);
    data.x = vector3D.x;
    data.y = vector3D.z;

    // 物件集名
    if(TriggerElement->Attribute("obs"))
    {
        data.obs_name = TriggerElement->Attribute("obs");
    }

    // 物件集位置
    if(TriggerElement->Attribute("obs_pos"))
    {
        std::string ObsPos = TriggerElement->Attribute("obs_pos");
        data.obs_pos = ParseVector3DInt(ObsPos);
    }

    // 拾取范围
    if(TriggerElement->Attribute("picksize"))
    {
        std::string pickSize = TriggerElement->Attribute("picksize");
        data.pick_size = ParseVector3DInt(pickSize);
    }

    // 拾取范围位置
    if(TriggerElement->Attribute("pickpos"))
    {
        std::string pickPos = TriggerElement->Attribute("pickpos");
        data.pick_pos = ParseVector3DInt(pickPos);
    }

    // 方向
    data.angle = 0;
    TriggerElement->QueryIntAttribute("angle", &data.angle);

    // 触发半径
    data.radius = 0;
    TriggerElement->QueryIntAttribute("r", &data.radius);

    // 触发次数
    int32 triggerCount;
    TriggerElement->QueryIntAttribute("c", &triggerCount);

    // 类型(cc\cs)
    data.funcType = 1;
    TriggerElement->QueryIntAttribute("type", &data.funcType);

    // 是否广播
    int32 triggerBroadcast;
    TriggerElement->QueryIntAttribute("b", &triggerBroadcast);

    // 是否触摸触发
    int32 touch = 0;
    TriggerElement->QueryIntAttribute("touch", &touch);
    data.isTouch = (touch == 1);

    // 功能ID
    data.funcId = 0;
    TriggerElement->QueryIntAttribute("fun", &data.funcId);

    // 功能参数
    if(TriggerElement->Attribute("args"))
    {
        data.funcArgs = TriggerElement->Attribute("args");
    }

    return data;
}

CheckPointData ParseCheckPointData(const TiXmlElement* PinElement)
{
    CheckPointData data;
    data.name = PinElement->Attribute("name");

    data.pinID = 0;
    PinElement->QueryUnsignedAttribute("pinid", &data.pinID);

    std::string triggerPos;
    triggerPos = PinElement->Attribute("pos");
    std::vector<std::string> vpos;
    vpos = Crown::SDSplitStrings(triggerPos, ' ');
    int32 y;
    data.x = atoi(vpos[0].c_str());
    y = atoi(vpos[1].c_str());
    data.y = atoi(vpos[2].c_str());

    data.angle = 0;
    PinElement->QueryIntAttribute("angle", &data.angle);

    data.radius = 0;
    PinElement->QueryIntAttribute("r", &data.radius);

    return data;
}

SpawnData ParseSpawnData(const TiXmlElement* spawnElement)
{
    SpawnData data;
    data.name = spawnElement->Attribute("name");

    data.spawnId = 0;
    spawnElement->QueryUnsignedAttribute("spawnid", &data.spawnId);

    std::string spawnPos;
    spawnPos = spawnElement->Attribute("pos");
    std::vector<std::string> vpos;
    vpos = Crown::SDSplitStrings(spawnPos, ' ');
    int32 y;
    data.x = atoi(vpos[0].c_str());
    y = atoi(vpos[1].c_str());
    data.y = atoi(vpos[2].c_str());

    data.angle = 0;
    spawnElement->QueryIntAttribute("angle", &data.angle);

    data.radius = 0;
    spawnElement->QueryIntAttribute("r", &data.radius);

    // 刷怪类型
    data.spawnType = 0;
    spawnElement->QueryIntAttribute("type", &data.spawnType);

    // 最大怪数(按数量刷)
    data.spawnMax = 0;
    spawnElement->QueryIntAttribute("max", &data.spawnMax);

    // 出生周期(按周期刷)
    data.spawnPeriod = 0;
    spawnElement->QueryIntAttribute("period", &data.spawnPeriod);

    // 出生数量(按周期刷)
    data.spawnCount = 0;
    spawnElement->QueryIntAttribute("count", &data.spawnCount);

    // 出生持续时间(按周期刷)
    data.spawnSecond = 0;
    spawnElement->QueryIntAttribute("ms", &data.spawnSecond);

    // 关联的trigger id
    data.triggerId = 0;
    spawnElement->QueryUnsignedAttribute("tid", &data.triggerId);

    // 所在组
    data.group = 0;
    spawnElement->QueryIntAttribute("group", &data.group);

    // 循环刷怪
    int32 loop = 0;
    spawnElement->QueryIntAttribute("loop", &loop);
    data.loop = loop;

    // 单位碰撞
    int32 no_collison_object = 0;
    spawnElement->QueryIntAttribute("no_collison_object", &no_collison_object);
    data.no_collison_object = no_collison_object;

    // 默认不开启
    int32 no_start = 0;
    spawnElement->QueryIntAttribute("nostart", &no_start);
    data.no_start = no_start? true: false;

    
    if(spawnElement->FirstChildElement())
    {
        const TiXmlElement* childElement = spawnElement->FirstChildElement()->ToElement();
        for(; childElement; childElement = childElement->NextSiblingElement())
        {
            std::string nodeValue = childElement->Value();
            // 解析波数据
            if (nodeValue == "wave")    // wave
            {
                if(!childElement->FirstChildElement())
                    continue;

                SpawnWaveData wavaData = ParseSpawnWaveData(childElement);

                data.WaveDatas.push_back(wavaData);
            }
            // 解析出怪控制器
            else if(nodeValue == "spawn_control")       // spawn_control
            {
                SpawnWaveControlData waveControlData = ParseSpawnWaveControlData(childElement);
                data.WaveDataControls.push_back(waveControlData);
            }
        }
    }

    return data;
}
// 解析波数据
SpawnWaveData ParseSpawnWaveData(const TiXmlElement* waveElement)
{
    SpawnWaveData data;

    int32 delay = 0;
    waveElement->QueryIntAttribute("delay", &delay);
    data.delay = delay * 1000;    // 秒->毫秒

    const TiXmlElement* childElement = waveElement->FirstChildElement()->ToElement();
    for(; childElement; childElement = childElement->NextSiblingElement())
    {
        std::string nodeValue = childElement->Value();
        if (nodeValue == "pawn")    // pawn
        {
            MonsterData monsterData = ParseMonsterAndNpcData(childElement);
            uint32 num = 0;
            childElement->QueryUnsignedAttribute("num", &num);
            monsterData.num = num;

            data.MonsterDatas.push_back(monsterData);
        }
    }

    return data;
}

SpawnWaveControlData ParseSpawnWaveControlData(const TiXmlElement* waveElement)
{
    SpawnWaveControlData waveControlData;

    int32 type = SPAWN_CONTROL_NONE;
    waveElement->QueryIntAttribute("type", &type);
    
    waveControlData.type = type;
    waveControlData.waveControlXML = waveElement;

    return waveControlData;
}

TouchData ParseTouchData(const TiXmlElement* patchElement)
{
    TouchData data;
    data.name = patchElement->Attribute("name");

    patchElement->QueryUnsignedAttribute("touchId", &data.touchId);


    // 位置
    std::string triggerPos = patchElement->Attribute("pos");
    VECTOR3D_INT vector3D = ParseVector3DInt(triggerPos);
    data.x = vector3D.x;
    data.y = vector3D.z;

    patchElement->QueryIntAttribute("angle", &data.angle);
    patchElement->QueryIntAttribute("r", &data.radius);
    patchElement->QueryIntAttribute("time", &data.time);

    // 物件集名
    if(patchElement->Attribute("obs"))
    {
        data.obs_name = patchElement->Attribute("obs");
    }

    // 物件集位置
    if(patchElement->Attribute("obs_pos"))
    {
        std::string ObsPos = patchElement->Attribute("obs_pos");
        data.obs_pos = ParseVector3DInt(ObsPos);
    }

    // 拾取范围
    if(patchElement->Attribute("picksize"))
    {
        std::string pickSize = patchElement->Attribute("picksize");
        data.pick_size = ParseVector3DInt(pickSize);
    }

    // 拾取范围位置
    if(patchElement->Attribute("pickpos"))
    {
        std::string pickPos = patchElement->Attribute("pickpos");
        data.pick_pos = ParseVector3DInt(pickPos);
    }

    return data;
}

const CheckPointData* RegularDoc::GetPinData(const std::string &name) const
{
    CheckPointDataMap::const_iterator ite = m_pinMap.find(name);
    if (ite != m_pinMap.end())
    {
        return &ite->second;
    }
    return NULL;
}

const TriggerData* RegularDoc::GetTriggerData(const std::string &name) const
{
    TriggerDataMap::const_iterator ite = m_triggerMap.find(name);
    if (ite != m_triggerMap.end())
    {
        return &ite->second;
    }
    return NULL;
}

const SpawnData* RegularDoc::GetSpawnData(const std::string &name) const
{
    SpawnDataMap::const_iterator ite = m_spawnMap.find(name);
    if (ite != m_spawnMap.end())
    {
        return &ite->second;
    }
    return NULL;
}

const TouchData* RegularDoc::GetTouchData(const std::string &name) const
{
    TouchDataMap::const_iterator ite = m_touchMap.find(name);
    if (ite != m_touchMap.end())
    {
        return &ite->second;
    }
    return NULL;
}

RegularDoc::RegularDoc() : m_pDoc(NULL), m_type(0), m_ReadyTime(0), m_RunTime(0), m_EndTime(0)
{}

RegularDoc::~RegularDoc()
{
    if (m_pDoc)
    {
        CnDelete m_pDoc;
    }
}