#include "gameholder_pch.h"
#include "fashion_manager.h"
#include "fashion_config.h"
#include "player.h"
#include "property_set_calculater.h"
#include "item_manager.h"
#include "trade_manager.h"
#include "item_config_manager.h"
#include "sign_manager.h"
#include "event_define.h"
#include "game_util.h"
#include "game_define.h"
#include "global_setting_manager.h"
#include "package_manager.h"
#include "vip_config.h"
#include "config_manager.h"

bool SortFashion(const FASHION_INFO& a, const FASHION_INFO& b)
{
    return a.fashion_id < b.fashion_id;
}

FashionManager::FashionManager(Player* player):
m_player(player)
{
    Clear();
}

FashionManager::~FashionManager()
{
}

// ==================================================
// 清除
// ==================================================
void FashionManager::Clear()
{
    m_face_id       = 0;            ///< 脸ID
    m_fashion_id    = 0;            ///< 时装ID

    m_face_list.clear();            ///< 脸谱列表
}

// ==================================================
// 接收消息
// ==================================================
void FashionManager::OnRecv(uint32 msgID, void * data)
{
    switch(msgID)
    {
    case MSG_SET_FASHION_REQ:
        {
            OnSetFashionReq(data);          // 设置时装
        }
        break;
    case MSG_FASHION_COMPOSE_REQ:
        {
            // 合成时装
            FashionCompose(data);
        }
        break;
    case MSG_FASHION_ACTIVATE_REQ:
        {
            OnActivateFashion(data);
        }
        break;
    case MSG_FASHION_LEVEL_UP_REQ:
        {
            OnFashionLevelUp(data);
        }
        break;
    case MSG_FASHION_GET_TREASURE_REQ:
        {
            OnGetTreasureReq(data);
        }
        break;
    case MSG_FASHION_SMELT_REQ:
        {
            onFashionSmeltReq(data);
        }
        break;
    case MSG_INCARNATION_BUY_REQ:
        {
            onIncarnationBuyReq(data);
        }
        break;
    case MSG_FASHION_SKILL_REQ:
        {
            OnSetFashionSkillId(data);
        }
        break;
    case MSG_FASHION_SKILL_INFO_REQ:
        {
            OnGetFashionSkillInfo(data);
        }
        break;
    default:
        break;
    }
}

// ==================================================
// 加载装备信息
// ==================================================
void FashionManager::LoadFashionInfo(const FASHION_BAG& info)
{
    ////m_face_id = info.face_id;           ///< 脸ID
    //m_fashion_id = info.fashion_id;     ///< 时装ID
    //m_fashionSkillId = info.fashion_skill_id;
    //const FashionSkillInfo* fashionSkill = FashionSkillConfig::Instance()->GetFashionSkill(m_fashionSkillId);
    //if(fashionSkill)
    //{
    //    m_RSkillId = fashionSkill->skill_id;
    //}else{
    //    m_RSkillId = 0;
    //}
    //// 脸谱列表
    ////m_face_list = info.face_list;

    //// 复制时装列表
    //m_fashion_list = info.fashion_list;
    //m_randomList = info.random_list;
    //m_last_finish_time = info.last_finish_time;

    //m_packageCountMap.clear();
    //for (std::vector<PACKAGE_COUNT_INFO>::const_iterator counti = info.package_count.begin(); counti != info.package_count.end(); ++counti)
    //{
    //    m_packageCountMap[counti->package_id] = counti->package_count;
    //}
    //
    //m_packageItemTime.clear();
    //Property itemTime;
    //for (std::vector<PACKAGE_TIME_INFO>::const_iterator timei = info.package_time.begin(); timei != info.package_time.end(); ++timei)
    //{
    //    itemTime.Clear();
    //    itemTime.type = timei->item_id;
    //    itemTime.value = timei->item_time;
    //    m_packageItemTime[timei->package_id].push_back(itemTime);
    //}

    //InitQualityNum();
    //InitFashionSkill(FALSE);
    //SetFashionSkillID();
}

// ==================================================
// 填充装备信息
// ==================================================
void FashionManager::FillFashionInfo(FASHION_BAG& info)
{
    //info.face_id = m_face_id;                 // 脸ID
    info.fashion_id = m_fashion_id;             // 时装ID
    info.fashion_skill_id = m_fashionSkillId;
    // 脸谱列表
    //info.face_list = m_face_list;

    // 时装列表
    info.fashion_list = m_fashion_list;
    info.random_list = m_randomList;
    info.last_finish_time = m_last_finish_time;
//     info.fashion_battle_power = m_player->GetPropertySetCalculater()->GetBattlePowerByFashion();

    info.package_count.clear();
    PACKAGE_COUNT_INFO count_info;
    for(std::map<uint32, uint32>::iterator counti = m_packageCountMap.begin(); counti != m_packageCountMap.end(); ++counti)
    {
        count_info.Clear();
        count_info.package_id = counti->first; 
        count_info.package_count = counti->second;
        info.package_count.push_back(count_info);
    }
    

    info.package_time.clear();
    PACKAGE_TIME_INFO time_info;
    for(std::map<uint32, PropertyList>::iterator timei = m_packageItemTime.begin(); timei != m_packageItemTime.end(); ++timei)
    {
        for (std::vector<Property>::iterator it = m_packageItemTime[timei->first].begin(); it != m_packageItemTime[timei->first].end(); it++)
        {
           time_info.Clear();
           time_info.package_id = timei->first;
           time_info.item_id = it->type;
           time_info.item_time = it->value;
           info.package_time.push_back(time_info);
        }
    }
}

// ==================================================
// 计算时装属性
// ==================================================
void FashionManager::CalFashionProp()
{
    PropertySet propSet;


    // 化身属性
    for(FashionList::iterator fashionvo = m_fashion_list.begin(); fashionvo != m_fashion_list.end(); fashionvo++)
    {
        const FashionItem* fashionItem  = FashionConfig::Instance()->GetNewFashion(fashionvo->fashion_id);
        if(!fashionItem)
        {
            CnWarn("fashion id=%d not exist\n", fashionvo->fashion_id);
            continue;
        }
        propSet.Add(fashionItem->prolist);      // 化身基础属性
        for(FashionLevelList::const_iterator levelPro = fashionItem->levelUpList.begin(); levelPro != fashionItem->levelUpList.end(); levelPro++)
        {
            if(fashionvo->fashion_level == (uint32)levelPro->level)
            {
                propSet.Add(levelPro->prolist);     // 化身升级属性
            }
        }
    }

    // 化身连携
    bool isopen = true;
    const SynergismList& jointList = FashionConfig::Instance()->GetSynergismList();
    for(SynergismList::const_iterator joint = jointList.begin(); joint != jointList.end(); joint++)
    {
        isopen = true;
        for(PropertyList::const_iterator fashion = joint->fashuins.begin(); fashion != joint->fashuins.end(); fashion++)
        {
            if(fashion->type == JOINT_TYPE_FASHION)
            {
                if(!CheckHaveFashion(fashion->value))
                    isopen= false;
            }
            else if (fashion->type == JOINT_TYPE_PET)
            {
                //if(!m_player->GetFollowPetManager()->CheckActivieFollowpet(fashion->value))
                //    isopen = false;
            }
        }

        if(isopen)
        {
            propSet.Add(joint->prolist);
        }
    }
    

//    m_player->GetPropertySetCalculater()->CalcBattlePowerByFashion(propSet);
}

