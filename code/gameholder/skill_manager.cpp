#include "gameholder_pch.h"
#include "skill_manager.h"
#include "skill_define.h"
#include "game_define.h"
#include "skill_data.h"
#include "skill_data_parser.h"
#include "skill_object.h"
#include "skill_node_pool.h"
#include "skill_config.h"
#include "battle_unit.h"
#include "player.h"
#include "skill_info_manager.h"
#include "game_util.h"


IMPLEMENT_SINGLETON(SkillManager)


SkillManager::SkillManager()
{
    m_SkillCount = 0;
    
    m_pSkillNodePool = CnNew SkillNodePool;
}

SkillManager::~SkillManager()
{
    for(SkillDataMap::iterator iter = m_skillDataMap.begin(); iter != m_skillDataMap.end(); iter++)
    {
        CnDelete iter->second;
    }

    CnDelete m_pSkillNodePool;
}

bool SkillManager::Init()
{
    if(!ParseSkillList())
        return false;

    return true;
}

bool SkillManager::ParseSkillList()
{
    // 加载技能配置文件
    Crown::CSDFile skillFile;
    if(!skillFile.Open(RES_SKILL_PATH""SKILL_LIST_FILE, "rt"))
    {
        CnDbg("load skill file failed\n");
        return false;
    }

    skillFile.SeekToEnd();
    int32 fileLen = skillFile.GetPosition();
    skillFile.SeekToBegin();

    char* fileString = (char*)CnMalloc(sizeof(char)*(fileLen+1));

    uint32 readlen = skillFile.Read(fileString, fileLen);
    fileString[readlen] = '\0';

    skillFile.Close();

    // 正则表达式解析
    const char* pattern = "^.*skl";
    std::vector<std::string>    m_skillFileNames;
    CommonLib::FindStrings(pattern, fileString, m_skillFileNames);

    CnFree(fileString);

    SkillDataParser skillParser;

    // 加载各技能文件
    for(std::vector<std::string>::iterator iter = m_skillFileNames.begin(); iter != m_skillFileNames.end(); iter++)
    {
        //CnDbg((*iter).c_str());
        // 加载文件
        std::string fileName = RES_SKILL_PATH;
        fileName += (*iter).c_str();
        TiXmlDocument* pXMLDoc = CnNew TiXmlDocument(fileName.c_str());
        if(!pXMLDoc->LoadFile())
        {
            CRITICAL("skill %s load failed", fileName.c_str());
            CnDelete pXMLDoc;
            continue;
        }
        
        // 查找id
        TiXmlElement* pRoot = pXMLDoc->RootElement();
        int32 skillID = 0;
        pRoot->Attribute("id", &skillID);
        if(skillID<=0)
        {
            CRITICAL("skill %s no id", fileName.c_str());
            CnDelete pXMLDoc;
            continue;
        }

        // 查找技能版本号
        int32 skillVersion = 0;
        pRoot->Attribute("mode", &skillVersion);
        if(skillVersion<=0)
        {
            CRITICAL("skill %s no version", fileName.c_str());
            CnDelete pXMLDoc;
            continue;
        }

        if(m_skillDataMap.find(skillID) != m_skillDataMap.end())       // 技能id重复
        {
            CRITICAL("skill id %d already exist, file %s", skillID, fileName.c_str());
            CnDelete pXMLDoc;
            continue;
        }

        // 技能参数解析
        if(skillVersion == SKILL_ATTACK4)
        {
            SkillData* pSkillData = skillParser.ParseSkillData(skillID, skillVersion, pXMLDoc);
            CnAssert(pSkillData);
            m_skillDataMap[skillID] = pSkillData;
        }
        else
        {
            CRITICAL("skill id %d mode %d not support, file %s", skillID, skillVersion, fileName.c_str());
            CnDelete pXMLDoc;
            continue;
        }

        CnDelete pXMLDoc;
    }

    CnInfo("load %d skill ok!\n", m_skillDataMap.size());

    return true;
}

