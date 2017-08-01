#include "gameholder_pch.h"
#include "ride_manager.h"
#include "player.h"
#include "ridetrain.h"
#include "ride_config.h"
#include "item_manager.h"
#include "trade_manager.h"
#include "event_define.h"
#include "item_config_manager.h"
#include "vip_config.h"
#include "gameplaytimes_notifier.h"
#include "guild_science_recorder.h"
#include "config_manager.h"
#include "property_set_calculater.h"

RideTrain::RideTrain(Player* player, RideManager* ridemg):
m_player(player),
ridemg(ridemg)
{
}

RideTrain::~RideTrain(void)
{
}

void RideTrain::SetTrainInfo(const RidetTrainList& ridelist)
{
    m_rideTrainList = ridelist;
}

RidetTrainList& RideTrain::GetTrainInfo()
{
    return m_rideTrainList;
}

void RideTrain::TrainInit()
{
    if(m_rideTrainList.size() <=0 )
    {
        Map_TraiFiles::iterator iterTrain;
        Map_TraiFiles curTrain = Ride_Config::Instance()->GetTrainFields();
        for (iterTrain = curTrain.begin(); iterTrain != curTrain.end(); iterTrain++)
        {
            if(iterTrain->second.deafult == OPEN)
                AddTrain(iterTrain->first,OPEN);
            else
                AddTrain(iterTrain->first,NO_OPEN);
        }
    }
}
void RideTrain::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
        //开启训练栏位
    case  MSG_RIDE_PET_TRAIN_POS_REQ:
        {
            SetTrainField(data);
        }
        break;
        // 使用升级石开始训练请求
    case MSG_RIDE_PET_STONE_TRAIN_REQ:
        {
            StarTrain(data);
        }
        break;
        //骑宠完成训练请求
    case MSG_RIDE_PET_END_TRAIN_REQ:
        {
            EndTrain(data);
        }
        break;
        //骑宠立即完成训练请求
    case MSG_RIDE_PET_RIGHT_NOW_TRAIN_REQ:
        {
            RightNowTrain(data);
        }
        break;
        //设置骑宠的训练模式
    case MSG_RIDE_PET_SET_MODELS_REQ:
        {
            SetTrainModels(data);
        }
        break;
    default:
        break;
    }
}


void RideTrain::FieldCloseCondi()
{
    RidetTrainList  updateTrainList;
    RidetTrainList::reverse_iterator trainIt;
    int32 vipTrainNum = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_21 ,m_player->GetVipLevel());
    int32 trainNum = FindOpenTrainFields();

    // vip没有过期
    if(vipTrainNum >= trainNum) return;

    for(trainIt = m_rideTrainList.rbegin(); 
        (trainIt!= m_rideTrainList.rend()) && (vipTrainNum < trainNum);
        trainIt++)
    {
        //if(trainIt->state == NO_OPEN) continue;
        if(trainIt->state == WAIT_OPEN)
        {
            trainIt->state = NO_OPEN;
            updateTrainList.push_back(*trainIt);
            --trainNum;
        }
    }
    NotifyTrainNum();
    SendUpdateRideTrainList(updateTrainList);
}


void RideTrain::FieldOpenCondi()
{
    RidetTrainList updateTrainList;

    int32 num = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_21 ,m_player->GetVipLevel());
    int32 opennum = FindOpenTrainFields();
    if(opennum >=  num) return;

    RIDE_TRAIN* ridetrain = 0;
    RidetTrainList::iterator itertrain;
    for(itertrain = m_rideTrainList.begin(); (itertrain!= m_rideTrainList.end()) && (opennum < num ); itertrain++)
    {
        if(itertrain->state == NO_OPEN)
        {
            itertrain->state = WAIT_OPEN;
            updateTrainList.push_back(*itertrain);
            --num;
        }

    }

    NotifyTrainNum();
    SendUpdateRideTrainList(updateTrainList);
}


void RideTrain::SendUpdateRideTrainList(RidetTrainList& updateTrainList)
{
    RIDE_PET_TRAIN_LIST sendMsg;
    sendMsg.Clear();
    sendMsg.opt_flag = OPT_UPDATE_FLAG;
    sendMsg.ride_train_list = updateTrainList;
    m_player->SendToGate(MSG_RIDE_PET_TRAIN_INIT_LIST,&sendMsg);
}
void RideTrain::AddTrain(uint32 fieldid,int32 defaut)
{

    RIDE_TRAIN* curride = GetTrainingPet(fieldid);
    if(curride)
        return;
    RIDE_TRAIN ridetrain;
    ridetrain.Clear();
    ridetrain.id = fieldid;
    ridetrain.mode_id = 0 ;
    ridetrain.pet_id = 0;
    ridetrain.start_time = 0;
    ridetrain.state = defaut;
    m_rideTrainList.push_back(ridetrain);
}

