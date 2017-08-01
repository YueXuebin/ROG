#include "robot_pch.h"
#include "robot.h"
#include "robot_config.h"
#include "main.h"
#include "case_define.h"

extern bool mainLoop;

Robot::Robot()
{
    Clear();

    m_pCase = CnNew CaseLua(this);
    //switch()
    //{
    //case 1:
    //    m_pCase = CnNew CaseLua(this);
    //    break;
    //default:
    //    m_pCase = CnNew Case1(this);
    //    break;
    //}

}

Robot::~Robot()
{
    CnDelete m_pCase;

    CloseNetConnect();

    m_state = kStateUnint;
}

void Robot::Clear()
{
    m_svrIP = "";
    m_svrPort = 0;
    
    m_account = "";

    m_prop.Clear();
    //m_equip.Clear();
    //m_bag.Clear();
    m_task.Clear();

    m_state = kStateInit;
    m_clientProxy = NULL;

}

void Robot::Send(uint32 msgID, void* data)
{
    if(m_clientProxy)
        m_clientProxy->Send(msgID, data);
}

bool Robot::SendPacket(Net_Packet* packet)
{
    if(!m_clientProxy)
        return false;

    return m_clientProxy->SendPacket(packet);
}

void Robot::Update(uint32 tickcount, uint32 deltacount)
{
    switch(GetState())
    {
    case kStateInit:
        CreateNetConnect();
        break;
    case kStateConnected:
        break;
    case kStateInGame:
        {
            m_pCase->Update(tickcount, deltacount);
        }
        break;
        // 需要忽略的状态
    case kStateConnectReq:
    case kStateCreateSend:
    case kStateLoginSend:
    case kStateNetClosed:
    case kStateLoadPlayerSend:
    case kStateLoadPlayer:
        break;
    case kStateClose:
    case kStateBreakLine:
    case kStateUnint:
        break;
    default:
        CnInfo("except state: %d\n", GetState());
        CnAssert(false);
        break;
    }
}

void Robot::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data, void* userdata)
{
    Robot* pRobot = (Robot*)userdata;

    if(!pRobot) return;

    switch(msgID)
    {
    case MSG_NET_CONNECT:
        {
            //CnDbg("connect succeed\n");
            CnAssert(pRobot->GetState() == kStateConnectReq);
            pRobot->SetState(kStateConnected);
            pRobot->SendEncryptConnectReq();
        }
        break;
    case MSG_NET_ERROR:
        {
            CnInfo("%s connect failed\n", pRobot->GetAccount().c_str());
            pRobot->SetState(kStateClose);
        }
        break;
    case MSG_NET_CLOSE:
        {
            CnInfo("%s net closed\n", pRobot->GetAccount().c_str());
            pRobot->SetState(kStateClose);
        }
        break;
    case MSG_ENCRYPT_PUBLIC_KEY_NTF:
        pRobot->RecvEncryptPublicKeyNet(data);
        break;
    case MSG_LOGIN_GAME_ACK:
        pRobot->RecvLoginAck(data);
        break;
    case MSG_LOAD_PLAYER_ACK:
        pRobot->RecvLoadPlayerAck(data);
        break;
    case MSG_CREATE_PLAYER_ACK:
        pRobot->RecvCreateAck(data);
        break;
    case MSG_PLAYER_INFO:
        pRobot->RecvPlayerInfo(data);
        break;
    case MSG_CHANGE_DUNGEON_ACK:
        pRobot->RecvJumpMap(data);
        break;
    case MSG_BATTLEUNIT_APPEAR_LIST:
        pRobot->RecvBattleUnitAppear(data);
        break;
    case MSG_BATTLEUNIT_DISAPPEAR:
        pRobot->RecvBattleUnitDisappear(data);
        break;
    case MSG_SCENEOBJECT_APPEAR_LIST:
        pRobot->RecvSceneObjectAppear(data);
        break;
    case MSG_SCENEOBJECT_DISAPPEAR:
        pRobot->RecvSceneObjectDisappear(data);
        break;
    case MSG_PROPERTY_LIST_NTF:
        pRobot->RecvPropertyChange(data);
        break;
    case MSG_CREATURE_MOVE_LIST:
        pRobot->RecvOtherMove(data);
        break;
    default:
        break;
    }

    if(pRobot->GetState() == kStateInGame)
        pRobot->m_pCase->OnRecv(msgID, data);
}

