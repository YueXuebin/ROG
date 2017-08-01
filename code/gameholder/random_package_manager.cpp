#include "gameholder_pch.h"
#include "random_package_manager.h"
#include "player.h"
#include "system_config_manager.h"
#include "item_config_manager.h"
#include "center_handler.h"

const RANDOM_PACKAGE_RECORD* GetPackageRecord(uint32 package_id, const RANDOM_PACKAGE_RECORD_INFO* pInfo)
{
    if (!pInfo)
    {
        return NULL;
    }

    for (std::vector<RANDOM_PACKAGE_RECORD>::const_iterator cIter = pInfo->records.begin(); cIter!= pInfo->records.end(); ++cIter)
    {
        if (cIter->id == package_id)
        {
            return &(*cIter);
        }
    }

    return NULL;
}

const RANDOM_PACKAGE_CARD_RECORD* GetPackageCardRecord(uint32 card_id, const RANDOM_PACKAGE_RECORD* pInfo)
{
    if (!pInfo)
    {
        return NULL;
    }

    for (std::vector<RANDOM_PACKAGE_CARD_RECORD>::const_iterator cIter = pInfo->records.begin(); cIter!= pInfo->records.end(); ++cIter)
    {
        if (cIter->id == card_id)
        {
            return &(*cIter);
        }
    }

    return NULL;
}

bool CheckCD(uint32 cd, const RANDOM_PACKAGE_CARD_RECORD* pCardRecord)
{
    if (!cd)
    {
        return true;
    }

    if (!pCardRecord)
    {
        return true;
    }

    // 上次抽取的时间+CD > 当前时间 = CD没到
    if (pCardRecord->timestamp + cd > Crown::SDNow().GetTimeValue())
    {
        return false;
    }

    return true;
}

void AddPackageRecord(uint32 package_id, RANDOM_PACKAGE_RECORD_INFO& info)
{
    for (std::vector<RANDOM_PACKAGE_RECORD>::iterator mIter = info.records.begin();
        mIter != info.records.end(); ++mIter)
    {
        if (package_id == mIter->id)
        {
            ++mIter->times;
            return;
        }
    }
    
    RANDOM_PACKAGE_RECORD package_record;
    package_record.id = package_id;
    package_record.times = 1;
    info.records.push_back(package_record);
}

void AddCardRecord(uint32 package_id, uint32 card_id, RANDOM_PACKAGE_RECORD_INFO& info)
{
    for (std::vector<RANDOM_PACKAGE_RECORD>::iterator mPackageIter = info.records.begin();
        mPackageIter != info.records.end(); ++mPackageIter)
    {
        RANDOM_PACKAGE_RECORD& Record = *mPackageIter;

        if (package_id == Record.id)
        {
            
            for (std::vector<RANDOM_PACKAGE_CARD_RECORD>::iterator mCardIter = Record.records.begin();
                mCardIter != Record.records.end(); ++mCardIter)
            {
                if (card_id == mCardIter->id)
                {
                    mCardIter->timestamp = (uint32)Crown::SDNow().GetTimeValue();
                    return;
                }
            }

            RANDOM_PACKAGE_CARD_RECORD card_record;
            card_record.id = card_id;
            card_record.timestamp = (uint32)Crown::SDNow().GetTimeValue();
            Record.records.push_back(card_record);
            return;
        }
    }

    RANDOM_PACKAGE_RECORD package_record;
    package_record.id = package_id;

    RANDOM_PACKAGE_CARD_RECORD card_record;
    card_record.id = card_id;
    card_record.timestamp = (uint32)Crown::SDNow().GetTimeValue();
    package_record.records.push_back(card_record);

    info.records.push_back(package_record);
}

IMPLEMENT_SINGLETON(ServerRandomPackageManager)

ServerRandomPackageManager::ServerRandomPackageManager()
: m_bDirty(false)
{

}

void ServerRandomPackageManager::OnRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_SERVER_PACKAGE_RECORD_LOAD:
        {
            // 更新配置信息
            RANDOM_PACKAGE_RECORD_INFO* recvMsg = (RANDOM_PACKAGE_RECORD_INFO*)data;
            LoadInfo(*recvMsg);
        }
        break;
    default:
        break;
    }
}