//训练石添加经验
void RideTrain::StarTrain(void* data)
{
    STONE_TRAIN_REQ * recvMsg = (STONE_TRAIN_REQ*)data;
    if(!recvMsg)
        return;
    uint32  fieldid = recvMsg->id;
    uint32  petid = recvMsg->pet_id;
    // int32 itempos = recvMsg->item_pos;
    RIDE_TRAIN*  curtrain = 0;
    bool b_find = false;

    RIDE_TRAIN* itertrain = GetTrainingPet(fieldid);
    if(!itertrain)
        return;

    if(!itertrain->mode_id)
        return;

    if(petid != itertrain->pet_id)
        return;


    RIDE_PET_INFO ridepet;
    RidePetTemplate * ridePetTmpl = Ride_Config::Instance()->GetRidePet(itertrain->pet_id);
    if(!ridePetTmpl)
        return;

    RIDE_PET_INFO* ride_pet = ridemg->GetRideInfo(itertrain->pet_id);
    if(!ride_pet)
        return;

    if(ride_pet->b_train != ON_TRAINING) //训练中
        return;

    if(!CanLeveUp(&(*ride_pet)))
    {
        m_player->NotifyErrorCode(ERR_MAX_PLAYER);
        return;
    }

    int32 needNum = 1;
    int32 curvalue = 1;
    //curvalue = ItemConfig::Instance()->GetItemVaule(FOLLOW_PET_ITEM_STONE,ITEM_TYPE_RIDE_EXP);
    if(recvMsg->type == RIDE_LEVE_UP_USE_ITEM_ONCE)
    {
        int32 vip_stone = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_54, m_player->GetVipLevel());
        if(!vip_stone)
        {
            m_player->NotifyErrorCode(ERR_NO_ENOUGH_VIP);
            return;
        }

        needNum = CalNeedStoneNum(ride_pet);
        if(needNum <= 0)
        {
            needNum = 1;
            CnAssert(false);
        }
    }
    else if (recvMsg->type == RIDE_LEVE_UP_USE_ITEM_NO_ONCE)
    {
        needNum = 1;
    }

    //if(m_player->GetItemManager()->GetItemNum(FOLLOW_PET_ITEM_STONE) < needNum)
    //{
    //    m_player->NotifyErrorCode(ERR_FAILED);
    //    return;
    //}

    //m_player->GetItemManager()->DelItemByID(FOLLOW_PET_ITEM_STONE,needNum);

    curvalue = curvalue * needNum;
    if(curvalue <= 0)
        CnAssert(false);

    ride_pet->express = ride_pet->express + curvalue;
    TrainLeveup((*ride_pet));
}

int32 RideTrain::CalNeedStoneNum(RIDE_PET_INFO* ride_pet)
{
    if(!ride_pet)
        return 0;

    int32 itemnum = 0;
    int32 needexp = 0;
    int32 exp = 0;

    int32 max_exp = Ride_Config::Instance()->GetRidePetLeveupTem(ride_pet->pet_id, ride_pet->level);
    if(max_exp > ride_pet->express)
        needexp = max_exp - ride_pet->express;
    else
        CnAssert(false);

    //int32 curvalue = ItemConfig::Instance()->GetItemVaule(FOLLOW_PET_ITEM_STONE,ITEM_TYPE_RIDE_EXP);
    //if(curvalue <= 0)
    //    CnAssert(false);


    //itemnum = needexp / curvalue;
    //exp= needexp % curvalue;
    //if(exp > 0)
    //    itemnum = itemnum + 1;

    return itemnum;
}
bool RideTrain::CanLeveUp(RIDE_PET_INFO* ride_pet)
{
    if(!ride_pet)
    {
        return false;
    }
    if(ride_pet->level > m_player->GetLevel())
        return false;

    if(ride_pet->level == m_player->GetLevel())
    {
        int32 max_exp = Ride_Config::Instance()->GetRidePetLeveupTem(ride_pet->pet_id, ride_pet->level);
        if(ride_pet->express == (max_exp-1))
        {
            return false;
        }
    }

    return true;
}

