#include "gameholder_pch.h"
#include "item_manager.h"
#include "player.h"
#include "event_define.h"
#include "ride_manager.h"
#include "scene.h"
#include "trade_manager.h"
#include "game_util.h"
#include "property_set_calculater.h"
#include "ride_config.h"
#include "ridetrain.h"
#include "vip_config.h"
#include "gameplaytimes_notifier.h"
#include "guild_manager.h"
#include "battle_state_manager.h"
#include "config_manager.h"



int32 ComparObjNum(const RideObject& objx, const RideObject& objy)  
{
    return objx.second <objy.second;
}

//从小到大比较品质
int32 ComparQuality(const REWARD_ITEM& objx, const REWARD_ITEM& objy)  
{
    return objx.id <objy.id;
}

RideManager::RideManager(Player* player) :
m_player(player)
{
    Clear();
    m_ridetrain = CnNew RideTrain(m_player,this);
}

RideManager::~RideManager(void)
{
    CnDelete m_ridetrain;
    m_ridetrain = NULL;
}

void RideManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_INIT_RIDE_PET_REQ:
        {
            OnInitRidePet(data);
        }
        break;
    //放生或骑乘骑宠
    case MSG_RIDE_PET_RIDEORFREE_REQ:
        {
            RideOrFree(data);
        }
        break;
    //坐骑刻印
    case MSG_RIDE_PET_MARK_REQ:
        {
            MinMarkRide(data);
        }
        break;
    //吸收请求
    case MSG_RIDE_PET_ABSORB_REQ:
        {
            RideAbsorb(data);
        }
        break;
    //开启栅栏请求
    case MSG_RIDE_PET_OPENfILED_REQ:
        {
            //暂时没有用没有扣钱
            OpenFiled(data);
        }
        break;
    //刷出骑宠物请求
    case MSG_RIDE_PET_UPDATE_REQ:
        {
            GetUpdateRide(data);
        }
        break;
    //捕获请求
    case MSG_RIDE_PET_CAPTURE_REQ:
        {
            CaptureRide(data);
        }
        break;
    //骑宠赠送礼物请求
    case MSG_RIDE_PET_SENDGIF_REQ:
        {
            SendGif(data);
        }
        break;
    //骑宠刻印一键卸下请求
    case MSG_RIDE_PET_CLEAR_MARK_REQ:
        {
            ClearMarkRide(data);
        }
        break;
    default:
        {
            if (m_ridetrain)
                m_ridetrain->OnRecv(msgID,data);
        }
        break;
    }
}

void RideManager::Clear() 
{
    m_ride_id = 0; 
    m_startTime = 0;
    m_level = 0;
    m_randomRide_pet.clear();

    m_fieldnum =  Ride_Config::Instance()->GetInitField();
    m_enum_capture = Capture_Init;

    m_fieldride_id = 0;
    m_lastRideid = 0;
    m_ridetrain = NULL;
}

void RideManager::RandomTypeOject(int32 type)
{
    int32 numobject = 0;//要生成的物品的个数

    if(type == RIDE_PET_ORD)
    {
        int32 blue = Ride_Config::Instance()->GetOrdBlue(); //生成蓝色物品的概率50%
        int32 purple = Ride_Config::Instance()->GetOrdPureple();//生成紫色物品的概率40%
        int32 goldint = Ride_Config::Instance()->GetOrdGold();//生成金色物品的概率10%

        numobject = Ride_Config::Instance()->GetOrdRandnum();
        int32 defaultnum =Ride_Config::Instance()->GetRideOrdUpdate();//默认返点返回蓝色多少点
        OnUpdateReturnPoint(RIDE_PET_ORD);//大于一定的个数，返点
        RandomObject(blue,purple,goldint,numobject,defaultnum);
    }

    if(type == RIDE_PET_SER)
    {
        numobject = Ride_Config::Instance()->GetSerRandnum();
        int32 blue = Ride_Config::Instance()->GetSerBule(); //生成蓝色物品的概率15%
        int32 purple = Ride_Config::Instance()->GetSerPureple(); //生成紫色物品的概率50%
        int32 _gold = Ride_Config::Instance()->GetSerGold();//生成金色物品的概率35% 
        int32 defaultnum = Ride_Config::Instance()->GetRideSerUpdate();
        OnUpdateReturnPoint(RIDE_PET_SER);
        RandomObject(blue,purple,_gold,numobject,defaultnum);
    }
}

void RideManager::LoadRideInfo(const RIDE_INFO& info)
{
    m_ride_pet_list = info.ride_pet_list;

    // set default flag
    for (uint32 i = 0; i<m_ride_pet_list.size(); i++)
    {
        m_ride_pet_list[i].b_mark = RIDE_PET_NO_MARK;
        m_ride_pet_list[i].b_train = ON_IDLE;
    }

    m_randomRide_pet = info.ride_random_list;
    m_randomObject = info.object_random_list;

    m_fieldnum = info.filed_num;

    b_usefree = info.b_free;
    m_free_time = info.time;

    // set mark flag
    for (uint32 i = 0; i<info.mark_list.size(); i++)
    {
        uint32 petId = info.mark_list[i].pet_id;
        for (uint32 j = 0; j<m_ride_pet_list.size(); j++)
        {
            if (petId == m_ride_pet_list[j].pet_id)
                m_ride_pet_list[j].b_mark = RIDE_PET_MARK;
        }
    }
    m_markList = info.mark_list;

    m_rideAltasList = info.ride_altas_list;

    // set train flag
    for (uint32 i = 0; i<info.ride_train_list.size(); i++)
    {
        uint32 petId = info.ride_train_list[i].pet_id;
        for (uint32 j = 0; j<m_ride_pet_list.size(); j++)
        {
            if (petId == m_ride_pet_list[j].pet_id)
                m_ride_pet_list[j].b_train = ON_TRAINING;
        }
    }

    if (m_ridetrain)
        m_ridetrain->SetTrainInfo(info.ride_train_list);

    m_ridequalitylist = info.ride_quality_list;

    SetRidePetID(info.ride_id);
}

void RideManager::FillRideInfo(RIDE_INFO& info)
{
//     info.ride_battle_power = m_player->GetPropertySetCalculater()->GetBattlePowerByRideMark() 
//         + m_player->GetPropertySetCalculater()->GetBattlePowerByRideAltas();

    info.ride_id = m_ride_id;           // 坐骑ID
    // 坐骑列表
    info.ride_pet_list = m_ride_pet_list;
    info.ride_random_list = m_randomRide_pet;
    info.object_random_list = m_randomObject;
    info.filed_num = m_fieldnum;
    info.b_free = b_usefree;
    info.time = m_free_time;
    info.mark_list = m_markList;
    info.ride_altas_list = m_rideAltasList;

    if (m_ridetrain)
    {
        info.ride_train_list = m_ridetrain->GetTrainInfo();
    }
    info.ride_quality_list = m_ridequalitylist;
}

void RideManager::UseMoneyWay(uint32 itemType, uint32 itemID, uint32 itemNum)
{
    if (m_ridetrain)
        m_ridetrain->UseMoneyWay(itemType,itemID,itemNum);
}

void RideManager::OnUpdateReturnPoint(int32 type)
{
    if(m_ridequalitylist.size()<= 0)
    {
        RIDE_QUALITY_INFO ride_info;
        ride_info.Clear();
        ride_info.type = RIDE_PET_ORD;
        ride_info.id =  QUALITY_PURPLE;
        ride_info.num = 0;
        m_ridequalitylist.push_back(ride_info);

        RIDE_QUALITY_INFO ride_info1;
        ride_info1.Clear();
        ride_info1.type = RIDE_PET_ORD;
        ride_info1.id =  QUALITY_GOLD;
        ride_info1.num = 0;
        m_ridequalitylist.push_back(ride_info1);

        RIDE_QUALITY_INFO ride_info2;
        ride_info2.Clear();
        ride_info2.type = RIDE_PET_SER;
        ride_info2.id =  QUALITY_PURPLE;
        ride_info2.num = 0;
        m_ridequalitylist.push_back(ride_info2);

        RIDE_QUALITY_INFO ride_info3;
        ride_info3.Clear();
        ride_info3.type = RIDE_PET_SER;
        ride_info3.id =  QUALITY_GOLD;
        ride_info3.num = 0;
        m_ridequalitylist.push_back(ride_info3);
    }

    int32 curnum  =0;
    RideQualityList::iterator ridequality;
    for (ridequality = m_ridequalitylist.begin();ridequality!= m_ridequalitylist.end();ridequality++)
    {
        if(ridequality->type == type)
        {
            if(type == RIDE_PET_ORD)
            {
                OrdReturnPoint(*ridequality);
            }
            else if (type == RIDE_PET_SER)
            {
                SerReturnPoint(*ridequality);
            }

            ridequality->num =   ridequality->num + 1;
        }
    }
}

void RideManager::OrdReturnPoint(RIDE_QUALITY_INFO& ridequality)
{
    int32 ordpurletime = Ride_Config::Instance()->GetOrdTimePuple();
    if(ordpurletime)
    {
        if (ridequality.id == QUALITY_PURPLE)
        {
            if(ridequality.num>= ordpurletime)
            {
                ridequality.num = 0;
                //紫的个数
                int32 ordpurple =Ride_Config::Instance()->GetOrdReutrnPurple();
                //紫色的
                SetAddRandomObject(BUG,ordpurple);
            }
        }
    }

    int32 ordgoldtime = Ride_Config::Instance()->GetOrdTimeGold();
    if(ordgoldtime)
    {
        if (ridequality.id == QUALITY_GOLD)
        {
            if(ridequality.num>= ordgoldtime)
            {
                ridequality.num = 0;
                //金的个数
                int32 ordgold =Ride_Config::Instance()->GetOrdReturnGold();
                SetAddRandomObject(HONEY,ordgold);
            }
        }
    }
}

void RideManager::SerReturnPoint(RIDE_QUALITY_INFO& ridequality)
{
    int32 ordpurletime = 0;
    ordpurletime = Ride_Config::Instance()->GetSerTimePurple();
    if(ordpurletime)
    {
        if (ridequality.id == QUALITY_PURPLE)
        {
            if(ridequality.num>= ordpurletime)
            {
                ridequality.num = 0;
                //紫的个数
                int32 ordpurple =Ride_Config::Instance()->GetSerReturnPurple();
                //紫色的
                SetAddRandomObject(BUG,ordpurple);
            }
        }
    }

    int32 ordgoldtime  = Ride_Config::Instance()->GetSerTimeGold();
    if(ordgoldtime)
    {
        if (ridequality.id == QUALITY_GOLD)
        {
            if(ridequality.num>= ordgoldtime)
            {
                ridequality.num = 0;
                //金的个数
                int32 ordgold =Ride_Config::Instance()->GetSerReturnGold();
                //紫色的
                SetAddRandomObject(HONEY,ordgold);
            }
        }
    }
}