void Robot::CreateNetConnect()
{
    m_clientProxy = CreateTransferClientProxy(m_svrIP.c_str(), m_svrPort, &OnRecv, this, false);
    CnAssert(GetState() == kStateInit);
    SetState(kStateConnectReq);
}

void Robot::CloseNetConnect()
{
    if(m_clientProxy)
    {
        m_clientProxy->Close();
        CnDelete m_clientProxy;
        m_clientProxy = NULL;
    }
}

void Robot::SendEncryptConnectReq()
{
    NONE msg;
    Send(MSG_ENCRYPT_CONNECT_REQ, &msg);
}

void Robot::RecvEncryptPublicKeyNet( void* data )
{
    ENCRYPT_TOKEN* pToken = (ENCRYPT_TOKEN*)data;
    pToken->key ^= ENCRYPT_MAGIC_NUMBER;

    // 创建私有key
    uint32 private_key = Crown::SDRandom(RAND_MAX);
    private_key = 1234;

    m_PublicKey = pToken->key;
    m_PrivateKey = private_key;

    ENCRYPT_TOKEN sendToken;
    sendToken.key = m_PrivateKey ^ m_PublicKey;

    Send(MSG_ENCRYPT_PRIVATE_KEY_NTF, &sendToken);

    m_Encrptyer.SetPublicKey(m_PublicKey);
    m_Encrptyer.SetPrivateKey(m_PrivateKey);
    m_clientProxy->SetEncrpty(&m_Encrptyer);

    // 在请求登陆
    SendLoginReq();
}

void Robot::SendLoginReq()
{
    LOGIN_GAME_REQ sendmsg;
    sendmsg.version = VERSION_FULL;

    uint32 regionID = Crown::SDRandom(1, 5);
    char regionChar[100];
    Crown::SDItoa(regionChar, 100, regionID);
    if(m_account.length() < NAME_MIN_LEN)
        sendmsg.token = "username=robot_"+m_account+"&server_id="+regionChar;
    else
        sendmsg.token = "username="+m_account+"&server_id="+regionChar;
    sendmsg.net_compress = 1;           // 协议压缩
    Send(MSG_LOGIN_GAME_REQ, &sendmsg);
    SetState(kStateLoginSend);
}

void Robot::RecvLoginAck(void* data)
{
    LOGIN_GAME_ACK* recvmsg = (LOGIN_GAME_ACK*)data;
    if(!recvmsg) return;
    if(recvmsg->errcode == ERR_SUCCEED)
    {   // 登陆成功
        SendLoadPlayerReq();
        SetState(kStateLoadPlayerSend);
    }
    else if(recvmsg->errcode == ERR_UNFOUND_PLAYER)
    {   // 没有角色
        SendCreateReq();
        SetState(kStateCreateSend);
    }
    else if(recvmsg->errcode == ERR_SERVER_NO_READY)
    {
        CnDbg("%s login again\n", GetAccount().c_str());
        SendLoginReq();
    }
    else
    {
        CnError("%s Login error\n", GetAccount().c_str());
        SetState(kStateUnint);
    }
}

void Robot::SendLoadPlayerReq()
{
    LOAD_PLAYER_REQ req;
    req.character_id = 0;
    Send(MSG_LOAD_PLAYER_REQ, &req);
}

