#include "gameholder_pch.h"
#include "item_stone.h"
#include "item_config_manager.h"
#include "skill_config_manager.h"
#include "gem_config_manager.h"
#include "item_factory.h"

CnImplementRTTI(ItemStone, ItemBase);

ItemStone::ItemStone(uint32 id) : ItemBase(id, ITEM_TYPE_STONE),
m_SubType(0),
m_FuncId(0),
m_Quality(QUALITY_WHITE),
m_Level(1),
m_Exp(0)
{

}

ItemStone::~ItemStone()
{

}

bool ItemStone::Init()
{
    const STONE_ROOT_TEMPLATE_STRUCT* pTplt = ItemConfigManager::Instance()->FindStoneTpltById(GetId());
    if (!pTplt)
    {
        CnWarn("ItemStone::Init not found template by id=%u!\n", GetId());
        return false;
    }

    m_MaxStackNum = 1;

    m_SubType = pTplt->sub_type;
    m_FuncId = pTplt->func_id;

    return true;
}

void ItemStone::LoadItem(const void* pInfo)
{
    CnAssert(pInfo);
    const ITEM_STONE* pStone = static_cast<const ITEM_STONE*>(pInfo);
    CnAssert(pStone);

    LoadBase(pStone->base);

    SetQuality(pStone->base.quality);
    SetLevel(pStone->level);
    SetExp(pStone->exp);
}

void ItemStone::SaveItem(void* pInfo) const
{
    CnAssert(pInfo);
    ITEM_STONE* pStone = static_cast<ITEM_STONE*>(pInfo);
    CnAssert(pStone);

    SaveBase(pStone->base);

    pStone->base.quality = GetQuality();
    pStone->level = GetLevel();
    pStone->exp = GetExp();
}

void ItemStone::SetLevel(int32 val)
{
    switch (GetSubType())
    {
    case STONE_SUB_TYPE_SKILL:
    case STONE_SUB_TYPE_RUNE:
        {
            const int32 nMaxLevel = SkillConfigManager::Instance()->GetSkillMaxLevel();
            val = Crown::SDRange(1, val, nMaxLevel);
        }
        break;
    case STONE_SUB_TYPE_GEM:
        {
            const int32 nMaxLevel = GemConfigManager::Instance()->GetGemMaxLevel(GetId());
            val = Crown::SDRange(1, val, nMaxLevel);
        }
        break;
    }
    
    m_Level = val;
}

int32 ItemStone::GetTotalExp() const
{
    int32 totalExp = 0;

    switch (GetSubType())
    {
    case STONE_SUB_TYPE_SKILL:
    case STONE_SUB_TYPE_RUNE:
        {
            // TODO£º
        }
        break;
    case STONE_SUB_TYPE_GEM:
        {
            const int32 nMaxLevel = GemConfigManager::Instance()->GetGemMaxLevel(GetId());
            if (GetLevel() >= nMaxLevel)
            {
                totalExp = GemConfigManager::Instance()->GetTotalExpByLevel(GetId(), nMaxLevel);
            }
            else
            {
                totalExp = GemConfigManager::Instance()->GetTotalExpByLevel(GetId(), GetLevel() - 1) + GetExp();
            }
        }
        break;
    }

    return totalExp;
}

void ItemStone::SetExp(int32 val)
{
    switch (GetSubType())
    {
    case STONE_SUB_TYPE_SKILL:
    case STONE_SUB_TYPE_RUNE:
        {
            const int32 nMaxLevel = SkillConfigManager::Instance()->GetSkillMaxLevel();
            if (GetLevel() >= nMaxLevel)
            {
                m_Exp = 0;
                return;
            }

            const int32 nMaxExp = SkillConfigManager::Instance()->GetSkillExpByLevelQuality(GetLevel(), GetQuality());
            val = Crown::SDRange(0, val, nMaxExp);
        }
        break;
    case STONE_SUB_TYPE_GEM:
        {
            const int32 nMaxLevel = GemConfigManager::Instance()->GetGemMaxLevel(GetId());
            if (GetLevel() >= nMaxLevel)
            {
                m_Exp = 0;
                return;
            }

            const int32 nMaxExp = GemConfigManager::Instance()->GetExpByLevel(GetId(), GetLevel());
            val = Crown::SDRange(0, val, nMaxExp);
        }
        break;
    }

    m_Exp = val;
}

void ItemStone::AddExp(int32 val)
{
    if(val <= 0)
    {
        return;
    }

    switch (GetSubType())
    {
    case STONE_SUB_TYPE_SKILL:
    case STONE_SUB_TYPE_RUNE:
        {
            const int32 nMaxLevel = SkillConfigManager::Instance()->GetSkillMaxLevel();
            if (GetLevel() >= nMaxLevel)
            {
                return;
            }

            int32 nMaxExp = SkillConfigManager::Instance()->GetSkillExpByLevelQuality(GetLevel(), GetQuality());
            val += GetExp();
            while(val >= nMaxExp)
            {
                val -= nMaxExp;
                AddLevel();

                nMaxExp = SkillConfigManager::Instance()->GetSkillExpByLevelQuality(GetLevel(), GetQuality());

                SetExp(0);

                if (GetLevel() >= nMaxLevel)
                {
                    return;
                }
            }
        }
        break;
    case STONE_SUB_TYPE_GEM:
        {
            const int32 nMaxLevel = GemConfigManager::Instance()->GetGemMaxLevel(GetId());
            if (GetLevel() >= nMaxLevel)
            {
                return;
            }

            int32 nMaxExp = GemConfigManager::Instance()->GetExpByLevel(GetId(), GetLevel());
            val += GetExp();
            while(val >= nMaxExp)
            {
                val -= nMaxExp;
                AddLevel();

                nMaxExp = GemConfigManager::Instance()->GetExpByLevel(GetId(), GetLevel());

                SetExp(0);

                if (GetLevel() >= nMaxLevel)
                {
                    return;
                }
            }
        }
        break;
    }

    SetExp(val);
}

void ItemStone::AddExpToItem(ITEM_STONE* pItem, int32 exp)
{
    if (!pItem)
    {
        CnAssert(false);
        return;
    }

    ItemStone* mainGemStone = ItemFactory::Instance()->CreateStone(pItem->base.id);
    mainGemStone->LoadItem(pItem);
    mainGemStone->AddExp(exp);
    mainGemStone->SaveItem(pItem);
    ItemFactory::Instance()->DestoryItem(mainGemStone);
}