//===================================================
//添加时装（脸，翅膀，时装）
//===================================================
void FashionManager::AddFashion(uint32 fashion_id)
{
    CnAssert(false);
    return;
    // 再无此功能
    /*
    FASHION_INFO fashionInfo;
    const FashionTemplate* fashionTmpl = 0;
    bool find_fashion = false;

    // 查找时装
    fashionTmpl = FashionConfig::Instance()->GetFashion(fashion_id);
    if(!fashionTmpl) return ;

    if(fashionTmpl->type == EQUIP_FACE)
    {
        // 检查是否已有同样ID的脸谱
        for(FashionList::iterator it = m_face_list.begin(); it != m_face_list.end(); ++it)
        {
            if(it->fashion_id != fashion_id) continue;
            find_fashion = true;
            break;
        }
    }

    else if(fashionTmpl->type == EQUIP_FASHION ||(fashionTmpl->type == EQUIP_MEDAL))
    {
        // 检查是否已有同样ID的时装
        for(FashionList::iterator it = m_fashion_list.begin(); it != m_fashion_list.end(); ++it)
        {
            if(it->fashion_id != fashion_id) continue;
            find_fashion = true;
            break;
        }
    }
    else
    {
        return;
    }

    if(find_fashion)
    {
        m_player->NotifyErrorCode(ERR_ADD_FASHION);
        return; // 不重复添加同样ID的时装
    }

    switch(fashionTmpl->type)
    {
    case EQUIP_FACE:        // 脸
        {
            fashionInfo.fashion_id = fashion_id;
            m_face_list.push_back(fashionInfo);

            // 按时装的ID排序
            std::sort(m_face_list.begin(), m_face_list.end(), SortFashion);
        }
        break;
    case EQUIP_FASHION:     // 时装
    case EQUIP_MEDAL:
        {
            fashionInfo.fashion_id = fashion_id;
            m_fashion_list.push_back(fashionInfo);
            m_player->CalPlayerProp();
            // 按时装的ID排序
            std::sort(m_fashion_list.begin(), m_fashion_list.end(), SortFashion);
        }
        break;
    default:
        {
            return ;
        }
        break;
    }


    // 通知时装列表更新
    SendFashionListNotify(0,fashionTmpl->type, fashion_id);
    */
}

// ==================================================
// 发送时装列表通知
// ==================================================
void FashionManager::SendFashionListNotify()
{
    FASHION_LIST sendMsg;

    sendMsg.opt_flag = OPT_UPDATE_FLAG;
    sendMsg.fashion_list = m_fashion_list;
    sendMsg.skill_list = m_skillIdList;
       
    m_player->SendToGate(MSG_FASHION_LIST_NTF, &sendMsg);
}

// ==================================================
// 时装合成
// ==================================================
void FashionManager::FashionCompose(void* data)
{
    CnAssert(false);
    return;
    // 再无此功能
    /*
    uint16 error = ERR_FAILED;
    FASHION_COMPOSE_REQ* fashioncompo =(FASHION_COMPOSE_REQ*) data;
    const FashionTemplate* fashionTmpl = 0;
    bool find_fashion = false;

    // 查找时装
    fashionTmpl = FashionConfig::Instance()->GetFashion(fashioncompo->id);
    if(!fashionTmpl)
        return ;

    bool b_exe = false;
    if(fashionTmpl->flag == FASHION_COMPOSE || fashionTmpl->flag == FASHION_COMPOSE_BUY)
    {
        b_exe = true;
    }
    else
    {
        error = ERR_COMPOSE_FASHION;
        m_player->NotifyErrorCode(error);
        return;
    }

    if(!b_exe)
        return;

    if(CheckHaveFashion(fashioncompo->id))
    {
        error = ERR_ADD_FASHION;
        m_player->NotifyErrorCode(error);
        return;
    }

    error = SubFashionCompose(fashionTmpl);
    if(error != ERR_SUCCEED)
    {
        m_player->NotifyErrorCode(error);
        return;
    }

    AddFashion(fashioncompo->id);
    */
}

void FashionManager::OnSetFashionReq(void* data)
{
    //SET_FASHION_REQ* recvMsg = (SET_FASHION_REQ*)data;
    //const FashionItem* fashionTmpl = 0;
    //bool is_find = false;

    //if(!recvMsg)
    //{
    //    m_player->NotifyErrorCode(ERR_FAILED);
    //    return;
    //}

    //if(recvMsg->fashion_id == 0 && recvMsg->fashion_type == EQUIP_FASHION)
    //{
    //    m_fashion_id = 0;
    //    m_player->NotifyProperty(PROP_FASHION, m_fashion_id);
    //}
    //else
    //{
    //    if(recvMsg->fashion_type == EQUIP_FASHION)
    //    {
    //        // 检查是否已有同样ID的时装
    //        for(FashionList::iterator it = m_fashion_list.begin(); it != m_fashion_list.end(); ++it)
    //        {
    //            if(it->fashion_id != recvMsg->fashion_id) continue;
    //            is_find = true;
    //            break;
    //        }
    //    }

    //    if(!is_find)
    //    {
    //        m_player->NotifyErrorCode(ERR_FAILED);
    //        return;
    //    }

    //    // 查找时装模板
    //    fashionTmpl = FashionConfig::Instance()->GetNewFashion(recvMsg->fashion_id);
    //    if(!fashionTmpl)
    //    {
    //        m_player->NotifyErrorCode(ERR_FAILED);
    //        return;
    //    }

    //    m_fashion_id = fashionTmpl->id;
    //    m_player->NotifyProperty(PROP_FASHION, m_fashion_id);
    //}
}

void FashionManager::SetUseItemFashionRecuit(int32 curid)
{
    AddFashion(curid);
}

//时装图鉴
void FashionManager::FashionAlatsReq(void* data)
{
    SendFashionListNotify();
}

