/*
 *	AI��������
 */
#ifndef ai_skill_data_h__
#define ai_skill_data_h__

class AISkillData
{
public:
    uint32  skillId;        // ����ID
    int32   probability;    // ����
    
    AISkillData()
    {
        skillId = 0;
        probability = 0;
    }
};

#endif // ai_skill_data_h__