void ServerRandomPackageManager::Update()
{
    if (GetDirty())
    {
        SendPackageRecordToCenter();
    }
}

void ServerRandomPackageManager::SendPackageRecordToCenter()
{
    RANDOM_PACKAGE_RECORD_INFO sendMsg;
    FillInfo(sendMsg);
    CenterHandler::Instance()->SendToMasterCenter(MSG_SERVER_PACKAGE_RECORD_FILL, &sendMsg);
}

RandomPackageManager::RandomPackageManager(Player* player) :
m_player(player)
{
}

RandomPackageManager::~RandomPackageManager(void)
{
}

void RandomPackageManager::LoadInfo(const RANDOM_PACKAGE_RECORD_INFO& info)
{
    m_RandomPackageRecordInfo = info;
}

void RandomPackageManager::FillInfo(RANDOM_PACKAGE_RECORD_INFO& info) const
{
    info = m_RandomPackageRecordInfo;
}

bool RandomPackageManager::GetPackage(const uint32 reason, const uint32 pkgID, REWARD_TABLE & table, const int32 loopCount)
{
    if (loopCount >= MAX_LOOP_COUNT)
    {
        CnAssert(false);
        CnWarn("Random Package loop count over %d!", MAX_LOOP_COUNT);
        return false;
    }

    const RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT* pPackage = SystemConfigManager::Instance()->GetRandomPackage(pkgID);
    if (!pPackage)
    {
        CnAssert(false);
        return false;
    }

    std::set<uint32> limitSet;
    uint32 value(0);
    REWARD_ITEM rewardItem;

    // 需要记录的，抽前先记录一次
    if (SystemConfigManager::IsRecord(pPackage))
    {
        AddPackageRecord(pPackage->id, m_RandomPackageRecordInfo);
    }

    for (int32 i=0; i<pPackage->random_num; ++i)
    {
        const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT* pCard = RandomItem(reason, pPackage, limitSet);
        if (!pCard)
        {
            continue;
        }

        // 记录不重复
        if (pCard->is_repeat)
        {
            limitSet.insert(pCard->card_id);
        }

        // 记录个人CD
        if (SystemConfigManager::ParseStringCD(pCard->personal_cd))
        {
            AddCardRecord(pPackage->id, pCard->card_id, m_RandomPackageRecordInfo);
        }

        // 记录全服CD
        if (SystemConfigManager::ParseStringCD(pCard->server_cd))
        {
            AddCardRecord(pPackage->id, pCard->card_id, ServerRandomPackageManager::Instance()->GetInfo());
            ServerRandomPackageManager::Instance()->SetDirty();
        }

        if (!pCard->reward_id)
        {
            continue;
        }

        value = Crown::SDRandom(pCard->min_val, pCard->max_val + 1);

        if (SystemConfigManager::Instance()->GetRandomPackage(pCard->reward_id))
        {
            for (uint32 rp=0; rp<value; ++rp)
            {
                GetPackage(reason, pCard->reward_id, table, loopCount + 1);
            }
            continue;
        }

        uint32 itemType = ItemConfigManager::Instance()->GetItemTypeById(pCard->reward_id);
        if (!itemType)
        {
            CnError("RandomPackageManager::GetPackage ERROR! Package=%d! Card=%d! Reward=%d!", pPackage->id, pCard->card_id, pCard->reward_id);
            continue;
        }

        rewardItem.Clear();

        rewardItem.id = pCard->reward_id;

        if ((ITEM_TYPE_EQUIP == itemType) ||  (ITEM_TYPE_STONE == itemType))
        {
            rewardItem.num = 1;
            rewardItem.flag = value;
            table.items.push_back(rewardItem);
            continue;
        }

        if (value < 1)
        {
            CnError("RandomPackageManager::GetPackage ERROR! Package=%d! Card=%d! Value=%d!", pPackage->id, pCard->card_id, value);
            continue;
        }

        rewardItem.num = value;
        table.items.push_back(rewardItem);
    }

    return true;
}