void RideManager::OnInitRidePet(void* data)
{
    INIT_RIDE_PET_ACK sendMsg;

    RIDE_PET_LIST ridelist;
    ridelist.opt_flag = OPT_INIT_FLAG;
    ridelist.ride_pet_list = m_ride_pet_list;
    sendMsg.ride_pet_list = ridelist;

    sendMsg.ride_random_list = m_randomRide_pet;

    sendMsg.object_random_list = m_randomObject;

    // 
    if(m_randomRide_pet.size() >= 0)
        m_enum_capture = NoCapture;

    //刻印列表
    if(m_markList.size()<=0)
    {
        Map_MarkCondit::iterator mapmark;
        Map_MarkCondit curMark =  Ride_Config::Instance()->GetMarkCondition();
        for(mapmark = curMark.begin(); mapmark != curMark.end(); mapmark++)
        {
            AddMarkPos(mapmark->first);
        }
    }

    sendMsg.mark_list = m_markList;

    //骑宠图鉴
    if(m_rideAltasList.size() <=0)
    {
        Map_Atlas::iterator mapatlas; 

        Map_Atlas curAltas = Ride_Config::Instance()->GetMapAltas();
        for (mapatlas = curAltas.begin(); mapatlas != curAltas.end(); mapatlas++)
        {
            AddAtlas(mapatlas->first);
        }
    }


    sendMsg.ride_altas_list = m_rideAltasList;

    if(m_ridetrain)
    {
        m_ridetrain->TrainInit();
        sendMsg.ride_train_list = m_ridetrain->GetTrainInfo();
    }

    uint32 curtime = (uint32)Crown::SDNow().GetTimeValue();
    uint32 _time = DAY_SECONDS;//1天多少秒

    //判读每天得到一次免费升级机会
    if(curtime >=m_free_time)
    {
        b_usefree = RIDE_PET_FREE_NO;
    }
    sendMsg.b_free = b_usefree;

    NotifyNum();

    m_player->SendToGate(MSG_INIT_RIDE_PET_ACK, &sendMsg);
}

void RideManager::SetUseItemRidePetRecuit(int32 curid)
{
    uint32 pet_id = curid;
    if (!pet_id)
        return;

    AddRidePet(pet_id);
}

void RideManager::AddAtlas(uint32 altasid)
{
    RideAltasList::iterator iter_altas;
    bool b_find = false;
    for (iter_altas = m_rideAltasList.begin(); iter_altas!= m_rideAltasList.end(); iter_altas++)
    {
        if(altasid == iter_altas->id)
        {
            b_find = true;
            break;
        }
    }

    if(!b_find)
    {
        RIDE_ALTAS ridealtas;
        ridealtas.Clear();
        ridealtas.id = altasid;
        ridealtas.num = 0;
        ridealtas.state = NO_OPEN;
        m_rideAltasList.push_back(ridealtas);
    }
}

void RideManager::AddRidePet(uint32 pet_id)
{
    RIDE_PET_INFO* ridepet = 0;
    RIDE_PET_INFO rideinfo;

    RidePetTemplate* ridePetTmpl = Ride_Config::Instance()->GetRidePet(pet_id);
    if(!ridePetTmpl)
        return ;

    RIDE_PET_INFO* curpet = GetRideInfo(pet_id);
    if(curpet)
        return;

    RidePetLeveUpTempl* petTmpl = Ride_Config::Instance()->GetRideLeveTempl(pet_id, ridePetTmpl->level);
    if(!petTmpl)
        return ;

    rideinfo.pet_id = pet_id;
    rideinfo.name = ridePetTmpl->name;
    rideinfo.express = 0;
    rideinfo.b_get = RIDE_PET_CAPTURE;
    rideinfo.level = ridePetTmpl->level;
    rideinfo.state = RIDE_PET_REST;
    rideinfo.b_mark = RIDE_PET_NO_MARK;
    rideinfo.b_train = ON_IDLE;
    m_ride_pet_list.push_back(rideinfo);

    SendMarQuee(pet_id);

    ActiveAltas(pet_id);

    ridepet = &rideinfo;

    SendRidePetListNotify(OPT_ADD_FLAG, ridepet);

    SendCollectTask(pet_id);

    m_player->SendPlaySystemLog(PLAYSYSTEM_RIDE_OPEN, ridepet->pet_id);
}

void RideManager::SendCollectTask(uint32 pet_id)
{
    int32 quality = 0;
    uint32 num = 0;

    quality = GetRideQuality(pet_id);
    num = GetRideNumByQuality(quality);

    m_player->SendTaskEvent(TASK_EVENT_RIDE_NUM_BY_QUALITY, quality , num);
}

int32 RideManager::GetRideQualityNum(int32 quality)
{
    if(quality == ERR_QUALITY)
        return 0;

    int32 num = 0; 

    RidePetList::iterator ridelistiter;
    for (ridelistiter = m_ride_pet_list.begin(); ridelistiter != m_ride_pet_list.end(); ridelistiter++)
    {
        int32 curquality = SetRideQuality(ridelistiter->pet_id);
        if(curquality == quality)
        {
            num = num + 1;
        }
    }
    return num;
}

void RideManager::ActiveAltas(uint32 petid)
{
    int32 quality=  GetRideQuality(petid);

    RideAltasList updateAltas;
    updateAltas.clear();

    if(ERR_QUALITY != quality)
    {
        RideAltasList::iterator iteraltas;
        for (iteraltas = m_rideAltasList.begin(); iteraltas != m_rideAltasList.end(); iteraltas++)
        {
            AtlasStruct* atlastruct = Ride_Config::Instance()->GetAltasStruct(iteraltas->id);
            if(atlastruct)
            {
                if(iteraltas->state != OPEN)
                {
                    if(atlastruct->quality == quality)
                    {
                        iteraltas->num = iteraltas->num + 1;
                        if(iteraltas->num == atlastruct->num)
                        {
                            iteraltas->state = OPEN;
                            CollectQuality(iteraltas->id);
                            m_player->CalcProperty();
                        }
                        updateAltas.push_back(*iteraltas);
                    }
                }
            }
        }
    }

    RIDE_ALTAS_ACK sendMsg;
    sendMsg.opt_flag  = OPT_UPDATE_FLAG;
    sendMsg.ride_altas_list = updateAltas;
    m_player->SendToGate(MSG_RIDE_PET_ATLAS_ACK,&sendMsg);
}

void RideManager::CollectQuality(int32 atlaID)
{
    if(ConfigManager::Instance()->IsTW())
        return;

    SERVER_MARQUEE marquee;
    marquee.id = MARQUEE_GET_QUALITY_RIDE;
    marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
    marquee.parame_list.push_back(MarQueeRideCollect(atlaID));
    m_player->OnMarQuee(marquee);
}

void RideManager::OpenFiled(void* data)
{
    int32 m_num = Ride_Config::Instance()->GetStoneNum();
    if(m_player->GetMoneyPoint()< m_num)
    {
        return;
    }

    if(m_fieldnum > Ride_Config::Instance()->GetMaxField())
        return;

    m_fieldnum = m_fieldnum +1;

    /*  if(!m_player->SubMoneyPoint(m_num, CONSUME_RIDE_OPEN_FILED))
    {
    return;
    }*/
    SendOpenFieldAck();
}

void RideManager::RideOrFree(void* data)
{
    RIDE_PET_RIDEORFREE_REQ * recvMsg = (RIDE_PET_RIDEORFREE_REQ*)data;
    int32 _type = recvMsg->type;
    uint32 pet_id = recvMsg->pet_id;

    RIDE_PET_INFO ridepet;
    RidePetTemplate * ridePetTmpl = Ride_Config::Instance()->GetRidePet(pet_id);
    if(!ridePetTmpl)
        return;

    RIDE_PET_INFO* ride_pet = GetRideInfo(pet_id);
    if(!ride_pet)
        return;

    bool b_delete = false;
    switch(_type)
    {
    case RIDE_PET_RIDE:             // 骑上坐骑
        {
            if((ride_pet->state == RIDE_PET_RIDE))
            {
                return;
            }
            if(m_player->GetBattleStateManager()->IsInBattle())     // 战斗状态不能上坐骑
            {
                return;
            }

            ChangeRidePet(pet_id);
        }
        break;
    case RIDE_PET_REST:             // 取消坐骑
        {
            ChangeRidePet(0);
            ride_pet->state = RIDE_PET_REST;
        }
        break;
    default:
        CnWarn("rideorfree type:%d is no exist\n", _type);
        break;
    }

    if(!b_delete)
    {
        ridepet = (*ride_pet);
    }

    SendRideOrFreeAck(_type, ridepet);
}

void RideManager::MinMarkRide(void* data)
{
    //设置刻印的条件改变
    RIDE_PET_MARK_REQ * recvMsg = (RIDE_PET_MARK_REQ*)data;
    int32 _type = recvMsg->type;
    uint32 pet_id = recvMsg->pet_id;
    uint32 id = recvMsg->id;

    // 检查功能开启
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_RIDE_MARK, m_player) != ERR_SUCCEED)
    //{
    //    m_player->NotifyErrorCode(ERR_FUNCT_NOT_OPEN);
    //    return;
    //}

    if(!pet_id)
        return;
    m_updateRideList.clear();
    m_updatemarkList.clear();

    RidePetTemplate * ridePetTmpl = Ride_Config::Instance()->GetRidePet(pet_id);
    if(!ridePetTmpl)
        return;

    RIDE_PET_INFO* ride_pet = GetRideInfo(pet_id);
    if(!ride_pet)
        return;

    MarkCondit* markCondit = Ride_Config::Instance()->FindMarkCondit(id);

    if(!markCondit)
        return;

    if(_type == RIDE_PET_MARK)
    {
        if(ride_pet->b_mark == RIDE_PET_MARK)
            return;
    }

    if(markCondit->type != RIDE_GENERAL_FIELEDS)
    {
        if(ridePetTmpl->type != markCondit->type)
        {
            return;
        }
    }

    RIDE_MARKT* vecRideMark = GetMarkID(id);
    if(!vecRideMark)
        return;

    if(vecRideMark->state != OPEN)
        return;

    bool b_exe = false;

    if(_type == RIDE_PET_MARK)
    {
        if(vecRideMark->pet_id != 0)
        {
            SetNoMark(vecRideMark->pet_id);
        }
        ride_pet->b_mark = RIDE_PET_MARK;
        m_updateRideList.push_back(*ride_pet);
        SetMarkRide(id, pet_id);
        m_player->CalcProperty();
        b_exe = true;
    }
    else if(_type == RIDE_PET_NO_MARK)
    {
        //查找这个栏位上是否有这坐骑
        if(FieldIsHaveRide(id,ride_pet->pet_id))
        {
        ride_pet->b_mark = RIDE_PET_NO_MARK;
        m_updateRideList.push_back(*ride_pet);
        SetMarkRide(id, 0);
        m_player->CalcProperty();
        b_exe = true;
        }
    }

    if(!b_exe)
        return;

    RIDE_MARKT* changemark = GetMarkID(id);
    if(changemark)
        m_updatemarkList.push_back(*changemark);

    SendMarkAck();