void Robot::RecvLoadPlayerAck(void* data)
{
    ERROR_CODE* pErrorCode = (ERROR_CODE*)data;
    if(pErrorCode->errcode == ERR_SUCCEED)
    {
        
    }
    else if(pErrorCode->errcode == ERR_UNFOUND_PLAYER)
    {   // 没有角色
        SendCreateReq();
        SetState(kStateCreateSend);
    }
    else
    {   // 加载角色信息失败


    }
}

void Robot::SendCreateReq()
{
    CREATE_PLAYER_REQ sendmsg;
    sendmsg.token = m_account;
    if(m_account.length() < NAME_MIN_LEN)
        sendmsg.player_name = "robot_"+m_account;
    else
        sendmsg.player_name = m_account;
    sendmsg.sex = rand() % SEX_FEMALE + 1;
    sendmsg.career = rand() % CAREER_PREIST + 1;
    Send(MSG_CREATE_PLAYER_REQ, &sendmsg);

    //CnInfo("CreatePlayer: %s\n", sendmsg.token.c_str());
}

void Robot::RecvCreateAck(void* data)
{
    ERROR_CODE* pErrorCode = (ERROR_CODE*)data;
    if(pErrorCode->errcode == ERR_SUCCEED)
    {
        CnInfo("%s create player succeed!\n", GetAccount().c_str());
        SendLoadPlayerReq();
        SetState(kStateLoadPlayerSend);
    }
    else
    {
        CnInfo("%s create player failed %d\n", GetAccount().c_str(), pErrorCode->errcode);
        SetState(kStateClose);
    }
}

void Robot::RecvPlayerInfo(void* data)
{
    PLAYER_INFO* recvmsg = (PLAYER_INFO*)data;
    if(!recvmsg) return;

    CnAssert(GetState() != kStateInGame);
    SetState(kStateInGame);

    m_prop = recvmsg->prop;
    //m_equip = recvmsg->equip;
    //m_bag = recvmsg->bag;
    m_task = recvmsg->task;

    m_DungeonID = DUNGEON_ID(recvmsg->prop.unionmap_id);
    m_ChannelID = CHANNEL_ID(recvmsg->prop.unionmap_id);

    CnDbg("%s login! port=%d\n", m_account.c_str(), m_svrPort);
    m_pCase->OnEnterGame();
}

void Robot::RecvJumpMap(void* data)                         //接收跳转地图请求
{
    CHANGE_DUNGEON_ACK* jumpmap = (CHANGE_DUNGEON_ACK*) data;
    if (jumpmap->errcode == ERR_SUCCEED)
    {
        m_DungeonID = DUNGEON_ID(jumpmap->union_id);
        m_ChannelID = CHANNEL_ID(jumpmap->union_id);

        m_BattleUnits.clear();
        m_SceneObjectUnits.clear();

        // 通知游戏开始
        GAME_START sendstart;
        sendstart.start = 1;
        Send(MSG_GAME_START_REQ, &sendstart);
        //CnInfo("%s jump %d \n",GetAccount().c_str(),DUNGEON_ID(jumpmap->union_id));
    }
    else
        CnError("Error >> %s jump dungeon %d error, error code: %d \n", GetAccount().c_str(), DUNGEON_ID(jumpmap->union_id), jumpmap->errcode);
}