SkillData* SkillManager::GetSkillData( uint32 skillId )
{
    if(m_skillDataMap.find(skillId) != m_skillDataMap.end())
        return m_skillDataMap[skillId];
    else
    {
        CnAssert(false);
        return NULL;
    }
}

SkillObject* SkillManager::CreateSkill( uint32 skillId, BattleUnit* owner, const SkillArgumentData& skill_args )
{
    if( m_skillDataMap.find(skillId) == m_skillDataMap.end() )
    {
        CnWarn("skill id=%d no exist\n", skillId);
        return NULL;
    }

    SkillData* pSkillData = m_skillDataMap[skillId];

    SkillObject*  pNewSkill = NULL;
    switch(pSkillData->m_SkillVersion)
    {
    case SKILL_ATTACK4:
        pNewSkill = CnNew SkillObject(m_skillDataMap[skillId], owner, skill_args);
        break;
    default:
        CnWarn("skill id=%d version=%d, type no exist\n", skillId, pSkillData->m_SkillVersion);
        break;
    }

    return pNewSkill;
}
/*
void SkillManager::ParseSkillData(uint32 skillId, uint32 version, TiXmlDocument* pXmlDoc)
{
    switch(version)
    {
    case SKILL_ATTACK4:
        ParseSkillDataV4(skillId, pXmlDoc);
        break;
    default:
        CnAssert(false);        // 不支持的技能版本
        break;
    }
}



void SkillManager::ParseSkillDataV4(uint32 skillId, TiXmlDocument* pXmlDoc)
{
    if(m_skillDataMap.find(skillId) != m_skillDataMap.end())
    {
        CnFatal("skill %d already exist\n", skillId);
        return;
    }

    SkillData* pSkillData = CnNew SkillData;
    pSkillData->m_SkillId = skillId;
    pSkillData->m_SkillVersion = SKILL_ATTACK4;

    m_skillDataMap[skillId] = pSkillData;

    int32 tmpInt = 0;

    // 技能解析
    TiXmlElement* pRoot = pXmlDoc->RootElement();

    // 功能类型
    tmpInt = 0;
    pRoot->Attribute("usage", &tmpInt);
    pSkillData->m_Usage = tmpInt;

    // 目标
    TiXmlElement* pTargetNode = pRoot->FirstChild("target")->ToElement();

    tmpInt = 0;
    pTargetNode->Attribute("target_type", &tmpInt);     // 目标类型
    pSkillData->m_TargetType = tmpInt;

    tmpInt = 0;
    pTargetNode->Attribute("range", &tmpInt);           // 射程
    pSkillData->m_Range = tmpInt;

    tmpInt = 0;
    pTargetNode->Attribute("next_range", &tmpInt);      // 再生射程
    pSkillData->m_NextRange = tmpInt;

    // 效果类型
    tmpInt = 0;
    pTargetNode->Attribute("can_self", &tmpInt);
    pSkillData->m_canSelf = tmpInt;
    tmpInt = 0;
    pTargetNode->Attribute("can_friend", &tmpInt);
    pSkillData->m_canFriend = tmpInt;
    tmpInt = 0;
    pTargetNode->Attribute("can_enemy", &tmpInt);
    pSkillData->m_canEnemy = tmpInt;

    // 吟唱
    TiXmlElement* pSingingNode = pRoot->FirstChild("singing")->ToElement();

    pSingingNode->Attribute("time", &tmpInt);
    pSkillData->m_SingingTime = tmpInt;     // 吟唱时间

    tmpInt = 0;
    pSingingNode->Attribute("can_break", &tmpInt);
    pSkillData->m_canSingingBreak = tmpInt;

    // 吟唱阶段的弹道
    TiXmlElement* shootsEle = pSingingNode->FirstChildElement("shoots");
    if(shootsEle)
    {
        for(TiXmlElement* shootEle = shootsEle->FirstChildElement("shoot"); shootEle; shootEle = shootEle->NextSiblingElement("shoot"))
        {
            CastShootData castShootData;
            ParseCastShootData(&castShootData, shootEle);
            pSkillData->m_SingingShoot.push_back(castShootData);
        }
    }

    // 吟唱阶段的位移
    TiXmlElement* moveEle = pSingingNode->FirstChildElement("move");
    if(moveEle)
    {
        ParseMoveData(&pSkillData->m_SingingMove, moveEle);
    }

    // 施法
    TiXmlElement* pCastNode = pRoot->FirstChild("cast")->ToElement();

    pCastNode->Attribute("time", &tmpInt);
    pSkillData->m_CastTime = tmpInt;        // 施法时间

    // 施法阶段的弹道
    shootsEle = pCastNode->FirstChildElement("shoots");
    if(shootsEle)
    {
        for(TiXmlElement* shootEle = shootsEle->FirstChildElement("shoot"); shootEle; shootEle = shootEle->NextSiblingElement("shoot"))
        {
            CastShootData castShootData;
            ParseCastShootData(&castShootData, shootEle);
            pSkillData->m_CastShoot.push_back(castShootData);
        }
    }

    // 施法阶段的位移
    moveEle = pCastNode->FirstChildElement("move");
    if(moveEle)
    {
        ParseMoveData(&pSkillData->m_CastMove, moveEle);
    }

    // 通道
    TiXmlElement* pHoldingNode = pRoot->FirstChild("holding")->ToElement();

    pHoldingNode->Attribute("time", &tmpInt);
    pSkillData->m_HoldingTime = tmpInt;             // 通道时间

    tmpInt = 0;
    pHoldingNode->Attribute("can_break", &tmpInt);
    pSkillData->m_canHoldingBreak = tmpInt;

    // 通道阶段的弹道
    shootsEle = pHoldingNode->FirstChildElement("shoots");
    if(shootsEle)
    {
        for(TiXmlElement* shootEle = shootsEle->FirstChildElement("shoot"); shootEle; shootEle = shootEle->NextSiblingElement("shoot"))
        {
            CastShootData castShootData;
            ParseCastShootData(&castShootData, shootEle);
            pSkillData->m_HoldingShoot.push_back(castShootData);
        }
    }

    // 通道阶段的位移
    moveEle = pHoldingNode->FirstChildElement("move");
    if(moveEle)
    {
        ParseMoveData(&pSkillData->m_HoldingMove, moveEle);
    }

    // 所有的弹道数据
    TiXmlElement* pShootsNode = pRoot->FirstChild("shoots")->ToElement();
    if(pShootsNode)
    {
        for(TiXmlElement* shootEle = pShootsNode->FirstChildElement("shoot"); shootEle; shootEle = shootEle->NextSiblingElement("shoot"))
        {
            ShootData shootData;
            ParseShootData(&shootData, shootEle);

            // 弹道目标类型修正
            if(pSkillData->m_TargetType == SKILL_TARGET_POS)
            {
                shootData.m_ShootTargetType = SKILL_TARGET_POS;     // 如果技能是目标为位置，则弹道的目标必须为位置
            }

            pSkillData->m_ShootDatas.push_back(shootData);
        }
    }


    // 死亡
    TiXmlNode* pDeadNode = pRoot->FirstChild("dead");
    if(pDeadNode && pDeadNode->FirstChild("push_enemy"))
    {
        TiXmlNode* pushemeny = pDeadNode->FirstChild("push_enemy");
        if(!pushemeny)
            return;

        TiXmlElement* pDeadPushEnemyNode = pushemeny->ToElement();
        if(pDeadPushEnemyNode)
            return;

        tmpInt = 0;
        pDeadPushEnemyNode->Attribute("mode", &tmpInt);
        pSkillData->m_DeadPushEnemyMode = tmpInt;

        tmpInt = 0;
        pDeadPushEnemyNode->Attribute("time", &tmpInt);
        pSkillData->m_DeadPushEnemyTime = tmpInt;

        tmpInt = 0;
        pDeadPushEnemyNode->Attribute("distance", &tmpInt);
        pSkillData->m_DeadPushEnemyDistance = tmpInt;

        tmpInt = 0;
        pDeadPushEnemyNode->Attribute("bounce_time", &tmpInt);
        pSkillData->m_DeadPushEnemyBounceTime = tmpInt;

        tmpInt = 0;
        pDeadPushEnemyNode->Attribute("bounce_height", &tmpInt);
        pSkillData->m_DeadPushEnemyBounceHeight = tmpInt;
    }

}

void SkillManager::ParseCastShootData(CastShootData* pCastShootData, TiXmlElement* pCastShootXmlDoc)
{
    int32 tmpInt = 0;

    // 触发时间
    tmpInt = 0;
    pCastShootXmlDoc->Attribute("delay_time", &tmpInt);
    pCastShootData->m_DelayTime = tmpInt;

    // 弹道ID
    tmpInt = 0;
    pCastShootXmlDoc->Attribute("shoot_id", &tmpInt);
    pCastShootData->m_ShoodId = tmpInt;
}

void SkillManager::ParseMoveData(MoveData* pMoveData, TiXmlElement* pMoveXmlDoc)
{
    // 解析推动数据
    int32 tmpInt = 0;

    tmpInt = 0;
    pMoveXmlDoc->Attribute("mode", &tmpInt);
    pMoveData->m_moveMode = tmpInt;

    tmpInt = 0;
    pMoveXmlDoc->Attribute("time", &tmpInt);
    pMoveData->m_moveTime = tmpInt;

    tmpInt = 0;
    pMoveXmlDoc->Attribute("distance", &tmpInt);
    pMoveData->m_moveDistance = tmpInt;

    tmpInt = 0;
    pMoveXmlDoc->Attribute("bounce_time", &tmpInt);
    pMoveData->m_BounceTime = tmpInt;

    tmpInt = 0;
    pMoveXmlDoc->Attribute("bounce_height", &tmpInt);
    pMoveData->m_BounceHeight = tmpInt;

}

void SkillManager::ParseShootData(ShootData* pShootData, TiXmlElement* pShootXmlDoc)
{
    int32 tmpInt = 0;
    
    tmpInt = 0;
    pShootXmlDoc->Attribute("shoot_id", &tmpInt);
    pShootData->m_ShootId = tmpInt;
    
    // 碰撞型弹道参数
    tmpInt = 0;
    pShootXmlDoc->Attribute("shoot_mode", &tmpInt);
    pShootData->m_ShootMode = tmpInt;

    tmpInt = 0;
    pShootXmlDoc->Attribute("target_type", &tmpInt);
    pShootData->m_ShootTargetType = (tmpInt == 0 ? SKILL_TARGET_OBJECT : tmpInt);

    tmpInt = 0;
    pShootXmlDoc->Attribute("collision_time", &tmpInt);
    pShootData->m_CollisionTime = tmpInt;

    tmpInt = 0;
    pShootXmlDoc->Attribute("collision_count", &tmpInt);
    pShootData->m_CollisionCount = tmpInt;

    tmpInt = 0;
    pShootXmlDoc->Attribute("collision_radius", &tmpInt);
    pShootData->m_CollisionRadius = tmpInt;

    tmpInt = 0;
    pShootXmlDoc->Attribute("end_hit", &tmpInt);
    pShootData->m_CollisionEndHit = (tmpInt > 0);

    // 弹道参数
    tmpInt = 0;
    pShootXmlDoc->Attribute("time", &tmpInt);
    if(tmpInt < 0) tmpInt = 0;
    pShootData->m_ShootTime = tmpInt;

    tmpInt = 0;
    pShootXmlDoc->Attribute("speed", &tmpInt);
    if(tmpInt < 0) tmpInt = 0;
    pShootData->m_ShootSpeed = tmpInt;

    // 弹道偏角
    tmpInt = 0;
    pShootXmlDoc->Attribute("offset_angle", &tmpInt);
    pShootData->m_OffsetAngle = tmpInt;

    /////////////////////////////////////////////////////////////////////////////////
    // 击中
    TiXmlElement* pHitNode = pShootXmlDoc->FirstChild("hit")->ToElement();

    pHitNode->Attribute("time", &tmpInt);
    pShootData->m_HitTime = tmpInt;

    // 再生弹道
    TiXmlElement* shootsEle = pHitNode->FirstChildElement("shoots");
    if(shootsEle)
    {
        for(TiXmlElement* shootEle = shootsEle->FirstChildElement("shoot"); shootEle; shootEle = shootEle->NextSiblingElement("shoot"))
        {
            CastShootData castShootData;
            ParseCastShootData(&castShootData, shootEle);
            pShootData->m_NextShoot.push_back(castShootData);
        }
    }

    // 击中范围
    tmpInt = 0;
    pHitNode->Attribute("is_rect", &tmpInt);
    pShootData->m_IsRect = (tmpInt == 1);

    tmpInt = 0;
    pHitNode->Attribute("range", &tmpInt);
    pShootData->m_HitRange = tmpInt;

    tmpInt = 0;
    pHitNode->Attribute("angle", &tmpInt);
    pShootData->m_HitAngle = tmpInt;

    // 击中频率
    tmpInt = 0;
    pHitNode->Attribute("hit_rate", &tmpInt);
    pShootData->m_HitRate = tmpInt;

    // 击中逻辑
    TiXmlElement* pHitLogicNode = pHitNode->FirstChild("hit_logic")->ToElement();

    // 击中buff
    int32 buffNumber = 0;

    TiXmlNode* pBuffNode = pHitLogicNode->FirstChild("buffs");
    if(pBuffNode)
    {
        for(TiXmlElement* childEle = pBuffNode->FirstChildElement("buff"); childEle; childEle = childEle->NextSiblingElement("buff"))
        {
            const char *name = childEle->GetText();

            tmpInt = 0;
            childEle->Attribute("buffid", &tmpInt);
            pShootData->m_Buff[buffNumber].m_BuffInfo.buff_id = tmpInt;

            // buff time
            TiXmlElement* buffTimeXML = childEle->FirstChildElement("time");
            pShootData->m_BuffTime[buffNumber] = ParseLevelValue(buffTimeXML);

            // buff value
            TiXmlElement* buffValueXML = childEle->FirstChildElement("value");
            pShootData->m_BuffVal[buffNumber] = ParseLevelValue(buffValueXML);

            tmpInt = 0;
            childEle->Attribute("probability", &tmpInt);
            pShootData->m_Buff[buffNumber].m_BuffProbability = tmpInt;

            tmpInt = 1;
            childEle->Attribute("add_buff", &tmpInt);
            pShootData->m_Buff[buffNumber].m_addBuff = tmpInt;

            tmpInt = 0;
            childEle->Attribute("can_self", &tmpInt);
            pShootData->m_Buff[buffNumber].m_canSelf = tmpInt;

            tmpInt = 0;
            childEle->Attribute("can_friend", &tmpInt);
            pShootData->m_Buff[buffNumber].m_canFriend = tmpInt;

            tmpInt = 0;
            childEle->Attribute("can_enemy", &tmpInt);
            pShootData->m_Buff[buffNumber].m_canEnemy = tmpInt;

            buffNumber++;
        }
    }

    // 嘲讽
    tmpInt = 0;
    pHitLogicNode->Attribute("taunt",&tmpInt);
    pShootData->m_hasTaunt = (tmpInt > 0);

    // 伤害
    tmpInt = 0;
    pHitLogicNode->Attribute("damage", &tmpInt);
    pShootData->m_hasDamage = (tmpInt > 0);
    if(pShootData->m_hasDamage)
    {
        // 解析伤害数值
        TiXmlElement* pDamageNode = pHitLogicNode->FirstChild("damage")->ToElement();
        pShootData->m_Damage = ParseLevelValue(pDamageNode);
    }

    // 治疗
    tmpInt = 0;
    pHitLogicNode->Attribute("heal", &tmpInt);
    pShootData->m_hasHeal = (tmpInt > 0);
    if(pShootData->m_hasHeal)
    {
        // 解析伤害数值
        TiXmlElement* pHealNode = pHitLogicNode->FirstChild("heal")->ToElement();
        pShootData->m_Heal = ParseLevelValue(pHealNode);
    }

    // 推动敌人
    tmpInt = 0;
    pHitLogicNode->Attribute("push_enemy", &tmpInt);
    pShootData->m_hasPushEnemy = (tmpInt > 0);
    if(pShootData->m_hasPushEnemy)
    {
        // 解析推动敌人数据
        TiXmlElement* pPushEnemyNode = pHitLogicNode->FirstChild("push_enemy")->ToElement();

        tmpInt = 0;
        pPushEnemyNode->Attribute("mode", &tmpInt);
        pShootData->m_PushEnemyMode = tmpInt;

        tmpInt = 0;
        pPushEnemyNode->Attribute("time", &tmpInt);
        pShootData->m_PushEnemyTime = tmpInt;

        tmpInt = 0;
        pPushEnemyNode->Attribute("distance", &tmpInt);
        pShootData->m_PushEnemyDistance = tmpInt;

        tmpInt = 0;
        pPushEnemyNode->Attribute("bounce_time", &tmpInt);
        pShootData->m_PushEnemyBounceTime = tmpInt;

        tmpInt = 0;
        pPushEnemyNode->Attribute("bounce_height", &tmpInt);
        pShootData->m_PushEnemyBounceHeight = tmpInt;
    }

    // 召唤
    tmpInt = 0;
    pHitLogicNode->Attribute("summon", &tmpInt);
    pShootData->m_hasSummon = (tmpInt > 0);
    if(pShootData->m_hasSummon)
    {
        // 解析召唤数据
        TiXmlElement* pSummonNode = pHitLogicNode->FirstChild("summon")->ToElement();

        tmpInt = 0;        
        pSummonNode->Attribute("summon_type", &tmpInt);     // 召唤类型
        pShootData->m_SummonType = tmpInt;

        tmpInt = 0;        
        pSummonNode->Attribute("monster_id", &tmpInt);
        pShootData->m_SummonMonsterId = tmpInt;     // 召唤怪id

        tmpInt = 0;        
        pSummonNode->Attribute("monster_number", &tmpInt);
        pShootData->m_SummonMonsterNumber = tmpInt;     // 招唤怪数量

        tmpInt = 0;
        pSummonNode->Attribute("summon_radius", &tmpInt);
        pShootData->m_SummonRadius = tmpInt;            // 召唤半径

        tmpInt = 0;
        pSummonNode->Attribute("dead_by_master", &tmpInt);
        pShootData->m_SummonMonsterDeadByMaster = tmpInt;   // 召唤怪随主人死亡

        const char* aiString = pSummonNode->Attribute("summon_ai");
        if(aiString && Crown::SDStrlen(aiString)>0)
            pShootData->m_SummonAIName = aiString;

    }

}
*/
bool SkillManager::HasSkill( uint32 skillid )
{
    SkillDataMap::iterator it2 = m_skillDataMap.find(skillid);
    if(it2 != m_skillDataMap.end())
    {
        return true;
    }
    return false;
}


int32 SkillManager::ParseLevelValue(TiXmlElement* pXML)
{
    return 1;
}

int32 SkillManager::GetSkillDamage( uint32 skillId, uint32 skillLevel, BattleUnit* owner )
{
    return 100;
}
