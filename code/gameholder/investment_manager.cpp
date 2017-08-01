#include "gameholder_pch.h"
#include "investment_manager.h"
#include "player.h"


IMPLEMENT_SINGLETON(InvestmentManager)

InvestmentManager::InvestmentManager()
{

}

InvestmentManager::~InvestmentManager()
{

}

void InvestmentManager::OnRecv( Player* player, uint32 msgID, void* data )
{
    switch (msgID)
    {
    case MSG_INVESTMENT_OPERATION_REQ:
        {
            OnInvestmentOperationReq(player, msgID, data);
            break;
        }
    default:
        break;
    }

}

void InvestmentManager::OnInvestmentOperationReq( Player* player, uint32 msgID, void* data )
{
    if (!player)
        return;

    player->SendToCenter(msgID, data);
}

void InvestmentManager::InvestmentOperationNTF(Player* player,  INVESTMENT_OPERATION_NTF* info )
{
    if (!player)
        return;

    player->SendToCenter(MSG_INVESTMENT_OPERATION_NTF, info);
}