//     uint32 battle = m_player->GetPropertySetCalculater()->GetBattlePowerByRideMark() 
//         + m_player->GetPropertySetCalculater()->GetBattlePowerByRideAltas();
    m_player->SendTaskEvent(TASK_EVENT_BATTLE_POWER_MOUNT, 0, 1 ,0);
}

//栏位上是否有这个坐骑
bool RideManager::FieldIsHaveRide(uint32 markid, uint32 pet_id)
{
    VecRideMarkList::iterator markLit;
    for (markLit = m_markList.begin(); markLit != m_markList.end(); markLit++)
    {
        if(markLit->id == markid)
        {
            if(markLit->pet_id == pet_id)
            {
                return true;
            }
        }
    }

    return false;
}

void RideManager::SetMarkRide(uint32 markid, uint32 pet_id)
{
    VecRideMarkList::iterator markLit;
    for (markLit = m_markList.begin(); markLit != m_markList.end(); markLit++)
    {
        if(markLit->id == markid)
        {
            markLit->pet_id = pet_id;
            m_player->SendPlaySystemLog(PLAYSYSTEM_RIDE_MARK, markid, pet_id);
            break;
        }
    }
}
void RideManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    switch(eventId)
    {
    case EVENT_RIDE_MARK_PET://活动完成后
        {
            RideMarkCondi();
        }
        break;
    case EVENT_CHANGE_VIP:
        {
            RideMarkCondi();
            if (!m_ridetrain)
                return;
            m_ridetrain->FieldOpenCondi();
            m_ridetrain->FieldCloseCondi();
        }
        break;
        //case  EVENT_RIDE_VIP_EXPIRE:
        //    {
        //        if (m_ridetrain)
        //            m_ridetrain->FieldCloseCondi();
        //    }
        // break;
    default:
        break;
    }
}

RIDE_MARKT* RideManager::GetMarkID(uint32 markid)
{
    RIDE_MARKT* ride_mark = 0;

    // bool b_find = false;
    VecRideMarkList::iterator markLit;
    for (markLit = m_markList.begin(); markLit != m_markList.end(); markLit++)
    {
        if(markLit->id == markid)
        {
            ride_mark = &(*markLit);
            break;
        }
    }

    return ride_mark;
}

void RideManager::SendMarkAck()
{
    RIDE_PET_LIST ridelist;
    if(m_updateRideList.size() >0 )
    {
        ridelist.opt_flag = OPT_UPDATE_FLAG;
        ridelist.ride_pet_list = m_updateRideList;
    }

    RIDE_PET_MARK_LIST marklist;
    marklist.opt_flag = OPT_UPDATE_FLAG;
    marklist.mark_list = m_updatemarkList;

    RIDE_PET_MARK_ACK sendMsg;
    sendMsg.ride_pet_list = ridelist;
    sendMsg.mark_pet_list = marklist;
    m_player->SendToGate(MSG_RIDE_PET_MARK_ACK,&sendMsg);
}

void RideManager::SetNoMark(uint32 petid)
{
    RidePetList::iterator ride_pet_;
    for (ride_pet_ = m_ride_pet_list.begin(); ride_pet_ != m_ride_pet_list.end(); ride_pet_++)
    {
        if (ride_pet_->pet_id == petid)
        {
            ride_pet_->b_mark = RIDE_PET_NO_MARK;
            m_updateRideList.push_back(*ride_pet_);
            break;
        }
    }
}

void RideManager::AddMarkPos(uint32 markid)
{
    RIDE_MARKT* markLit = GetMarkID(markid);
    if(markLit)
        return;

    MarkCondit* markCondit = Ride_Config::Instance()->FindMarkCondit(markid);
    if(!markCondit)
        return;

    RIDE_MARKT ride_mark;
    ride_mark.Clear();
    ride_mark.id = markid;
    ride_mark.pet_id = 0;

    if(markCondit->deafult == OPEN)
    {
        ride_mark.state = OPEN; 
    }
    else
    {
        ride_mark.state = NO_OPEN;
    }
    m_markList.push_back(ride_mark);
}

void RideManager::RideMarkCondi()
{
    m_updatemarkList.clear();
    m_updateRideList.clear();

    VecRideMarkList::iterator markLit;
    for (markLit = m_markList.begin(); markLit != m_markList.end(); markLit++)
    {
        MarkCondit* markCondit = 0;
        markCondit = Ride_Config::Instance()->FindMarkCondit(markLit->id);
        if(!markCondit) continue;
        if (markCondit->deafult != OPEN)
        {
            if(OpenMarkByvip(markCondit->vip_level) && (m_player->GetLevel() >= markCondit->level) )
            {
                if(markLit->state != OPEN)
                {
                    markLit->state = OPEN;
                    m_updatemarkList.push_back(*markLit);
                }
            }
            else
            {
                //vip 过期
                if(markLit->state == OPEN)
                {
                    markLit->state = NO_OPEN;

                    if(markLit->pet_id!= 0)
                    {
                        SetNoMark(markLit->pet_id);
                    }
                    markLit->pet_id = 0;
                    m_updatemarkList.push_back(*markLit);
                }
            }
        }

    }

    SendMarkAck();
}

bool RideManager::OpenMarkByvip(int32 id)
{
    if(id != 0)
    {
        int32 b_open = VipConfig::Instance()->GetVipValue(id , m_player->GetVipLevel());
        if(b_open)
            return true;
        else 
            return false;
    }

    return false;
}
void RideManager::SendOpenFieldAck()
{
    RIDE_PET_OPENFILED_ACK senMsg;
    senMsg.Clear();
    senMsg.fieldnum = m_fieldnum;
    m_player->SendToGate(MSG_RIDE_PET_OPENfILED_ACK, &senMsg);
}

void RideManager::SendRideOrFreeAck(int32 _type, const RIDE_PET_INFO& ride_pet)
{
    RIDE_PET_LIST sendMsg;
    switch(_type)
    {
    case RIDE_PET_REST:
        {
            sendMsg.Clear();
            sendMsg.opt_flag = OPT_UPDATE_FLAG;
            sendMsg.ride_pet_list.push_back(ride_pet);
        }
        break;
    case RIDE_PET_RIDE:
        {
            sendMsg.Clear();
            sendMsg.opt_flag = OPT_UPDATE_FLAG;
            sendMsg.ride_pet_list = m_ride_pet_list;
        }
        break;
    default:
        CnWarn("send ride or free type:%d is not exist\n", _type);
        break;

    }
    m_player->SendToGate(MSG_RIDE_PET_LIST_NTF, &sendMsg);
}

void RideManager::SendRidePetListNotify(uint16 opt_flag,RIDE_PET_INFO * pet)
{
    RIDE_PET_LIST  sendMsg;
    sendMsg.Clear();
    if(opt_flag == OPT_ADD_FLAG)
    {
        if(pet)
        {
            sendMsg.opt_flag = OPT_ADD_FLAG;
            sendMsg.ride_pet_list.push_back(*pet);
        }
    }
    else if (opt_flag == OPT_UPDATE_FLAG)
    {
        if(pet)
        {
            sendMsg.opt_flag = OPT_UPDATE_FLAG;
            sendMsg.ride_pet_list.push_back(*pet);
        }
    }
    m_player->SendToGate(MSG_RIDE_PET_LIST_NTF,&sendMsg);
}

void RideManager::SendGif(void* data)
{
    SET_RIDE_PET_REQ* recvMsg = (SET_RIDE_PET_REQ*)data;
    int32  pet_id= recvMsg->pet_id;

    //如果补货，再次点击的时候，分配对应的兽耳点数
    //如果没有补货，当前状态没有补货
    if(m_randomRide_pet.size() <= 0)
        return;

    RIDE_PET_INFO* riderandom = GetRandomInfo(pet_id);
    if (!riderandom)
        return;

    RIDE_PET_INFO* ride_iter = GetRideInfo(pet_id);
    if (!ride_iter)
        return;

    if(ride_iter->b_get !=RIDE_PET_CAPTURE )
        return ;

    int32 num = 10; //返还兽耳个数

    //白色和绿色不返回任何东西 
    int32 curquality = GetRideQuality(pet_id);
    switch (curquality)
    {
    case QUALITY_WHITE:
        break;
    case QUALITY_BLUE:
        {
            SetAddRandomObject(BUG,num);//返回紫色品质兽耳
        }
        break;
    case QUALITY_GREEN:
        SetAddRandomObject(GRASS,num);
        break;
    case QUALITY_PURPLE:
        {
            SetAddRandomObject(HONEY,num);
        }
        break;
    case QUALITY_GOLD:
        {
            SetAddRandomObject(HONEY,num);
        }
        break;
    }
    //返回物品列表馈赠后要清除列表
    m_randomRide_pet.clear();
    SendRandomAck();

}

void RideManager::CalRidePro()
{
    PropertySet atlasPropSet;       // 图鉴加的属性
    PropertySet markPropSet;        // 刻印加的属性

    AddAtlasPro(atlasPropSet);      // 坐骑图鉴

    // 坐骑刻印属性
    RidePetList::iterator ride_pet;
    for (ride_pet = m_ride_pet_list.begin(); ride_pet != m_ride_pet_list.end(); ride_pet++)
    {
        if(ride_pet->b_mark == RIDE_PET_MARK)
        {
            RidePetLeveUpTempl* petTmpl = 0;
            petTmpl = Ride_Config::Instance()->GetRideLeveTempl(ride_pet->pet_id,ride_pet->level);
            if(petTmpl)
            {
                if(petTmpl->res_mp >0)
                    markPropSet.Add(PROP_MP_REC_RATE, petTmpl->res_mp);
                if(petTmpl->res_hp > 0)
                    markPropSet.Add(PROP_HP_REC_RATE, petTmpl->res_hp);
            }
        }
    }

//     m_player->GetPropertySetCalculater()->CalcBattlePowerByRideMark(markPropSet);
//     m_player->GetPropertySetCalculater()->CalcBattlePowerByRideAltas(atlasPropSet);

    // 移动速度
    if(!m_ride_id)
        return;
    if(!m_level)
        return ;

    RIDE_PET_INFO* ride_petls = GetRideInfo(m_ride_id);
    if (!ride_petls)
        return;

    RidePetLeveUpTempl* petTmpl2 = Ride_Config::Instance()->GetRideLeveTempl(m_ride_id, m_level);
    if(!petTmpl2)
        return ;

    if(ride_petls->state == RIDE_PET_RIDE)
    {
        if(!BASE_RATE_NUM)
            return;

        //int32 speed = petTmpl2->speed;
        //if(speed > 0)
        //{
        //    m_player->AddProp(PROP_MOVE_SPEED, PROP_UNIT_PET, speed);
        //}
    }
}

