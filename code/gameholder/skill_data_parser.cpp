#include "gameholder_pch.h"
#include "skill_data_parser.h"
#include "skill_define.h"
#include "skill_data.h"


SkillDataParser::SkillDataParser()
{

}

SkillDataParser::~SkillDataParser()
{

}

SkillData* SkillDataParser::ParseSkillData(uint32 skillId, uint32 version, TiXmlDocument* pXmlDoc)
{
    TiXmlElement* pRoot = pXmlDoc->RootElement();
    // 技能ID
    int32 skillID = 0;
    pRoot->Attribute("id", &skillID);
    CnAssert(skillID == skillId);

    // 查找技能版本
    int32 skillVersion = 0;
    pRoot->Attribute("mode", &skillVersion);
    CnAssert(skillVersion == version);

    if(skillVersion == SKILL_ATTACK4)
    {
        return ParseSkillData4(skillId, pXmlDoc);
    }
    else
    {
        CRITICAL("skill id %d version error", skillId);
        return NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 版本4解析
SkillData* SkillDataParser::ParseSkillData4(uint32 skillId, TiXmlDocument* pXmlDoc)
{
    int32 tmpInt = 0;
    TiXmlElement* pRoot = pXmlDoc->RootElement();

    SkillData* pSkillData = CnNew SkillData;
    pSkillData->m_SkillId = skillId;
    pSkillData->m_SkillVersion = SKILL_ATTACK4;
    
    // 功能类型
    tmpInt = 0;
    pRoot->Attribute("usage", &tmpInt);
    pSkillData->m_Usage = tmpInt;
    
    // 目标
    ParseSkillTargetData(pSkillData, pRoot);

    // 施法
    ParseSkillCastData(pSkillData, pRoot);

    // 发射
    ParseSkillLaunchData(pSkillData, pRoot);

    // 弹道
    ParseSkillBulletData(pSkillData, pRoot);

    // 命中
    ParseSkillHitData(pSkillData, pRoot);

    // 打击
    ParseSkillStrikeData(pSkillData, pRoot);

    return pSkillData;
}
// 目标
void SkillDataParser::ParseSkillTargetData(SkillData* pSkillData, TiXmlElement* pRoot)
{
    int32 tmpInt = 0;
    TiXmlElement* pTargetNode = pRoot->FirstChild("target")->ToElement();

    tmpInt = 0;
    pTargetNode->Attribute("target_type", &tmpInt);     // 目标类型
    pSkillData->m_TargetData.m_TargetType = tmpInt;

    tmpInt = 0;
    pTargetNode->Attribute("range", &tmpInt);           // 射程
    pSkillData->m_TargetData.m_Range = tmpInt;
}

// 施法
void SkillDataParser::ParseSkillCastData(SkillData* pSkillData, TiXmlElement* pRoot)
{
    int32 tmpInt = 0;
    TiXmlElement* pCastNode = pRoot->FirstChild("cast")->ToElement();

    // 持续时间
    tmpInt = 0;
    pCastNode->Attribute("time", &tmpInt);
    pSkillData->m_CastData.m_CastTime = tmpInt;

    // 是否可以打断
    tmpInt = 0;
    pCastNode->Attribute("can_break", &tmpInt);
    pSkillData->m_CastData.m_CanBreak = tmpInt;

    // 吟唱阶段的位移
    TiXmlElement* moveEle = pCastNode->FirstChildElement("move");
    if(moveEle)
    {
        ParseMoveData(&pSkillData->m_CastData.m_MoveData, moveEle);
    }
}
// 位移
void SkillDataParser::ParseMoveData(SkillMoveData* pMoveData, TiXmlElement* pMoveXmlDoc)
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

// 发射
void SkillDataParser::ParseSkillLaunchData(SkillData* pSkillData, TiXmlElement* pRoot)
{
    int32 tmpInt = 0;
    TiXmlElement* pLaunchNode = pRoot->FirstChild("launch")->ToElement();

    // 发射开始时间
    tmpInt = 0;
    pLaunchNode->Attribute("start_time", &tmpInt);
    pSkillData->m_LaunchData.StartTime = tmpInt;

}
// 弹道
void SkillDataParser::ParseSkillBulletData(SkillData* pSkillData, TiXmlElement* pRoot)
{
    int32 tmpInt = 0;
    TiXmlElement* pBulletNode = pRoot->FirstChild("bullet")->ToElement();

    // 弹道模式
    tmpInt = 0;
    pBulletNode->Attribute("shoot_mode", &tmpInt);
    pSkillData->m_BulletData.m_BulletMode = tmpInt;     // 弹道模式(指向型、碰撞型)

    // 目标类型
    tmpInt = 0;
    pBulletNode->Attribute("target_type", &tmpInt);
    pSkillData->m_BulletData.m_BulletTargetData.m_TargetType = (tmpInt == 0 ? SKILL_TARGET_OBJECT : tmpInt);

    // 弹道通用参数
    tmpInt = 0;
    pBulletNode->Attribute("time", &tmpInt);
    pSkillData->m_BulletData.m_ShootTime = Crown::SDMax(tmpInt, 0);

    tmpInt = 0;
    pBulletNode->Attribute("speed", &tmpInt);
    pSkillData->m_BulletData.m_ShootSpeed = Crown::SDMax(tmpInt, 0);

    // 碰撞弹道参数
    tmpInt = 0;
    pBulletNode->Attribute("collision_time", &tmpInt);
    pSkillData->m_BulletData.m_CollisionTime = tmpInt;      // 再碰撞时间间隔

    tmpInt = 0;
    pBulletNode->Attribute("collision_count", &tmpInt);
    pSkillData->m_BulletData.m_CollisionCount = tmpInt;     // 碰撞次数

    tmpInt = 0;
    pBulletNode->Attribute("collision_radius", &tmpInt);
    pSkillData->m_BulletData.m_CollisionRadius = tmpInt;    // 碰撞体半径

    tmpInt = 0;
    pBulletNode->Attribute("offset_angle", &tmpInt);
    pSkillData->m_BulletData.m_OffsetAngle = tmpInt;        // 弹道偏角

    tmpInt = 0;
    pBulletNode->Attribute("end_hit", &tmpInt);
    pSkillData->m_BulletData.m_CollisionEndHit = (tmpInt>0);    // 是否飞行结束时触发命中逻辑

}
// 命中
void SkillDataParser::ParseSkillHitData(SkillData* pSkillData, TiXmlElement* pRoot)
{
    int32 tmpInt = 0;
    TiXmlElement* pHitNode = pRoot->FirstChild("hit")->ToElement();

    tmpInt = 0;
    pHitNode->Attribute("time", &tmpInt);
    pSkillData->m_HitData.m_HitTime = tmpInt;
    
    tmpInt = 0;
    pHitNode->Attribute("is_rect", &tmpInt);     // 圆形，矩形
    pSkillData->m_HitData.m_IsRect = (tmpInt == 1);

    tmpInt = 0;
    pHitNode->Attribute("range", &tmpInt);       // 半径，长
    pSkillData->m_HitData.m_HitRange = tmpInt;

    tmpInt = 0;
    pHitNode->Attribute("angle", &tmpInt);       // 角度，宽
    pSkillData->m_HitData.m_HitAngle = tmpInt;

    tmpInt = 0;
    pHitNode->Attribute("hit_rate", &tmpInt);    // 击中频率
    pSkillData->m_HitData.m_HitRate = tmpInt;

}

void SkillDataParser::ParseSkillStrikeData(SkillData* pSkillData, TiXmlElement* pRoot)
{


}

// 版本4解析结束
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