bool FashionManager::CheckHaveFashion(uint32 fashion_id)
{
    for(FashionList::iterator it = m_fashion_list.begin(); it != m_fashion_list.end(); it++)
    {
        if (fashion_id == it->fashion_id)
        {
            return true;
        }
    }

    return false;
}

uint16 FashionManager::SubFashionCompose(const FashionTemplate* fashionTmpl)
{
    REWARD_TABLE expendList;
    expendList.reason = REWARD_REASON_FASHION;
    REWARD_ITEM expendItem;

    expendItem.id = PROP_GAMEPOINT;
    expendItem.num = fashionTmpl->m_money;
    expendList.items.push_back(expendItem);

    //std::vector<REWARD_ITEM>::const_iterator vecRewardItem;
    //for (vecRewardItem = fashionTmpl->vec_compose.begin(); vecRewardItem!= fashionTmpl->vec_compose.end(); vecRewardItem++)
    //{
    //    if(vecRewardItem->type == PROP_ITEM)
    //    {
    //        expendItem.Clear();
    //        expendItem.type = PROP_ITEM;
    //        expendItem.id = vecRewardItem->id;
    //        expendItem.num = vecRewardItem->num;
    //        expendList.items.push_back(expendItem);
    //    }
    //}

    return m_player->SubItemTable(expendList);
}

void FashionManager::SendFashionComposeAck(uint16 error,uint32 fashionid)
{
    FASHION_COMPOSE_ACK sendMsg;
    sendMsg.errcode = error;
    if(fashionid)
    {
        sendMsg.fashion_id = fashionid;
    }
    m_player->SendToGate(MSG_FASHION_COMPOSE_ACK, &sendMsg);
}

//================================================
//激活时装
//================================================
void FashionManager::OnActivateFashion( void* data )
{
    uint16 error = ERR_FAILED;
    FASHION_ACTIVATE_REQ* fashionreq =(FASHION_ACTIVATE_REQ*) data;
    if(!fashionreq)
    {
        SendActivateFashionAck(ERR_FAILED);
        return;
    }
    // 检查功能开启

    // 查找时装
    const FashionItem* fashionItem = FashionConfig::Instance()->GetNewFashion(fashionreq->fashion_id);
    if(!fashionItem)
    {
        SendActivateFashionAck(ERR_UNFOUND_CONFIG);
        return;
    }

    // 检查是否已有同样ID的时装
    bool find_fashion = false;
    for(FashionList::iterator it = m_fashion_list.begin(); it != m_fashion_list.end(); ++it)
    {
        if(it->fashion_id != fashionreq->fashion_id) 
            continue;
        find_fashion = true;
        break;
    }

    if(find_fashion)
    {
        SendActivateFashionAck(ERR_ADD_FASHION);
        return; // 不重复添加同样ID的时装
    }


    //激活物品是否足够
    int32 itemNum = 0;
    bool find_item = false;
    bool find_service = false;
    bool card_bool = false;
    for(FashionItemList::const_iterator bagItem = fashionItem->openList.begin(); bagItem != fashionItem->openList.end(); bagItem++)
    {
        //if(bagItem->type == PROP_ITEM)
        //{
        //    //if(fashionItem->incarnationCard.size() >= 1)
        //    //{
        //    //    itemNum = m_player->GetItemManager()->GetItemNum(fashionItem->incarnationCard[0].id);
        //    //    if(itemNum >= fashionItem->incarnationCard[0].num)
        //    //    {
        //    //        //有幻化卡
        //    //        card_bool = true;
        //    //        break;
        //    //    }
        //    //}

        //    //if(!card_bool)
        //    //{
        //    //    itemNum = m_player->GetItemManager()->GetItemNum(bagItem->id);
        //    //    if(itemNum >= bagItem->num)
        //    //    {
        //    //        find_item = true;
        //    //        break;
        //    //    }
        //    //}

        //}
        // else if(bagItem->type == FASHION_ACTIVATE_TYPE_VIP)
        //{  
        //    //vip
        //    if(bagItem->num <= m_player->GetVipLevel())
        //    {
        //        find_service = true;
        //        break;
        //    }
        //}
        //else if(bagItem->type == FASHION_ACTIVATE_TYPE_LEVEL)
        //{
        //    //等级
        //    if(bagItem->num <= m_player->GetLevel())
        //    {
        //        find_service = true;
        //        break;
        //    }
        //}
    }

    if(!card_bool && !find_item && !find_service)
    {
        SendActivateFashionAck(ERR_NO_ACTIVATE_ITEM);
        return; // 不重复添加同样ID的时装
    }

    for(FashionItemList::const_iterator bagItem = fashionItem->openList.begin(); bagItem != fashionItem->openList.end(); bagItem++)
    {
        //if(bagItem->type == PROP_ITEM)
        //{
        //    //if(fashionItem->incarnationCard.size() >= 1)
        //    //{
        //    //    itemNum = m_player->GetItemManager()->GetItemNum(fashionItem->incarnationCard[0].id);
        //    //    if(itemNum >= fashionItem->incarnationCard[0].num)
        //    //    {
        //    //        //有幻化卡
        //    //        //m_player->GetBag()->DelItemByID(fashionItem->incarnationCard[0].id, fashionItem->incarnationCard[0].num);
        //    //        break;
        //    //    }
        //    //}

        //    ////不是服务开启
        //    //if(!find_service)
        //    //{
        //    //    itemNum = m_player->GetItemManager()->GetItemNum(bagItem->id);
        //    //    if(itemNum >= bagItem->num)
        //    //    {
        //    //        //m_player->GetBag()->DelItemByID(bagItem->id, bagItem->num);
        //    //        break;
        //    //    }
        //    //}
        //}
    }

    FASHION_INFO fashionInfo;
    fashionInfo.fashion_id = fashionreq->fashion_id;
    fashionInfo.fashion_level = 0;
    m_fashion_list.push_back(fashionInfo);
    InitFashionSkill(TRUE);
    m_player->CalcProperty();
    // 按时装的ID排序
    std::sort(m_fashion_list.begin(), m_fashion_list.end(), SortFashion);

    // 通知时装列表更新
    SendFashionListNotify();

    SendActivateFashionAck(ERR_SUCCEED);

    //统计化身星级数，激活了哪些任务
    AddQualityNum(fashionItem->quality);
    SendTaskEvent();

    m_player->SendPlaySystemLog(PLAYSYSTEM_FASHION_OPEN, fashionInfo.fashion_id);

    m_player->SendTaskEvent(TASK_EVENT_BATTLE_POWER_FASHION, 0, 1 ,0);

    // 全服公告 激活化身
    if (fashionItem->quality >= 3)
    {
        FashionMarquee(fashionItem->id,fashionItem->quality);
    }
}

