#include "centerserver_pch.h"
#include "pay_manager.h"
#include "pay_job.h"
#include "game_handler.h"
#include "config_loader.h"
#include "player.h"
#include "player_manager.h"
#include "dongyi_log.h"
//#define  SHOP_TYPE  0
IMPLEMENT_SINGLETON(PayManager)

PayManager::PayManager()
{

}

PayManager::~PayManager()
{

}

// ==================================================
// 接收信息
// ==================================================
void PayManager::OnRecv(uint64 playerID, uint32 sessionID, uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_PAY_ORDER_REQ:
        RecvPayOrderReq(playerID, data);
        break;
    default:
        break;
    }
}

// ==================================================
// 生成交易订单ID
// ==================================================
uint64 PayManager::GenPayUUID()
{
    static uint16 serialNum = 0;
    // 时间戳 32bit, 序列号16bit, 
    uint32 high = (uint32)time(0);
    uint32 low = (++serialNum << 16);
    low |= ConfigLoader::Instance()->GetRegionID() & 0xFFFF;   // 区ID

    uint64 sn = high;
    sn = sn << 32;
    sn += low;

    return sn;
}

// ==================================================
// 请求商城数据
// ==================================================
void PayManager::RecvPayOrderReq(uint64 playerID, void* data)
{
    PAY_ORDER_INFO* recvMsg = (PAY_ORDER_INFO*)data;
    uint64 payID = 0;

    if(!recvMsg) return;

    // 生成交易订单ID
    payID = GenPayUUID();

    // 赋值交易订单ID
    recvMsg->payid.id_h = U64ID_HIGH(payID);
    recvMsg->payid.id_l = U64ID_LOW(payID);

    PayJob::Instance()->InsertPayOrder(*recvMsg);
}