void RideManager::AddAtlasPro(PropertySet& propSet)
{
    //RideAltasList::iterator altaslist;
    //for (altaslist = m_rideAltasList.begin(); altaslist!= m_rideAltasList.end(); altaslist++)
    //{
    //    if (altaslist->state == OPEN)
    //    {
    //        AtlasStruct* atlast = 0;
    //        atlast = Ride_Config::Instance()->GetAltasStruct(altaslist->id);
    //        if(atlast)
    //        {
    //            VecRewardList::iterator veciter;
    //            for (veciter = atlast->rewardlist.begin();veciter != atlast->rewardlist.end(); veciter++)
    //            {
    //                if(veciter->num > 0)
    //                    propSet.Add(veciter->type, veciter->num);
    //            }
    //        }
    //    }
    //}
}

// 设置骑宠ID
void RideManager::SetRidePetID(uint32 rideID)
{
    m_ride_id = rideID;
    m_player->NotifyProperty(PROP_RIDE_PET, m_ride_id);
}

// 骑宠变更
void RideManager::ChangeRidePet(uint32 val)
{
    RidePetLeveUpTempl* petTmpl = 0;
    if(val)
    {
        SetRidePetID(val);

        RidePetList::iterator ride_pet;
        for (ride_pet = m_ride_pet_list.begin(); ride_pet != m_ride_pet_list.end(); ride_pet++)
        {
            if (ride_pet->pet_id == m_ride_id)
            {
                m_level = ride_pet->level;
                ride_pet->state = RIDE_PET_RIDE;
                m_player->CalcProperty();

                // 添加骑宠BUFF
                AddRideBuff(ride_pet->pet_id);

                m_player->SendTaskEvent(TASK_EVENT_SET_RIDE_PET, m_ride_id, 0);
            }
            else
            {
                if(ride_pet->state == RIDE_PET_RIDE)
                {
                    ride_pet->state = RIDE_PET_REST;

                    // 去除骑宠BUFF
                    DelRideBuff(ride_pet->pet_id);
                }
            }
        }
    }
    else
    {
        // 去除骑宠BUFF
        DelRideBuff(m_ride_id);
        //有问题，没有减去
        SetRidePetID(0);
        m_level = 0;

        //m_player->CalcProperty();
    }
}
// 下坐骑
void RideManager::RequestDownRide()
{
    uint32 ridePetID = GetRidePetID();
    if(ridePetID)
    {
        RIDE_PET_RIDEORFREE_REQ data;
        data.pet_id = ridePetID;
        data.type = RIDE_PET_REST;
        RideOrFree(&data);
    }
}

// 添加骑宠BUFF(去掉此功能)(土豪金马)
void RideManager::AddRideBuff(uint32 rideID)
{
    RidePetTemplate* ridePetTmpl = Ride_Config::Instance()->GetRidePet(rideID);
    if(!ridePetTmpl)
    {
        return;
    }

    if(ridePetTmpl->skillID == 0)
    {
        return;
    }

    //m_player->UseSkill(ridePetTmpl->skillID, m_player->GetId(), m_player->GetX(), m_player->GetY());
}

// 删除骑宠BUFF(去掉此功能)
void RideManager::DelRideBuff(uint32 rideID)
{
    // 已经不在坐骑上不下坐骑
    if(m_ride_id == 0)
    {
        return;
    }

    //m_player->UseSkill(BUFF_RIDE_REST, m_player->GetId(), m_player->GetX(), m_player->GetY());
}

int32 RideManager::GetRideQuality(int32 pet_id)
{
    RidePetTemplate * ridePetTmpl = 0 ;
    ridePetTmpl = Ride_Config::Instance()->GetRidePet(pet_id);
    if(!ridePetTmpl)
    {
        return ERR_QUALITY;
    }

    int32 fight = ridePetTmpl->fighting;
    //品质范围
    int32 minwhite =0;
    int32 maxwhite = 0;
    int32 mingree = 0;
    int32 maxgree = 0;
    int32 minblue = 0;
    int32 maxblue = 0;
    int32 minpurple = 0;//紫色
    int32 maxpurple = 0;
    int32 mingold = 0;
    int32 maxgold = 0;

    RidePetQuality* whitequality = Ride_Config::Instance()->GetRidePetQuality("white");
    if(whitequality)
    {
        minwhite = whitequality->m_min;
        maxwhite = whitequality->m_max;
    }

    RidePetQuality* greequality = Ride_Config::Instance()->GetRidePetQuality("gree");
    if(greequality)
    {
        mingree = greequality->m_min;
        maxgree = greequality->m_max;
    }

    RidePetQuality* bluequality = Ride_Config::Instance()->GetRidePetQuality("blue");
    if(bluequality)
    {
        minblue = bluequality->m_min;
        maxblue = bluequality->m_max;
    }

    RidePetQuality* pureplequality = Ride_Config::Instance()->GetRidePetQuality("pureple");
    if(pureplequality)
    {
        minpurple = pureplequality->m_min;
        maxpurple = pureplequality->m_max;
    }

    RidePetQuality* goldquality = Ride_Config::Instance()->GetRidePetQuality("gold");
    if(goldquality)
    {
        mingold = goldquality->m_min;
        maxgold = goldquality->m_max;
    }
    if(fight >= 0 && fight<= maxwhite)
    {
        return QUALITY_WHITE;
    }
    else if (fight>mingree && fight<= maxgree)
    {
        return QUALITY_GREEN;
    }
    else if (fight>minblue && fight<= maxblue)
    {
        return QUALITY_BLUE;
    }
    else if (fight>minpurple && fight<= maxpurple)
    {
        return QUALITY_PURPLE;
    }
    else if (fight>mingold && fight<= maxgold)
    {
        return QUALITY_GOLD;
    }

    return ERR_QUALITY;
}

// 捕获骑宠 
void RideManager::CaptureRide(void* data)
{
    SET_RIDE_PET_REQ * recvMsg = ( SET_RIDE_PET_REQ*)data;
    uint32 pet_id = recvMsg->pet_id;

    if(m_randomRide_pet.size() <= 0)
        return;

    //设置坐骑捕获
    RIDE_PET_INFO * iter_ride = GetRandomInfo(pet_id);
    if(!iter_ride)
        return;

    RidePetTemplate * ridePetTmpl = 0 ;
    ridePetTmpl = Ride_Config::Instance()->GetRidePet(pet_id);
    if(!ridePetTmpl)
        return;

    RidePetLeveUpTempl* petTmpl = 0;
    petTmpl = Ride_Config::Instance()->GetRideLeveTempl(iter_ride->pet_id,iter_ride->level);
    if(!petTmpl)
        return;

    if(m_enum_capture == NoCapture)//刷出蓝色等级以上骑宠是否捕获
    {
        m_enum_capture = Capture;
    }

    if(iter_ride->b_get == RIDE_PET_NO_CAPTURE )
    {
        iter_ride->b_get = RIDE_PET_CAPTURE;
    }
    else
    {
        return;
    }

    m_randomRide_pet.clear();

    AddRidePet(pet_id);

//     uint32 battle = m_player->GetPropertySetCalculater()->GetBattlePowerByRideMark() 
//         + m_player->GetPropertySetCalculater()->GetBattlePowerByRideAltas();
    m_player->SendTaskEvent(TASK_EVENT_BATTLE_POWER_MOUNT, 0, 1 ,0);
}

