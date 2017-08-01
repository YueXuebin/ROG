#include "gameholder_pch.h"
#include "skill_info_manager.h"
#include "player.h"
#include "item_manager.h"
#include "action_controller.h"
#include "item_factory.h"
#include "equip_manager.h"
#include "skill_config_manager.h"
#include "game_util.h"
#include "property_set_calculater.h"
#include "event_define.h"
#include "scene.h"
#include "bag_manager.h"
#include "soldier.h"
#include "buff_controller.h"
#include "skill_manager.h"
#include "skill_data.h"



ItemManager* SkillInfoManager::GetItemMgr()
{
    if (m_pCreature->IsKindOf(&Player::ms_RTTI))
    {
        Player* pPlayer = CnDynamicCast(Player, m_pCreature);
        return pPlayer->GetItemManager();
    }
    else if(m_pCreature->IsKindOf(&Pet::ms_RTTI))
    {
        Pet* pPet = CnDynamicCast(Pet, m_pCreature);
        if (pPet->GetMaster() && pPet->GetMaster()->IsKindOf(&Player::ms_RTTI))
        {
            Player* pPlayer = CnDynamicCast(Player, pPet->GetMaster());
            return pPlayer->GetItemManager();
        }
    }

    return NULL;
}

SkillInfoManager::SkillInfoManager(Creature* pCreature, uint8 place, uint32 num):
m_pCreature(pCreature),
m_Place(place),
MAX_SKILL_NUM(num),
MAX_SKILL_GRID_NUM(num * 3)
{
    Clear();
    for (int32 i = 0; i < MAX_SKILL_GRID_NUM; ++i)
    {
        m_StoneGrid.push_back(NULL);
    }

    for(int32 i=0; i<MAX_SKILL_NUM; ++i)
    {
        m_SkillInfoDirty[i] = true;
    }

    m_NormalAttackDirty = true;
}

SkillInfoManager::~SkillInfoManager()
{
    Clear();
}


void SkillInfoManager::Clear()
{
    m_StoneGrid.clear();
}

// ==================================================
// 加载技能配置信息
// ==================================================
void SkillInfoManager::LoadSkillComponentInfo(const SKILL_COMPONENT_INFO& info)
{
    ItemStone* pStone(NULL);

    for (std::vector<ITEM_STONE>::const_iterator cStoneIter = info.list.begin();
        cStoneIter != info.list.end(); ++cStoneIter)
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
            //CnAssert(false);
            CnWarn("create stone id=%d failed\n", stone.base.id);
            continue;
        }

        pStone->LoadItem(&stone);

        if (!CheckStonePosWithType(pStone->GetPos(), pStone->GetSubType()))
        {
            CnAssert(false);
            ItemFactory::DestoryItem(pStone);
            continue;
        }

        AddToGrid(pStone);
        GetItemMgr()->AddToItemList(pStone);
    }
}

// ==================================================
// 填充技能配置信息
// ==================================================
void SkillInfoManager::FillSkillComponentInfo(SKILL_COMPONENT_INFO& info)
{
    // 技能书和符文位
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
        info.list.push_back(stone);
    }

    // 计算技能信息
    CalcAllSkillInfo();

    // 填充技能信息
    FillFinalSkillInfoList(info.final_skills);
}

