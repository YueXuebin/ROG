/*
*   被动技能
*/
#ifndef passive_skill_manager_h__
#define passive_skill_manager_h__

class Player;


class PassiveSkillManager
{
public:
    PassiveSkillManager(Player* player);
    ~PassiveSkillManager();

    void LoadPassiveSkillInfo(const PASSIVE_SKILL_LIST& info);
    void FillPassiveSkillInfo(PASSIVE_SKILL_LIST& info);

    void OnRecv(uint32 msgID, void* data);

    void AddAvailablePoints(uint32 points); // 增加天赋可用点数
    void ResetPoints();                     // 洗点
    void LevelUpSkill(uint32 skillID);      // 升级技能

    //被动技能事件响应
    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );

    void CalcPassiveSkillProp();                 // 天赋属性加成及战斗力

    int32 GetMaxPassiveSkillLevel();
    PASSIVE_SKILL_INFO* GetOneSkillInfo(uint32 id);

    PASSIVE_SKILL_LIST& GetPassiveSkillInfo() {return m_passiveSkillInfo;}

private:
    void OnRecvLevelUp(void* data);

    void CheckUnlockSkills();

    void SendPassiveSkillInfo();            // 发送被动技能信息


private:
    Player*             m_player;
    PASSIVE_SKILL_LIST  m_passiveSkillInfo;
};

#endif // passive_skill_manager_h__