void RideManager::SendMarQuee(uint32 pet_id)
{
    int32 curqualiy = 0;
    curqualiy = GetRideQuality(pet_id);

    if (pet_id == 20049/*草泥马*/)    // 特殊坐骑
    {
        SERVER_MARQUEE marquee;
        marquee.id = MARQUEE_GET_SPECIAL_RIDE;
        marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
        marquee.parame_list.push_back(MarQueeRideQuality(curqualiy, pet_id));
        m_player->OnMarQuee(marquee);
    }
    else if(curqualiy!= ERR_QUALITY)  
    {
        if(curqualiy >= QUALITY_PURPLE)
        {
            SERVER_MARQUEE marquee;
            marquee.id = MARQUEE_GET_BLUE_RIDE;
            marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
            marquee.parame_list.push_back(MarQueeRideQuality(curqualiy, pet_id));
            m_player->OnMarQuee(marquee);
        }
    }
}
void RideManager::GetUpdateRide(void* data)
{
    RIDE_PET_UPDATE_REQ * recvMsg = ( RIDE_PET_UPDATE_REQ*)data;
    int32 type = recvMsg->type;

    switch(type)
    {
    case RIDE_PET_ORD:
        {
            int32 m_stonemoney = Ride_Config::Instance()->GetOrdStoneNum(); //魔石个数
            int32 itemid = BEASTEAR_ID; //兽耳id
            int32 itemNum = Ride_Config::Instance()->GetOrdinaryUpdateItemNum();

            if(JudgeByRandom(m_stonemoney, itemid, itemNum, RIDE_PET_ORD))
            {
                int32 _grassper = Ride_Config::Instance()->GetOrdRideBlue();
                int32 _bugper   = Ride_Config::Instance()->GetOrdRidePureple();
                int32 _honeyper = Ride_Config::Instance()->GetOrdRideGold();
                RandomRideAndOjbect(_grassper, _bugper, _honeyper, RIDE_PET_ORD);
                SendUpdateRideAck();
                m_player->SendTaskEvent(TASK_EVNET_SUMMON_RIDE, 0, 0);
            }
            else
            {
                return;
            }
        }
        break;
    case RIDE_PET_SER:
        {
            if (recvMsg->use_moneypoint == 1)
            {
                if (VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_57 , m_player->GetVipLevel()) == 0) // 不是VIP
                {
                    return;
                }
            }

            int32 m_stonemoney = Ride_Config::Instance()->GetSerStoneNum(); //魔石个数
            int32 itemid = BEASTEAR_ID; //兽耳id            
            int32 itemNum = Ride_Config::Instance()->GetSeniorUpdateItemNum();

            if(JudgeByRandom(m_stonemoney, itemid, itemNum, RIDE_PET_SER))
            {
                int32 _grassper = Ride_Config::Instance()->GetSerRideBlue();
                int32 _bugper   = Ride_Config::Instance()->GetSerRidePureple();
                int32 _honeyper = Ride_Config::Instance()->GetSerRideGold();
                RandomRideAndOjbect(_grassper, _bugper, _honeyper, RIDE_PET_SER);
                SendUpdateRideAck();
                m_player->SendTaskEvent(TASK_EVNET_SUMMON_RIDE, 0, 0);
            }
            else
            {
                return;
            }
        }
        break;
    case RIDE_PET_SUPERME:
        {
            UpdateSupermeRide();
            m_player->SendTaskEvent(TASK_EVNET_SUMMON_RIDE, 0, 0);
        }
        break;
    default:
        CnWarn("updata ride type:%d is not exist\n", type);
        break;
    }

    NotifyUpdateNum();
}
void RideManager::UpdateSupermeRide()
{
    if (ConfigManager::Instance()->IsTW() && m_player->GetVipLevel() < VIP_1)
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_VIP);
        return;
    }
    else if (!ConfigManager::Instance()->IsTW() && m_player->GetVipLevel() < VIP_4)
    {
        m_player->NotifyErrorCode(ERR_NO_ENOUGH_VIP);
        return;
    }

    int32 needNum = Ride_Config::Instance()->GetSpendSupermeItemNum();
    if(needNum <= 0)
    {
        CnAssert(false);
        return;
    }

    //int32 bagnum = m_player->GetItemManager()->GetItemNum(BEASTEAR_ID);
    //if(bagnum < needNum)
    //{
    //    m_player->NotifyErrorCode(ERR_NO_ENOUGH_ITEM);
    //    return;
    //}


    NoCatchRandomObject(0);//RIDE_PET_ORD?? //没有抓捕，返点

    int32 itemNum = Ride_Config::Instance()->GetOrdinaryUpdateItemNum();
    int32 num = needNum / itemNum;
    for (int32 i= 0; i< num; i++)
    {
        RandomTypeOject(RIDE_PET_ORD);
    }


    std::sort(m_randomObject.begin(),m_randomObject.end(),ComparQuality);
    //如果三个都不满给绿色的
    //如果有其中一个满，从高，到低质量刷出坐骑
    UpdateRideByQualityNum();
    //int32 quality = GetQualityByArriveMaxNum();

    //JudgeTypeRandom(quality, hownum);
    int32 retrunpoint = Ride_Config::Instance()->GetSuperBlueNum();
    SetAddRandomObject(GRASS, retrunpoint);

    //m_player->GetItemManager()->DelItemByID(BEASTEAR_ID, needNum);

    SendUpdateRideAck();
}
//是否到达最大数，那么给出相应坐骑品质
void RideManager::UpdateRideByQualityNum()
{
    int32 qualiy = QUALITY_GREEN;
    VecRewardItem::iterator iter_reitem;
    for (iter_reitem = m_randomObject.begin();  iter_reitem!= m_randomObject.end(); iter_reitem++ )
    {
        if(iter_reitem->id  == HONEY)
        {
            if(iter_reitem->num >=  Ride_Config::Instance()->GetMaxHoney() )
            {
                //return QUALITY_GOLD;
                int32 hownum =  iter_reitem->num - Ride_Config::Instance()->GetMaxHoney();
                JudgeTypeRandom(QUALITY_GOLD, hownum);
                return;
            }
        }
        else if(iter_reitem->id  == BUG)
        {
            if(iter_reitem->num >= Ride_Config::Instance()->GetMaxBug())
            {
                //return QUALITY_PURPLE;
                int32 hownum =  iter_reitem->num - Ride_Config::Instance()->GetMaxBug();
                JudgeTypeRandom(QUALITY_PURPLE, hownum);
                return;
            }
        }
        else if(iter_reitem->id == GRASS)
        {
            if(iter_reitem->num >= Ride_Config::Instance()->GetMaxGrass() )
            {
                //return QUALITY_BLUE;
                int32 hownum =  iter_reitem->num - Ride_Config::Instance()->GetMaxGrass();
                JudgeTypeRandom(QUALITY_BLUE, hownum);
                return;
            }
        }
    }

    //如果三个都不满给绿色的
   JudgeTypeRandom(qualiy, 0);
}
void RideManager::SendUpdateRideAck()
{
    RIDE_PET_UPDATE_ACK sendMsg;
    sendMsg.ride_random_list = m_randomRide_pet;
    sendMsg.object_random_list = m_randomObject;
    m_player->SendToGate(MSG_RIDE_PET_UPDATE_ACK,&sendMsg);
}

void  RideManager::SendRandomAck()
{
    RIDE_PET_SENDGIF_ACK sendMsg;
    sendMsg.object_random_list = m_randomObject;
    m_player->SendToGate(MSG_RIDE_PET_SENDGIF_ACK,&sendMsg);
}

bool RideManager::JudgeByRandom(int32 m_stonemoney, int32 itemid,int32 itemNum ,int32 type)
{
    //int32 curexe = 0;
    //if (m_player->GetItemManager()->GetItemNum(itemid)>= itemNum)
    //{
    //    m_player->GetItemManager()->DelItemByID(itemid,itemNum);
    //    return true;
    //}
    //else if(m_player->GetMoneyPoint()>= m_stonemoney)
    //{
    //    curexe = 1 ;
    //    TradeManager::Instance()->SendTradeOrderReq(m_player,REWARD_REASON_RIDE_PET,PROP_RIDE_PET_UPDATE,type,curexe,m_stonemoney);
    //}

    return false;
}

void RideManager::NotifyNum()
{
    NotifyUpdateNum();
    if (m_ridetrain)
        m_ridetrain->NotifyTrainNum();
}

uint32 RideManager::GetRideNumByQuality(int32 quailty)
{
    uint32 num = 0;
    int32 rideQuality = 0;

    for (RidePetList::iterator ridelistiter = m_ride_pet_list.begin(); 
        ridelistiter != m_ride_pet_list.end(); ridelistiter++)
    {
        rideQuality = GetRideQuality(ridelistiter->pet_id);
        if(rideQuality != quailty)
        {
            continue;
        }
        ++num;
    }

    return num;
}

void RideManager::NotifyUpdateNum()
{
    int32 stonemoney = Ride_Config::Instance()->GetOrdinaryUpdateItemNum();
    if(stonemoney <= 0)
    {
        CnAssert(false);
        return;
    }

    //int32 itemnum = m_player->GetItemManager()->GetItemNum(BEASTEAR_ID);
    //int32 catchnum  = itemnum / stonemoney ;

    //m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_RIDE_SUMMON, catchnum);
}
void RideManager::UpdateRideByMoney(int32 type,int32 curexe)
{
    if (curexe)
    {
        if(type == RIDE_PET_SER)
        {
            int32 _grassper = Ride_Config::Instance()->GetSerRideBlue();
            int32 _bugper   = Ride_Config::Instance()->GetSerRidePureple();
            int32 _honeyper = Ride_Config::Instance()->GetSerRideGold();
            RandomRideAndOjbect(_grassper,_bugper,_honeyper,RIDE_PET_SER);
            SendUpdateRideAck();//发送随机物品
            m_player->SendTaskEvent(TASK_EVNET_SUMMON_RIDE, 0, 0);
        }
        else if (type == RIDE_PET_ORD)
        {
            int32 _grassper = Ride_Config::Instance()->GetOrdRideBlue();
            int32 _bugper   = Ride_Config::Instance()->GetOrdRidePureple();
            int32 _honeyper = Ride_Config::Instance()->GetOrdRideGold();
            RandomRideAndOjbect(_grassper,_bugper,_honeyper,RIDE_PET_ORD);
            SendUpdateRideAck();
            m_player->SendTaskEvent(TASK_EVNET_SUMMON_RIDE, 0, 0);
        }
    }
}

int32 RideManager::SetRideQuality(int32 pet_id)
{

    RidePetTemplate * ridePetTmpl = 0 ;
    ridePetTmpl = Ride_Config::Instance()->GetRidePet(pet_id);
    if(!ridePetTmpl)
    {
        return ERR_QUALITY;
    }

    int32 fight = ridePetTmpl->fighting;
    //品质范围
    int32 minwhite =0;
    int32 maxwhite = 0;
    int32 mingree = 0;
    int32 maxgree = 0;
    int32 minblue = 0;
    int32 maxblue = 0;
    int32 minpurple = 0;//紫色
    int32 maxpurple = 0;
    int32 mingold = 0;
    int32 maxgold = 0;

    RidePetQuality* whitequality = Ride_Config::Instance()->GetRidePetQuality("white");
    if(whitequality)
    {
        minwhite = whitequality->m_min;
        maxwhite = whitequality->m_max;
    }

    RidePetQuality* greequality = Ride_Config::Instance()->GetRidePetQuality("gree");
    if(greequality)
    {
        mingree = greequality->m_min;
        maxgree = greequality->m_max;
    }

    RidePetQuality* bluequality = Ride_Config::Instance()->GetRidePetQuality("blue");
    if(bluequality)
    {
        minblue = bluequality->m_min;
        maxblue = bluequality->m_max;
    }

    RidePetQuality* pureplequality = Ride_Config::Instance()->GetRidePetQuality("pureple");
    if(pureplequality)
    {
        minpurple = pureplequality->m_min;
        maxpurple = pureplequality->m_max;
    }

    RidePetQuality* goldquality = Ride_Config::Instance()->GetRidePetQuality("gold");
    if(goldquality)
    {
        mingold = goldquality->m_min;
        maxgold = goldquality->m_max;
    }
    if(fight >= 0 && fight<= maxwhite)
    {
        return QUALITY_WHITE;
    }
    else if (fight>mingree && fight<= maxgree)
    {
        return QUALITY_GREEN;
    }
    else if (fight>minblue && fight<= maxblue)
    {
        return QUALITY_BLUE;
    }
    else if (fight>minpurple && fight<= maxpurple)
    {
        return QUALITY_PURPLE;
    }
    else if (fight>mingold && fight<= maxgold)
    {
        return QUALITY_GOLD;
    }

    return ERR_QUALITY;
}

void RideManager::JudgeTypeRandom(int32 type, int32 num)
{
    switch(type)
    {
    case QUALITY_BLUE:
        {
            bool b_randomRide = false;
            b_randomRide =  RandomRide(QUALITY_BLUE,0);
            if(b_randomRide)
            {
                //ClearRandomObjNum(GRASS);
                SetPointNum(GRASS, num);
                m_enum_capture = NoCapture;
            }
            else
            {
                CnWarn("not random ride \n", type);
            }

        }
        break;
    case QUALITY_PURPLE:
        {
            bool b_randomRide = false;
            b_randomRide = RandomRide(QUALITY_PURPLE,0);
            if(b_randomRide)
            {
                //ClearRandomObjNum(BUG);
                SetPointNum(BUG, num);
                m_enum_capture = NoCapture;
            }
            else
            {
                CnWarn("not random ride \n", type);
            }
        }
        break;
    case QUALITY_GOLD:
        {
            bool b_randomRide = false;
            b_randomRide = RandomRide(QUALITY_GOLD,0);
            if(b_randomRide)
            {
                //ClearRandomObjNum(HONEY);
                SetPointNum(HONEY, num);
                m_enum_capture = NoCapture;
            } 
            else
            {
                CnWarn("not random ride \n", type);
            }
        }
        break;
    case QUALITY_WHITE:
        {
            bool b_randomRide = false;
            b_randomRide = RandomRide(QUALITY_WHITE,0);
            if(b_randomRide)
            {
                m_enum_capture = NoCapture;
            } 
            else
            {
                CnWarn("not random ride \n", type);
            }
        }
        break;
    case QUALITY_GREEN:
        {
            bool b_randomRide = false;
            b_randomRide =  RandomRide(QUALITY_GREEN,0);
            if(b_randomRide)
            {
                m_enum_capture = NoCapture;
            }
            else
            {
                CnWarn("not random ride \n", type);
            }

        }
        break;
    default:
        CnWarn("Judge type:%d is not exist\n", type);
        break;
    }
}


