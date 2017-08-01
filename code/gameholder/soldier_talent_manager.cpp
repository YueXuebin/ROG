#include "gameholder_pch.h"
#include "soldier_talent_manager.h"
#include "player.h"
#include "item_manager.h"
#include "item_factory.h"
#include "item_config_manager.h"
#include "property_set_calculater.h"
#include "game_util.h"
#include "bag_manager.h"
#include "soldier.h"
#include "passive_skill_config_manager.h"

ItemManager* SoldierTalentManager::GetItemMgr()
{
    Player* pPlayer = CnDynamicCast(Player, m_pSoldier->GetMaster());
    return pPlayer->GetItemManager();
}

SoldierTalentManager::SoldierTalentManager(Soldier* pSoldier) :
m_pSoldier(pSoldier),
m_PropertyDirty(true)
{
    Clear();

    for (uint32 i = 0; i < SOLDIER_TALENT_WHOLE; ++i)
    {
        m_StoneGrid.push_back(NULL);
    }
}

SoldierTalentManager::~SoldierTalentManager()
{
    Clear();
}

void SoldierTalentManager::Clear()
{
    m_StoneGrid.clear();

    m_PropertyDirty = true;
}

void SoldierTalentManager::OnRecv(uint32 msgID, void* data)
{

}

void SoldierTalentManager::LoadStoneInfo(const STONE_INFO& info)
{
    ItemStone * pStone(NULL);

    for (std::vector<ITEM_STONE>::const_iterator cStoneIter = info.stone_list.begin();
        cStoneIter != info.stone_list.end(); ++cStoneIter)
    {
        const ITEM_STONE & stone = *cStoneIter;
        if (!IsEmptyGrid(stone.base.pos))
        {
            CnAssert(false);
            continue;
        }
        
        pStone = ItemFactory::CreateStone(stone.base.id);
        if (!pStone)
        {
            continue;
        }

        pStone->LoadItem(&stone);

        AddToGrid(pStone);
        GetItemMgr()->AddToItemList(pStone);
    }
}

void SoldierTalentManager::FillStoneInfo(STONE_INFO& info) const
{
    info.Clear();

    ITEM_STONE stone;
    const ItemStone * pStone(NULL);

    for (StoneGrid::const_iterator cIter = m_StoneGrid.begin();
        cIter != m_StoneGrid.end(); ++cIter)
    {
        pStone = *cIter;
        if (!pStone)
        {
            continue;
        }

        pStone->SaveItem(&stone);
        info.stone_list.push_back(stone);
    }
}

bool SoldierTalentManager::IsEmptyGrid( int32 pos )
{
    if (!IsValidGrid(pos))
    {
        return false;
    }

    return (NULL == m_StoneGrid[pos]);
}

void SoldierTalentManager::CalcProp()
{
    if (!m_PropertyDirty) return;
    m_PropertyDirty = false;

    // 汇总所有装备属性
    PropertySet& propSet = m_pSoldier->GetPropertySetCalculater()->GetTalentProp();
    propSet.Clear();

    const ItemStone * pStone(NULL);
    for (StoneGrid::const_iterator cIter = m_StoneGrid.begin();
        cIter != m_StoneGrid.end(); ++cIter)
    {
        pStone = *cIter;
        if (!pStone)
        {
            continue;
        }

        const PASSIVE_SKILL_CARD_ROOT_TPLT_STRUCT* pStruct = PassiveSkillConfigManager::Instance()->GetPassiveSkillCardConfig(pStone->GetId());
        if(!pStruct)
        {
            CnAssert(false);
            continue;
        }

        if(pStruct->prop1_id)
        {
            propSet.Add(pStruct->prop1_id, pStruct->prop1_value);
        }

        if(pStruct->prop2_id)
        {
            propSet.Add(pStruct->prop2_id, pStruct->prop2_value);
        }
    }
}

uint16 SoldierTalentManager::OptInstall( ItemBase* pSrc,  const int32 dstPos )
{
    if (!IsValidGrid(dstPos))
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    ItemStone* pStone = CnDynamicCast(ItemStone, pSrc);
    if (!pStone)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    if (!CheckStoneTypeAndPos(pStone->GetSubType(), dstPos))
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    ItemStone* pDst = GetStoneByPos(dstPos);
    if (pDst)
    {
        // 进行交换逻辑
        int16 errCode = OptSwap(pStone->GetPlace(), pStone->GetPos(), pDst->GetPlace(), pDst->GetPos());
        if (ERR_SUCCEED == errCode) 
        {
            return ERR_SUCCEED_SWAP;
        }
        
        CnAssert(false);
        return ERR_FAILED;
    }

    // 安装
    pStone->SetPos(dstPos);
    AddToGrid(pStone);

    // 重新计算角色属性
    m_PropertyDirty = true;
    m_pSoldier->CalcProperty();

    return ERR_SUCCEED;
}

