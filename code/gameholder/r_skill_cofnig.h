#ifndef r_skill_config_h__
#define r_skill_config_h__


typedef struct RSkillInfo
{
    int32 id;
    PropertyList pro;
    int32 percentage;
    int32 attack;
    void Clear()
    {
        id = 0;
        pro.clear();
        percentage = 0;
        attack = 0;
    }
}RSkillInfo;

typedef std::vector<RSkillInfo>  RSkillInfoList;

class RSkillCofnig
{
public:
    RSkillCofnig(void);
    ~RSkillCofnig(void);

    DECLARE_SINGLETON(RSkillCofnig)

    bool LoadConfig(const char* path);                                      // 加载配置文件

    const RSkillInfo* GetSkill(uint32 id);                                   // 获取技能信息

private:
    RSkillInfoList  m_skillList;                                            // 技能列表
};
#endif