void RideManager::NoCatchRandomObject(int32 type)
{
    bool b_addexe = false;

    //刷出蓝色坐骑，没有进行捕获，再次直接刷新得到的兽耳
    if (m_enum_capture != Capture_Init && m_enum_capture== NoCapture)
    {
        if(m_randomRide_pet.size() >0)
        {
            m_lastRideid = m_randomRide_pet.at(0).pet_id;
        }

        int32 quality =  GetRideQuality(m_lastRideid);
        //if (quality == QUALITY_BLUE)
        //{
        //    //// 再次刷新得到兽耳的个数
        //    int32 addNum = 0;
        //    addNum = Ride_Config::Instance()->GetblueStone();
        //    m_player->GetItemManager()->AddItemByNum(BEASTEAR_ID, addNum, REWARD_REASON_RIDE_NO_CATCH);
        //    b_addexe = true;
        //}
        //else if (quality == QUALITY_PURPLE)
        //{
        //    int32 addNum = 0;
        //    addNum = Ride_Config::Instance()->GetPureStone();
        //    m_player->GetItemManager()->AddItemByNum(BEASTEAR_ID, addNum, REWARD_REASON_RIDE_NO_CATCH);
        //    b_addexe = true;
        //}
        //else if (quality == QUALITY_GOLD)
        //{
        //    int32 addNum = 0;
        //    addNum = Ride_Config::Instance()->GetGoldStone();
        //    m_player->GetItemManager()->AddItemByNum(BEASTEAR_ID, addNum, REWARD_REASON_RIDE_NO_CATCH);
        //    b_addexe = true;
        //}

        //没有抓捕返点
        NoCatchReturnPoint(quality);
    }

    if(!b_addexe && type)
    {
        RandomTypeOject(type);
    }
}

//没有抓捕，返点
void RideManager::NoCatchReturnPoint(int32 quality)
{
    int32 retrunpoint = 10;
    if(quality == QUALITY_GREEN)
    {
        SetAddRandomObject(GRASS,retrunpoint);
    }
    else if (quality == QUALITY_BLUE)
    {
        SetAddRandomObject(BUG,retrunpoint);
    }
    else if (quality == QUALITY_PURPLE)
    {
        SetAddRandomObject(HONEY,retrunpoint);
    }
    else if (quality == QUALITY_GOLD)
    {
        SetAddRandomObject(HONEY,retrunpoint);
    }
}
void RideManager::RandomRideAndOjbect(int32 _grassper,int32 _bugper,int32 _honeyper,int32 type)
{

    NoCatchRandomObject(type);

    //JudgeTypeRandom(type);
    RadnomRideByStone(_grassper, _bugper, _honeyper ,  type);
    if(m_randomRide_pet.size() < 4)
    {
        CnWarn("随机坐骑的个数小4个\n");
    }
}

//大于最大数一定生成坐骑
bool RideManager::CanRandomRide(int32 num ,int32 maxvalue ,int32 type)
{
    if(num >= maxvalue)
    {
        GetPointByQuality(num,maxvalue); //计算还剩多少点
        JudgeTypeRandom(type, num);
        return true;
    }
    return false;
}
//根据品质计算还有多少点
void RideManager::GetPointByQuality(int32& hownum ,int32 maxvalue)
{
    if(hownum >= maxvalue)
    {
        hownum = hownum - maxvalue;
    }
    else
    {
        hownum = 0;
        CnWarn("wrong num in ride manager");
    }
}

void RideManager::SetPointNum(uint32 ride_id, int num)
{
    VecRewardItem::iterator iter_reitem;
    for (iter_reitem = m_randomObject.begin();  iter_reitem!= m_randomObject.end(); iter_reitem++ )
    {
        if(iter_reitem->id == ride_id)
        {
            iter_reitem->num = num;
            break;
        }
    }
}
//根据兽饵越多能随到的坐骑的概率越大
void RideManager::RadnomRideByStone(int32 _grassper,int32 _bugper,int32 _honeyper ,int32 type)
{
    int32 maxgrass = Ride_Config::Instance()->GetMaxGrass(); //青草的最大数量
    int32 maxbug = Ride_Config::Instance()->GetMaxBug(); //虫的最大数量
    int32 maxhoney = Ride_Config::Instance()->GetMaxHoney();//蜂蜜的最大数量

    //除数不能为0
    if(!maxgrass)
        return;
    if(!maxbug)
        return;
    if (!maxhoney)
        return;

    int32 _grass = Ride_Config::Instance()->GetGrassRange() ;//大于这个值得话，那么随机生成四个蓝色性质的宠物
    int32 _bug = Ride_Config::Instance()->GetBugRange();//大于这个值得话，那么随机生成四个紫色性质的宠物
    int32 _honey = Ride_Config::Instance()->GetHoneyRange();

    std::vector<RideObject> vec_obj;
    vec_obj.clear();

    bool b_ride = false;
    ////兽耳越多，刷出对应坐骑的几率越大
    VecRewardItem::iterator iter_reitem;
    for (iter_reitem = m_randomObject.begin();  iter_reitem!= m_randomObject.end(); iter_reitem++ )
    {
        //顺序翻一下就可以从高级道低级出来了
        if((iter_reitem->id == GRASS))
        {
            int32 quality =  FindQualityByItem(iter_reitem->id);
            if(CanRandomRide(iter_reitem->num,maxgrass,quality))
            {
                b_ride =  true;
                break;
            }
            PushProbability(iter_reitem->num , maxgrass , _grassper , _grass ,quality,vec_obj);
        }
        else if(iter_reitem->id == BUG)
        {
            int32 quality =  FindQualityByItem(iter_reitem->id);
            if(CanRandomRide(iter_reitem->num,maxbug,quality))
            {
                b_ride =  true;
                break;
            }
            PushProbability(iter_reitem->num , maxbug , _bugper , _bug ,quality,vec_obj);
        }
        else if (iter_reitem->id == HONEY)
        {
            int32 quality =  FindQualityByItem(iter_reitem->id);
            if(CanRandomRide(iter_reitem->num,maxhoney,quality))
            {
                b_ride =  true;
                break;
            }
            PushProbability(iter_reitem->num , maxhoney , _honeyper , _honey ,quality,vec_obj);
        }
    }

    if(!b_ride)
    {
        if (!FindRideQuality(vec_obj,QUALITY_BLUE))
            vec_obj.push_back(std::make_pair(QUALITY_BLUE,0));
        if (!FindRideQuality(vec_obj,QUALITY_PURPLE))
            vec_obj.push_back(std::make_pair(QUALITY_PURPLE,0));
        if (!FindRideQuality(vec_obj,QUALITY_GOLD))
            vec_obj.push_back(std::make_pair(QUALITY_GOLD,0));
        RideByProbability(vec_obj,type);
    }
}
bool RideManager::FindRideQuality(std::vector<RideObject>& vec_obj,int32 quality)
{
    std::vector<RideObject>::iterator veciter;
    for(veciter = vec_obj.begin(); veciter != vec_obj.end(); veciter++)
    { 
        if(veciter->first == quality)
        {
            return true;
        }
    }

    return false;
}
int32 RideManager::FindQualityByItem(int32 type)
{
    if(type == GRASS)
        return QUALITY_BLUE;
    else if (type == BUG)
        return QUALITY_PURPLE;
    else if (type == HONEY)
        return QUALITY_GOLD;

    return 0;
}