void Robot::RecvSceneObjectAppear( void* data )                         //接收场景旗帜数据
{
    SCENEOBJECT_APPEAR_LIST *sceneObject = (SCENEOBJECT_APPEAR_LIST*)data;

    SceneObject_unit unit;
    
    for (uint32 i= 0; i < sceneObject ->trigger_list.size(); i++)
    {
        unit.info = sceneObject->trigger_list[i].base;
        CnAssert(m_SceneObjectUnits.find(unit.info.object_id) == m_SceneObjectUnits.end());        // 新出现的对象必定不存在
        m_SceneObjectUnits[unit.info.object_id] = unit;                  //triggerID
        CnDbg("triggerID: %d\n", unit.info.arg1);
    }
    for (uint32 i= 0; i < sceneObject ->touch_list.size(); i++)
    {
        unit.info = sceneObject->touch_list[i].base;
        CnAssert(m_SceneObjectUnits.find(unit.info.object_id) == m_SceneObjectUnits.end());        // 新出现的对象必定不存在
        m_SceneObjectUnits[unit.info.object_id] = unit;                  //touchID
        CnDbg("touchID: %d\n", unit.info.arg1);
    }
    for (uint32 i= 0; i < sceneObject ->drop_object_list.size(); i++)
    {
        unit.info = sceneObject->drop_object_list[i].base;
        CnAssert(m_SceneObjectUnits.find(unit.info.object_id) == m_SceneObjectUnits.end());        // 新出现的对象必定不存在
        m_SceneObjectUnits[unit.info.object_id] = unit;                  //dropObjectID
        CnDbg("dropObjectID: %d\n", unit.info.arg1);
    }
}

void Robot::RecvSceneObjectDisappear(void* data)
{
    SCENEOBJECT_DISAPPEAR* pData = (SCENEOBJECT_DISAPPEAR*)data;

    SceneObjectUnitMap::iterator iter = m_SceneObjectUnits.find(pData->object_id);
    CnAssert(iter != m_SceneObjectUnits.end());     // 消失的对象，必定存在

    if (iter == m_SceneObjectUnits.end())
    {
        CnFatal("disapper sceneObj id %d no exist \n",pData->object_id);
    }
    else
    {
        m_SceneObjectUnits.erase(iter);
    }
}

void Robot::RecvBattleUnitAppear( void* data )
{
    BATTLEUNIT_APPEAR_LIST* battleUnitList = (BATTLEUNIT_APPEAR_LIST*) data;

    BattleUnit unit;
    for (uint32 i = 0; i < battleUnitList->boss_list.size(); i++)
    {
        unit.info = battleUnitList->boss_list[i].base;
        CnAssert(m_BattleUnits.find(unit.info.object_id) == m_BattleUnits.end());       // 出现的对象必不存在
        m_BattleUnits[unit.info.object_id] = unit;
    }
    for (uint32 i = 0; i < battleUnitList->monster_list.size(); i++)
    {
        unit.info = battleUnitList->monster_list[i].base;
        CnAssert(m_BattleUnits.find(unit.info.object_id) == m_BattleUnits.end());
        m_BattleUnits[unit.info.object_id] = unit;
    }
    for (uint32 i = 0; i < battleUnitList->offline_player_list.size(); i++)
    {
        unit.info = battleUnitList->offline_player_list[i].base;
        CnAssert(m_BattleUnits.find(unit.info.object_id) == m_BattleUnits.end());
        m_BattleUnits[unit.info.object_id] = unit;
    }
    for (uint32 i = 0; i < battleUnitList->pet_list.size(); i++)
    {
        unit.info = battleUnitList->pet_list[i].base;
        CnAssert(m_BattleUnits.find(unit.info.object_id) == m_BattleUnits.end());
        m_BattleUnits[unit.info.object_id] = unit;
    }
    for (uint32 i = 0; i < battleUnitList->player_list.size(); i++)
    {
        unit.info = battleUnitList->player_list[i].base;
        CnAssert(m_BattleUnits.find(unit.info.object_id) == m_BattleUnits.end());
        m_BattleUnits[unit.info.object_id] = unit;
    }
}

void Robot::RecvBattleUnitDisappear( void* data )
{
    BATTLEUNIT_DISAPPEAR* pData = (BATTLEUNIT_DISAPPEAR*)data;
    BattleUnitMap::iterator iter = m_BattleUnits.find(pData->object_id);
    CnAssert(iter != m_BattleUnits.end());          // 消失的对象必定存在

    if(iter == m_BattleUnits.end())
    {
        CnFatal("disappear id %d no exist\n", pData->object_id);
    }
    else
    {
        m_BattleUnits.erase(iter);
    }
}

