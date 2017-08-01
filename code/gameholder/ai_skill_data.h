/*
 *	AI技能数据
 */
#ifndef ai_skill_data_h__
#define ai_skill_data_h__

class AISkillData
{
public:
    uint32  skillId;        // 技能ID
    int32   probability;    // 概率
    
    AISkillData()
    {
        skillId = 0;
        probability = 0;
    }
};

#endif // ai_skill_data_h__