void SkillInfoManager::ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 )
{
    const int32 TRIGGER_RANGE = 800;

    // 受伤触发
    if(eventId == BATTLE_EVENT_SELF_HURT)
    {
        uint32 attackerID = arg1;
        int32 damage = arg2;
        
        for(int32 si=0; si<MAX_SKILL_NUM; si++)
        {
            SkillInfo& skillInfo = m_SkillInfo[si];
            if(!skillInfo.IsTriggered())
                continue;

            // 伤害触发型技能判断
            skillInfo.AddDamage(damage);
            int32 hurtTriggerVal = skillInfo.GetProp(PROP_SKILL_HURT_TRIGGER);
            int32 fullDamage = int32(ToPercent(hurtTriggerVal) * m_pCreature->GetMaxHp());
            if(fullDamage == 0)
                continue;

            if(skillInfo.GetDamage() >= fullDamage)
            {
                skillInfo.SetDamage(0);

                const SkillData* skillData = SkillManager::Instance()->GetSkillData(skillInfo.GetSkillId());
                if(!skillData)
                {
                    CnAssert(false);
                    continue;
                }
                bool needTarget = (skillData->m_TargetData.m_TargetType != SKILL_TARGET_SELF);


                // 查找目标
                SHOOT_TARGET target;
                target.caster_id = m_pCreature->GetId();
                BattleUnit* enemy = m_pCreature->GetScene()->FindNearestEnemyBattleUnitInCircle(m_pCreature, m_pCreature->GetPosition(), TRIGGER_RANGE);
                if(enemy)
                {
                    target.target_id = enemy->GetId();
                    target.target_x = (int32)enemy->GetPosition().x;
                    target.target_y = (int32)enemy->GetPosition().y;
                }
                else if(needTarget)
                    continue;       // 没有目标则不触发

                // 触发技能
                SkillArgumentData args;
                args.fromNetArgument(*skillInfo.GetSkillArgs());
                args.m_SkillMode = SKILL_MODE_PASSIVE;
                skillInfo.FillPropertySet(args.m_SkillProps);

                m_pCreature->UseSkill(skillInfo.GetSkillId(), target, args, 0, IndexToSlot(si));
            }
        }
    }
    // 暴击触发
    else if(eventId == BATTLE_EVENT_SELF_SKILL_CRITICAL)
    {
        uint32 defenderID = arg1;

        for(int32 si=0; si<MAX_SKILL_NUM; si++)
        {
            SkillInfo& skillInfo = m_SkillInfo[si];
            if(!skillInfo.IsTriggered())
                continue;

            int32 criticalTriggerPro = skillInfo.GetProp(PROP_SKILL_CRITICAL_TRIGGER);
            if((int32)Crown::SDRandom(BASE_RATE_NUM) >= criticalTriggerPro)
                continue;
            
            const SkillData* skillData = SkillManager::Instance()->GetSkillData(skillInfo.GetSkillId());
            if(!skillData)
            {
                CnAssert(false);
                continue;
            }
            bool needTarget = (skillData->m_TargetData.m_TargetType != SKILL_TARGET_SELF);

            // 查找目标
            SHOOT_TARGET target;
            target.caster_id = m_pCreature->GetId();
            
            BattleUnit* enemy = m_pCreature->GetScene()->GetBattleUnit(defenderID);
            if(!enemy)
                enemy = m_pCreature->GetScene()->FindNearestEnemyBattleUnitInCircle(m_pCreature, m_pCreature->GetPosition(), TRIGGER_RANGE);
            if(enemy)
            {
                target.target_id = enemy->GetId();
                target.target_x = (int32)enemy->GetPosition().x;
                target.target_y = (int32)enemy->GetPosition().y;
            }
            else if(needTarget)
                continue;       // 没有目标则不触发

            // 触发技能
            SkillArgumentData args;
            args.fromNetArgument(*skillInfo.GetSkillArgs());
            args.m_SkillMode = SKILL_MODE_PASSIVE;
            skillInfo.FillPropertySet(args.m_SkillProps);

            m_pCreature->UseSkill(skillInfo.GetSkillId(), target, args, 0, IndexToSlot(si));
        }

    }
    // 近战击中触发
    else if(eventId == BATTLE_EVENT_SELF_SKILL_MELEE)
    {
        uint32 defenderID = arg1;

        for(int32 si=0; si<MAX_SKILL_NUM; si++)
        {
            SkillInfo& skillInfo = m_SkillInfo[si];
            if(!skillInfo.IsTriggered())
                continue;

            int32 meleeTriggerPro = skillInfo.GetProp(PROP_SKILL_MELEE_TRIGGER);
            if((int32)Crown::SDRandom(BASE_RATE_NUM) >= meleeTriggerPro)
                continue;

            const SkillData* skillData = SkillManager::Instance()->GetSkillData(skillInfo.GetSkillId());
            if(!skillData)
            {
                CnAssert(false);
                continue;
            }
            bool needTarget = (skillData->m_TargetData.m_TargetType != SKILL_TARGET_SELF);

            // 查找目标
            SHOOT_TARGET target;
            target.caster_id = m_pCreature->GetId();

            BattleUnit* enemy = m_pCreature->GetScene()->GetBattleUnit(defenderID);
            if(!enemy)
                enemy = m_pCreature->GetScene()->FindNearestEnemyBattleUnitInCircle(m_pCreature, m_pCreature->GetPosition(), TRIGGER_RANGE);
            if(enemy)
            {
                target.target_id = enemy->GetId();
                target.target_x = (int32)enemy->GetPosition().x;
                target.target_y = (int32)enemy->GetPosition().y;
            }
            else if(needTarget)
                continue;       // 没有目标则不触发

            // 触发技能
            SkillArgumentData args;
            args.fromNetArgument(*skillInfo.GetSkillArgs());
            args.m_SkillMode = SKILL_MODE_PASSIVE;
            skillInfo.FillPropertySet(args.m_SkillProps);

            m_pCreature->UseSkill(skillInfo.GetSkillId(), target, args, 0, IndexToSlot(si));
        }
    }
    // 投射命中触发
    else if(eventId == BATTLE_EVENT_SELF_SKILL_PROJECTILE)
    {
        uint32 defenderID = arg1;

        for(int32 si=0; si<MAX_SKILL_NUM; si++)
        {
            SkillInfo& skillInfo = m_SkillInfo[si];
            if(!skillInfo.IsTriggered())
                continue;

            int32 projectileTriggerPro = skillInfo.GetProp(PROP_SKILL_PROJECTILE_TRIGGER);
            if((int32)Crown::SDRandom(BASE_RATE_NUM) >= projectileTriggerPro)
                continue;

            const SkillData* skillData = SkillManager::Instance()->GetSkillData(skillInfo.GetSkillId());
            if(!skillData)
            {
                CnAssert(false);
                continue;
            }

            bool toSelf = (skillData->m_TargetData.m_TargetType == SKILL_TARGET_SELF);

            BattleUnit* caster = m_pCreature->GetScene()->GetBattleUnit(defenderID);
            if(!caster)
                continue;

            // 查找目标
            SHOOT_TARGET target;
            target.caster_id = caster->GetId();

            BattleUnit* enemy = m_pCreature->GetScene()->FindNearestEnemyBattleUnitInCircle(m_pCreature, caster->GetPosition(), TRIGGER_RANGE, caster->GetId());
            if(enemy)
            {
                target.target_id = enemy->GetId();
                target.target_x = (int32)enemy->GetPosition().x;
                target.target_y = (int32)enemy->GetPosition().y;
            }
            else if(!toSelf)
                continue;       // 没有目标则不触发

            // 触发技能
            SkillArgumentData args;
            args.fromNetArgument(*skillInfo.GetSkillArgs());
            args.m_SkillMode = SKILL_MODE_PASSIVE;
            skillInfo.FillPropertySet(args.m_SkillProps);

            m_pCreature->UseSkill(skillInfo.GetSkillId(), target, args, 0, IndexToSlot(si));
        }
    }
    
}

// ==================================================
// 接收消息
// ==================================================
void SkillInfoManager::OnRecv(uint32 msgID, void * data)
{
    switch(msgID)
    {
    case MSG_SKILL_COMPONENT_INFO_REQ:            // 请求技能信息
        OnSkillInfoReq();
        break;
    case MSG_USE_BASE_SKILL_REQ:
        OnUseBaseSkill(data);
        break;
    default:
        break;
    }
}

void SkillInfoManager::OnPropertyChange()
{
    for(int32 i=0; i<MAX_SKILL_NUM; ++i)
    {
        m_SkillInfoDirty[i] = true;
    }
    m_NormalAttackDirty = true;

    SendFinalSkillInfoList();
}

void SkillInfoManager::AddExp(int32 val)
{
    bool bSend(false);

    for (int32 i = 0; i < MAX_SKILL_GRID_NUM; ++i)
    {
        if (!m_StoneGrid[i])
        {
            continue;
        }

        m_StoneGrid[i]->AddExp(val);

        m_SkillInfoDirty[i % MAX_SKILL_NUM] = true;

        bSend = true;
    }

    if (bSend)
    {
        OnSkillInfoReq();
    }
}

bool SkillInfoManager::CheckStonePosWithType(int32 pos, int32 type)
{
    switch(type)
    {
    case STONE_SUB_TYPE_SKILL:
        return (pos >= 0 && pos < MAX_SKILL_NUM);
    case STONE_SUB_TYPE_RUNE:
        return (pos >= MAX_SKILL_NUM && pos < MAX_SKILL_GRID_NUM);
    }

    return false;
}

