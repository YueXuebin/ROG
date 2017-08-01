#include "robot_pch.h"
#include "CLuaRobot.h"
#include "script.h"
#include "case_lua.h"
#include "action_define.h"
//#include "luaGen.h"
#include "robot.h"
#include "robot_config.h"
#include "buy_action.h"
#include "friend_action.h"
#include "followpet_aciton.h"
#include "task_aciton.h"
#include "horoscope_action.h"
#include "online_reward_action.h"
#include "item_action.h"
#include "achive_aciton.h"
#include "gm_action.h"
#include "send_packet_action.h"
#include "dance_start.h"
#include "skill_action.h"
#include "touch_start_action.h"
#include "guild_createaction.h"
#include "passive_skill.h"
#include "chat_action.h"
#include "ride_petAction.h"
//#include "ridemark_aciton.h"


CLuaRobot::CLuaRobot(CaseLua* pLuaCase) :
CLuaObj("test"),
m_pCase(pLuaCase),
m_bInit(false)
{
    m_ActionFunctionMap["MoveRandom"] = DoMoveRandom;
    m_ActionFunctionMap["Move"] = DoMove;
    m_ActionFunctionMap["Wait"] = DoWait;
    m_ActionFunctionMap["JumpDungeon"] = DoJumpDungeon;
    m_ActionFunctionMap["EquipLeveUp"] = DoEquipLeveUp;
    m_ActionFunctionMap["BuyObject"] = DoBuyObject;
    m_ActionFunctionMap["DoFriend"] = DoFriend;
    m_ActionFunctionMap["FollowShoworHide"] = DoFollowpet;
    m_ActionFunctionMap["TaskAction"] = DoTask;
    m_ActionFunctionMap["HoroscopeLeveup"] = DoHorcope;
    m_ActionFunctionMap["OnlineReward"] = DoOnlineReward;
    m_ActionFunctionMap["Item"] = DoItem;
    m_ActionFunctionMap["AchiveAction"] = DoAchiveAction;
    m_ActionFunctionMap["SendPacket"] = DoSendPacketAction;
    m_ActionFunctionMap["Dance"] = DoDance;
    m_ActionFunctionMap["Skill_action"] = DoSkillAction;
    m_ActionFunctionMap["TouchStart"] = DoTouchActoin;
    m_ActionFunctionMap["GuildCreate"] = DoGuildAction;
    m_ActionFunctionMap["PassiveSkillAction"] = DoPassiveSkillActionn;
    m_ActionFunctionMap["chatAction"] = DoChatAction;
    m_ActionFunctionMap["RidePet"] = DoRidePetAction;
    //m_ActionFunctionMap["RideMark"] = DoRideMarkAction;


}

CLuaRobot::~CLuaRobot()
{
    UnInit();
}

bool CLuaRobot::Init()
{
    char dir[SD_MAX_PATH];
    Crown::SDGetCurrentDirectory(dir, SD_MAX_PATH);

    std::string filename = RobotConfig::Instance()->GetCaseFileName();

    bool needLua = false;
    int32 p = filename.find_last_of(".");
    if(p > 0)
    {
        std::string extName = filename.substr(p);
        if(extName != ".lua")
            needLua = true;
    }
    else
        needLua = true;

    if(needLua)
        filename += ".lua";

    bool bOk = CScriptEngine::Instance()->LoadScript(filename.c_str());
    if(!bOk)
    {
        CnAssert(bOk);
        Error("load lua %s failed", filename.c_str());
        return false;
    }

    bool exportOk = CScriptEngine::Instance()->ExportCLuaObj("Robot", "CLuaRobot", this);
    if(!exportOk)
    {
        CnAssert(FALSE);
        Error("create lua Object Robot failed");
        return false;
    }

    int nFuncIndex = FindFuncIndex("Init");
    if (-1 != nFuncIndex)
    {
        int ret = CScriptEngine::Instance()->CallLuaScriptFunc(this, nFuncIndex, 1);
        if(ret != 0)
        {
            CnAssert(FALSE);
            Error("call lua func Init error");
            return false;
        }
    }
    else
    {
        Error("no Init function");
        return false;
    }

    m_bInit = true;

    return true;
}

void CLuaRobot::UnInit()
{
    if(m_bInit)
    {
        CScriptEngine::Instance()->ReleaseCObj(this);
        m_bInit = false;
    }
}

void CLuaRobot::OnActionEnd( ActionBase* action )
{
    const char* name = action->name();

    int nFuncIndex = FindFuncIndex("ActionEnd");
    if (-1 != nFuncIndex)
    {
        int ret = CScriptEngine::Instance()->CallLuaScriptFunc(this, nFuncIndex, name);
        if(ret != 0)
        {
            CnAssert(false);
            Error("call lua func ActionEnd error");
            return;
        }
    }
    else
    {
        return;
    }
}