void Robot::RecvPropertyChange( void* data )
{
    PROPERTY_LIST_NTF* pData = (PROPERTY_LIST_NTF*)data;
    BATTLEUNIT_APPEAR* pInfo = GetBattleUnit(pData->object_id);
    if(pInfo)
    {
        for(uint32 i=0; i<pData->list.size(); i++)
        {
            PROPERTY_NTF& ntf = pData->list[i];
            switch(ntf.property_id)
            {
            case PROP_GROUP:
                pInfo->group = ntf.value;
                break;
            }
        }
    }
    else if(m_prop.object_id == pData->object_id)
    {
        for(uint32 i=0; i<pData->list.size(); i++)
        {
            PROPERTY_NTF& ntf = pData->list[i];
            switch(ntf.property_id)
            {
            case PROP_GROUP:
                m_prop.group = ntf.value;
                break;
            }
        }
    }
}

void Robot::RecvOtherMove( void* data )
{
    CREATURE_MOVE_LIST* pData = (CREATURE_MOVE_LIST*)data;
    for(uint32 i=0; i<pData->list.size(); i++)
    {
        CREATURE_MOVE& m = pData->list[i];
        BATTLEUNIT_APPEAR* info = GetBattleUnit(m.object_id);
        if(info)
        {
            info->x = m.x;
            info->y = m.y;
        }
    }
}

void Robot::UseSkill( int32 objectId, int32 skillId )
{
    BATTLEUNIT_APPEAR* targetUnit = GetBattleUnit(objectId);
    if(!targetUnit)
        return;

    USE_SKILL_REQ useSkill;
    useSkill.skill_args.skill_mode = 1;
    useSkill.skill_obj_id = 0;
    useSkill.target.target_id = targetUnit->object_id;
    useSkill.target.target_x = targetUnit->x;
    useSkill.target.target_y = targetUnit->y;
    useSkill.skill_id = skillId;
    Send(MSG_USE_SKILL_REQ, &useSkill);
}


int32 Robot::GetBagItemNum(uint32 itemid)
{
    std::vector<ITEM_INFO>::iterator iter;
    //for(iter = m_bag.list.begin(); iter != m_bag.list.end(); iter++)
    //{
    //    if(itemid == iter->id)
    //        return iter->num;
    //}
    return 0;
}
BATTLEUNIT_APPEAR* Robot::GetBattleUnit(uint32 objectId)
{
    BattleUnitMap::iterator iter = m_BattleUnits.find(objectId);
    if(iter == m_BattleUnits.end())
        return NULL;
    return &iter->second.info;
}

uint32 Robot::GetOneEnemy()
{
    for(BattleUnitMap::iterator iter = m_BattleUnits.begin(); iter != m_BattleUnits.end(); iter++)
    {
        const BATTLEUNIT_APPEAR& info = iter->second.info;
        if(info.group !=  m_prop.group)
        {
            return info.object_id;
        }
    }
    return 0;
}

SCENEOBJECT_APPEAR* Robot::GetSceneObjUnit(uint32 touchId)
{
    SceneObjectUnitMap::iterator iter = m_SceneObjectUnits.find(touchId);
    if(iter == m_SceneObjectUnits.end())
        return NULL;
    return &iter->second.info;
}

uint32 Robot::GetSceneObj()
{
    for (SceneObjectUnitMap::iterator iter = m_SceneObjectUnits.begin();iter != m_SceneObjectUnits.end();iter++)
    {
        const SCENEOBJECT_APPEAR&info = iter->second.info;
        CnDbg(" GetSceneObj  arg1=%d  arg2=%d group=%d \n",info.arg1,info.arg2,m_prop.group);
        if (info.arg2 != m_prop.group)
        {
            return info.arg1;
        }
    }
    return 0;
}

void Robot::CloseNet()
{
    CloseNetConnect();
    SetState(kStateClose);
}