bool SkillInfoManager::TryToCorrectStonePosWithType(int32 type, int32* pos)
{
    if(pos == NULL)
    {
        CnAssert(false);
        return false;
    }

    switch(type)
    {
    case STONE_SUB_TYPE_SKILL:
        {
            for (int32 i=0; i<MAX_SKILL_NUM; ++i)
            {
                if (!m_StoneGrid[i])
                {
                    (*pos) = i;
                    return true;
                }
            }
        }
        break;
    case STONE_SUB_TYPE_RUNE:
        {
            int32 runePos1 = (*pos)+MAX_SKILL_NUM;
            int32 runePos2 = (*pos)+MAX_SKILL_NUM*2;
            if(!m_StoneGrid[runePos1])      // 符文位1为空
            {
                (*pos) = runePos1;
                return true;
            }
            else if(!m_StoneGrid[runePos2])  // 符文位2为空
            {
                (*pos) = runePos2;
                return true;
            }
            else                            // 无空符文位则替换符文位1
            {
                (*pos) = runePos1;
                return true;
            }
        }
        break;
    }

    return false;
}

void SkillInfoManager::AddToGrid(ItemStone * pStone, bool bSwap /*= false*/)
{
    CnAssert(pStone);
    CnAssert(IsValidGrid(pStone->GetPos()));
    CnAssert(pStone->GetNum() > 0);
    if (!bSwap)
    {
        CnAssert(!m_StoneGrid[pStone->GetPos()]);
    }

    pStone->SetPlace(m_Place);
    m_StoneGrid[pStone->GetPos()] = pStone;
}

bool SkillInfoManager::IsEmptyGrid(int32 pos)
{
    if (pos < 0 || pos >= MAX_SKILL_GRID_NUM)
    {
        CnAssert(false);
        return false;
    }

    return (NULL == m_StoneGrid[pos]);
}

bool SkillInfoManager::IsValidGrid(int32 pos) const
{
    return (pos >= 0 && pos < MAX_SKILL_GRID_NUM);
}

ItemStone* SkillInfoManager::GetStoneByPos(int32 pos)
{
    if (!IsValidGrid(pos))
    {
        return NULL;
    }

    return m_StoneGrid[pos];
}

void SkillInfoManager::SendFinalSkillInfoList()
{
    CalcAllSkillInfo();

    FINAL_SKILL_INFO_LIST msgFinalSkillInfoList;
    FillFinalSkillInfoList(msgFinalSkillInfoList);
    if (m_pCreature->IsKindOf(&Soldier::ms_RTTI))
    {   // 佣兵的技能信息
        m_pCreature->SendToGate(MSG_SOLDIER_FINAL_SKILL_INFO_NTF, &msgFinalSkillInfoList);
    }
    else
    {
        // 主角的技能信息
        GetNormalAttackSkillInfo(NORMAL_ATTACK_NEAR);
        // 再加上近战普攻和远程普攻
        const FINAL_SKILL_INFO& nearFinalSkillInfo = m_NormalAttackNear.GetSkillInfo();
        msgFinalSkillInfoList.list.push_back(nearFinalSkillInfo);
        const FINAL_SKILL_INFO& farFinalSkillInfo = m_NormalAttackFar.GetSkillInfo();
        msgFinalSkillInfoList.list.push_back(farFinalSkillInfo);

        m_pCreature->SendToGate(MSG_FINAL_SKILL_INFO_NTF, &msgFinalSkillInfoList);
    }
}

void SkillInfoManager::FillFinalSkillInfoList(FINAL_SKILL_INFO_LIST& info)
{
    info.Clear();
    for(int32 si=0; si<MAX_SKILL_NUM; ++si)
    {
        SkillInfo& finalSkillInfo = m_SkillInfo[si];
        const FINAL_SKILL_INFO& msgFinalSkillInfo = finalSkillInfo.GetSkillInfo();
        info.list.push_back(msgFinalSkillInfo);
    }
}

void SkillInfoManager::OnSkillInfoReq()
{
    // 装备的技能书和符文
    SKILL_COMPONENT_INFO skillInfo;
    FillSkillComponentInfo(skillInfo);

    // 计算最终技能信息
    CalcAllSkillInfo();

    FillFinalSkillInfoList(skillInfo.final_skills);

    if (m_pCreature->IsKindOf(&Soldier::ms_RTTI))
    {
        m_pCreature->SendToGate(MSG_SOLDIER_SKILL_COMPONENT_INFO_ACK, &skillInfo);
    }
    else
    {
        m_pCreature->SendToGate(MSG_SKILL_COMPONENT_INFO_ACK, &skillInfo);
    }
}

uint16 SkillInfoManager::OptInstall( ItemBase* pSrc,  const int32 dstPos )
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

    if (!CheckStonePosWithType(dstPos, pStone->GetSubType()))
    {
        CnAssert(false);
        return ERR_FAILED;
    }

    ItemStone* pDst = GetStoneByPos(dstPos);

    if (pDst)
    {
        // 进行交换逻辑
        OptSwap(pStone->GetPlace(), pStone->GetPos(), pDst->GetPlace(), pDst->GetPos());
        return ERR_SUCCEED_SWAP;
    }

    pStone->SetPos(dstPos);
    AddToGrid(pStone);

    // 更新最终技能信息
    int32 index = dstPos % MAX_SKILL_NUM;
    m_SkillInfoDirty[index] = true;
    SendFinalSkillInfoList();

    return ERR_SUCCEED;
}

uint16 SkillInfoManager::OptUninstall( int32 srcPos, int32 dstPos /*= -1*/ )
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
        GetItemMgr()->FindDstPlace(pStone, PLACE_BAG, info.dst.pos);
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
    GetItemMgr()->RemovePlaceItem(m_Place, info.src.pos, pStone);

    // 更新最终技能信息
    int32 index = info.src.pos % MAX_SKILL_NUM;
    m_SkillInfoDirty[index] = true;
    SendFinalSkillInfoList();

    return ERR_SUCCEED;
}

uint16 SkillInfoManager::OptSwap( uint32 srcPlace, int32 srcPos, uint32 dstPlace, int32 dstPos )
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

    // 更新最终技能信息
    int32 srcIndex = srcPos % MAX_SKILL_NUM;
    int32 dstIndex = dstPos % MAX_SKILL_NUM;
    m_SkillInfoDirty[srcIndex] = true;
    m_SkillInfoDirty[dstIndex] = true;
    SendFinalSkillInfoList();

    return ERR_SUCCEED;
}