uint16 SoldierTalentManager::OptUninstall( int32 srcPos, int32 dstPos /*= -1*/ )
{
    if (!IsValidGrid(srcPos))
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    ItemStone* pStone = GetStoneByPos(srcPos);
    if (!pStone)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    // 通知卸下装备到背包
    OPT_ITEM_INFO info;
    info.opt = ITEM_OPT_MOVE;

    // 填充起点数据
    pStone->SaveBase(info.src);

    // 填充终点数据
    info.dst.place = PLACE_BAG;
    info.dst.id = pStone->GetId();
    info.dst.num = pStone->GetNum();
    if(NULL_GRID == dstPos)
    {
        GetItemMgr()->FindDstPlace(pStone, info.dst.place, info.dst.pos);
        if (NULL_GRID == info.dst.pos)
        {
            return ERR_FAILED;
        }
    }
    else
    {
        info.dst.pos = dstPos;
    }

    // 移动
    uint16 errCode = GetItemMgr()->MoveItem(pStone, info.dst.place, info.dst.pos);
    if (ERR_SUCCEED != errCode)
    {
        CnAssert(false);
        return errCode;
    }

    GetItemMgr()->NtfOptItem(info);
    // 移除起点数据
    GetItemMgr()->RemovePlaceItem(PLACE_SOLDIER_TALENT, info.src.pos, pStone);

    // 重新计算角色属性
    m_PropertyDirty = true;
    m_pSoldier->CalcProperty();

    return ERR_SUCCEED;
}

uint16 SoldierTalentManager::OptSwap( uint32 srcPlace, int32 srcPos, uint32 dstPlace, int32 dstPos )
{
    ItemBase* pSrc = GetItemMgr()->GetPlaceItem(srcPlace, srcPos);
    ItemBase* pDst = GetItemMgr()->GetPlaceItem(dstPlace, dstPos);

    if (!pSrc || !pDst)
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    OPT_ITEM_INFO info;
    info.opt = ITEM_OPT_SWAP;
    pSrc->SaveBase(info.src);
    pDst->SaveBase(info.dst);

    // 移动
    uint16 errCode = GetItemMgr()->SwapItem(pSrc, pDst);
    if (ERR_SUCCEED != errCode)
    {
        CnAssert(false);
        return errCode;
    }

    GetItemMgr()->NtfOptItem(info);

    // 重新计算角色属性
    m_PropertyDirty = true;
    m_pSoldier->CalcProperty();

    return ERR_SUCCEED;
}

void SoldierTalentManager::AddToGrid(ItemStone * pStone, bool bSwap /*= false*/)
{
    CnAssert(pStone);
    CnAssert(IsValidGrid(pStone->GetPos()));
    CnAssert(pStone->GetNum() > 0);
    CnAssert(pStone->GetSubType() == STONE_SUB_TYPE_TALENT);
    if (!bSwap)
    {
        CnAssert(!m_StoneGrid[pStone->GetPos()]);
    }

    pStone->SetPlace(PLACE_SOLDIER_TALENT);
    m_StoneGrid[pStone->GetPos()] = pStone;
}

ItemStone* SoldierTalentManager::GetStoneByPos(int32 pos)
{
    if (!IsValidGrid(pos))
    {
        CnAssert(false);
        return NULL;
    }

    return m_StoneGrid[pos];
}

bool SoldierTalentManager::IsValidGrid(int32 pos) const
{
    return (pos >= 0 && pos < SOLDIER_TALENT_WHOLE);
}

bool SoldierTalentManager::CheckStoneTypeAndPos(uint32 type, int32 pos)
{
    if (!IsValidGrid(pos))
    {
        return false;
    }

    return (STONE_SUB_TYPE_TALENT == type);
}

bool SoldierTalentManager::TryToCorrectStonePosWithType(int32 type, int32* pos)
{
    if (!pos)
    {
        CnAssert(false);
        return false;
    }

    for (int32 i=0; i<SOLDIER_TALENT_WHOLE; ++i)
    {
        if (!m_StoneGrid[i])
        {
            *pos = i;
            return true;
        }
    }

    *pos = 0;
    return true;
}

