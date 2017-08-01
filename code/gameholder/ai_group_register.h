#ifndef ai_group_register_h__
#define ai_group_register_h__



class Regular;
class BattleUnit;
class AIGroup;

/*
 *	AI群管理器，管理当前Regular中的所有群
 */
class AIGroupRegister
{
public:
    AIGroupRegister(Regular * pRegular);
    ~AIGroupRegister(void);

    void Update();

    AIGroup* GetAIGroupWithCreate(uint32 id);

    Regular* GetRegular() {return m_pRegular;}

private:
    AIGroup* CreateAIGroup(uint32 id);
    AIGroup* GetAIGroup(uint32 id);

private:
    Regular* m_pRegular;

    uint32  m_GroupCounter;

    typedef std::map<uint32, AIGroup*> AIGroupMap;          // <AI群id, AI群>
    AIGroupMap  m_GroupMap;
};

#endif // ai_group_register_h__