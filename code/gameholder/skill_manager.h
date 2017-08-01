/*
 *	技能管理器
 */
#ifndef skill_manager_h__
#define skill_manager_h__


class TiXmlDocument;
class SkillObject;
class SkillData;
class BattleUnit;
class SkillNodePool;
class MoveData;
class SkillArgumentData;

class SkillManager
{
public:
    SkillManager();
    ~SkillManager();

    DECLARE_SINGLETON(SkillManager)

    bool Init();

    bool HasSkill(uint32 skillid);
    SkillData*  GetSkillData(uint32 skillId);

    SkillObject*  CreateSkill(uint32 skillId, BattleUnit* owner, const SkillArgumentData& skill_args);
    int32   GetSkillDamage(uint32 skillId, uint32 skillLevel, BattleUnit* owner);


    void    IncSkillCount() {m_SkillCount++;}
    void    DecSkillCount() {m_SkillCount--;}
    uint32  GetSkillCount() {return m_SkillCount;}

    SkillNodePool*  GetSkillNodePool() {return m_pSkillNodePool;}

private:
    bool ParseSkillList();
    void ParseSkillData(uint32 skillId, uint32 version, TiXmlDocument* pXmlDoc);

    void ParseSkillDataV4(uint32 skillId, TiXmlDocument* pXmlDoc);     // 解析新技能模型参数

    int32 ParseLevelValue(TiXmlElement* pXML);


private:
    typedef std::map<uint32, SkillData*>        SkillDataMap;
    SkillDataMap m_skillDataMap;

    uint32  m_SkillCount;                   // 技能对象计数

    SkillNodePool*      m_pSkillNodePool;
};


#endif // skill_manager_h__