//统计xx星化身的个数
void FashionManager::InitQualityNum()
{
    qualityNumList.clear();

    int32 maxstart = FashionConfig::Instance()->GetMaxQuality(); //最大的星级数
    if(maxstart <= 0)
    {
        CnAssert(false);
        return;
    }

    for (int32 i =1; i<maxstart; i++)
    {
        FashionQualityNum curquality;
        curquality.level = i;
        curquality.num = 0;
        qualityNumList.push_back(curquality);
    }

    for (FashionList::iterator fashioniter = m_fashion_list.begin(); fashioniter !=  m_fashion_list.end(); fashioniter++)
    {
         const FashionItem* fashionItem = FashionConfig::Instance()->GetNewFashion(fashioniter->fashion_id);
         if(fashionItem)
         {
             AddQualityNum(fashionItem->quality);
         }
    }
  
}

void FashionManager::SendTaskEvent()
{
    QualityNumList::iterator quality_iter;
    for (quality_iter = qualityNumList.begin(); quality_iter != qualityNumList.end(); quality_iter++)
    {
        if(quality_iter->num >0)
            m_player->SendTaskEvent(TASK_EVENT_FASHION, quality_iter->level, quality_iter->num);
    }
}

//添加xx化身的个数
void FashionManager::AddQualityNum(int32 quality)
{
    QualityNumList::iterator quality_iter;
    for (quality_iter = qualityNumList.begin(); quality_iter != qualityNumList.end(); quality_iter++)
    {
        if(quality == quality_iter->level)
        {
            quality_iter->num = quality_iter->num + 1;
            break;
        }
    }
}
//=================================================
//激活升级
//=================================================
void FashionManager::OnFashionLevelUp( void* data )
{
    uint16 error = ERR_FAILED;
    FASHION_ACTIVATE_REQ* fashionreq =(FASHION_ACTIVATE_REQ*) data;
    if(!fashionreq)
    {
        SendFashionLevelUpAck(ERR_FAILED);
        return;
    }
    // 检查功能开启

    // 检查是否已有同样ID的时装
    FASHION_INFO* curfashion = NULL;
    for(FashionList::iterator it = m_fashion_list.begin(); it != m_fashion_list.end(); ++it)
    {
        if(it->fashion_id == fashionreq->fashion_id) 
        {
            curfashion = &(*it);
            break;
        }
    }

    if(!curfashion)
    {
        SendFashionLevelUpAck(ERR_NO_ENOUGH_INCARNATION);
        return;
    }

    //查找升级需要物品
    const FashionLevelUp* lvup = FashionConfig::Instance()->GetFashionNextLevel(curfashion->fashion_id, curfashion->fashion_level + 1);
    if(!lvup)
    {
        SendFashionLevelUpAck(ERR_UNFOUND_CONFIG);
        return;
    }

    //激活物品是否足够
    int32 itemNum = 0;
    bool find_item = false;
    //for(FashionItemList::const_iterator bagItem = lvup->itemList.begin(); bagItem != lvup->itemList.end(); bagItem++)
    //{
    //    if(bagItem->type == PROP_ITEM)
    //    {
    //        itemNum = m_player->GetItemManager()->GetItemNum(bagItem->id);
    //        if(itemNum < bagItem->num)
    //        {
    //            find_item = true;
    //            break;
    //        }
    //    }
    //}

    if(find_item)
    {
        SendFashionLevelUpAck(ERR_NO_FASHION_LEVEL_UP_ITEM);
        return; // 升级物品不足
    }

    //for(FashionItemList::const_iterator bagItem = lvup->itemList.begin(); bagItem != lvup->itemList.end(); bagItem++)
    //{
    //    if(bagItem->type == PROP_ITEM)
    //    {
    //        itemNum = m_player->GetItemManager()->GetItemNum(bagItem->id);
    //        if(itemNum >= bagItem->num)
    //        {
    //            //m_player->GetBag()->DelItemByID(bagItem->id, bagItem->num);
    //            break;
    //        }
    //    }
    //}

    curfashion->fashion_level += 1;
     m_player->CalcProperty();
    InitFashionSkill(TRUE);

    // 通知时装列表更新
    SendFashionListNotify();
    SendFashionLevelUpAck(ERR_SUCCEED);

    //const FashionItem* fashionItem  = FashionConfig::Instance()->GetNewFashion(curfashion->fashion_id);

    //m_player->SendTaskEvent(TASK_EVENT_FASHION_LEVEL, curfashion->fashion_level, curfashion->fashion_id);
    //m_player->SendTaskEvent(TASK_EVENT_FASHION_NUM_STAR_LEVEL, fashionItem->quality, curfashion->fashion_level);
    m_player->SendTaskEvent(TASK_EVENT_FASHION_LEVEL, 0, 0);
    m_player->SendTaskEvent(TASK_EVENT_FASHION_NUM_STAR_LEVEL, 0, 0);

    m_player->SendTaskEvent(TASK_EVENT_BATTLE_POWER_FASHION, 0, 1 ,0);
}

//========================================================
//时装激活通知
//========================================================
void FashionManager::SendActivateFashionAck( uint16 error )
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = error;
    m_player->SendToGate(MSG_FASHION_ACTIVATE_ACK, &sendMsg);
}

//========================================================
//时装升级通知
//========================================================
void FashionManager::SendFashionLevelUpAck( uint16 error )
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = error;
    m_player->SendToGate(MSG_FASHION_LEVEL_UP_ACK, &sendMsg);
}

