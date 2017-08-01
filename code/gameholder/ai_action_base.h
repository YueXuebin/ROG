#ifndef ai_action_base_h__
#define ai_action_base_h__

#include "ai_skill_data.h"

class AIController;

/*
 *	行为基类
 */
class AIActionBase
{
    friend class AIFactory;
public:
    typedef std::vector<AISkillData>    AISkillList;

    AIActionBase(uint32 id);
    virtual ~AIActionBase();

    virtual void onEnterState();
    virtual void onLeaveState() {}

    uint32 GetID() {return m_id;}

    AIController*   GetController() {return m_pController;}

    virtual void ParseXML(TiXmlElement* pDoc);
    virtual void Do() = 0;

    virtual void Update(int32 deltaTime);

    void    AddSkill(uint32 skillId, int32 probability);
    void    ClearSkill();
    const AISkillList&  GetSkillList() {return m_SkillList;}

private:
    bool IntervalAndCount();    // 间隔时间和次数是否满足

protected:
    uint32  m_id;
    AIController*   m_pController;

    // AI用数据
    AISkillList     m_SkillList;        // 技能列表

    int32   m_interval;                     // 间隔时间
    int32   m_count;                        // 执行次数

    int32   m_intervalTimer;
    int32   m_countTimer;
    bool    m_run;                          // 是否跑

};


#endif // ai_action_base_h__
