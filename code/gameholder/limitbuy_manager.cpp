#include "gameholder_pch.h"
#include "limitbuy_manager.h"
#include "player.h"
#include "limitbuy_config.h"


LimitBuy_Manager::LimitBuy_Manager(Player* player ) :
m_player(player)
{
}

LimitBuy_Manager::~LimitBuy_Manager(void)
{
}

void LimitBuy_Manager::LoadLimitBuyInfo(const LIMIT_BUY_LIST& limitbuy_info)
{
    m_buylimtList.bind_money_list = limitbuy_info.bind_money_list;
    m_buylimtList.mall_buy_list = limitbuy_info.mall_buy_list;
}

void LimitBuy_Manager::FillLimitBuyInfo(LIMIT_BUY_LIST& limitbuy_info)
{
    limitbuy_info.bind_money_list = m_buylimtList.bind_money_list;
    limitbuy_info.mall_buy_list = m_buylimtList.mall_buy_list;
}

void LimitBuy_Manager::CalDailyRefresh()
{
    LimitBuyList::iterator limitbuy;
    for (limitbuy = m_buylimtList.bind_money_list.begin(); limitbuy != m_buylimtList.bind_money_list.end(); limitbuy++)
    {
        limitbuy->num = 0;
    }

    LimitBuyList::iterator limitbuymall;
    for (limitbuymall = m_buylimtList.mall_buy_list.begin(); limitbuymall != m_buylimtList.mall_buy_list.end(); limitbuymall++)
    {
        if (limitbuymall->type != VIP_SELL_TYPE)
        {
            limitbuymall->num = 0;
        }
    }

    LIMIT_BUY_INFO_ACK sendmsg;
    sendmsg.code = OPT_UPDATE_FLAG;
    sendmsg.bind_money_list = m_buylimtList.bind_money_list;
    sendmsg.mall_buy_list = m_buylimtList.mall_buy_list;
    SendLimitList(sendmsg);
}

void LimitBuy_Manager::OnRecv(uint32 msgID, void * data)
{
    switch(msgID)
    {
    case  MSG_LIMIT_BUY_REQ:
        {
            LIMIT_BUY_INFO_ACK sendmsg;
            sendmsg.code = OPT_INIT_FLAG;
            sendmsg.bind_money_list = m_buylimtList.bind_money_list;
            sendmsg.mall_buy_list = m_buylimtList.mall_buy_list;
            SendLimitList(sendmsg);
        }
        break;
    case MSG_LIMIT_BUY_INFO_CLEAR_REQ:
        {
            LimitBuyList::iterator limitbuymall;
            for (limitbuymall = m_buylimtList.mall_buy_list.begin(); limitbuymall != m_buylimtList.mall_buy_list.end(); limitbuymall++)
            {
                if (limitbuymall->type == VIP_SELL_TYPE)
                {
                    limitbuymall->num = 0;
                }
            }

            LIMIT_BUY_INFO_ACK sendmsg;
            sendmsg.code = OPT_UPDATE_FLAG;
            sendmsg.mall_buy_list = m_buylimtList.mall_buy_list;
            SendLimitList(sendmsg);
        }
        break;
    default:
        break;
    }
}

void LimitBuy_Manager::SendLimitList(LIMIT_BUY_INFO_ACK& sendmsg)
{
    m_player->SendToGate(MSG_LIMIT_BUY_INFO,&sendmsg);
}

bool LimitBuy_Manager::AddLimitProduct(LIMIT_BUY& buylimt,int32 type)
{
    if(FindProduct(buylimt.product_id,type))
        return false;

    switch(type)
    {
    case BIND_MONEY_SELL_TYPE:
        {
            m_buylimtList.bind_money_list.push_back(buylimt);

            LIMIT_BUY_INFO_ACK sendmsg;
            sendmsg.code = OPT_ADD_FLAG;
            sendmsg.bind_money_list.push_back(buylimt);
            SendLimitList(sendmsg);
            return true;
        }
        break;
    case MALL_SELL_TYPE:
    case VIP_SELL_TYPE:
        {
            m_buylimtList.mall_buy_list.push_back(buylimt);

            LIMIT_BUY_INFO_ACK sendmsg;
            sendmsg.code = OPT_ADD_FLAG;
            sendmsg.mall_buy_list.push_back(buylimt);
            SendLimitList(sendmsg);
            return true;
        }
        break;
    }

     return false;
}