void RideTrain::SetTrainField(void* data)
{
    TRAIN_POST_REQ* recvMsg = (TRAIN_POST_REQ*)data;
    uint32 fieldid = recvMsg->id;
    RIDE_TRAIN* ridetrain = 0;

    RIDE_TRAIN* itertrain = GetTrainingPet(fieldid);
    if(!itertrain)
        return;

    int32 num = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_21 ,m_player->GetVipLevel());
    if(itertrain->state == WAIT_OPEN)
    {
        ////花费钱玩了，那么开启
        TrainField* trainField = 0;
        trainField = Ride_Config::Instance()->FindTrainField(fieldid);
        if(!trainField)
            return;

        if(FindOpenTrainFields() <= num)
        {
            if (m_player->GetMoneyPoint()< trainField->monyepoint)
            {
                m_player->NotifyErrorCode(ERR_NO_ENOUGH_MONEYPOINT);
                return;
            }
            else
            {
                if (trainField->deafult == WAIT_OPEN)
                {
                    SetTrainField(fieldid);
                    return;
                }
                TradeManager::Instance()->SendTradeOrderReq(m_player,REWARD_REASON_RIDE_PET,PROP_RIDN_PET_TRAIN_FIELED,0,fieldid,trainField->monyepoint);
            }
        }


    }
}


void RideTrain::SetTrainField(uint32 fieldid)
{
    bool b_find = false;
    RIDE_TRAIN* ridetrain = 0;

    RIDE_TRAIN* itertrain = GetTrainingPet(fieldid);
    if(!itertrain)
        return;
    itertrain->state = OPEN;
    ridetrain = &(*itertrain);
    SendRideTrainList(OPT_UPDATE_FLAG,ridetrain);
    NotifyTrainNum();
}

void RideTrain::SendRideTrainList(int32 flag, RIDE_TRAIN* ridetrain)
{
    RIDE_PET_TRAIN_LIST sendMsg;
    sendMsg.Clear();
    switch(flag)
    {
    case OPT_UPDATE_FLAG:
        {
            sendMsg.opt_flag = OPT_UPDATE_FLAG;
            if(ridetrain)
            {
                sendMsg.ride_train_list.push_back(*ridetrain);
            }
        }
        break;
    default:
        sendMsg.opt_flag = flag;
        break;
    }

    m_player->SendToGate(MSG_RIDE_PET_TRAIN_INIT_LIST,&sendMsg);
}


void RideTrain::NotifyTrainNum()
{
    //   int32 num = FindOpenTrainFields();
    int32 num = 0;
    RidetTrainList::iterator itertrain;
    for(itertrain = m_rideTrainList.begin(); itertrain!= m_rideTrainList.end(); itertrain++)
    {
        if (itertrain->state == OPEN) 
        {
            if(!itertrain->pet_id)
                num = num + 1;
        }
    }
    m_player->GetNotifyManager()->SetGameplayTimes(NOTIFY_RIDE_PET_TRAINING, num);
}
void RideTrain::EndTrain(void* data)
{
    RIDE_TRAIN_REQ* recvMsg = (RIDE_TRAIN_REQ*)data;
    RIDE_TRAIN*  cur_ride_train = 0;
    RIDE_PET_INFO* cur_rideinfo = 0;
    uint32 fieldid = recvMsg->field_id;

    RIDE_TRAIN* itertrain = GetTrainingPet(fieldid);
    if(!itertrain)
        return;

    if(!itertrain->mode_id)
        return;
    if(!itertrain->pet_id)
        return;

    RIDE_PET_INFO* ride_pet = ridemg->GetRideInfo(itertrain->pet_id);
    if(!ride_pet)
        return;

    TrainModes* trainModes = 0;
    trainModes = Ride_Config::Instance()->GetTrainMode(itertrain->mode_id);
    if(!trainModes)
        return;



    uint32 nowTime = 0;
    nowTime = (uint32)Crown::SDNow().GetTimeValue();

    uint32 curtime = 0;
    curtime = nowTime - itertrain->start_time;
    uint32 modestime = 0;
    modestime = trainModes->time*HOUR_SECONDS;
    if(curtime >= modestime)
    {
        curtime = 0;
        itertrain->start_time = 0;
        itertrain->mode_id = 0;
        itertrain->pet_id = 0;
        ride_pet->b_train = ON_IDLE;
        cur_ride_train = &(*itertrain);
        cur_rideinfo = &(*ride_pet);
        SendRideEndTrain(ON_IDLE,curtime,cur_ride_train,cur_rideinfo);
        NotifyTrainNum();
    }
    else
    {
        curtime = modestime - curtime;
        cur_ride_train = &(*itertrain);
        SendRideEndTrain(ON_TRAINING,curtime,cur_ride_train);
    }
    //否则没有完成，发送时间差END_TRAIN_REQ
    //客户端每次初始化要申请一次是否完成训练，来得到时间差
}