//=================================================
//时装宝藏抽取
//=================================================
void FashionManager::OnGetTreasureReq( void* data )
{
    uint16 error = ERR_FAILED;
    FASHION_GET_TREASURE_REQ* fashionreq =(FASHION_GET_TREASURE_REQ*) data;
    if(!fashionreq)
        return;
    // 检查功能开启
    
    const FashionTreasureItem* trea = FashionConfig::Instance()->GetFashionTreasureNeed(fashionreq->type);
    if(!trea)
    {
        SendFashionGetTREasureAck(error, fashionreq->type, m_last_finish_time);
        return;
    }

    std::vector<uint32> vc;
    //int32 goodNum;
    bool freeGold = false;
    FashionTreasureConfigItemList item;

    bool find_item = false;
    for(FashionItemList::const_iterator bagItem = trea->itemList.begin(); bagItem != trea->itemList.end(); bagItem++)
    {
        //if(bagItem->type == PROP_GAMEPOINT)
        //{
        //    if (m_player->GetGamePoint() < bagItem->num) // 检查金币数量
        //    {
        //        SendFashionGetTREasureAck(ERR_NO_ENOUGH_GAMEPOINT, fashionreq->type, m_last_finish_time);
        //        return;
        //    }
        //    //m_player->SubGamePoint(bagItem->num, REWARD_REASON_MONEY_FASHION_TREASURE);
        //}
        //else if(bagItem->type == PROP_ITEM && bagItem->id == GameParamConfig::Instance()->GetTreasureTroveCardId())
        //{
        //    if(fashionreq->type == TREASURE_GOLD)
        //    {
        //        int32 curTime = (int32)Crown::SDNow().GetTimeValue();
        //        if(curTime - (int32)m_last_finish_time > FashionConfig::Instance()->GetGoldTreasureFreeTime())
        //        {
        //            //m_last_finish_time = curTime;
        //            freeGold = true;
        //            //免费时间到了
        //        }
        //    }

        //    if(!freeGold)
        //    {
        //        int32 itemNum = m_player->GetBag()->GetItemNum(GameParamConfig::Instance()->GetTreasureTroveCardId());
        //        if(itemNum < bagItem->num)
        //        {
        //            SendFashionGetTREasureAck(ERR_NO_FASHION_GET_TREASURE_ITEM, fashionreq->type, m_last_finish_time);
        //            return;
        //        }
        //        //m_player->GetBag()->DelItemByID(bagItem->id, bagItem->num);
        //    }
        //}
        //else
        //{
        //    SendFashionGetTREasureAck(ERR_NO_FASHION_NO_TYPE, fashionreq->type, m_last_finish_time);
        //    return; // 没有特殊消耗类型
        //}
    }

    if(m_randomList.size() < TREASURE_BRONZE_LEN)
    {
        for(int32 i = m_randomList.size(); i < TREASURE_BRONZE_LEN; i++)
        {
            m_randomList.push_back(0);
        }
    }
    
    bool haveCard = false;
    FASHION_GET_TREASURE_ACK sendMsg;
    REWARD_TABLE rewardTable;

    // 金币一抽
    if(fashionreq->type == TREASURE_BRONZE)
    {
        if(m_randomList[TREASURE_BRONZE_FIRST] == 0)
        {
            GetTreasureFirstHandle(fashionreq->type, FashionConfig::Instance()->GetBronzFirstID(), &rewardTable.items);
            m_randomList[TREASURE_BRONZE_FIRST] = 1;
        }
        else
        {
             RandomTreasureHandle(fashionreq->type, trea->package_id, &rewardTable.items, haveCard);
        }
       m_player->SendTaskEvent(TASK_EVENT_FASHION_BUY_BOX, 1, 1);
    }
    // 金币十抽
    else if (fashionreq->type == TREASURE_BRONZE_TEN)
    {
        for (int32 i = 0; i < 10; i++)
        {
            RandomTreasureHandle(fashionreq->type, trea->package_id, &rewardTable.items, haveCard);
        }

        m_player->SendTaskEvent(TASK_EVENT_FASHION_BUY_BOX, 2, 1);
    }
    // 魔石十抽
    else if (fashionreq->type == TREASURE_GOLD_TEN)
    {
        if (VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_58 , m_player->GetVipLevel()) > 0)
        {
            for (int32 i = 0; i < 10; i++)
            {
                RandomTreasureHandle(fashionreq->type, trea->package_id, &rewardTable.items, haveCard);
            }

            m_player->SendTaskEvent(TASK_EVENT_FASHION_BUY_BOX, 4, 1);
        }
    }

    // 铂金50抽
    else if (fashionreq->type == TREASURE_PLATINUM_GOLD)
    {
        if (VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_60 , m_player->GetVipLevel()) > 0)
        {
            for (int32 i = 0; i < 50; i++)
            {
                RandomTreasureHandle(fashionreq->type, trea->package_id, &rewardTable.items, haveCard);
            }
            if(m_randomList[TREASURE_PLATINUM_GOLD] == 0)
            {
                m_randomList[TREASURE_PLATINUM_GOLD] = 1;
            }

            m_player->SendTaskEvent(TASK_EVENT_FASHION_BUY_PLATINUM_NUM, TASK_TARGET_101, 1);
        }
    }

    // 至尊100抽
    else if (fashionreq->type == TREASURE_EXTREME_GOLD)
    {
        if (VipConfig::Instance()->GetVipValue(VIP_PRIVILEGE_61 , m_player->GetVipLevel()) > 0)
        {
            for (int32 i = 0; i < 100; i++)
            {
                RandomTreasureHandle(fashionreq->type, trea->package_id, &rewardTable.items, haveCard);
            }

            if(m_randomList[TREASURE_EXTREME_GOLD] == 0)
            {
                m_randomList[TREASURE_EXTREME_GOLD] = 1;
            }

            m_player->SendTaskEvent(TASK_EVENT_FASHION_BUY_EXTREME_NUM, TASK_TARGET_102, 1);
        }
    }

    else if(fashionreq->type == TREASURE_GOLD)
    {
        if(m_randomList[TREASURE_GOLD_FIRST] == 0)
        {
            GetTreasureFirstHandle(fashionreq->type, FashionConfig::Instance()->GetGoldFirstID(), &rewardTable.items);
            m_randomList[TREASURE_GOLD_FIRST] = 1;

            m_player->SendTaskEvent(TASK_EVENT_FASHION_BUY_TOTAL_NUM, TASK_TARGET_100, 1);
        }
        else
        {
            RandomTreasureHandle(fashionreq->type, trea->package_id, &rewardTable.items, haveCard);
        }
        m_player->SendTaskEvent(TASK_EVENT_FASHION_BUY_BOX, 3, 1);
    }

    // 检查包裹
    //if(!m_player->GetItemManager()->CanAddRewardTable(rewardTable))
    //{
    //    SendFashionGetTREasureAck(ERR_NO_ENOUGH_BAGPOS, fashionreq->type, m_last_finish_time);
    //    return;
    //}
    
    //for(FashionItemList::const_iterator bagItem = trea->itemList.begin(); bagItem != trea->itemList.end(); bagItem++)
    //{
    //    if(bagItem->type == PROP_GAMEPOINT)
    //    {
    //        if (m_player->GetGamePoint() < bagItem->num) // 检查金币数量
    //        {
    //            SendFashionGetTREasureAck(ERR_NO_ENOUGH_GAMEPOINT, fashionreq->type, m_last_finish_time);
    //            return;
    //        }
    //        m_player->SubGamePoint(bagItem->num, REWARD_REASON_MONEY_FASHION_TREASURE);
    //    }
    //    else if(bagItem->type == PROP_ITEM && bagItem->id == GameParamConfig::Instance()->GetTreasureTroveCardId())
    //    {
    //        if(fashionreq->type == TREASURE_GOLD)
    //        {
    //            int32 curTime = (int32)Crown::SDNow().GetTimeValue();
    //            if(curTime - (int32)m_last_finish_time > FashionConfig::Instance()->GetGoldTreasureFreeTime())
    //            {
    //                m_last_finish_time = curTime;
    //                freeGold = true;
    //                //免费时间到了
    //            }
    //        }

    //        if(!freeGold)
    //        {
    //            int32 itemNum = m_player->GetBag()->GetItemNum(GameParamConfig::Instance()->GetTreasureTroveCardId());
    //            if(itemNum < bagItem->num)
    //            {
    //                SendFashionGetTREasureAck(ERR_NO_ENOUGH_ITEM, fashionreq->type, m_last_finish_time);
    //                return;
    //            }
    //            //m_player->GetBag()->DelItemByID(bagItem->id, bagItem->num);
    //        }
    //    }
    //    else
    //    {
    //        SendFashionGetTREasureAck(ERR_NO_ENOUGH_ITEM, fashionreq->type, m_last_finish_time);
    //        return; // 没有特殊消耗类型
    //    }
    //}

    //for (std::vector<REWARD_ITEM>::iterator re = rewardTable.items.begin(); re != rewardTable.items.end(); re++)
    //{
    //    m_player->GetItemManager()->AddItemByNum(re->id, re->num, REWARD_REASON_MONEY_FASHION_TREASURE);
    //}
    
    if(rewardTable.items.size() <= 0)
    {
        error = ERR_FAILED;
    }
    else
    {
        error = ERR_SUCCEED;
    }

    m_player->SendMyselfEvent(LIVENESS_EVENT_TARGET_FASHION, 1, 0, 0);

    sendMsg.errcode = error;
    sendMsg.type = fashionreq->type;
    sendMsg.item_list = rewardTable.items;
    sendMsg.last_finish_time = m_last_finish_time;
    m_player->SendToGate(MSG_FASHION_GET_TREASURE_ACK, &sendMsg);

}

