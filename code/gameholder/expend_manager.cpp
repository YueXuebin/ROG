#include "gameholder_pch.h"
#include "expend_manager.h"
#include "player.h"
#include "item_manager.h"

ExpendManager::ExpendManager(Player* player) :
m_player(player)
{

}

ExpendManager::~ExpendManager()
{

}

uint16 ExpendManager::ExpendItem(const REWARD_TABLE& expendTable)
{
    uint16 err = ERR_SUCCEED;

    // 合并同类物品
    REWARD_TABLE rewardTable = MergeRewardTable(expendTable);

    // 检查消耗
    err = CheckExpendItem(rewardTable);
    if(err != ERR_SUCCEED)
    {
        return err;
    }

    // 扣除消耗
    SubItem(rewardTable);

    CnAssert(err == ERR_SUCCEED);
    return err;
}


REWARD_TABLE ExpendManager::MergeRewardTable(const REWARD_TABLE& expendTable)
{
    REWARD_ITEM ountItem;
    //ountItem.Clear();
    REWARD_TABLE outTable;
    //outTable.Clear();
    //outTable.reason = expendTable.reason;
    //outTable.reason_arg = expendTable.reason_arg;

    //for(std::vector<REWARD_ITEM>::const_iterator inIt = expendTable.items.begin(); 
    //    inIt != expendTable.items.end(); ++inIt)
    //{
    //    std::vector<REWARD_ITEM>::iterator outIt = outTable.items.begin();
    //    for(; outIt != outTable.items.end(); ++outIt)
    //    {
    //        if(outIt->type == inIt->type && outIt->id == inIt->id)
    //        {
    //            break;
    //        }
    //    }
    //    if(outIt == outTable.items.end())
    //    {
    //        ountItem.Clear();
    //        ountItem = *inIt;
    //        outTable.items.push_back(ountItem);
    //    }
    //    else
    //    {
    //        outIt->num += inIt->num;
    //    }
    //}
    return outTable;
}

uint16 ExpendManager::CheckExpendItem(const REWARD_TABLE& expendTable)
{
    uint16 err = ERR_SUCCEED;
    REWARD_TABLE newTable = MergeRewardTable(expendTable);      // 合并同类物品
    for(std::vector<REWARD_ITEM>::const_iterator expendIt = newTable.items.begin();
        expendIt != newTable.items.end(); expendIt++)
    {
        err = CheckExpendItemByType(*expendIt);

        if(err != ERR_SUCCEED)
        {
            return err;
        }
    }

    return err;
}
uint16 ExpendManager::CheckExpend(int32 type, int32 num, uint32 itemID)
{
    // 负值检查
    if(num < 0)
    {
        return ERR_FAILED;
    }

    uint16 err = ERR_SUCCEED;
    switch(type)
    {
    case PROP_GAMEPOINT:
        err = CheckGamePoint(num);
        break;
    case PROP_SOULPOINT:
        err = CheckSoulPoint(num);
        break;
    case PROP_ITEM:
        err = CheckBagItem(itemID, num);
        break;
    case PROP_HONORPOINT:
        err = CheckHonorPoint(num);
        break;
    case PROP_BIND_MONEYPOINT:
        err = CheckBindMoneyPoint(num);
        break;
    case PROP_INCARNATION_POINT:
        err = CheckIncarnationPoint(num);
        break;
    case PROP_MONEYPOINT:
        err = CheckMoneypoint(num);
        break;
    default:
        CnAssert(false);
        break;
    }

    return err;
}