void RideTrain::SendRideEndTrain(int32 flag,uint32 time,RIDE_TRAIN* ridetrain,RIDE_PET_INFO* rideinfo)
{
    END_TRAIN_ACK sendMsg;
    sendMsg.Clear();
    if(flag == ON_IDLE)
    {
        sendMsg.time= time;
        sendMsg.opt_flag = ON_IDLE;
        if(ridetrain)
        {
            sendMsg.ride_train = (*ridetrain);
        }
        if(rideinfo)
        {
            sendMsg.ride_petinfo = (*rideinfo);
        }
    }
    else if (flag == ON_TRAINING)
    {
        sendMsg.time= time;
        sendMsg.opt_flag = ON_TRAINING;
        if(ridetrain)
        {
            sendMsg.ride_train = (*ridetrain);
        }
        if(rideinfo)
        {
            sendMsg.ride_petinfo = (*rideinfo);
        }
    }

    m_player->SendToGate(MSG_RIDE_PET_TRAIN_END_ACK,&sendMsg);
}

void RideTrain::RightNowTrain(void* data)
{
    NOW_TRAIN_REQ* rec = (NOW_TRAIN_REQ*) data;
    uint32  fieldid = rec->id;
    uint32  petid = rec->pet_id;

    RIDE_PET_INFO* ride_pet = ridemg->GetRideInfo(petid);
    if(!ride_pet)
        return;

    //if(!CanLeveUp(&(*ride_pet)))
    //    return;


    if(ride_pet->b_train != ON_TRAINING) //训练中
        return;

    // 1魔石等于多长时间
    RIDE_TRAIN* itertrain = GetTrainingPet(fieldid);
    if(!itertrain)
        return;

    TrainModes* trainModes = 0;
    trainModes = Ride_Config::Instance()->GetTrainMode(itertrain->mode_id);
    if(!trainModes)
        return;

    uint32 timeToMoney = Ride_Config::Instance()->GetMoneyToScecond();

    uint32 needtime = 0;

    uint32 nowTime = 0;
    nowTime = (uint32)Crown::SDNow().GetTimeValue();

    uint32 curtime = 0;
    curtime = nowTime - itertrain->start_time;

    needtime = curtime;
    if(!needtime)
        return;
    uint32 modetime = 0;
    modetime = trainModes->time*HOUR_SECONDS;
    if(needtime >= modetime)
        return;

    if(!timeToMoney)
    {
        return;
    }

    int32 elsetime = 0;
    elsetime = modetime - needtime;
    if(!elsetime)
        return;

    int32 howmany= 0;
    howmany = elsetime/timeToMoney;
    int32 money = 0;
    money =  elsetime%timeToMoney;
    if(money >0)
    {
        howmany = howmany +1 ;
    }

    if(m_player->GetMoneyPoint() < howmany)
    {
        return;
    }
    else
    {
        TradeManager::Instance()->SendTradeOrderReq(m_player, REWARD_REASON_RIDE_PET, PROP_RIDN_PET_TRAIN_NOW, fieldid, petid, howmany);
    }
}

void RideTrain::RightNowTrain(uint32 fieldid,uint32 petid)
{
    RIDE_TRAIN*  cur_ride_train = 0;
    RIDE_PET_INFO* cur_rideinfo = 0;
    RIDE_TRAIN* itertrain = GetTrainingPet(fieldid);
    if(!itertrain)
        return;

    if(itertrain->pet_id != petid) //栏位上是否有对应的坐骑
        return;

    RIDE_PET_INFO* ride_pet =  ridemg->GetRideInfo(petid);
    if (!ride_pet)
        return;

    TrainModes* trainModes = 0;
    trainModes = Ride_Config::Instance()->GetTrainMode(itertrain->mode_id);
    if(!trainModes)
        return;


    itertrain->start_time = 0;
    itertrain->mode_id = 0;
    itertrain->pet_id = 0;
    ride_pet->b_train = ON_IDLE;
    cur_rideinfo = &(*ride_pet);
    cur_ride_train = &(*itertrain);

    ridemg->SendRidePetListNotify(OPT_UPDATE_FLAG,cur_rideinfo);
    SendRideTrainList(OPT_UPDATE_FLAG,cur_ride_train);

    NotifyTrainNum();

}