void SkillInfoManager::RemoveStone(int32 pos)
{
    ItemStone* pStone = GetStoneByPos(pos);
    if (!pStone)
    {
        CnAssert(false);
        return;
    }

    m_StoneGrid[pos] = NULL;

    int32 index = pos % MAX_SKILL_NUM;
    m_SkillInfoDirty[index] = true;
    SendFinalSkillInfoList();
}

SkillInfo* SkillInfoManager::GetSkillInfoById(uint32 skillId)
{
    for(int32 i=0; i<MAX_SKILL_NUM; i++)
    {
        if(m_SkillInfo[i].GetSkillId() == skillId)
            return &m_SkillInfo[i];
    }
    return NULL;
}

SkillInfo* SkillInfoManager::GetSkillInfoBySlot(int32 skillSlotId)
{
    if(skillSlotId > MAX_SKILL_NUM || skillSlotId < 1)
    {
        CnAssert(false);
        return NULL;
    }
    SkillInfo& slotSkillInfo = m_SkillInfo[SlotToIndex(skillSlotId)];
    if(slotSkillInfo.GetSkillId())
        return &slotSkillInfo;

    return NULL;
}

SkillInfo* SkillInfoManager::GetNormalAttackSkillInfo(uint32 skillId)
{
    if(!IsNormalAttack(skillId))
    {
        CnAssert(false);
        return NULL;
    }

    if(m_NormalAttackDirty)
    {
        m_NormalAttackDirty = false;
        
        // 近战普攻
        m_NormalAttackNear.Clear();

        m_NormalAttackNear.SetSkillId(NORMAL_ATTACK_NEAR);
        m_NormalAttackNear.SetQuality(QUALITY_WHITE);
        m_NormalAttackNear.SetLevel(1);
        m_NormalAttackNear.SetBookValid(true);
        m_NormalAttackNear.SetSkillValid(true);

        CalcSkillInfoEx(&m_NormalAttackNear, NULL, NULL);

        // 远程普攻
        m_NormalAttackFar.Clear();

        m_NormalAttackFar.SetSkillId(NORMAL_ATTACK_FAR);
        m_NormalAttackFar.SetQuality(QUALITY_WHITE);
        m_NormalAttackFar.SetLevel(1);
        m_NormalAttackFar.SetBookValid(true);
        m_NormalAttackFar.SetSkillValid(true);

        CalcSkillInfoEx(&m_NormalAttackFar, NULL, NULL);
    }

    if(skillId == NORMAL_ATTACK_NEAR)
        return &m_NormalAttackNear;
    else
        return &m_NormalAttackFar;
}

// 使用技能
void SkillInfoManager::OnUseBaseSkill(void* data)
{
    USE_SKILL_REQ* recvMsg = (USE_SKILL_REQ*)data;
    int32 skillID = recvMsg->skill_id;
    uint32 slotID = recvMsg->skill_slot;

    SkillInfo* pSkillInfo = GetSkillInfoBySlot(slotID);
    if(!pSkillInfo)
    {
        CnWarn("used skill %d not exist\n", skillID);
        return;
    }

    bool replaceToNormalAttack = false;

    if(pSkillInfo->GetSkillId() != skillID)
    {
        if(IsNormalAttack(skillID))
        {   // 鼠标左键的普攻
            pSkillInfo = GetNormalAttackSkillInfo(skillID);
            if(!pSkillInfo)
            {
                CnAssert(false);
                return;
            }
            replaceToNormalAttack = true;
        }
        else
        {
            CnDbg("skill %d change to %d\n", skillID, pSkillInfo->GetSkillId());
            return;
        }
    }

    if(!pSkillInfo->GetSkillInfo().skill_valid)
    {
        CnDbg("skill %d not valid\n", skillID);
        return;
    }

    const SKILL_ARGUMETNS* serverSkillArgs = pSkillInfo->GetSkillArgs();
    SKILL_ARGUMETNS* clinetSkillArgs = &recvMsg->skill_args;

    if(!replaceToNormalAttack)
    {
        // 技能参数检查(排除数据未同步时的瞬时,一般情况下,客户端发送的技能参数和服务端一致)
        if(serverSkillArgs->cast_speed != clinetSkillArgs->cast_speed)
        {
            CnWarn("skill %d server client cast_speed not same\n", skillID);
            clinetSkillArgs->cast_speed = serverSkillArgs->cast_speed;
        }
        if(serverSkillArgs->cast_multi != clinetSkillArgs->cast_multi)
        {
            CnWarn("skill %d server client cast_multi not same\n", skillID);
            clinetSkillArgs->cast_multi = serverSkillArgs->cast_multi;
        }
        if(serverSkillArgs->bullet_speed != clinetSkillArgs->bullet_speed)
        {
            CnWarn("skill %d server client bullet_speed not same\n", skillID);
            clinetSkillArgs->bullet_speed = serverSkillArgs->bullet_speed;
        }
        if(serverSkillArgs->bullet_multi != clinetSkillArgs->bullet_multi)
        {
            CnWarn("skill %d server client bullet_multi not same\n", skillID);
            clinetSkillArgs->bullet_multi = serverSkillArgs->bullet_multi;
        }
        if(serverSkillArgs->bullet_multi_angle != clinetSkillArgs->bullet_multi_angle)
        {
            CnWarn("skill %d server client bullet_multi_angle not same\n", skillID);
            clinetSkillArgs->bullet_multi_angle = serverSkillArgs->bullet_multi_angle;
        }
        if(serverSkillArgs->bullet_repeat != clinetSkillArgs->bullet_repeat)
        {
            CnWarn("skill %d server client bullet_repeat not same\n", skillID);
            clinetSkillArgs->bullet_repeat = serverSkillArgs->bullet_repeat;
        }
        if(serverSkillArgs->bullet_repeat_interval != clinetSkillArgs->bullet_repeat_interval)
        {
            CnWarn("skill %d server client bullet_repeat_interval not same\n", skillID);
            clinetSkillArgs->bullet_repeat_interval = serverSkillArgs->bullet_repeat_interval;
        }
        if(serverSkillArgs->bullet_chain != clinetSkillArgs->bullet_chain)
        {
            CnWarn("skill %d server client bullet_chain not same\n", skillID);
            clinetSkillArgs->bullet_chain = serverSkillArgs->bullet_chain;
        }
        if(serverSkillArgs->bullet_chain_id != clinetSkillArgs->bullet_chain_id)
        {
            CnWarn("skill %d server client bullet_chain_id not same\n", skillID);
            clinetSkillArgs->bullet_chain_id = serverSkillArgs->bullet_chain_id;
        }
    }

    // 生成技能逻辑用参数
    SkillArgumentData args;
    args.fromNetArgument(*clinetSkillArgs);

    // 技能的mp消耗
    args.m_Mp = pSkillInfo->GetMp();

    // 技能是否暴击
    if(int32(Crown::SDRandom(BASE_RATE_NUM)) < m_pCreature->GetCrit())
    {
        args.m_IsCritical = true;
    }

    // 技能等级
    args.m_SkillLevel = pSkillInfo->GetLevel();
    // 技能品质
    args.m_SkillQuality = pSkillInfo->GetQuality();

    // 设置技能的属性条目
    pSkillInfo->FillPropertySet(args.m_SkillProps);

    m_pCreature->GetActionController()->UseSkill(skillID, recvMsg->target, args, recvMsg->skill_obj_id, recvMsg->skill_slot);
}

