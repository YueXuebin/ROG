#ifndef skill_data_parser_h__
#define skill_data_parser_h__


class SkillData;
class SkillMoveData;

/*
 *	技能XML数据解析器
 */
class SkillDataParser
{
public:
    SkillDataParser();
    ~SkillDataParser();

    SkillData* ParseSkillData(uint32 skillId, uint32 version, TiXmlDocument* pXmlDoc);

private:
    // 版本4
    SkillData* ParseSkillData4(uint32 skillId, TiXmlDocument* pXmlDoc);
    void ParseSkillTargetData(SkillData* pSkillData, TiXmlElement* pRoot);      // 目标
    void ParseSkillCastData(SkillData* pSkillData, TiXmlElement* pRoot);        // 施法
    void ParseSkillLaunchData(SkillData* pSkillData, TiXmlElement* pRoot);      // 发射
    void ParseSkillBulletData(SkillData* pSkillData, TiXmlElement* pRoot);      // 弹道
    void ParseSkillHitData(SkillData* pSkillData, TiXmlElement* pRoot);         // 命中
    void ParseSkillStrikeData(SkillData* pSkillData, TiXmlElement* pRoot);      // 打击
    //
    void ParseMoveData(SkillMoveData* pMoveData, TiXmlElement* pMoveXmlDoc);

};



#endif // skill_data_parser_h__