uint16 ExpendManager::CheckExpendItemByType(const REWARD_ITEM& expendItem)
{
    // 负值检查
    if(expendItem.num < 0)
    {
        return ERR_FAILED;
    }

    uint16 err = ERR_SUCCEED;
    switch(expendItem.id)
    {
    case PROP_GAMEPOINT:
        err = CheckGamePoint(expendItem.num);
        break;
    case PROP_SOULPOINT:
        err = CheckSoulPoint(expendItem.num);
        break;
    //case PROP_STAR_SOUL:
    //    err = CheckStarSoul(expendItem.num);
    //    break;
    case PROP_ITEM:
        err = CheckBagItem(expendItem.id, expendItem.num);
        break;
    case PROP_HONORPOINT:
        err = CheckHonorPoint(expendItem.num);
        break;
    case PROP_VIGOR:
        err = CheckVigor(expendItem.num);
        break;
//     case PROP_GUILDPOINT:
//         err = CheckGuildPoint(expendItem.num);
//         break;
    //case PROP_BATTLE_SOUL:
    //    err = CheckBattleSoul(expendItem.num);
    //    break;
    case PROP_BIND_MONEYPOINT:
        err = CheckBindMoneyPoint(expendItem.num);
        break;
    case PROP_INCARNATION_POINT:
        err = CheckIncarnationPoint(expendItem.num);
        break;
    default:
        CnAssert(false);
        break;
    }

    return err;
}

uint16 ExpendManager::CheckGamePoint(int32 number)
{
    if(number > m_player->GetGamePoint())
    {
        return ERR_NO_ENOUGH_GAMEPOINT;
    }

    return ERR_SUCCEED;
}

uint16 ExpendManager::CheckSoulPoint(int32 number)
{
    if(number > m_player->GetSoulPoint())
    {
        return ERR_NO_ENOUGH_SOUL;
    }

    return ERR_SUCCEED;
}

uint16 ExpendManager::CheckHonorPoint(int32 number)
{
    if(number > m_player->GetHonorPoint())
    {
        return ERR_NO_ENOUGH_HONOR;
    }

    return ERR_SUCCEED;
}

uint16 ExpendManager::CheckVigor(int32 number)
{
    if(number > m_player->GetVigor())
    {
        return ERR_NO_ENOUGH_VIGOR;
    }

    return ERR_SUCCEED;
}

uint16 ExpendManager::CheckGuildPoint(int32 number)
{
    if(number > m_player->GetGuildPoint())
    {
        return ERR_NO_ENOUGH_GUILDPOINT;
    }

    return ERR_SUCCEED;
}

uint16 ExpendManager::CheckIncarnationPoint(int32 number)
{
    if(number > m_player->GetIncarnationPoint())
    {
        return ERR_NO_ENOUGH_INCARNATION_POINT;
    }

    return ERR_SUCCEED;
}

uint16 ExpendManager::CheckMoneypoint(int32 num)
{
    if(num > m_player->GetMoneyPoint())
    {
        return ERR_NO_ENOUGH_BIND_MONEY_POINT;
    }

    return ERR_SUCCEED;
}

uint16 ExpendManager::CheckStarSoul(int32 number)
{
    if(number > m_player->GetStarPoint())
    {
        return ERR_NO_ENOUGH_STAR_SOUL_POINT;
    }
    return ERR_SUCCEED;
}

uint16 ExpendManager::CheckBattleSoul(int32 number)
{
    if(number > m_player->GetBattleSoulPoint())
    {
        return ERR_NO_ENOUGH_BATTLE_SOUL;
    }
    return ERR_SUCCEED;
}

uint16 ExpendManager::CheckBagItem(uint32 id, int32 number)
{
    //if(number > m_player->GetItemManager()->GetItemNum(id))
    //{
    //    return ERR_UNFOUND_ITEM;
    //}

    return ERR_SUCCEED;
}

uint16 ExpendManager::CheckBindMoneyPoint( int32 number )
{
    if(number > m_player->GetBindMoneyPoint())
    {
        return ERR_NO_ENOUGH_BIND_MONEY_POINT;
    }
    return ERR_SUCCEED;
}

void ExpendManager::SubItem(const REWARD_TABLE& expendTable)
{
    for(std::vector<REWARD_ITEM>::const_iterator expendIt = expendTable.items.begin(); expendIt != expendTable.items.end(); ++expendIt)
    {
        SubItemByType(*expendIt, expendTable.reason);
    }
}