void SkillInfoManager::ReFreshSkillInfo(int32 skillPos)
{
    if(skillPos < 0 || skillPos >= SKILL_SLOT_WHOLE)
    {
        for(int32 i=0; i<MAX_SKILL_NUM; ++i)
        {
            m_SkillInfoDirty[i] = true;
        }
    }
    else
    {
        m_SkillInfoDirty[skillPos] = true;
    }

    SendFinalSkillInfoList();
}

// 计算技能属性
void SkillInfoManager::CalcSkillInfoEx(SkillInfo* skillInfo, ItemStone* pRuneStone1, ItemStone* pRuneStone2)
{
    CnAssert(skillInfo);

    const SKILL_ROOT_SKILL_STRUCT* skillConfig = SkillConfigManager::Instance()->GetSkillConfigData(skillInfo->GetSkillId());

    PropertySet skillPropSet;

    // mp消耗
    skillInfo->SetMp(skillConfig->mp);

    // 技能属性
    int32 skill_grow = SkillConfigManager::Instance()->GetSkillGrowByLevelQuality(skillInfo->GetLevel(), skillInfo->GetQuality());

    int32 skill_prop1_id = skillConfig->prop1_id;
    int32 skill_prop1_val = int32(skillConfig->prop1_base + skillConfig->prop1_grow * ToPercent(skill_grow));
    if(skill_prop1_id)
    {
        skillPropSet.Add(skill_prop1_id, skill_prop1_val);
    }

    int32 skill_prop2_id = skillConfig->prop2_id;
    int32 skill_prop2_val = int32(skillConfig->prop2_base + skillConfig->prop2_grow * ToPercent(skill_grow));
    if(skill_prop2_id)
    {
        skillPropSet.Add(skill_prop2_id, skill_prop2_val);
    }

    // 技能原始参数
    int32 cooldown = skillConfig->cooldown;                   // 冷却时间
    int32 attack_speed = skillConfig->attack_speed;           // 施法速度
    int32 cast_multi = skillConfig->cast_multi;               // 重复施法次数
    int32 bullet_multi = skillConfig->bullet_multi;           // 多重弹道
    int32 bullet_multi_angle = skillConfig->bullet_multi_angle;   // 多重弹道夹角
    int32 bullet_repeat = skillConfig->bullet_repeat;         // 弹道重复
    int32 bullet_repeat_interval = skillConfig->bullet_repeat_interval;   // 弹道重复间隔
    int32 bullet_speed = 10000;                             // 弹道速度
    int32 bullet_chain = skillConfig->bullet_chain;           // 弹射
    int32 bullet_chain_id = skillConfig->bullet_chain_id;     // 弹射技能id

    // 查找对应的符文位
    for(int32 ri=0; ri<2; ri++)
    {
        ItemStone* pRuneStone = NULL;
        if(ri == 0)
            pRuneStone = pRuneStone1;
        else
            pRuneStone = pRuneStone2;

        if(!pRuneStone || pRuneStone->GetSubType() != STONE_SUB_TYPE_RUNE)
        {
            CnAssert(!pRuneStone || pRuneStone->GetSubType() == STONE_SUB_TYPE_RUNE);
            continue;
        }

        const RUNE_ROOT_RUNE_STRUCT* runeConfigData = SkillConfigManager::Instance()->GetRuneConfigData(pRuneStone->GetId());
        if(!runeConfigData)
        {
            CnAssert(false);
            continue;
        }

        // 检查符文不能重复
        bool isExist = false;
        for(uint32 ri=0; ri<skillInfo->GetSkillInfo().rune_ids.size(); ri++)
        {
            if(skillInfo->GetSkillInfo().rune_ids[ri] == pRuneStone->GetId())
            {
                isExist = true;
                break;
            }
        }

        // 检查符文与技能是否匹配
        int32 needFeature = 0;
        if(!isExist)
        {
            if( (runeConfigData->feature & skillConfig->feature) == 0 )
            {
                needFeature = runeConfigData->feature;
            }
        }

        if(!isExist && needFeature == 0)        // 技能与符文匹配
        {
            // 计算符文对技能的增强效果
            int32 grow = SkillConfigManager::Instance()->GetSkillGrowByLevelQuality(pRuneStone->GetLevel(), pRuneStone->GetQuality());   // 符文的成长率

            // 魔法消耗增加
            skillInfo->SetMp(int32(skillInfo->GetMp() * ToPercent(runeConfigData->mana)));

            // 符文提供技能属性
            int32 rune_prop1_id = runeConfigData->prop1_id;
            int32 rune_prop1_val = int32(runeConfigData->prop1_base + runeConfigData->prop1_grow * ToPercent(grow));
            if(rune_prop1_id)
            {
                skillPropSet.Add(rune_prop1_id, rune_prop1_val);
            }

            int32 rune_prop2_id = runeConfigData->prop2_id;
            int32 rune_prop2_val = int32(runeConfigData->prop2_base + runeConfigData->prop2_grow * ToPercent(grow));
            if(rune_prop2_id)
            {
                skillPropSet.Add(rune_prop2_id, rune_prop2_val);
            }

            skillInfo->AddRuneIDs(pRuneStone->GetId());

            // 是否为触发
            if(runeConfigData->trigger)
            {
                skillInfo->SetSkillValid(false);        // 技能设置为不能主动使用
                skillInfo->SetTriggered(true);
            }
        }

        if(isExist)
            skillInfo->AddRuneOks(-1);          // 符文重复
        else
            skillInfo->AddRuneOks(needFeature);
    }

    skillInfo->SetCoolDown(cooldown);

    ////////////////////////////////////////////////////////////////////////////////////
    // 技能和符文汇总
    ////////////////////////////////////////////////////////////////////////////////////
    for(PropertyList::iterator iter = skillPropSet.m_list.begin(); iter != skillPropSet.m_list.end(); iter++)
    {
        Property& src_prop = *iter;
        Property des_prop = PropertySetCalculater::CollectProperty(m_pCreature, src_prop, skillPropSet, true);
        src_prop = des_prop;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // 技能属性受到角色属性的影响
    ////////////////////////////////////////////////////////////////////////////////////
    makeSkillPropSetByBattleUnit(&skillPropSet, m_pCreature, skillConfig);


    ////////////////////////////////////////////////////////////////////////////////////
    // 设置技能属性条目
    ////////////////////////////////////////////////////////////////////////////////////
    for(PropertyList::iterator propIter = skillPropSet.m_list.begin(); propIter != skillPropSet.m_list.end(); propIter++)
    {
        KEY_VALUE prop;
        prop.key = propIter->type;
        prop.value = propIter->value;
        if(prop.key > 0 && prop.value != 0)
            skillInfo->AddProp(prop);
    }


    ////////////////////////////////////////////////////////////////////////////////////
    // 根据技能属性计算最终技能参数
    ////////////////////////////////////////////////////////////////////////////////////
    PropertySet& finalPlayerPropSet = m_pCreature->GetPropertySetCalculater()->GetFinalProp();

    // 攻击/施法速度
    if(skillConfig->attack_type == SKILL_TYPE_ATTACK)
    {   // 技能攻击速度=技能配置攻速*(符文增强攻速+角色攻速)
        attack_speed = int32( attack_speed * (ToPercent(skillPropSet.GetValue(PROP_ATTACK_PER_SEC)) + ToPercent(finalPlayerPropSet.GetValue(PROP_ATTACK_PER_SEC))) );
    }
    else if(skillConfig->attack_type == SKILL_TYPE_MAGIC)
    {   // 技能施法速度=技能配置施法速度*(符文增加施法速度+角色施法速度)
        attack_speed = int32( attack_speed * (ToPercent(skillPropSet.GetValue(PROP_CAST_PER_SEC)) + ToPercent(finalPlayerPropSet.GetValue(PROP_CAST_PER_SEC))) );
    }

    // 多重打击/多重施法
    int32 cast_multi_add = 0;
    if(skillConfig->attack_type == SKILL_TYPE_ATTACK)
        cast_multi_add += skillPropSet.GetValue(PROP_SKILL_ATTACK_REPEAT);
    else if(skillConfig->attack_type == SKILL_TYPE_MAGIC)
        cast_multi_add += skillPropSet.GetValue(PROP_SKILL_CAST_REPEAT);

    // 多重投射
    int bullet_multi_add = 0;
    bullet_multi_add += skillPropSet.GetValue(PROP_SKILL_BULLET_ADD);


    ////////////////////////////////////////////////////////////////////////////////////
    // 设置技能参数
    SKILL_ARGUMETNS* skillArgs = skillInfo->GetSkillArgs();
    skillArgs->cast_speed = attack_speed;
    skillArgs->cast_multi = cast_multi + cast_multi_add;
    skillArgs->bullet_multi = bullet_multi + bullet_multi_add;
    skillArgs->bullet_multi_angle = skillArgs->bullet_multi>0 ? 15 : 0;         // 15度弹道夹角
    skillArgs->bullet_repeat = bullet_repeat;
    skillArgs->bullet_repeat_interval = bullet_repeat_interval;
    skillArgs->bullet_speed = bullet_speed;
    skillArgs->bullet_chain = bullet_chain;
    skillArgs->bullet_chain_id = bullet_chain_id;

}

void SkillInfoManager::CalcAllSkillInfo()
{
    const uint32 nSlotId = (m_pCreature->GetObjectType() == OBJ_PET) ? SKILL_SLOT_Q : SKILL_SLOT_ML;

    PropertySet skillPropSet;

    for(int32 si=0; si<MAX_SKILL_NUM; si++)
    {
        if(!m_SkillInfoDirty[si])
        {
            continue;
        }
        m_SkillInfoDirty[si] = false;

        skillPropSet.Clear();
    
        SkillInfo& finalSkillInfo = m_SkillInfo[si];
        uint32 oldSkillId = finalSkillInfo.GetSkillId();

        finalSkillInfo.Clear();

        finalSkillInfo.SetBookValid(true);

        // 先确定技能书
        ItemStone* pSkillStone = m_StoneGrid[si];
        
        uint32 skillID = 0;
        int32 skillLevel = 1;
        int32 skillQuality = QUALITY_WHITE;
        bool bookValid = false;             // 技能书是否生效
        bool skillValid = false;            // 技能是否可用

        if(!pSkillStone || pSkillStone->GetSubType() != STONE_SUB_TYPE_SKILL)
        {
            if(si != SlotToIndex(nSlotId))
            {
                continue;
            }

            skillValid = false;
        }
        else
        {
            skillID = pSkillStone->GetStoneFuncId();
            skillLevel = pSkillStone->GetLevel();
            skillQuality = pSkillStone->GetQuality();
            skillValid = bookValid = CheckSkillInvalid(skillID);            // 计算技能书是否可用
        }

        // 左鼠的技能书不可用,则技能设置为普攻
        if(!skillValid && (si == SlotToIndex(nSlotId)))
        {
            if(m_pCreature->GetEquipAnimation() == EQUIP_ANIMATION_GJ)
            {
                skillID = NORMAL_ATTACK_FAR;
            }
            else
            {
                skillID = NORMAL_ATTACK_NEAR;
            }
            skillLevel = 1;
            skillQuality = QUALITY_WHITE;
            skillValid = true;
        }

        finalSkillInfo.SetSkillId(skillID);
        finalSkillInfo.SetQuality(skillQuality);           // 技能品质=技能书品质
        finalSkillInfo.SetLevel(skillLevel);               // 技能等级=技能书等级
        finalSkillInfo.SetBookValid(bookValid);
        finalSkillInfo.SetSkillValid(skillValid);

        ItemStone* pRuneStone1 = m_StoneGrid[MAX_SKILL_NUM*1+si];
        ItemStone* pRuneStone2 = m_StoneGrid[MAX_SKILL_NUM*2+si];

        if(IsNormalAttack(finalSkillInfo.GetSkillId()))
        {
            pRuneStone1 = pRuneStone2 = NULL;       // 没有技能书
        }

        CalcSkillInfoEx(&finalSkillInfo, pRuneStone1, pRuneStone2);

        if(oldSkillId != finalSkillInfo.GetSkillId())
        {
            ///////////////////////////////////////////////////////////////////////////////////
            // 技能CD重置
            if(m_pCreature->IsKindOf(&Player::ms_RTTI))
            {
                Player* player = CnDynamicCast(Player, m_pCreature);
                if(player->GetNetState() == Player::kStateInGame)
                    player->MakeSkillCD(finalSkillInfo.GetSkillId(), finalSkillInfo.GetCoolDown());
            }
        }
    }

    // 检查技能对应的光环
    CheckAura();

    // 根据技能切换佣兵AI
    if(m_pCreature->IsKindOf(&Soldier::ms_RTTI))
    {
        Soldier* pSoldier = CnDynamicCast(Soldier, m_pCreature);
        pSoldier->ReloadAI();
    }
}

void SkillInfoManager::makeSkillPropSetByBattleUnit(PropertySet* skillPropertySet, BattleUnit* battleUnit, const SKILL_ROOT_SKILL_STRUCT* skillConfig)
{
    if(!skillPropertySet || !battleUnit || !skillConfig)
    {
        CnAssert(false);
        return;
    }

    int32 attack_type = skillConfig->attack_type;
    
    


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // 技能本身的属性
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    PropertySet& playerBaseProp = battleUnit->GetPropertySetCalculater()->GetBaseProp();

    PropertySet sumPlayerPropSet;
    sumPlayerPropSet.Add(playerBaseProp);

    // 针对技能特性的属性效果
    int32 featureMul = 0;
    // 投射伤害
    if(skillConfig->feature & SKILL_FEATURE_PROJECTILE)
    {
        int32 projectile_damage_mul = sumPlayerPropSet.GetValue(PROP_PROJECTILE_DAMAGE_MUL);
        if(projectile_damage_mul > 0)
        {
            featureMul += projectile_damage_mul;
        }
    }
    
    // 近战伤害
    if(skillConfig->feature & SKILL_FEATURE_MELEE)
    {
        int32 melee_damage_mul = sumPlayerPropSet.GetValue(PROP_MELEE_DAMAGE_MUL);
        if(melee_damage_mul > 0)
        {
            featureMul += melee_damage_mul;
        }
    }

    // 范围伤害
    if(skillConfig->feature & SKILL_FEATURE_AOE)
    {
        int32 aoe_damage_mul = sumPlayerPropSet.GetValue(PROP_AOE_DAMAGE_MUL);
        if(aoe_damage_mul > 0)
        {
            featureMul += aoe_damage_mul;
        }
    }
    
    sumPlayerPropSet.Add(PROP_PHYSICS_DAMAGE_MUL, featureMul);
    sumPlayerPropSet.Add(PROP_FIRE_DAMAGE_MUL, featureMul);
    sumPlayerPropSet.Add(PROP_ICE_DAMAGE_MUL, featureMul);
    sumPlayerPropSet.Add(PROP_THUNDER_DAMAGE_MUL, featureMul);
    sumPlayerPropSet.Add(PROP_POISON_DAMAGE_MUL, featureMul);


    // 角色对技能属性的影响
    for(PropertyList::iterator propIter = skillPropertySet->m_list.begin(); propIter != skillPropertySet->m_list.end(); propIter++)
    {
        Property& srcProp = *propIter;
        Property desProp = PropertySetCalculater::CollectProperty(battleUnit, srcProp, sumPlayerPropSet, false);
        srcProp = desProp;
    }

    PropertySetCalculater::CollectPropertySet(sumPlayerPropSet, battleUnit);

    // buff和地图对技能的影响
    PropertySet& playerBuffMapProp = battleUnit->GetPropertySetCalculater()->GetBuffAndMapProp();
    for(PropertyList::iterator propIter = skillPropertySet->m_list.begin(); propIter != skillPropertySet->m_list.end(); propIter++)
    {
        Property& srcProp = *propIter;
        Property desProp = PropertySetCalculater::CollectProperty(battleUnit, srcProp, playerBuffMapProp, false);
        srcProp = desProp;
    }

    sumPlayerPropSet.Add(playerBuffMapProp);
    PropertySetCalculater::CollectPropertySet(sumPlayerPropSet, battleUnit);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // 源自基础伤害的伤害
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // 源自基础伤害的物理伤害
    int32 sourcePhysicsDamagePercent = skillPropertySet->GetValue(PROP_SKILL_SOURCE_PHYSICS_DAMAGE);
    if(sourcePhysicsDamagePercent > 0)
    {
        int32 source_physics_damage = int32( ToPercent(sourcePhysicsDamagePercent) * sumPlayerPropSet.GetValue(PROP_PHYSICS_DAMAGE) );
        if(source_physics_damage > 0)
            skillPropertySet->Add(PROP_PHYSICS_DAMAGE, source_physics_damage);
        skillPropertySet->Set(PROP_SKILL_SOURCE_PHYSICS_DAMAGE, 0);
    }
    // 源自基础伤害的火焰伤害
    int32 sourceFireDamagePercent = skillPropertySet->GetValue(PROP_SKILL_SOURCE_FIRE_DAMAGE);
    if(sourceFireDamagePercent > 0)
    {
        int32 source_fire_damage = int32( ToPercent(sourceFireDamagePercent) * sumPlayerPropSet.GetValue(PROP_PHYSICS_DAMAGE) );
        if(source_fire_damage > 0)
            skillPropertySet->Add(PROP_FIRE_DAMAGE, source_fire_damage);
        skillPropertySet->Set(PROP_SKILL_SOURCE_FIRE_DAMAGE, 0);
    }
    // 源自基础伤害的冰霜伤害
    int32 sourceIceDamagePercent = skillPropertySet->GetValue(PROP_SKILL_SOURCE_ICE_DAMAGE);
    if(sourceIceDamagePercent > 0)
    {
        int32 source_ice_damage = int32( ToPercent(sourceIceDamagePercent) * sumPlayerPropSet.GetValue(PROP_PHYSICS_DAMAGE) );
        if(source_ice_damage > 0)
            skillPropertySet->Add(PROP_ICE_DAMAGE, source_ice_damage);
        skillPropertySet->Set(PROP_SKILL_SOURCE_ICE_DAMAGE, 0);
    }
    // 源自基础伤害的闪电伤害
    int32 sourceLightningDamagePercent = skillPropertySet->GetValue(PROP_SKILL_SOURCE_LIGHTNING_DAMAGE);
    if(sourceLightningDamagePercent > 0)
    {
        int32 source_lightning_damage = int32( ToPercent(sourceLightningDamagePercent) * sumPlayerPropSet.GetValue(PROP_PHYSICS_DAMAGE) );
        if(source_lightning_damage > 0)
            skillPropertySet->Add(PROP_ICE_DAMAGE, source_lightning_damage);
        skillPropertySet->Set(PROP_SKILL_SOURCE_LIGHTNING_DAMAGE, 0);
    }
    // 源自基础伤害的毒素伤害
    int32 sourcePoisonDamagePercent = skillPropertySet->GetValue(PROP_SKILL_SOURCE_POISON_DAMAGE);
    if(sourcePoisonDamagePercent > 0)
    {
        int32 source_poison_damage = int32( ToPercent(sourcePoisonDamagePercent) * sumPlayerPropSet.GetValue(PROP_PHYSICS_DAMAGE) );
        if(source_poison_damage > 0)
            skillPropertySet->Add(PROP_POISON_DAMAGE, source_poison_damage);
        skillPropertySet->Set(PROP_SKILL_SOURCE_POISON_DAMAGE, 0);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // 角色自带法术伤害
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    if( !(skillConfig->feature & SKILL_FEATURE_DURATION) &&         // 非增益类
        (skillConfig->feature & SKILL_FEATURE_SPELL) || (skillConfig->feature & SKILL_FEATURE_ATTACK) )       // 战斗技能才附加
    {
        int32 attach_fire_damage = sumPlayerPropSet.GetValue(PROP_FIRE_DAMAGE);
        if(attach_fire_damage)
        {
            skillPropertySet->Add(PROP_FIRE_DAMAGE, attach_fire_damage);
        }

        int32 attach_ice_damage = sumPlayerPropSet.GetValue(PROP_ICE_DAMAGE);
        if(attach_ice_damage)
        {
            skillPropertySet->Add(PROP_ICE_DAMAGE, attach_ice_damage);
        }

        int32 attach_lightning_damage = sumPlayerPropSet.GetValue(PROP_THUNDER_DAMAGE);
        if(attach_lightning_damage)
        {
            skillPropertySet->Add(PROP_THUNDER_DAMAGE, attach_lightning_damage);
        }

        int32 attach_poison_damage = sumPlayerPropSet.GetValue(PROP_POISON_DAMAGE);
        if(attach_poison_damage)
        {
            skillPropertySet->Add(PROP_POISON_DAMAGE, attach_poison_damage);
        }
    }



    // 总伤害减少
    int32 all_damage_sub_mul = skillPropertySet->GetValue(PROP_SKILL_DAMAGE_SUB_MUL);
    if(all_damage_sub_mul > 0)
    {
        CalcDamageMul(skillPropertySet, Crown::SDMax(0.0f, 1.0f - ToPercent(all_damage_sub_mul)) );
    }
}

void SkillInfoManager::CalcDamageMul(PropertySet* skillPropertySet, real mul)
{
    int32 old_physics_damage = skillPropertySet->GetValue(PROP_PHYSICS_DAMAGE);
    if(old_physics_damage > 0)
    {
        int32 new_physics_damage = int32(old_physics_damage * mul);
        skillPropertySet->Set(PROP_PHYSICS_DAMAGE, new_physics_damage);
    }

    int32 old_fire_damage = skillPropertySet->GetValue(PROP_FIRE_DAMAGE);
    if(old_fire_damage > 0)
    {
        int32 new_fire_damage = int32(old_fire_damage * mul);
        skillPropertySet->Set(PROP_FIRE_DAMAGE, new_fire_damage);
    }

    int32 old_ice_damage = skillPropertySet->GetValue(PROP_ICE_DAMAGE);
    if(old_ice_damage > 0)
    {
        int32 new_ice_damage = int32(old_ice_damage * mul);
        skillPropertySet->Set(PROP_ICE_DAMAGE, new_ice_damage);
    }

    int32 old_lightning_damage = skillPropertySet->GetValue(PROP_THUNDER_DAMAGE);
    if(old_lightning_damage > 0)
    {
        int32 new_lightning_damage = int32(old_lightning_damage * mul);
        skillPropertySet->Set(PROP_THUNDER_DAMAGE, new_lightning_damage);
    }

    int32 old_poison_damage = skillPropertySet->GetValue(PROP_POISON_DAMAGE);
    if(old_poison_damage > 0)
    {
        int32 new_poison_damage = int32(old_poison_damage * mul);
        skillPropertySet->Set(PROP_POISON_DAMAGE, new_poison_damage);
    }
}

bool SkillInfoManager::CheckSkillInvalid(uint32 skillID)
{
    const SKILL_ROOT_SKILL_STRUCT* skillConfig = SkillConfigManager::Instance()->GetSkillConfigData(skillID);
    if(!skillConfig)
    {
        CnAssert(false);
        return false;
    }

    int32 equipAnimation = m_pCreature->GetEquipAnimation();

    int32 feature = skillConfig->feature;
    if(feature & SKILL_FEATURE_MELEE)
    {   // 近战,要求近战武器
        if(equipAnimation == EQUIP_ANIMATION_GJ)
            return false;
    }
    if(feature & SKILL_FEATURE_BOW)
    {   // 弓箭,要求弓武器
        if(equipAnimation != EQUIP_ANIMATION_GJ)
            return false;
    }

    return true;
}

void SkillInfoManager::CheckAura()
{
    std::vector<uint32> buffIDList;
    m_pCreature->GetBuffController()->GetBuffIDList(&buffIDList);

    for(std::vector<uint32>::iterator iter = buffIDList.begin(); iter != buffIDList.end(); iter++)
    {
        uint32 buffID = *iter;

        bool needEraseBuff = true;

        const BUFF_ROOT_BUFF_STRUCT* buffConfig = SkillConfigManager::Instance()->GetBuffConfigData(buffID);
        if(buffConfig)
        {
            if(buffConfig->is_aure)
            {
                for(int32 i=0; i<SKILL_SLOT_WHOLE; i++)
                {
                    const SKILL_ROOT_SKILL_STRUCT* skillConfig = SkillConfigManager::Instance()->GetSkillConfigData(m_SkillInfo[i].GetSkillId());
                    if(skillConfig)
                    {
                        if(skillConfig->buff == buffID)
                        {
                            needEraseBuff = false;
                            break;
                        }
                    }
                }
            }
            else
            {
                needEraseBuff = false;
            }
        }
        else
        {
            CnAssert(false);
        }

        if(needEraseBuff)
        {
            m_pCreature->DelBuff(buffID);
        }
    }
}