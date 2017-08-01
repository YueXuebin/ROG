#ifndef CLuaRobot_h__
#define CLuaRobot_h__

#include "CLuaObj.h"

extern uint32 g_GameTime;
extern char g_Account[100];

class CaseLua;
class CLuaRobot;
class ActionBase;
class Robot;

typedef void (*DoActionFunction)(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 agr3);
void DoRidePetAction( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3);
void DoChatAction( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3);
void DoPassiveSkillActionn( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3);
void DoTouchActoin( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3);
void DoSkillAction( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3);
void DoDance( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3);
void DoMoveRandom(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 agr3 = 0);
void DoMove(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 agr3 = 0);
void DoWait(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 agr3 = 0);
void DoJumpDungeon(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 agr3 = 0);
void DoEquipLeveUp(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 agr3 = 0);
void DoBuyObject(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3 = 0);
void DoFollowpet(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3 = 0);
void DoFriend(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3);
void DoTask(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3 = 0);
void DoHorcope(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3 = 0);
void DoOnlineReward( CaseLua* pCase, const std::string& actionName, int32 arg1 = 0, int32 arg2 = 0, int32 arg3 = 0);
void DoItem(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3 = 0);
void DoAchiveAction( CaseLua* pCase, const std::string& actionName, int32 arg1 = 0, int32 arg2 = 0, int32 arg3 = 0);
void DoSendPacketAction( CaseLua* pCase, const std::string& actionName, int32 arg1 = 0, int32 arg2 = 0, int32 arg3 = 0);
void DoGuildAction( CaseLua* pCase, const std::string& actionName, int32 arg1 = 0, int32 arg2 = 0, int32 arg3 = 0);
//void DoRideMarkAction( CaseLua* pCase, const std::string& actionName, int32 arg1 = 0, int32 arg2 = 0, int32 arg3 = 0);

class CLuaRobot : public CLuaObj
{
public:
    CLuaRobot(CaseLua* pLuaCase);
    virtual ~CLuaRobot();


    bool Init();
    void UnInit();

    Robot* GetRobot();

    // lua -> C 函数
    void OnActionEnd(ActionBase* action);
    void OnALLActionEnd();

    // C -> lua 函数
    void ShowActionList();
    void DoAction(const char* actionName, int32 arg1 = 0, int32 arg2 = 0,int32 arg3 = 0);
    void DoActionTwo(const char* actionName, int32 arg1 = 0, int32 arg2 = 0,int32 arg3 = 0);

    int     GetSecneObjectUintX(int touchId);
    int     GetSecneObjectUintY(int touchId);
    int     GetSceneObjectOne();

    int     GetPlayerLevel();
    int     GetDungeonID();
    int     GetChannelID();
    int     GetRandomDungeonID();
    int     GetRandomWorldMapID();
    int     GetRandomPlayerSkillID();
    int     GetX();
    int     GetY();
    int     GetUnitX(int id);
    int     GetUnitY(int id);
    int     GetOneEnemy();
    void    CallGM(const char* command);
    void    UseSkill(int objectId, int skillId);
    void    ChangeName(const char* name);
    int     GetTime();
    const char* GetAccount();
    int     GetMsgIDCount();
    int     GetMsgID(int i);
    int     GetRandomMsgID();
    int     GetBagItemNum(int itemid);

    void    Close();

    //
private:
    void Error(const char* error, ...);

private:
    bool    m_bInit;
    CaseLua*    m_pCase;

    typedef std::map<std::string, DoActionFunction>     ActionFunctionMap;  // <action名字， 处理函数>
    ActionFunctionMap   m_ActionFunctionMap;    

};

#endif // CLuaRobot_h__