bool LimitBuy_Manager::FindProduct(uint32 id ,int32 type)
{
    switch(type)
    {
    case BIND_MONEY_SELL_TYPE:
        {
            LimitBuyList::iterator limitbuy;
            for (limitbuy = m_buylimtList.bind_money_list.begin(); limitbuy != m_buylimtList.bind_money_list.end(); limitbuy++)
            {
                if(limitbuy->product_id == id)
                    return true;
            }
        }
        break;
    case MALL_SELL_TYPE:
    case VIP_SELL_TYPE:
        {
            LimitBuyList::iterator limitbuy;
            for (limitbuy = m_buylimtList.mall_buy_list.begin(); limitbuy != m_buylimtList.mall_buy_list.end(); limitbuy++)
            {
                if(limitbuy->product_id == id)
                    return true;
            }
        }
        break;
    }
    return false;
}

bool LimitBuy_Manager::JudgeLimitNum(uint32 productid, int32 productnum,int32 type)
{
    bool b_maxlimit = BMaxLimitNum(productid,productnum,type);
    if(b_maxlimit)
    {
        return true;
    }
    else
    {
        return false;
    }
    return false;
}

void LimitBuy_Manager::AddLimitProductNum(uint32 productid, int32 productnum,int32 type)
{
    LimitBuyTemp* limittemp = LimitBuyConfig::Instance()->FindLimitTemp(productid);
    if(!limittemp)
        return;

    switch(type)
    {
    case BIND_MONEY_SELL_TYPE:
        {
            LimitBuyList::iterator limitbuy;
            for (limitbuy = m_buylimtList.bind_money_list.begin(); limitbuy != m_buylimtList.bind_money_list.end(); limitbuy++)
            {
                if(limitbuy->product_id == productid)
                {
                    limitbuy->num = limitbuy->num + productnum;

                    LIMIT_BUY_INFO_ACK sendmsg;
                    sendmsg.code = OPT_UPDATE_FLAG;
                    sendmsg.bind_money_list.push_back(*limitbuy);
                    SendLimitList(sendmsg);
                }
            }
        }
        break;
    case MALL_SELL_TYPE:
    case VIP_SELL_TYPE:
        {
            LimitBuyList::iterator limitbuy;
            for (limitbuy = m_buylimtList.mall_buy_list.begin(); limitbuy != m_buylimtList.mall_buy_list.end(); limitbuy++)
            {
                if(limitbuy->product_id == productid)
                {
                    limitbuy->num = limitbuy->num + productnum;

                    LIMIT_BUY_INFO_ACK sendmsg;
                    sendmsg.code = OPT_UPDATE_FLAG;
                    sendmsg.mall_buy_list.push_back(*limitbuy);
                    SendLimitList(sendmsg);
                }
            }
        }
        break;
    }
}

bool LimitBuy_Manager::BMaxLimitNum(uint32 productid, int32 productnum,int32 type)
{
    LimitBuyTemp*  limittemp = LimitBuyConfig::Instance()->FindLimitTemp(productid);
    if(limittemp)
    {
        LIMIT_BUY buylimit;
        buylimit.num = productnum;
        buylimit.product_id = productid;
        buylimit.type = type;

        int32 havenum =  m_player->GetLimitBuyManager()->FindProductNum(productid,type);
        if((productnum + havenum) > limittemp->num )
        {
            return true;
        }
        else
        {
            bool addlimt = m_player->GetLimitBuyManager()->AddLimitProduct(buylimit,type);
            if(!addlimt)
                AddLimitProductNum(productid,productnum,type);
        }
    }

    return false;
}

int32 LimitBuy_Manager::FindProductNum(uint32 id ,int32 type)
{
    switch(type)
    {
    case BIND_MONEY_SELL_TYPE:
        {
            LimitBuyList::iterator limitbuy;
            for (limitbuy = m_buylimtList.bind_money_list.begin(); limitbuy != m_buylimtList.bind_money_list.end(); limitbuy++)
            {
                if(id == limitbuy->product_id)
                    return limitbuy->num;
            }
        }
        break;
    case MALL_SELL_TYPE:
    case VIP_SELL_TYPE:
        {
            LimitBuyList::iterator limitbuy;
            for (limitbuy = m_buylimtList.mall_buy_list.begin(); limitbuy != m_buylimtList.mall_buy_list.end(); limitbuy++)
            {
                if(id == limitbuy->product_id)
                    return limitbuy->num;
            }
        }
        break;
    }
    return 0;
}

bool LimitBuy_Manager::JudgeLimitNumToNum( uint32 productid, int32 productnum,int32 type, int32 maxnum )
{
    if(maxnum == 0)
        return false;

    LIMIT_BUY buylimit;
    buylimit.num = productnum;
    buylimit.product_id = productid;
    buylimit.type = type;

    int32 havenum =  m_player->GetLimitBuyManager()->FindProductNum(productid,type);
    if((productnum + havenum) > maxnum )
    {
        return true;
    }
    else
    {
        bool addlimt = m_player->GetLimitBuyManager()->AddLimitProduct(buylimit,type);
        if(!addlimt)
            AddLimitProductNum(productid,productnum,type);
    }
    return false;
}