void CLuaRobot::OnALLActionEnd()
{
    int nFuncIndex = FindFuncIndex("ALLActionEnd");
    if (-1 != nFuncIndex)
    {
        int ret = CScriptEngine::Instance()->CallLuaScriptFunc(this, nFuncIndex, 1);
        if(ret != 0)
        {
            CnAssert(false);
            Error("call lua func ALLActionEnd error");
            return;
        }
    }
    else
    {
        return;
    }
}

void CLuaRobot::Error( const char* error, ... )
{
    const int errorLen = 100;
    char errorMsg[errorLen];

    va_list arglist;
    va_start(arglist, error);
    memset(errorMsg, 0, errorLen);
    vsnprintf(errorMsg, errorLen, error, arglist); 
    va_end(arglist);
#ifdef WIN
    _RPT1(_CRT_WARN, "%s", errorMsg);
#endif
    printf("%s\n", errorMsg);
}

void CLuaRobot::DoAction(const char* actionName, int32 arg1, int arg2,int arg3)
//void CLuaRobot::DoAction(const char* actionName, void* data)
{
    std::string _actionName = actionName;
    ActionFunctionMap::iterator iter = m_ActionFunctionMap.find(_actionName);
    if(iter == m_ActionFunctionMap.end())
    {
        Error("no action %s", _actionName.c_str());
    }
    else
    {
        DoActionFunction pFunc = iter->second;
        (*pFunc)(m_pCase, _actionName, arg1, arg2, arg3);
    }
}

void CLuaRobot::DoActionTwo(const char* actionName, int32 arg1, int arg2,int arg3)
{
    std::string _actionName = actionName;

    ActionFunctionMap::iterator iter = m_ActionFunctionMap.find(_actionName);
    if(iter == m_ActionFunctionMap.end())
    {
        Error("no action %s", _actionName.c_str());
    }
    else
    {
        DoActionFunction pFunc = iter->second;
        (*pFunc)(m_pCase, _actionName, arg1, arg2, arg3);
    }
}

void CLuaRobot::ShowActionList()
{
    for(ActionFunctionMap::iterator iter = m_ActionFunctionMap.begin(); iter != m_ActionFunctionMap.end(); iter++)
    {
        const std::string& actionName = iter->first;
        printf("%s\n", actionName.c_str());
    }
}

Robot* CLuaRobot::GetRobot()
{
    return m_pCase->GetRobot();
}

int CLuaRobot::GetPlayerLevel()
{
 return GetRobot()->GetPropInfo().level;
}

int CLuaRobot::GetDungeonID()
{
    return GetRobot()->m_DungeonID;
}

int CLuaRobot::GetChannelID()
{
    return GetRobot()->m_ChannelID;
}

int CLuaRobot::GetX()
{
    return GetRobot()->GetPropInfo().x;
}

int CLuaRobot::GetY()
{
    return GetRobot()->GetPropInfo().y;
}

int CLuaRobot::GetSceneObjectOne()
{
    return GetRobot() ->GetSceneObj();
}

int CLuaRobot::GetSecneObjectUintX(int touchId)
{
    SCENEOBJECT_APPEAR* info = GetRobot()->GetSceneObjUnit(touchId);
    if (info)
        return info->x;
    else
        return 0;
}

int CLuaRobot::GetSecneObjectUintY(int touchId)
{
    SCENEOBJECT_APPEAR* info = GetRobot()->GetSceneObjUnit(touchId);
    if (info)
        return info->y;
    else
        return 0;
}

int CLuaRobot::GetUnitX(int id)
{
    BATTLEUNIT_APPEAR* info = GetRobot()->GetBattleUnit(id);
    if(info)
        return info->x;
    else
        return 0;
}

int CLuaRobot::GetUnitY(int id)
{
    BATTLEUNIT_APPEAR* info = GetRobot()->GetBattleUnit(id);
    if(info)
        return info->y;
    else
        return 0;
}

int CLuaRobot::GetOneEnemy()
{
    return GetRobot()->GetOneEnemy();
}

void CLuaRobot::Close()
{
    GetRobot()->CloseNet();
}

void CLuaRobot::CallGM( const char* command )
{
    m_pCase->PushAction(CnNew GM_Action(m_pCase->GetRobot(), command));
}

void CLuaRobot::UseSkill( int objectId, int skillId )
{
    GetRobot()->UseSkill(objectId, skillId);
}

void CLuaRobot::ChangeName( const char* name )
{
    m_pCase->PushAction(CnNew NameAction(m_pCase->GetRobot(), name));
}

int CLuaRobot::GetTime()
{
    return g_GameTime;
}

const char* CLuaRobot::GetAccount()
{
    return GetRobot()->GetAccount().c_str();
}

int CLuaRobot::GetRandomDungeonID()
{
    std::vector<uint32> DungeonIDList = RobotConfig::Instance()->GetConfigFileMap();
    uint32 index = Crown::SDRandom(DungeonIDList.size()-1);
    return DungeonIDList[index];
}