void FashionManager::RandomTreasureHandle(int type, int goodf, std::vector<REWARD_ITEM>* itemList, bool& have)
{
    if(!itemList)
        return;
    //根据包裹抽物品
    const TREASURE_PACKAGE_ITEM* packageItem = m_player->GetPackageManager()->GetPackageItem(goodf, PACKAGE_TYPE_INCARNATION);
    if(!packageItem)
    {
        m_player->NotifyErrorCode(ERR_FAILED);
        return;
    }

    REWARD_ITEM rewardItem;
    bool incarnationFlag = false;
    
    if(CheckHaveIncarnationCard(packageItem->item_id))
    {
        // 重复成卡兑换成碎片
        const IncarnationItem* ri = FashionConfig::Instance()->GetIncarnationItemFragment(packageItem->item_id);
        if(ri)
        {
            rewardItem.id = ri->item.id;
            rewardItem.num = ri->item.num;
            incarnationFlag = true;
        }
    }
    
   if(!incarnationFlag)
   {
        int32 num = 0;
        if(packageItem->count_max > packageItem->count_min)
        {
            //随机物品数量
            num = packageItem->count_min + Crown::SDRandom(packageItem->count_max - packageItem->count_min);
        }
        else
        {
            num =  packageItem->count_min;
        }
        //添加抽到的物品
        rewardItem.id = packageItem->item_id;
        rewardItem.num = num;
    }
    itemList->push_back(rewardItem);

    m_player->SendTaskEvent(TASK_EVENT_FASHION_BUY_TOTAL_NUM, TASK_TARGET_100, 1);
}

void FashionManager::GetTreasureFirstHandle(int type, int itemID, std::vector<REWARD_ITEM>* itemList)
{
    if(!itemList)
        return;

    REWARD_ITEM  firstItem;
    //firstItem.type = PROP_ITEM;
    firstItem.num = 1;
    firstItem.id = itemID;

    itemList->push_back(firstItem);
}

void FashionManager::SendFashionGetTREasureAck( uint16 error, int32 type, int32 lasttime )
{
    FASHION_GET_TREASURE_ACK sendMsg;
    sendMsg.errcode = error;
    sendMsg.type = type;
    sendMsg.last_finish_time = lasttime;
    m_player->SendToGate(MSG_FASHION_GET_TREASURE_ACK, &sendMsg);
}

//==================================================
//时装碎片熔炼
//==================================================
void FashionManager::onFashionSmeltReq( void* data )
{
    //FASHION_SMELT_ACK sendMsg;
    //uint16 error = ERR_FAILED;
    //FASHION_SMELT_REQ* smeltreq =(FASHION_SMELT_REQ*) data;
    //if(!smeltreq)
    //    return;

    //const ItemTemplate* itemTmpl;
    //int32 itemNum = 0;
    //int point = 0;
    //if(smeltreq->type == FASHION_SMELT_NONE)
    //{
    //    bool find_item = false;

    //    for(std::vector<FASHION_SMELT_ITEM>::iterator smeltItem = smeltreq->item_list.begin(); smeltItem != smeltreq->item_list.end(); smeltItem++)
    //    {
    //        itemNum = m_player->GetBag()->GetItemNum(smeltItem->item_id);
    //        if(itemNum < smeltItem->item_num)
    //        {
    //            find_item = true;
    //            break;
    //        }
    //    }

    //    if(find_item)
    //    {
    //        error = ERR_NO_ENOUGH_ITEM;
    //    }
    //    else
    //    {
    //        for(std::vector<FASHION_SMELT_ITEM>::iterator bagItem = smeltreq->item_list.begin(); bagItem != smeltreq->item_list.end(); bagItem++)
    //        {
    //            itemNum = m_player->GetBag()->GetItemNum(bagItem->item_id);
    //            if(itemNum >= bagItem->item_num)
    //            {
    //                itemTmpl = ItemConfig::Instance()->GetItemTmpl(bagItem->item_id);
    //                point += itemTmpl->smelt_value * bagItem->item_num;
    //                m_player->GetBag()->DelItemByID(bagItem->item_id, bagItem->item_num);
    //            }
    //        }
    //        m_player->AddIncarnationPoint(point, REWARD_REASON_FASHION_SMELT);
    //        error = ERR_SUCCEED;
    //    }
    //}
    //else if(smeltreq->type == FASHION_SMELT_ALL)
    //{
    //    std::vector<int32> itemIdList = m_player->GetBag()->GetItemListByType(ITEM_TYPE_SMELT);
    //    if(itemIdList.size() < 1)
    //    {
    //        error = ERR_BAG_NO_ITME;
    //    }
    //    else
    //    {
    //        for(std::vector<int32>::iterator iid = itemIdList.begin(); iid != itemIdList.end(); ++iid)
    //        {
    //            itemTmpl = ItemConfig::Instance()->GetItemTmpl(*iid);
    //            if(itemTmpl->level <= (int32)smeltreq->quality)
    //            {
    //                itemNum = m_player->GetBag()->GetItemNum(*iid);
    //                point += itemTmpl->smelt_value * itemNum;
    //                m_player->GetBag()->DelItemByID(*iid, itemNum);
    //            }
    //        }
    //        m_player->AddIncarnationPoint(point, REWARD_REASON_FASHION_SMELT);
    //        error = ERR_SUCCEED;
    //    }
    //}
    //else
    //{
    //    error = ERR_FAILED;
    //}
    //sendMsg.type = smeltreq->type;
    //sendMsg.errcode = error;
    //m_player->SendToGate(MSG_FASHION_SMELT_ACK, &sendMsg);
}