void RideManager::RideByProbability(std::vector<RideObject>& vec_obj, int32 type)
{

    if(type == RIDE_PET_ORD)
    {
        int32 m_gree = Ride_Config::Instance()->GetOrdRideGreen();
        int32 m_while = Ride_Config::Instance()->GetOrdRideWhile();
        vec_obj.push_back(std::make_pair(QUALITY_WHITE,m_while));
        vec_obj.push_back(std::make_pair(QUALITY_GREEN,m_gree));
    }
    else if (type == RIDE_PET_SER)
    {
        int32 m_gree = Ride_Config::Instance()->GetSerRideGreen();
        int32 m_while = Ride_Config::Instance()->GetSerRideWhile();
        vec_obj.push_back(std::make_pair(QUALITY_WHITE,m_while));
        vec_obj.push_back(std::make_pair(QUALITY_GREEN,m_gree));
    }

    if(vec_obj.size() > 5)
        CnAssert(false);


    std::sort(vec_obj.begin(),vec_obj.end(),ComparObjNum);
    int32 maxNum = vec_obj.at(0).second+vec_obj.at(1).second+vec_obj.at(2).second+vec_obj.at(3).second + vec_obj.at(4).second;
    if(maxNum <= 0)
        CnAssert(false);

    // int32 randomNum = Crown::SDRandom()% BASE_RATE_NUM;
    int32 randomNum = Crown::SDRandom()% BASE_RATE_NUM;
    if (randomNum >0 && randomNum <= vec_obj.at(0).second)
    {
        int32 type = vec_obj.at(0).first;
        int32 num  =  vec_obj.at(0).second;
        if(num!=0)
            JudgeTypeRandom(type);
    }
    else if (randomNum >  vec_obj.at(0).second 
        && randomNum <= (vec_obj.at(0).second+vec_obj.at(1).second) )
    {
        int32 type = vec_obj.at(1).first;
        int32 num  =  vec_obj.at(1).second;
        if(num!=0)
            JudgeTypeRandom(type);
    }
    else if(randomNum> (vec_obj.at(0).second+vec_obj.at(1).second) 
        && randomNum <= (vec_obj.at(0).second+vec_obj.at(1).second+vec_obj.at(2).second))
    {
        int32 type = vec_obj.at(2).first;
        int32 num  =  vec_obj.at(2).second;
        if(num!=0)
            JudgeTypeRandom(type);
    }
    else if(randomNum > (vec_obj.at(0).second+vec_obj.at(1).second+vec_obj.at(2).second ) 
        && randomNum <= (vec_obj.at(0).second+vec_obj.at(1).second+vec_obj.at(2).second + vec_obj.at(3).second))
    {
        int32 type = vec_obj.at(3).first;
        int32 num  =  vec_obj.at(3).second;
        if(num!=0)
            JudgeTypeRandom(type);
    }
    else /*if (randomNum > (vec_obj.at(0).second+vec_obj.at(1).second+vec_obj.at(2).second + vec_obj.at(3).second )&& randomNum <= maxNum)*/
    {
        int32 type = vec_obj.at(4).first;
        int32 num  =  vec_obj.at(4).second;
        if(num!=0)
            JudgeTypeRandom(type);
    }
}
void RideManager::PushProbability(int32 num, int32 maxvalue , int32 probability,
                                  int32 pointvalue , int32 type 
                                  , std::vector<RideObject>& vec_obj)
{
    int32 curprobablity = 0;
    if(num >= pointvalue)
    {
        //_grassjl = iter_reitem->num/maxgrass* _grassper/BASE_RATE_NUM;
        curprobablity = num*probability/maxvalue;
        //刷出坐骑后，物品清空
        vec_obj.push_back(std::make_pair(type , curprobablity));
    }
}
bool RideManager::FindOpenQuality(RideTempMap& curRideTemp,int32 quality1,int32 quality2)
{
    const RideTempMap& AllrideTempMap = Ride_Config::Instance()->GetRideTemplMap();
    RideTempMap rideTempMap;
    rideTempMap.clear();
    for (RideTempMap::const_iterator curss = AllrideTempMap.begin(); curss!= AllrideTempMap.end();curss++)
    {
        if(curss->second.isopen == SISTEM_OPEN)
        {
            rideTempMap[curss->first] = curss->second;
        }
    }
    if(!rideTempMap.size())
        return false;

    RideTempMap::iterator cur_iter;
    for (cur_iter = rideTempMap.begin(); cur_iter!=rideTempMap.end(); cur_iter ++)
    {

        int32 curquality = GetRideQuality(cur_iter->first);
        if(ERR_QUALITY != curquality)
        {
            if((curquality == quality1) || (curquality == quality2))
            {
                if(curquality != 0)
                {
                    curRideTemp[cur_iter->first] = cur_iter->second;
                }
            }
        }
    }

    if(curRideTemp.size()<4)
    {
        int32 coutnNum = 4 - curRideTemp.size();
        int32 recoeNum = 0;
        while(recoeNum < coutnNum)
        {
            int32 num = Crown::SDRandom()% rideTempMap.size();

            RideTempMap::iterator iterride;
            int32  curNum = 0;
            for(iterride = rideTempMap.begin(); iterride !=  rideTempMap.end(); iterride++)
            {
                if(curNum == num)
                {
                    break;
                }
                curNum = curNum +1;
            }

            RideTempMap::iterator curRideTempIter;
            curRideTempIter = curRideTemp.find(iterride->first);
            if(curRideTempIter == curRideTemp.end())
            {
                curRideTemp[iterride->first] = iterride->second;
                recoeNum = recoeNum +1;
            }
        }
    }

    return true;
}
bool RideManager::RandomRide(int32 quality1,int32 quality2)
{
    m_randomRide_pet.clear();
    int32 ridenum =  4;             //每次生成4个宠物 定义为宏??
    int32 m_num = 0;

    RideTempMap curRideTemp;
    bool b_exe = FindOpenQuality(curRideTemp,quality1,quality2);
    if(!b_exe)
        return false;

    while(m_num < ridenum)
    {
        int32 num = Crown::SDRandom()% curRideTemp.size();

        int32  curRideNum = 0;
        RideTempMap::iterator cur_rideiter;
        for (cur_rideiter = curRideTemp.begin(); cur_rideiter!=curRideTemp.end(); cur_rideiter ++)
        {
            if(curRideNum == num)
            {
                break;
            }
            curRideNum = curRideNum +1;
        }

        uint32 pet_id = cur_rideiter->second.id;

        RIDE_PET_INFO * iter_ride = GetRandomInfo(pet_id);
        if(!iter_ride)
        {
            RIDE_PET_INFO* ride_iter =  GetRideInfo(pet_id);
            //如果补货，再次点击的时候，分配对应的兽耳点数
            //如果没有补货，当前状态没有补货
            if(ride_iter)
            {
                ride_iter->b_get = RIDE_PET_CAPTURE;
                m_randomRide_pet.push_back(*ride_iter);
            }
            else
            {
                RIDE_PET_INFO ride_info;
                ride_info.level = cur_rideiter->second.level;
                ride_info.name = cur_rideiter->second.name;
                ride_info.pet_id = cur_rideiter->second.id;
                ride_info.express = 0;
                ride_info.state = RIDE_PET_REST;
                ride_info.b_get = RIDE_PET_NO_CAPTURE;
                m_randomRide_pet.push_back(ride_info);
            }
            m_num = m_num +1;
        }
    }


    if (m_randomRide_pet.size() < 4)
    {
        CnAssert(false);
    }
    return true;
}

void RideManager::ClearRandomObjNum (uint32 ride_id)
{
    VecRewardItem::iterator iter_reitem;
    for (iter_reitem = m_randomObject.begin();  iter_reitem!= m_randomObject.end(); iter_reitem++ )
    {
        if(iter_reitem->id == ride_id)
        {
            iter_reitem->num = 0;
            break;
        }
    }
}

void RideManager::RideAbsorb(void* data)
{
    RIDE_PET_ABSORB_REQ* recvMsg = (RIDE_PET_ABSORB_REQ*)data;
    int32 _type = recvMsg->type;//类型
    uint32 pet_id = recvMsg->pet_id;
    uint32 choose_id1 = recvMsg->choose_id1;
    uint32 choose_id2 = recvMsg->choose_id2;
    uint32 choose_id3 = recvMsg->choose_id3;

    if((pet_id == choose_id1)||(pet_id == choose_id2)||(pet_id == choose_id3))
        return;

    // 检查功能开启
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_RIDE_ABSORB, m_player) != ERR_SUCCEED)
    //{
    //    m_player->NotifyErrorCode(ERR_FUNCT_NOT_OPEN);
    //    return;
    //}

    RIDE_PET_INFO* rideInfo  = 0;
    RIDE_PET_INFO* rideInfo1  = 0;
    RIDE_PET_INFO* rideInfo2  = 0;
    RIDE_PET_INFO* rideInfo3  = 0;

    RidePetTemplate * ridePetTmpl3 = 0;
    RidePetTemplate * ridePetTmpl1 = 0;
    RidePetTemplate * ridePetTmpl2 = 0;

    if(pet_id)
    {
        RidePetTemplate * ridePetTmpl = 0;
        ridePetTmpl = Ride_Config::Instance()->GetRidePet(pet_id);
        if(!ridePetTmpl)
            return;
        rideInfo = GetRideInfo(pet_id);
        if(!rideInfo)
            return;
    }

    if(choose_id1)
    {
        ridePetTmpl1 = Ride_Config::Instance()->GetRidePet(choose_id1);
        if(!ridePetTmpl1)
        {
            return;
        }
        rideInfo1 = GetRideInfo(choose_id1);
        if(!rideInfo1)
            return;
    }


    if(choose_id2)
    {
        ridePetTmpl2 = Ride_Config::Instance()->GetRidePet(choose_id2);
        if(!ridePetTmpl2)
            return;
        rideInfo2 = GetRideInfo(choose_id2);
        if(!rideInfo2)
            return;
    }

    if(choose_id3)
    {
        ridePetTmpl3 = Ride_Config::Instance()->GetRidePet(choose_id3);
        if(!ridePetTmpl3)
            return;
        rideInfo3 = GetRideInfo(choose_id3);
        if(!rideInfo3)
            return;
    }

    int32 addExpress1 = 0;
    int32 addExpress2 = 0;
    int32 addExpress3 = 0;
    int32 exper = 0;
    if(ridePetTmpl1)
    {
        addExpress1 = rideInfo1->express + Ride_Config::Instance()->CaluteRideExpress(rideInfo1->pet_id,rideInfo1->level);
    }
    if(ridePetTmpl2)
    {
        addExpress2 = rideInfo2->express + Ride_Config::Instance()->CaluteRideExpress(rideInfo2->pet_id,rideInfo2->level);
    }
    if (ridePetTmpl3)
    {
        addExpress3 = rideInfo3->express + Ride_Config::Instance()->CaluteRideExpress(rideInfo3->pet_id,rideInfo3->level);
    }

    exper = addExpress1 + addExpress2 + addExpress3;
    //计算所有的经验

    if(_type == RIDE_PET_ORD)
    {
        int32 valueAbsob = Ride_Config::Instance()->GetAbsorbOrd();
        uint16 err = m_player->SubItemOne(REWARD_REASON_RIDE_ABSORB, PROP_GAMEPOINT, valueAbsob);
        if(err != ERR_SUCCEED)
        {
            m_player->NotifyErrorCode(ERR_NO_ENOUGH_GAMEPOINT);
            return;
        }
        //普通吸收的75%
        int32  orSwa = Ride_Config::Instance()->GetOrdSwarper();
        int32 cur_exper = 0;
        cur_exper = (int32)(exper*ToPercent(orSwa));
        CalRideAbsorb(cur_exper,(rideInfo),(rideInfo1),(rideInfo2),(rideInfo3));
    }
    else if(_type == RIDE_PET_DOUBLE)
    {
        int32 abob = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_33 ,m_player->GetVipLevel());
        if(!abob)
        {
            m_player->NotifyErrorCode(ERR_RIDE_ABOB);
            return ;
        }

        int32 doubleAbob = Ride_Config::Instance()->GetDoubleAbsorb();
        if (m_player->GetMoneyPoint()<doubleAbob)
        {
            m_player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
            return;
        }
        //jeson 
        Json::FastWriter writer;
        Json::Value jesion_info;
        jesion_info["pet_id"] = pet_id;
        jesion_info["pet_id1"] = choose_id1;
        jesion_info["pet_id2"] = choose_id2;
        jesion_info["pet_id3"] = choose_id3;
        jesion_info["exp"] = exper;
        std::string equip_txt;
        equip_txt = writer.write(jesion_info);
        TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_RIDE_ABSORB, PROP_RIDN_PET_ABSORB, 0, 0, doubleAbob, equip_txt);
    }

}