int CLuaRobot::GetRandomWorldMapID()
{
    std::vector<uint32> DungeonIDList = RobotConfig::Instance()->GetWorldMapDungoen();
    uint32 index = Crown::SDRandom(DungeonIDList.size()-1);
    return DungeonIDList[index];
}

int CLuaRobot::GetRandomPlayerSkillID()
{
    std::vector<uint32> SkillIDs = RobotConfig::Instance()->GetSkillIDs();
    uint32 SkillID = SkillIDs[Crown::SDRandom(SkillIDs.size()-1)];
    return SkillID;
}

int CLuaRobot::GetBagItemNum(int itemid)
{
   return GetRobot()->GetBagItemNum(itemid);
}
int CLuaRobot::GetMsgIDCount()
{
    return sizeof(gClientMsgIDSet)/sizeof(uint32);
}

int CLuaRobot::GetMsgID(int i)
{
    uint32 count = sizeof(gClientMsgIDSet)/sizeof(uint32);
    if(i < 0) i = 0;
    if(i >= (int32)count) i = (int32)count-1;
    return gClientMsgIDSet[i];
}

int CLuaRobot::GetRandomMsgID()
{
    uint32 count = sizeof(gClientMsgIDSet)/sizeof(uint32);
    uint32 index = Crown::SDRandom(count);
    return gClientMsgIDSet[index];
}

void DoRidePetAction( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
{
    pCase->PushAction(CnNew RidePetAction(pCase->GetRobot(), arg1 , arg2,arg3));
}

//void DoRideMarkAction( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
//{
//    pCase->PushAction(CnNew RideMarkAction(pCase->GetRobot(), arg1 , arg2,arg3));
//}

void DoChatAction( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
{
    pCase->PushAction(CnNew ChatAction(pCase->GetRobot(), arg1,arg2,arg3));
}
void DoPassiveSkillActionn( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
{
    pCase->PushAction(CnNew PassiveSkillAction(pCase->GetRobot(), arg1,arg2));
}

void DoTouchActoin( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
{
    pCase->PushAction(CnNew TouchStartAction(pCase->GetRobot(), arg1));
}

void DoSkillAction( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
{
    pCase->PushAction(CnNew SkillAction(pCase->GetRobot(), arg1));
}

void DoDance( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
{
    pCase->PushAction(CnNew DanceStart(pCase->GetRobot()));
}

void DoOnlineReward( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
{
    pCase->PushAction(CnNew OnlineRewardAction(pCase->GetRobot()));
}

void DoMoveRandom( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
{
    pCase->PushAction(CnNew MoveRandomAction(pCase->GetRobot()));
}

void DoMove( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3 )
{
    pCase->PushAction(CnNew MoveAction(pCase->GetRobot(), arg1, arg2));
}

void DoWait( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3  )
{
    pCase->PushAction(CnNew WaitAction(pCase->GetRobot(), arg1));
}

void DoJumpDungeon( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3  )
{
    pCase->PushAction(CnNew JumpDungeonAction(pCase->GetRobot(), arg1, arg2));
}

void DoEquipLeveUp(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3 )
{   // arg1 表示装备类型，agr2 表示装备是升级，升星，还是品质
    pCase->PushAction(CnNew Equip_Action(pCase->GetRobot(), arg1, arg2, arg3));
}

void DoBuyObject(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3 )
{
    pCase->PushAction(CnNew Buy_Action(pCase->GetRobot(), arg1, arg2) );
}

void DoFriend(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3)
{
    pCase->PushAction(CnNew Friend_Action(pCase->GetRobot(),arg1, arg2, arg3) );
}

void DoFollowpet(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3)
{
    pCase->PushAction(CnNew Followpet_Aciton(pCase->GetRobot(), arg1, arg2,arg3));
}
void DoTask(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3)
{
    pCase->PushAction(CnNew Task_Aciton(pCase->GetRobot(), arg1, arg2));
}
void DoHorcope(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3)
{
    pCase->PushAction(CnNew Horoscope_Action(pCase->GetRobot(), arg1, arg2));
}
void DoAchiveAction( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
{
    pCase->PushAction(CnNew Achive_aciton(pCase->GetRobot()));
}

void DoItem(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2,int32 arg3)
{
    pCase->PushAction(CnNew Item_Action(pCase->GetRobot(), arg1, arg2, arg3));
}

void DoSendPacketAction( CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2 , int32 arg3 )
{
    pCase->PushAction(CnNew Send_Packet_Action(pCase->GetRobot(), arg1, arg2));
}

void DoGuildAction(CaseLua* pCase, const std::string& actionName, int32 arg1, int32 arg2, int32 arg3)
{
    pCase->PushAction(CnNew Guild_CreateAction(pCase->GetRobot(), arg1, arg2, arg3));
}