void RideTrain::SetTrainModels(void* data)
{
    RIDE_PET_SET_MODELS_REQ* recvMsg = (RIDE_PET_SET_MODELS_REQ*)data;
    uint32 fieldid = recvMsg->fieldid; 
    uint32 modelid = recvMsg->modeid;
    uint32 petid = recvMsg->petid;
    uint16 error = ERR_FAILED;


    // 检查功能开启
    //if(OpenSysConfig::Instance()->IsOpenSys(OPEN_SYS_RIDE_TRAIN, m_player) != ERR_SUCCEED)
    //{
    //    m_player->NotifyErrorCode(ERR_FUNCT_NOT_OPEN);
    //    return;
    //}


    //如果已经在训练位上，不能在在其他的训练位上
    RidetTrainList::iterator curitertrain;
    for(curitertrain = m_rideTrainList.begin(); curitertrain!= m_rideTrainList.end(); curitertrain++)
    {
        if(curitertrain->pet_id == petid)
            return;
    }

    RIDE_TRAIN*  ridetrain = 0;
    RIDE_TRAIN* itertrain = GetTrainingPet(fieldid);
    if(!itertrain)
        return;

    itertrain->pet_id = petid;

    if(!itertrain->pet_id)
        return;

    TrainModes* trainModes = 0;
    trainModes = Ride_Config::Instance()->GetTrainMode(modelid);
    if(!trainModes)
        return;

    //if(!TrainModesOpen(trainModes))
    if(!TrainModesOpen(modelid))
    {
        error = ERR_TRAIN_MODELS_NO_OPEN;
        SendRideTrainList(error);
        return;
    }

    RIDE_PET_INFO* ride_pet =  ridemg->GetRideInfo(itertrain->pet_id);
    if (!ride_pet)
        return;

    if(ride_pet->b_train == ON_TRAINING)
        return;

    if(!CanLeveUp(&(*ride_pet)))
        return;

    //到底是花游戏币还是突飞石

    if(trainModes->spend.key == PROP_GAMEPOINT)
    {
        if(m_player->GetGamePoint() < trainModes->spend.num)
        {
            error = ERR_NO_ENOUGH_GAMEPOINT;
            SendRideTrainList(error);
        }
        else
        {
            m_player->SubItemOne(REWARD_REASON_RIDE_PET,PROP_GAMEPOINT,trainModes->spend.num);
            itertrain->mode_id = modelid;

            ride_pet->b_train = ON_TRAINING; //训练中

            // 工会科技加成：坐骑训练经验加成
            int32 addExp = trainModes->exp;
            m_player->GetGuildScienceManager()->AddAdditionPromoteNum(GUILD_ADDITION_MOUNT_TRAIN_EXP, addExp);

            ride_pet->express = ride_pet->express + addExp;
            TrainLeveup((*ride_pet));
            itertrain->start_time = (uint32)Crown::SDNow().GetTimeValue();
            ridetrain = &(*itertrain);
            SendRideTrainList(OPT_UPDATE_FLAG,ridetrain);
            NotifyTrainNum();
            // 活跃度
            m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_RIDE_TRAIN, 1, 0, 0);

//             uint32 battle = m_player->GetPropertySetCalculater()->GetBattlePowerByRideMark() 
//                 + m_player->GetPropertySetCalculater()->GetBattlePowerByRideAltas();
            m_player->SendTaskEvent(TASK_EVENT_BATTLE_POWER_MOUNT, 0, 1 ,0);
        }
    }
    else if(trainModes->spend.key == PROP_MONEYPOINT)
    {
        if(m_player->GetMoneyPoint() < trainModes->spend.num)
        {
            error = ERR_NO_ENOUGH_MONEYPOINT;
            SendRideTrainList(error);
        }
        else
        {
            //TradeManager::Instance()->SendTradeOrderReq(m_player,REWARD_REASON_RIDE_PET,PROP_SUB_RIDN_PET_MODELS,fieldid,modelid,trainModes->spend.num);
        }
    }
}

bool RideTrain::TrainModesOpen(uint32 id)
{
    if(ConfigManager::Instance()->IsTW())
    {
        //int32 curvaue = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_44, m_player->GetVipLevel());
        return true;
    }

    int32 curvaue = VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_44, m_player->GetVipLevel());
    int32 intnum = id % 1000;
    if(curvaue >= intnum)
        return true;
    else
        return false;

    return false;
}

