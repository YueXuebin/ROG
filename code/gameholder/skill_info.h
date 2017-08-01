#ifndef skill_info_h__
#define skill_info_h__


// 最终技能信息
class SkillInfo
{
public:
    SkillInfo();
    ~SkillInfo();

    void Clear();

    uint32 GetSkillId() const {return m_finalInfo.skill_id;}
    void SetSkillId(uint32 skillId) {m_finalInfo.skill_id = skillId;}

    void SetQuality(uint32 quality) {m_finalInfo.quality = quality;}
    uint32 GetQuality() {return m_finalInfo.quality;}

    void SetLevel(uint32 level) {m_finalInfo.level = level;}
    uint32 GetLevel() {return m_finalInfo.level;}

    void SetMp(uint32 mp) {m_finalInfo.mp = mp;}
    uint32 GetMp() {return m_finalInfo.mp;}

    void SetCoolDown(uint32 cd) {m_finalInfo.cooldown = cd;}
    uint32 GetCoolDown() const {return m_finalInfo.cooldown;}

    void SetTriggered(bool val) { m_IsTriggered = val; }
    bool IsTriggered() {return m_IsTriggered;}

    void SetBookValid(bool val) {m_finalInfo.book_valid = val;}
    void SetSkillValid(bool val) {m_finalInfo.skill_valid = val;}

    void ClearRuneIDs() {m_finalInfo.rune_ids.clear(); m_finalInfo.rune_oks.clear();}
    void AddRuneIDs(uint32 rune_id) {m_finalInfo.rune_ids.push_back(rune_id);}
    void AddRuneOks(int32 rune_ok) {m_finalInfo.rune_oks.push_back(rune_ok);}          // 符文是否能够和技能匹配(0为匹配, -1为重复)

    void AddProp(uint32 key, int32 value);
    void AddProp(const KEY_VALUE& prop) { m_finalInfo.props.push_back(prop); }
    int32 GetProp(uint32 key);

    void FillPropertySet(PropertySet& propertySet) const;

    FINAL_SKILL_INFO& GetSkillInfo() {return m_finalInfo;}

    SKILL_ARGUMETNS* GetSkillArgs() {return &m_finalInfo.skill_args;}

    void SetDamage(int32 damage) {m_Damage = damage;}
    void AddDamage(int32 damage) {m_Damage += damage;}
    int32 GetDamage() {return m_Damage;}

private:
    

private:
    FINAL_SKILL_INFO m_finalInfo;

    bool    m_IsTriggered;          // 技能是否为被动触发
    int32   m_Damage;               // 累计的伤害值

};

#endif // skill_info_h__
