#include "gameholder_pch.h"
#include "notifier_utils.h"
#include "player.h"
#include "npc.h"
#include "monster.h"
#include "pet.h"
#include "offline_player.h"
#include "boss.h"
#include "trigger.h"
#include "touch.h"
#include "drop_object.h"
#include "equip_manager.h"
#include "enemy_manager.h"
#include "title_manager.h"
#include "drop_object.h"
#include "sword_manager.h"
#include "soldier.h"
#include "state_controller.h"


void FILL_MSG_APPEAR_LIST(BATTLEUNIT_APPEAR_LIST& sendMsg, BattleUnit* pBattleUnit, Player* SeePlayer)
{
    if(pBattleUnit->GetObjectType() == OBJ_PLAYER)
    {
        PLAYER_APPEAR PlayerAppear;
        FILL_PLAYER_APPEAR(PlayerAppear, (Player*)pBattleUnit);
        sendMsg.player_list.push_back(PlayerAppear);
    }
    else if(pBattleUnit->GetObjectType() == OBJ_MONSTER)
    {
        MONSTER_APPEAR MonsterAppear;
        FILL_MONSTER_APPEAR(MonsterAppear, (Monster*)pBattleUnit);
        sendMsg.monster_list.push_back(MonsterAppear);
    }
    else if(pBattleUnit->GetObjectType() == OBJ_BOSS)
    {
        BOSS_APPEAR BossAppear;
        FILL_BOSS_APPEAR(BossAppear, (Boss*)pBattleUnit);
        sendMsg.boss_list.push_back(BossAppear);
    }
    else if(pBattleUnit->GetObjectType() == OBJ_PET)
    {
        PET_APPEAR PetAppear;
        FILL_PET_APPEAR(PetAppear, (Pet*)pBattleUnit);
        sendMsg.pet_list.push_back(PetAppear);
    }
    else if(pBattleUnit->GetObjectType() == OBJ_OFFLINE_PLAYER)
    {
        OFFLINE_PLAYER_APPEAR OfflinePlayerAppear;
        FILL_OFFLINE_PLAYER_APPEAR(OfflinePlayerAppear, (OfflinePlayer*)pBattleUnit);
        sendMsg.offline_player_list.push_back(OfflinePlayerAppear);
    }
    //else if(pBattleUnit->GetObjectType() == OBJ_NPC)
    //{
    //    NPC_APPEAR NpcAppear;
    //    FILL_NPC_APPEAR(NpcAppear, (Npc*)pBattleUnit);
    //    sendMsg.npc_list.push_back(NpcAppear);
    //}
    else
        CnAssert(false);

}

void FILL_PLAYER_APPEAR(PLAYER_APPEAR& PlayerAppear, Player* pPlayer)
{
    // 基础属性
    FILL_BATTLEUNIT_APPEAR(PlayerAppear.base, pPlayer);

    // 装备
    pPlayer->GetEquipMgr()->FillEquipInfo(PlayerAppear.equip_info);
    PlayerAppear.equip_info.wing_id = pPlayer->GetWingID(); // 翅膀ID
    
    PlayerAppear.player_id.id_h = U64ID_HIGH(pPlayer->GetPlayerID());      // 玩家 ID 高位
    PlayerAppear.player_id.id_l = U64ID_LOW(pPlayer->GetPlayerID());       // 玩家 ID 低位
    PlayerAppear.ride_id = pPlayer->GetRidePetID();                       // 骑宠ID
    PlayerAppear.face_id = pPlayer->GetFaceID();                          // 脸ID
    PlayerAppear.fashion_id = pPlayer->GetFashionID();                    // 时装ID
    PlayerAppear.sword_id = pPlayer->GetSwordManager()->GetEquipSwordID();// 圣剑ID
    PlayerAppear.guild_name = pPlayer->GetGuildName();                    // 公会名称

    PlayerAppear.battle_power = pPlayer->GetBattlePower();                  // 战斗力
    // 称号
    std::vector<TITLE_INFO> HaveTitle_Vec;
    std::vector<TITLE_INFO>::iterator haveTitle;
    std::vector<TITLE_INFO>& listTitle = pPlayer->GetTitleManager()->GetTitleList();
    for(haveTitle = listTitle.begin();haveTitle!= listTitle.end(); haveTitle++)
    {
        if (haveTitle->b_state == HAVE_TITLE)
        {
            HaveTitle_Vec.push_back(*haveTitle);
        }
    }
    PlayerAppear.title_list.list = HaveTitle_Vec;
    // 成长点
    PlayerAppear.growth_point = pPlayer->GetGrowthPoint();
    // VIP信息
    PlayerAppear.vip_info = pPlayer->GetVipInfo();
}