const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT* RandomPackageManager::RandomItem(const uint32 reason, const RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT* pPackage, const std::set<uint32> & limitSet)
{
    if (!pPackage)
    {
        return NULL;
    }

    uint32 totalRate(0);
    std::vector<const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT*> randomList;

    GetRandomList(reason, pPackage, limitSet, totalRate, randomList);

    if (randomList.empty())
    {
        return NULL;
    }

    if (1 == randomList.size())
    {
        return randomList[0];
    }

    uint32 calRate(0);
    uint32 randNum = Crown::SDRandom(totalRate);
    for (std::vector<const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT*>::const_iterator cIter = randomList.begin();
        cIter != randomList.end(); ++cIter)
    {
        calRate += (*cIter)->rate;
        if (randNum < calRate)
        {
            return (*cIter);
        }
    }

    return randomList[0];
}

void RandomPackageManager::GetRandomList(const uint32 reason, const RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT* pPackage, const std::set<uint32> & limitSet, uint32& totalRate, std::vector<const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT*>& randomList)
{
    totalRate = 0;
    randomList.clear();

    if (!pPackage)
    {
        return;
    }

    const RANDOM_PACKAGE_RECORD* pPackageRecord(NULL);
    int32 nMust(0);

    pPackageRecord = GetPackageRecord(pPackage->id, &m_RandomPackageRecordInfo);
    // 先添加必出
    if (SystemConfigManager::IsRecord(pPackage) && pPackageRecord)
    {
        for (std::vector<RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT>::const_iterator cIter = pPackage->item_list.begin();
            cIter != pPackage->item_list.end(); ++cIter)
        {
            // 没有计数的肯定不是必出的
            if (!cIter->get_times)
            {
                continue;
            }

            // 必出也需要检查重复出
            if (!CheckRepeat(&(*cIter), limitSet))
            {
                continue;
            }

            // 检查个人CD
            if (!CheckPersonalCD(&(*cIter), pPackageRecord))
            {
                continue;
            }

            // 检查全服CD
            if (!CheckServerCD(pPackage->id, &(*cIter)))
            {
                continue;
            }

            nMust = pPackageRecord->times % cIter->get_times;
            if (nMust)
            {
                continue;
            }

            // 必定出
            totalRate += cIter->rate;
            randomList.push_back(&(*cIter));
        }
    }

    // 如果不是空的，说明有必出的
    if (!randomList.empty())
    {
        return;
    }

    // 添加非必出装备
    for (std::vector<RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT>::const_iterator cIter = pPackage->item_list.begin();
        cIter != pPackage->item_list.end(); ++cIter)
    {
        // 跳过有计数的
        if (cIter->get_times)
        {
            continue;
        }

        // 检查重复出
        if (!CheckRepeat(&(*cIter), limitSet))
        {
            continue;
        }

        // 检查个人CD
        if (!CheckPersonalCD(&(*cIter), pPackageRecord))
        {
            continue;
        }

        // 检查全服CD
        if (!CheckServerCD(pPackage->id, &(*cIter)))
        {
            continue;
        }

        totalRate += cIter->rate;
        randomList.push_back(&(*cIter));
    }
}

bool RandomPackageManager::CheckRepeat(const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT* pCard, const std::set<uint32> & limitSet)
{
    if (!pCard)
    {
        CnAssert(false);
        return false;
    }

    if (pCard->is_repeat && limitSet.end() != limitSet.find(pCard->card_id))
    {
        return false;
    }

    return true;
}

bool RandomPackageManager::CheckPersonalCD(const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT* pCard, const RANDOM_PACKAGE_RECORD* pPackageRecord)
{
    if (!pCard)
    {
        CnAssert(false);
        return false;
    }

    return CheckCD(SystemConfigManager::ParseStringCD(pCard->personal_cd), GetPackageCardRecord(pCard->card_id, pPackageRecord));
}

bool RandomPackageManager::CheckServerCD(uint32 package_id, const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT* pCard)
{
    if (!pCard)
    {
        CnAssert(false);
        return false;
    }

    const RANDOM_PACKAGE_RECORD* pPackageRecord = GetPackageRecord(package_id, &ServerRandomPackageManager::Instance()->GetInfo());

    return CheckCD(SystemConfigManager::ParseStringCD(pCard->server_cd), GetPackageCardRecord(pCard->card_id, pPackageRecord));
}