//==================================================
// 化身点购买
//==================================================
void FashionManager::onIncarnationBuyReq( void* data )
{
    //INCARNATION_BUY_REQ* recvMsg = (INCARNATION_BUY_REQ*)data;

    //const ItemTemplate* itemTmpl = NULL;
    //int32 honor_point = 0;

    //if(!recvMsg) 
    //    return;

    //itemTmpl = FashionConfig::Instance()->GetSaleItem(1, recvMsg->item_id);
    //if(!itemTmpl)
    //{
    //    m_player->NotifyErrorCode(ERR_UNFOUND_ITEM);
    //    return;
    //}

    //// 超过购买限制的直接设置为购买最大值
    //recvMsg->item_num = (recvMsg->item_num > g_MaxBuyNum) ? (g_MaxBuyNum) : (recvMsg->item_num);
    //if((itemTmpl->incarnation_price <= 0) || (recvMsg->item_num < 1) || (recvMsg->item_num > g_MaxBuyNum))
    //{
    //    return;
    //}

    //// 检查包裹
    //if(!m_player->GetBag()->CanAddItem(recvMsg->item_id, recvMsg->item_num))
    //{
    //    m_player->NotifyErrorCode(ERR_NO_ENOUGH_BAGPOS);
    //    return;
    //}

    //// 所需荣誉值
    //honor_point = itemTmpl->incarnation_price * recvMsg->item_num;

    //// 检查并扣除荣誉值
    //int32 err = m_player->SubItemOne(REWARD_REASON_INCARNATON_BUY, PROP_INCARNATION_POINT, honor_point);
    //if(err != ERR_SUCCEED)
    //{
    //    m_player->NotifyErrorCode(err);
    //    return;
    //}

    //// 添加物品
    //m_player->GetBag()->AddItemByNum(recvMsg->item_id, recvMsg->item_num, REWARD_REASON_INCARNATON_BUY);

    //SendIncarnationBuyAck(ERR_SUCCEED);
}

//==========================================================
// 判断幻化卡是否可以抽取
//==========================================================
bool FashionManager::CheckRandomIncarnationCard( uint32 fashionid , bool& have)
{
    const FashionItem* fashionItem  = FashionConfig::Instance()->GetIncarnationCardFashionItem(fashionid);
    if(fashionItem)
    {
        if(have)
            return true;

        for(FashionList::iterator it = m_fashion_list.begin(); it != m_fashion_list.end(); ++it)
        {
            if(it->fashion_id == fashionItem->id) 
                return true;
        }

        //if(fashionItem->incarnationCard.size() >= 1)
        //{
        //    int32 itemNum = m_player->GetItemManager()->GetItemNum(fashionItem->incarnationCard[0].id);
        //    if(itemNum >= fashionItem->incarnationCard[0].num)
        //    {
        //        return true;
        //    }
        //}

        have = true;
    }
    return false;
}

bool FashionManager::CheckHaveIncarnationCard(uint32 fashionid)
{
    const FashionItem* fashionItem  = FashionConfig::Instance()->GetIncarnationCardFashionItem(fashionid);
    if(fashionItem)
    {
        for(FashionList::iterator it = m_fashion_list.begin(); it != m_fashion_list.end(); ++it)
        {
            if(it->fashion_id == fashionItem->id) 
                return true;
        }

        //if(fashionItem->incarnationCard.size() >= 1)
        //{
        //    int32 itemNum = m_player->GetItemManager()->GetItemNum(fashionItem->incarnationCard[0].id);
        //    if(itemNum >= fashionItem->incarnationCard[0].num)
        //    {
        //        return true;
        //    }
        //}
    }
    return false;
}

void FashionManager::SendIncarnationBuyAck( uint16 error )
{
    ERROR_CODE sendMsg;
    sendMsg.errcode = error;
    m_player->SendToGate(MSG_INCARNATION_BUY_ACK, &sendMsg);
}

// 全服公告激活化身
void FashionManager::FashionMarquee( int32 id,int32 quality )
{
    SERVER_MARQUEE marquee;
    marquee.id = MARQUEE_FASHIONS;
    marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
    marquee.parame_list.push_back(MarFashions(id,quality));
    m_player->OnMarQuee(marquee);
}

void FashionManager::GetFashionCardMarquee( int32 id )
{
    /**
    SERVER_MARQUEE marquee;
    marquee.id = MARQUEE_SYNTHESIS_GEM;
    marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
    marquee.parame_list.push_back(MarQueeItem(PROP_ITEM, 0, id));
    m_player->OnMarQuee(marquee);
    */
}