void RideManager::CalRideAbsorb(int32 exper,RIDE_PET_INFO* rideInfo,RIDE_PET_INFO* rideInfo1,RIDE_PET_INFO* rideInfo2,RIDE_PET_INFO* rideInfo3)
{
    m_updateRideList.clear();

    RIDE_PET_INFO* currideInfo  = 0;
    RIDE_PET_INFO* currideInfo1  = 0;
    RIDE_PET_INFO* currideInfo2  = 0;
    RIDE_PET_INFO* currideInfo3  = 0;


    int32 ordexp = 0;
    if(rideInfo)
    {
        ordexp = rideInfo->express;
        rideInfo->express = rideInfo->express +exper;
        SetRiddeInfo(rideInfo->pet_id, rideInfo->express);
        AbsorbLeveup(rideInfo->pet_id);
        currideInfo = GetRideInfo(rideInfo->pet_id);
        if(currideInfo)
        {
            m_updateRideList.push_back(*currideInfo);
        }
    }
    else
        return;

    if(rideInfo1)
    {
        SetRiddeInfo(rideInfo1->pet_id,0,1);
        currideInfo1 = GetRideInfo(rideInfo1->pet_id);
        if(currideInfo1)
        {
            m_updateRideList.push_back(*currideInfo1);
        }
    }
    if(rideInfo2)
    {
        SetRiddeInfo(rideInfo2->pet_id,0,1);
        currideInfo2 = GetRideInfo(rideInfo2->pet_id);
        if(currideInfo2)
            m_updateRideList.push_back(*currideInfo2);
    }
    if(rideInfo3)
    {
        SetRiddeInfo(rideInfo3->pet_id,0,1);
        currideInfo3 = GetRideInfo(rideInfo3->pet_id);
        if(currideInfo3)
            m_updateRideList.push_back(*currideInfo3);
    }


    m_player->CalcProperty();

    RIDE_PET_LIST  sendMsg;
    sendMsg.opt_flag = OPT_UPDATE_FLAG;
    sendMsg.ride_pet_list = m_updateRideList;
    m_player->SendToGate(MSG_RIDE_PET_LIST_NTF,&sendMsg);
}

int32 RideManager::GetMaxRideLevel()
{
    int32 level = 0;

    for (RidePetList::iterator ride_pet = m_ride_pet_list.begin();
        ride_pet != m_ride_pet_list.end(); ride_pet++)
    {
        if (ride_pet->level > level)
        {
            level = ride_pet->level;
        }
    }

    return level;
}

RIDE_PET_INFO* RideManager::GetRideInfo(uint32 pet_id)
{
    RIDE_PET_INFO* curinfo = NULL;
    RidePetList::iterator ride_pet;
    for (ride_pet = m_ride_pet_list.begin(); ride_pet != m_ride_pet_list.end(); ride_pet++)
    {
        if (ride_pet->pet_id == pet_id)
        {
            curinfo = &(*ride_pet);
            break;
        }
    }

    return curinfo;
}

RIDE_PET_INFO* RideManager::GetRandomInfo(uint32 pet_id)
{
    RIDE_PET_INFO* curinfo = NULL;
    RidePetList::iterator ride_pet;
    for (ride_pet = m_randomRide_pet.begin(); ride_pet != m_randomRide_pet.end(); ride_pet++)
    {
        if (ride_pet->pet_id == pet_id)
        {
            curinfo = &(*ride_pet);
            break;
        }
    }
    return curinfo;
}

void  RideManager::SetRiddeInfo(uint32 petid,int32 exp,int32 level)
{
    //bool b_find =false;
    RidePetList::iterator ride_pet;
    for (ride_pet = m_ride_pet_list.begin(); ride_pet != m_ride_pet_list.end(); ride_pet++)
    {
        if (ride_pet->pet_id == petid)
        {
            ride_pet->express = exp;
            if(level)
            {
                ride_pet->level = level;
            }
            break;
        }
    }
}

void RideManager::RideAbsorb(const std::string& str)
{
    Json::Reader reader;
    Json::Value value_json;

    Player* player =0;
    //std::string strpet_info;

    if(!reader.parse(str,value_json))
        return;

    uint32 petid = 0;
    uint32 petid1  = 0;
    uint32 petid2 = 0;
    uint32 petid3 = 0;
    int32 exp = 0;
    int32 cur_exper = 0;
    petid = value_json["pet_id"].asUInt();
    petid1 = value_json["pet_id1"].asUInt();
    petid2 = value_json["pet_id2"].asUInt();
    petid3 = value_json["pet_id3"].asUInt();
    exp = value_json["exp"].asInt();


    RIDE_PET_INFO* rideInfo  = 0;
    RIDE_PET_INFO* rideInfo1  = 0;
    RIDE_PET_INFO* rideInfo2  = 0;
    RIDE_PET_INFO* rideInfo3  = 0;

    if(petid)
    {
        rideInfo = GetRideInfo(petid);
        if(!rideInfo)
            return;
    }

    if(petid1)
    {
        rideInfo1 = GetRideInfo(petid1);
        if(!rideInfo1)
            return;
    }
    if(petid2)
    {
        rideInfo2 = GetRideInfo(petid2);
        if(!rideInfo1)
            return;
    }

    if(petid2)
    {
        rideInfo3 = GetRideInfo(petid3);
        if(!rideInfo1)
            return;
    }

    int32  senSwa = Ride_Config::Instance()->GetSerSwaPer();
    cur_exper = (int32)(exp*ToPercent(senSwa));
    CalRideAbsorb(cur_exper, (rideInfo), (rideInfo1), (rideInfo2), (rideInfo3));
}

void RideManager::AbsorbLeveup(uint32 petid)
{
    RIDE_PET_INFO* choosePet = GetRideInfo(petid);
    if(!choosePet)
        return;

    while(choosePet->level < m_player->GetLevel())
    {
        int32 max_exp = Ride_Config::Instance()->GetRidePetLeveupTem(choosePet->pet_id,choosePet->level);
        if(choosePet->express >= max_exp)
        {
            choosePet->express = choosePet->express - max_exp;
            choosePet->level = choosePet->level + 1;
        }
        else
        {
            break;
        }
    }

    if(choosePet->level == m_player->GetLevel())
    {
        int32 max_exp = Ride_Config::Instance()->GetRidePetLeveupTem(choosePet->pet_id,choosePet->level);
        if(choosePet->express >= max_exp && max_exp!=0)
        {
            choosePet->express = max_exp - 1;
        }
    }
}

void RideManager::DeleteRide(uint32 pet_id)
{
    RidePetList ::iterator rideitercur;
    for(rideitercur = m_ride_pet_list.begin(); rideitercur != m_ride_pet_list.end();)
    {
        if(rideitercur->pet_id == pet_id)
        {
            rideitercur = m_ride_pet_list.erase(rideitercur);
        }else
        {
            rideitercur++;
        }
    }
}

void RideManager::RandomObject(int32 blue,int32 purple,int32 gold,int32 numobject,int32 defaultnum)
{

    //默认返点高级的返回蓝色点默认10个，普通的默认返点返回1个点
    SetAddRandomObject(GRASS,defaultnum);

    int32 m_num = 1;
    while(m_num < numobject)
    {
        //配的的骑宠的随机返点的总数一定等于10000
        int32 sdrandom = Crown::SDRandom()%BASE_RATE_NUM;
        if(sdrandom <= blue)//blue青草  
        {
            AddObjectType(GRASS);
        }
        if(sdrandom > blue  && sdrandom <= (purple+blue))//purple 小虫
        {
            AddObjectType(BUG);
        }
        if(sdrandom> (purple+blue) && sdrandom <=(purple+blue+gold))//gold 蜂蜜
        {
            AddObjectType(HONEY);
        }
        m_num = m_num +1;
    }
}

void RideManager::AddObjectType(int32 type)
{

    if(!HasOjbect(type))
    {
        REWARD_ITEM rewaritem;
        rewaritem.Clear();
        rewaritem.flag = REWARD_FLAG_NORMAL ;
        rewaritem.id = type;
        rewaritem.num =  rewaritem.num +1;
        //rewaritem.type = PROP_RIDE_PET;
        m_randomObject.push_back(rewaritem);
    }
}

void RideManager::SetAddRandomObject(uint32 ojbectid,int32 addNum)
{
    int32 maxgrass = Ride_Config::Instance()->GetMaxGrass();  //青草的最大数量
    int32 maxbug = Ride_Config::Instance()->GetMaxBug();      //虫的最大数量
    int32 maxhoney = Ride_Config::Instance()->GetMaxHoney();  //蜂蜜的最大数量

    bool b_findreward = false;
    VecRewardItem::iterator iter_reitem;
    for (iter_reitem = m_randomObject.begin();  iter_reitem!= m_randomObject.end(); iter_reitem++ )
    {

        if( iter_reitem->id == ojbectid)
        {
            iter_reitem->num = iter_reitem->num + addNum ;
            b_findreward = true;
            break;

        }
    }

    if(!b_findreward)
    {
        REWARD_ITEM rewaritem;
        rewaritem.Clear();
        rewaritem.flag = REWARD_FLAG_NORMAL ;
        rewaritem.id = ojbectid;
        rewaritem.num =  rewaritem.num +addNum;
        //rewaritem.type = PROP_RIDE_PET;
        m_randomObject.push_back(rewaritem);
    }
}

bool RideManager::HasOjbect(uint32 ojbectid)
{
    VecRewardItem::iterator iter_reitem;
    for (iter_reitem = m_randomObject.begin();  iter_reitem!= m_randomObject.end(); iter_reitem++ )
    {
        if( iter_reitem->id == ojbectid)
        {
            iter_reitem->num = iter_reitem->num + 1 ;
            return true;
        }
    }
    return false;
}

void RideManager::ClearMarkRide( void* data )
{
    //设置刻印的条件改变
    bool b_exe = false;

    RIDE_MARKT* ride_mark = 0;
    VecRideMarkList::iterator markLit;
    for (markLit = m_markList.begin(); markLit != m_markList.end(); markLit++)
    {
        if(markLit->pet_id != 0)
        {
            //ride_mark = &(*markLit);
            SetNoMark(markLit->pet_id);
            RIDE_PET_INFO* ride_pet = GetRideInfo(markLit->pet_id);
            if(ride_pet)
                m_updateRideList.push_back(*ride_pet);

            SetMarkRide(markLit->id, 0);

            m_updatemarkList.push_back(*markLit);
            b_exe = true;
        }
    }

    // 清空所有列表内的mark标记
    RidePetList::iterator ridelistiter;
    for (ridelistiter = m_ride_pet_list.begin(); ridelistiter != m_ride_pet_list.end(); ridelistiter++)
    {
        ridelistiter->b_mark = RIDE_PET_NO_MARK;
    }

    if(!b_exe)
    {
        m_player->NotifyErrorCode(ERR_NO_CLEAR_MARK_RIDE);
        return;
    }

    m_player->CalcProperty();

//     uint32 battle = m_player->GetPropertySetCalculater()->GetBattlePowerByRideMark() 
//         + m_player->GetPropertySetCalculater()->GetBattlePowerByRideAltas();
    m_player->SendTaskEvent(TASK_EVENT_BATTLE_POWER_MOUNT, 0, 1 ,0);

    SendMarkAck();
}