void FILL_OFFLINE_PLAYER_APPEAR(OFFLINE_PLAYER_APPEAR& OfflinePlayerAppear, OfflinePlayer* pOfflinePlayer)
{
    // 基础属性
    FILL_BATTLEUNIT_APPEAR(OfflinePlayerAppear.base, pOfflinePlayer);

//     // 装备外形
//     const std::vector<OFFLINE_EQUIP_INFO>& equipList = pOfflinePlayer->GetPlayerInfo().equip_list;
//     for(std::vector<OFFLINE_EQUIP_INFO>::const_iterator iter = equipList.begin(); iter != equipList.end(); iter++)
//     {
//         OfflinePlayerAppear.appear_list.push_back(iter->appear_id);
//     }
}

void FILL_PET_APPEAR(PET_APPEAR& PetAppear, Pet* pPet)
{
    // 基础属性
    FILL_BATTLEUNIT_APPEAR(PetAppear.base, pPet);

    PetAppear.owner_object_id = (pPet->GetMaster() ? pPet->GetMaster()->GetId() : 0);       // 主人的id

    if (pPet->IsKindOf(&Soldier::ms_RTTI))
    {
        Soldier* pSoldier = CnDynamicCast(Soldier, pPet);
        pSoldier->GetEquipMgr()->FillEquipInfo(PetAppear.equip_info);
    }
}

void FILL_MONSTER_APPEAR(MONSTER_APPEAR& MonsterAppear, Monster* pMonster)
{
    // 基础属性
    FILL_BATTLEUNIT_APPEAR(MonsterAppear.base, pMonster);
    
    const std::vector<uint32>& modifierIds = pMonster->GetModifierIDs();
    for(uint32 i=0; i<modifierIds.size(); i++)
    {
        MonsterAppear.modifier_list.push_back(modifierIds[i]);
    }
}

void FILL_BOSS_APPEAR(BOSS_APPEAR& BossAppear, Boss* pBoss)
{
    // 基础属性
    FILL_BATTLEUNIT_APPEAR(BossAppear.base, pBoss);

    BossAppear.bossui = pBoss->GetBossUI();

    BossAppear.createtime = pBoss->GetCreateTime()/1000;
    BossAppear.lifetime = pBoss->GetLifeTime()/1000;     // 毫秒->秒
    BossAppear.corpse_time = pBoss->GetCorpseTime();
    BossAppear.b_corpse = pBoss->GetBcorpse();
}