void FashionManager::InitFashionSkill(bool flag)
{
    //m_skillIdList.clear();
    FashionSkillIdList tempIdlist;

    //const FashionSkillList& list = FashionSkillConfig::Instance()->GetSkillList();
    //for (FashionSkillList::const_iterator it = list.begin(); it != list.end(); it++)
    //{
    //    // 一个技能的多个条件
    //    bool is_skill_open = true;

    //    // 遍历条件
    //    for (std::vector<PropertyList>::const_iterator vpl = it->condition.begin(); vpl != it->condition.end(); vpl++)
    //    {
    //        int32 fashion_id = 0;
    //        int32 fashion_num = 999;
    //        int32 fashion_level = 0;
    //        int32 fashion_star = 0;

    //        // 遍历判断元素
    //        for (PropertyList::const_iterator pl = vpl->begin(); pl != vpl->end(); pl++)
    //        {
    //            if(pl->type == FASHION_SKILL_OPEN_TYPE_ID)
    //            {
    //                fashion_id = pl->value;
    //            }
    //            else if(pl->type == FASHION_SKILL_OPEN_TYPE_NUM)
    //            {
    //                fashion_num = pl->value;
    //            }
    //            else if(pl->type == FASHION_SKILL_OPEN_TYPE_LEVEL)
    //            {
    //                fashion_level = pl->value;
    //            }
    //            else if(pl->type == FASHION_SKILL_OPEN_TYPE_STAR)
    //            {
    //                fashion_star = pl->value;
    //            }
    //            else
    //                CnAssert(false);
    //        } // 遍历判断元素

    //        if(fashion_id != 0)
    //        {   // 指定化身id时
    //            fashion_num = 1;
    //            fashion_star = 0;
    //        }

    //        // 条件判断
    //        int32 valid_num = 0;
    //        for(FashionList::iterator fashionvo = m_fashion_list.begin(); fashionvo != m_fashion_list.end(); fashionvo++)
    //        {
    //            const FashionItem* fashionItem  = FashionConfig::Instance()->GetNewFashion(fashionvo->fashion_id);
    //            if(!fashionItem)
    //                continue;

    //            if(fashion_level > 0 && (int32)fashionvo->fashion_level < fashion_level)
    //                continue;
    //            if(fashion_star > 0 && fashionItem->quality < fashion_star)
    //                continue;
    //            if(fashion_id == 0 || (int32)fashionvo->fashion_id == fashion_id)
    //                valid_num++;
    //        }

    //        if(valid_num < fashion_num)
    //        {
    //            is_skill_open = false;
    //            break;
    //        }
    //    } // 遍历条件

    //    if(is_skill_open)
    //    {
    //        tempIdlist.push_back(it->id);
    //    }
    //}

    //if(!ConfigManager::Instance()->IsTW() && flag && tempIdlist.size() != m_skillIdList.size())
    //{
    //    // 检查新技能id
    //    for(FashionSkillIdList::iterator iter = tempIdlist.begin(); iter != tempIdlist.end(); iter++)
    //    {
    //        uint32 cid = (*iter);
    //        if(!HasFashionSkillID(cid))
    //        {
    //            if(m_skillIdList.size() > 1)
    //             {
    //                // 有新的技能
    //                SERVER_MARQUEE marquee;
    //                marquee.id = MARQUEE_GET_FASHION_SKILL;
    //                marquee.parame_list.push_back(MarQueePlayerName(m_player->GetVipLevel(), m_player->GetName()));
    //                marquee.parame_list.push_back(MarQueeLoginReceive(MARQUEE_GET_FASHION_SKILL, cid));
    //                m_player->OnMarQuee(marquee);
    //            }
    //        }
    //    }
    //}

    m_skillIdList.clear();
    m_skillIdList = tempIdlist;
}

std::vector<uint32> FashionManager::GetFashionIdList()
{
    std::vector<uint32> tempIdList;
    for(FashionList::iterator fashionvo = m_fashion_list.begin(); fashionvo != m_fashion_list.end(); fashionvo++)
    {
        tempIdList.push_back(fashionvo->fashion_id);
    }
    return tempIdList;
}

FASHION_INFO* FashionManager::GetFashionById( uint32 id )
{
    for(FashionList::iterator fashionvo = m_fashion_list.begin(); fashionvo != m_fashion_list.end(); fashionvo++)
    {
        if(fashionvo->fashion_id == id)
            return &(*fashionvo);
    }
    return NULL;
}

void FashionManager::SetFashionSkillID()
{
    // 设置化身技能
    m_player->NotifyProperty(PROP_FASHION_SKILL_ID, m_fashionSkillId, "");
    m_player->CalcProperty();  // 重新计算角色属性
}

bool FashionManager::HasFashionSkillID(uint32 fashionSkillId)
{
    for(FashionSkillIdList::iterator iter = m_skillIdList.begin(); iter != m_skillIdList.end(); iter++)
    {
        if((*iter) == fashionSkillId)
            return true;
    }
    return false;
}

void FashionManager::OnSetFashionSkillId( void* data )
{
    //SET_FASHION_REQ* recvMsg = (SET_FASHION_REQ*)data;

    //if(!recvMsg) 
    //    return;
    //
    //if(!HasFashionSkillID(recvMsg->fashion_id) && !m_player->IsGM())
    //{   // 此技能未激活
    //    return;
    //}

    //m_fashionSkillId = recvMsg->fashion_id;

    //const FashionSkillInfo* fashionSkill = FashionSkillConfig::Instance()->GetFashionSkill(m_fashionSkillId);
    //if(fashionSkill)
    //{
    //    if(fashionSkill->skill_id == m_RSkillId)
    //        return;     // 不重复设置

    //    m_RSkillId = fashionSkill->skill_id;
    //}
    //else
    //{
    //    m_RSkillId = 0;
    //}

    //SetFashionSkillID();

    //ERROR_CODE sendMsg;
    //sendMsg.errcode = ERR_SUCCEED;
    //m_player->SendToGate(MSG_FASHION_SKILL_NTF, &sendMsg);
}

void FashionManager::OnGetFashionSkillInfo( void* data )
{
    ERROR_CODE* recvMsg = (ERROR_CODE*)data;

    if(!recvMsg) 
        return;

    SendFashionListNotify();
}

int32 FashionManager::JudgeFashionTotalNum()
{
    return m_fashion_list.size();
}

int32 FashionManager::JudgeFashionTotalLevel()
{
    int32 totalLevel = 0;
    for(FashionList::iterator fashionvo = m_fashion_list.begin(); fashionvo != m_fashion_list.end(); fashionvo++)
    {
        totalLevel += fashionvo->fashion_level;
    }
    return totalLevel;
}

void FashionManager::GmSkillFashion()
{
    //m_skillIdList.clear();
    //const FashionSkillList& list = FashionSkillConfig::Instance()->GetSkillList();
    //for (FashionSkillList::const_iterator it = list.begin(); it != list.end(); it++)
    //{
    //    m_skillIdList.push_back(it->id);
    //}
    //SendFashionListNotify();
}

int32 FashionManager::GetTaskFashionLevel( int32 id )
{
    int32 level = 0;
    for(FashionList::iterator it = m_fashion_list.begin(); it != m_fashion_list.end(); ++it)
    {
        if(id != 0 && it->fashion_id == id) 
        {
            return it->fashion_level;
        }
        else
        {
            if(it->fashion_level > (uint32)level)
            {
                level = it->fashion_level;
            }
        }
    }
    return level;
}

int32 FashionManager::GetTaskFashionStarLevel( int32 star, int32 level )
{
    int32 num = 0;
    for(FashionList::iterator it = m_fashion_list.begin(); it != m_fashion_list.end(); ++it)
    {
        const FashionItem* fashionItem  = FashionConfig::Instance()->GetNewFashion(it->fashion_id);

        if(fashionItem->quality == star && it->fashion_level >= (uint32)level) 
        {
            num += 1;
        }
    }
    return num;
}