void ExpendManager::SubExpend(int32 type, int32 num, int32 reason, uint32 itemID)
{
    CnAssert(num >= 0);
    if(num < 0)
    {
        return;
    }

    switch(type)
    {
    case PROP_GAMEPOINT:
        m_player->SendBehaviorLog(PROP_GAMEPOINT, num, reason);
        SubGamePoint(num, reason);
        break;
    case PROP_SOULPOINT:
        m_player->SendBehaviorLog(PROP_SOULPOINT, num, reason);
        SubSoulPoint(num, reason);
        break;
    case PROP_ITEM:
        SubBagItem(itemID, num, reason);
        break;
    case PROP_HONORPOINT:
        m_player->SendBehaviorLog(PROP_HONORPOINT, num, reason);
        SubHonorPoint(num, reason);
        break;
    case PROP_BIND_MONEYPOINT:
        SubBindMoneyPoint(num, reason);
        break;
    case PROP_INCARNATION_POINT:
        SubIncarnationPoint(num, reason);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void ExpendManager::SubItemByType(const REWARD_ITEM& expendIt, int32 reason)
{
    CnAssert(expendIt.num >= 0);
    if(expendIt.num < 0)
    {
        return;
    }

    switch(expendIt.id)
    {
    case PROP_GAMEPOINT:
        SubGamePoint(expendIt.num, reason);
        break;
    case PROP_SOULPOINT:
        SubSoulPoint(expendIt.num, reason);
        break;
    //case PROP_STAR_SOUL:
    //    SubStarPoint(expendIt.num, reason);
    //    break;
    case PROP_ITEM:
        SubBagItem(expendIt.id, expendIt.num, reason);
        break;
    case PROP_HONORPOINT:
        SubHonorPoint(expendIt.num, reason);
        break;
    case PROP_VIGOR:
        m_player->SendBehaviorLog(PROP_VIGOR, expendIt.num, reason);
        SubVigor(expendIt.num);
        break;
    //case PROP_GUILDPOINT:
    //    SubGuildPoint(expendIt.num, reason);
    //    break;
    //case PROP_BATTLE_SOUL:
    //    m_player->SendBehaviorLog(PROP_BATTLE_SOUL, expendIt.num, reason);
    //    SubBattleSoulPoint(expendIt.num);
    //    break;
    case PROP_BIND_MONEYPOINT:
        SubBindMoneyPoint(expendIt.num, reason);
        break;
    case PROP_INCARNATION_POINT:
        SubIncarnationPoint(expendIt.num, reason);
        break;
    default:
        CnAssert(false);
        break;
    }
}

void ExpendManager::SubGamePoint(int32 number, int32 reason)
{
    m_player->SubGamePoint(number, reason);
}

void ExpendManager::SubSoulPoint(int32 number, int32 reason)
{
    m_player->SubSoulPoint(number, reason);
}

void ExpendManager::SubStarPoint(int32 number, int32 reason)
{
    m_player->SubStarPoint(number, reason);
}

void ExpendManager::SubBagItem(uint32 id, int32 number, int32 reason)
{
    //m_player->GetBag()->DelItemByID(id, number);
}

void ExpendManager::SubHonorPoint(int32 number, int32 reason)
{
    m_player->SubHonorPoint(number, reason);
}

void ExpendManager::SubVigor(int32 num)
{
    m_player->SubVigor(num);
}

void ExpendManager::SubGuildPoint(int32 number, int32 reason)
{
    m_player->SubGuildPoint(number, reason);
}

void ExpendManager::SubBattleSoulPoint(int32 number)
{
    m_player->SubBattleSoulPoint(number);
}

void ExpendManager::SubBindMoneyPoint( int32 number, int32 reason )
{
    m_player->SubBindMoneyPoint(number, reason);
}

void ExpendManager::SubIncarnationPoint( int32 number, int32 reason )
{
    m_player->SubIncarnationPoint(number, reason);
}