void FILL_BATTLEUNIT_APPEAR(BATTLEUNIT_APPEAR& appear, BattleUnit* pBattleUnit)
{
    appear.Clear();

    appear.object_id = pBattleUnit->GetId();
    if(pBattleUnit->IsKindOf(&Creature::ms_RTTI))
    {
        appear.creature_id = ((Creature*)pBattleUnit)->GetCreatureId();
        appear.speed = ((Creature*)pBattleUnit)->GetMoveSpeed();
    }
    else
    {
        CnAssert(false);        // BattleUnit是个接口类，不应该有实例存在
        appear.creature_id = 0;
    }
    appear.object_type = pBattleUnit->GetObjectType();
    appear.angle = pBattleUnit->GetAngle();

    if(pBattleUnit->GetObjectType() == OBJ_PLAYER)
    {
        Player* pPlayer = CnDynamicCast(Player, pBattleUnit);
        appear.sex = pPlayer->GetSex();
        appear.career = pPlayer->GetCareer();
        appear.name = pPlayer->GetName();
        appear.region = pPlayer->GetRegion();
    }
    else if(pBattleUnit->GetObjectType() == OBJ_OFFLINE_PLAYER)
    {
        OfflinePlayer* pOfflinePlayer = CnDynamicCast(OfflinePlayer, pBattleUnit);
        appear.sex = pOfflinePlayer->GetPlayerInfo().sex;
        appear.career = pOfflinePlayer->GetPlayerInfo().career;
        appear.name = pOfflinePlayer->GetPlayerInfo().name;
    }
    else if(pBattleUnit->IsKindOf(&Soldier::ms_RTTI))
    {
        Soldier* pSoldier = CnDynamicCast(Soldier, pBattleUnit);
        appear.sex = pSoldier->GetSex();
        appear.name = pSoldier->GetName();
    }
    else
    {
        appear.sex = 0;
        appear.career = 0;
        appear.name = "";
    }
    appear.attackable   = pBattleUnit->GetAttackable();
    appear.cantmove     = pBattleUnit->GetCantMove();
    appear.cantangle    = pBattleUnit->GetCantAngle();
    appear.cantbuff     = pBattleUnit->GetCantBuff();
    appear.cantheal     = pBattleUnit->GetCantHeal();
    appear.radius       = pBattleUnit->GetRadius();
    appear.group        = pBattleUnit->GetGroup();
    appear.team_id_h    = U64ID_HIGH(pBattleUnit->GetTeamID());
    appear.team_id_l    = U64ID_LOW(pBattleUnit->GetTeamID());
    appear.state        = pBattleUnit->GetState();
    if(appear.state == STATE_MOVIE)
    {
        appear.state_arg = pBattleUnit->GetStateController()->GetMovieName();
    }
    appear.showflag     = pBattleUnit->GetShowFlag();
    appear.x            = pBattleUnit->GetX();
    appear.y            = pBattleUnit->GetY();
    appear.hp           = pBattleUnit->GetHP();
    appear.max_hp       = pBattleUnit->GetMaxHp();
    appear.mp           = pBattleUnit->GetMP();
    appear.max_mp       = pBattleUnit->GetMaxMp();
    appear.es           = pBattleUnit->GetEnergyShield();
    appear.max_es       = pBattleUnit->GetMaxEnergyShield();
    appear.level        = pBattleUnit->GetLevel();
}

void FILL_MSG_SCENEOBJECT_APPEAR_LIST(SCENEOBJECT_APPEAR_LIST& sendMsg, SceneObject* pSceneObject, Player* SeePlayer)
{
    if(pSceneObject->GetObjectType() == OBJ_TRIGGER)
    {   // 触发器
        TRIGGER_APPEAR TriggerAppear;
        FILL_TRIGGER_APPEAR(TriggerAppear, (Trigger*)pSceneObject, SeePlayer);
        sendMsg.trigger_list.push_back(TriggerAppear);
    }
    else if(pSceneObject->GetObjectType() == OBJ_TOUCH)
    {   // 触摸点
        TOUCH_APPEAR TouchAppear;
        FILL_TOUCH_APPEAR(TouchAppear, (Touch*)pSceneObject);
        sendMsg.touch_list.push_back(TouchAppear);
    }
    else if(pSceneObject->GetObjectType() == OBJ_DROP_OBJECT)
    {   // 掉落物品
        DROP_OBJECT_APPEAR DropObjectAppear;
        FILL_DROP_OBJECT_APPEAR(DropObjectAppear, (DropObject*)pSceneObject);
        sendMsg.drop_object_list.push_back(DropObjectAppear);
    }
    else
        CnAssert(false);

}