void RideTrain::SetTrainModels(uint32 fieldid, uint32 modelid)
{
    bool b_find = false;
    RIDE_TRAIN* ridetrain = 0;
    RIDE_TRAIN* itertrain = GetTrainingPet(fieldid);
    if(!itertrain)
        return;

    RIDE_PET_INFO* ride_pet =  ridemg->GetRideInfo(itertrain->pet_id);
    if (!ride_pet)
        return;

    if(ride_pet->b_train == ON_TRAINING)
        return;

    itertrain->mode_id = modelid;

    TrainModes* trainModes = 0;
    trainModes = Ride_Config::Instance()->GetTrainMode(modelid);
    if(!trainModes)
        return;

    ride_pet->b_train = ON_TRAINING; //训练中

    // 工会科技加成：坐骑训练经验加成
    int32 addExp = trainModes->exp;
    m_player->GetGuildScienceManager()->AddAdditionPromoteNum(GUILD_ADDITION_MOUNT_TRAIN_EXP, addExp);

    ride_pet->express = ride_pet->express + addExp;
    TrainLeveup((*ride_pet));
    itertrain->start_time = (uint32)Crown::SDNow().GetTimeValue();
    ridetrain = &(*itertrain);
    SendRideTrainList(OPT_UPDATE_FLAG,ridetrain);

    NotifyTrainNum();
    // 活跃度
    m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_RIDE_TRAIN, 1, 0, 0);

//     uint32 battle = m_player->GetPropertySetCalculater()->GetBattlePowerByRideMark() 
//         + m_player->GetPropertySetCalculater()->GetBattlePowerByRideAltas();
    m_player->SendTaskEvent(TASK_EVENT_BATTLE_POWER_MOUNT, 0, 1 ,0);

}

RIDE_TRAIN* RideTrain::GetTrainingPet(uint32 fieldid)
{
    RIDE_TRAIN*  cur_ride_train = 0;
    RidetTrainList::iterator itertrain;
    for(itertrain = m_rideTrainList.begin(); itertrain!= m_rideTrainList.end(); itertrain++)
    {
        if(itertrain->id == fieldid)
        {
            cur_ride_train = &(*itertrain);
            break;
        }
    }
    return cur_ride_train;
}


int32 RideTrain::FindOpenTrainFields()
{
    int32  num = 0;
    RidetTrainList::iterator itertrain;
    for(itertrain = m_rideTrainList.begin(); itertrain!= m_rideTrainList.end(); itertrain++)
    {
        if((itertrain->state == WAIT_OPEN) || (itertrain->state == OPEN) )
        {
            num = num  + 1;
        }
    }
    return num;
}
void RideTrain::TrainLeveup(RIDE_PET_INFO& ride_pet)
{
    bool b_find = false;
    while(ride_pet.level < m_player->GetLevel())
    {
        int32 max_exp = Ride_Config::Instance()->GetRidePetLeveupTem(ride_pet.pet_id, ride_pet.level);

        if(ride_pet.express >= max_exp)
        {
            ride_pet.express =  ride_pet.express  - max_exp;
            ride_pet.level = ride_pet.level + 1;
            b_find  = true;
        }
        else
        {
            break;
        }
    }
    if(ride_pet.level == m_player->GetLevel())
    {
        int32 max_exp = Ride_Config::Instance()->GetRidePetLeveupTem(ride_pet.pet_id, ride_pet.level);

        if(ride_pet.express >= max_exp)
        {
            ride_pet.express = max_exp - 1; 
        }
    }

    if(b_find)
    {
        m_player->CalcProperty();
    }

    ridemg->SendRidePetListNotify(OPT_UPDATE_FLAG,&ride_pet);

    //发送任务训练等级
    m_player->SendTaskEvent(TASK_EVENT_RIDE_LEVEL, ride_pet.pet_id, ride_pet.level);
}

//花费磨石的处理
void RideTrain::UseMoneyWay(uint32 itemType, uint32 itemID, uint32 itemNum)
{
    switch(itemType)
    {
        // 购买骑宠训练模式
    //case PROP_SUB_RIDN_PET_MODELS:
    //    {
    //        SetTrainModels(itemID,itemNum);
    //    }
    //    break;
        // 购买骑宠的训练栏位
    case PROP_RIDN_PET_TRAIN_FIELED:
        {
            SetTrainField(itemNum);
        }
        break;
        // 立即完成锻炼的跟宠
    case PROP_RIDN_PET_TRAIN_NOW:
        {
            RightNowTrain(itemID,itemNum);
        }
        break;
    }
}