void FILL_TRIGGER_APPEAR(TRIGGER_APPEAR& TriggerAppear, Trigger* pTrigger, Player* SeePlayer)
{
    FILL_SCENEOBJECT_APPEAR(TriggerAppear.base, pTrigger);

    TriggerAppear.base.arg1 = pTrigger->GetTriggerId();
    TriggerAppear.show_id = pTrigger->GetShowID();
    TriggerAppear.obs_name = pTrigger->GetObsName();
    TriggerAppear.func_id = pTrigger->GetFuncID();
    TriggerAppear.func_type = pTrigger->GetFuncType();
    TriggerAppear.radius = pTrigger->GetRadius();
    TriggerAppear.is_touch = pTrigger->GetTouch();
    TriggerAppear.enable = pTrigger->IsEnable();
    TriggerAppear.triggered = pTrigger->IsTriggered();
    TriggerAppear.obs_pos = pTrigger->m_ObsPos;
    TriggerAppear.pick_size = pTrigger->m_PickSize;
    TriggerAppear.pick_pos = pTrigger->m_PickPos;

    // trigger是否对player可见
    if(SeePlayer && SeePlayer->NeedDisappearTrigger(pTrigger->GetTriggerId()))
    {
        TriggerAppear.base.object_flag |= SHOW_FLAG_DISAPPEAR;
    }
}

void FILL_TOUCH_APPEAR(TOUCH_APPEAR& TouchAppear, Touch* pTouch)
{
    FILL_SCENEOBJECT_APPEAR(TouchAppear.base, pTouch);
    
    TouchAppear.base.arg1 = pTouch->GetTouchID();
    TouchAppear.radius = pTouch->GetRadius();

    TouchAppear.show_id = pTouch->GetShowID();
    TouchAppear.obs_name = pTouch->GetObsName();
    TouchAppear.obs_pos = pTouch->m_ObsPos;
    TouchAppear.pick_size = pTouch->m_PickSize;
    TouchAppear.pick_pos = pTouch->m_PickPos;
}

void FILL_DROP_OBJECT_APPEAR(DROP_OBJECT_APPEAR& DropObjectAppear, DropObject* pDropObject)
{
    FILL_SCENEOBJECT_APPEAR(DropObjectAppear.base, pDropObject);

    DropObjectAppear.from_x = (int32)pDropObject->GetFromPos().x;
    DropObjectAppear.from_y = (int32)pDropObject->GetFromPos().y;
    DropObjectAppear.passtime = pDropObject->GetPassTime();
    DropObjectAppear.lifetime = pDropObject->GetLifeTime();
    DropObjectAppear.owner_id = pDropObject->GetOwnerID();
    FILL_ITEM_APPEAR(DropObjectAppear.item_info, pDropObject->GetItem());
}

void FILL_SCENEOBJECT_APPEAR(SCENEOBJECT_APPEAR& appear, SceneObject* pSceneObject)
{
    appear.Clear();

    appear.object_id = pSceneObject->GetId();
    appear.object_type = pSceneObject->GetObjectType();
    appear.x = pSceneObject->GetX();
    appear.y = pSceneObject->GetY();
    appear.angle = pSceneObject->GetAngle();
    appear.arg1 = appear.arg2 = 0;

    if(pSceneObject->IsKindOf(&Trigger::ms_RTTI))
    {
        appear.arg1 = ((Trigger*)pSceneObject)->GetTriggerId();
    }
    else if(pSceneObject->IsKindOf(&Touch::ms_RTTI))
    {
        appear.arg1 = ((Touch*)pSceneObject)->GetTouchID();
    }
    else if(pSceneObject->IsKindOf(&DropObject::ms_RTTI))
    {
         //
    }

}

void FILL_ITEM_APPEAR(ITEM_INFO& item_info, const ItemBase* pItemBase)
{
    if(pItemBase)
    {
        switch(pItemBase->GetItemType())
        {
        case ITEM_TYPE_EQUIP:
            {
                ITEM_EQUIP equip;
                pItemBase->SaveItem(&equip);
                item_info.equip_list.push_back(equip);
            }
            break;
        case ITEM_TYPE_CONSUME:
            {
                ITEM_CONSUME consume;
                pItemBase->SaveItem(&consume);
                item_info.consume_list.push_back(consume);
            }
            break;
        case ITEM_TYPE_MATERIAL:
            {
                ITEM_MATERIAL material;
                pItemBase->SaveItem(&material);
                item_info.material_list.push_back(material);
            }
            break;
        case ITEM_TYPE_STONE:
            {
                ITEM_STONE stone;
                pItemBase->SaveItem(&stone);
                item_info.stone_list.push_back(stone);
            }
            break;
        default:
            CnAssert(false);
            break;
        }
    }
    else
        CnAssert(false);
}