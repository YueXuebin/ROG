#include "gameholder_pch.h"
#include "regular_first.h"
#include "point_monster.h"
#include "event_define.h"
#include "scene.h"
#include "monster.h"
#include "rog_regular_event_monster_dead.h"
#include "player.h"
#include "pet_manager.h"
#include "item_factory.h"
#include "drop_object.h"
#include "item_config_manager.h"

RegularFirst::RegularFirst(RegularDoc* pDoc) :
RegularROGDungeon(pDoc)
{

}

RegularFirst::~RegularFirst()
{

}

void RegularFirst::FirstUpdate()
{
    RegularROGDungeon::FirstUpdate();

    PointMonster* pPointMonster  = NULL;
	PointMonster* pPointMonster11  = NULL;
	PointMonster* pPointMonster12  = NULL;
	PointMonster* pPointMonster13  = NULL;
    /////////////////////////////////////////////////////////////////////////////////
    // 关卡配置
    SetMapLevel(1);

    // 胜利条件
    ROGRegularEvent_MonsterDead* pEventMonsterDead = CnNew ROGRegularEvent_MonsterDead(this);
    pEventMonsterDead->SetMonster(3694, 1);
    AddSucceedEvent(pEventMonsterDead); // 杀死怪

    /////////////////////////////////////////////////////////////////////////////////
    // 点11
    pPointMonster11 = CnNew PointMonster(this, 11);
    pPointMonster11->SetRadius(0);
    pPointMonster11->SetMonster(3690, 1);         // 进入游戏第一个怪,空手打死
	// 点12
	pPointMonster = CnNew PointMonster(this, 12);
	pPointMonster->SetRadius(200);
	pPointMonster->SetMonster(3695, 2);         // 进入游戏第一个怪,空手打死
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetDelayTime(10);            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(300);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointMonster11);
	// 点13
	pPointMonster = CnNew PointMonster(this, 13);
	pPointMonster->SetRadius(250);
	pPointMonster->SetMonster(3695, 2);         // 进入游戏第一个怪,空手打死
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetDelayTime(3000);            // 触发后延时500ms出怪
	pPointMonster->SetSpawnSpeed(500);                // 出怪间隔300ms
	pPointMonster->LinkTrigger(pPointMonster11);

	// 点14
	pPointMonster = CnNew PointMonster(this, 14);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3804, 1);         // npc艾瑞贝丝
    pPointMonster->SetGroup(FRIEND_GROUP);
    pPointMonster->SetAttackable(false);

   /////////////////////////////////////////////////////////////////////////////////
    // 点21
    pPointMonster = CnNew PointMonster(this, 21);
    pPointMonster->SetRadius(300);
    pPointMonster->SetMonster(3697, 2);         // 穿装备后,白怪
	pPointMonster->SetTriggerID(11);                  // Trigger点事件触发
	// 点21
	pPointMonster = CnNew PointMonster(this, 21);
	pPointMonster->SetRadius(300);
	pPointMonster->SetMonster(3691, 1);         // 穿装备后,白怪
	pPointMonster->SetTriggerID(11);                  // Trigger点事件触发

   /////////////////////////////////////////////////////////////////////////////////
    // 点31
    pPointMonster = CnNew PointMonster(this, 31);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMonster(3692, 1);         // 绿怪,掉衣服
	pPointMonster->SetTriggerID(14);                  // Trigger点事件触发

	// 点32
	pPointMonster = CnNew PointMonster(this, 32);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3697, 1);         // 绿怪,掉衣服
	pPointMonster->SetTriggerID(14);                  // Trigger点事件触发

	// 点33
	pPointMonster = CnNew PointMonster(this, 33);
	pPointMonster->SetRadius(0);
	pPointMonster->SetMonster(3697, 1);         // 绿怪,掉衣服
	pPointMonster->SetTriggerID(14);                  // Trigger点事件触发

	// 点15
	pPointMonster = CnNew PointMonster(this, 15);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3695, 1);     
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetDelayTime(300);            // 触发后延时500ms出怪
	pPointMonster->SetTriggerID(15);                  // Trigger点事件触发

   /////////////////////////////////////////////////////////////////////////////////
    // 点41
    pPointMonster = CnNew PointMonster(this, 41);
    pPointMonster->SetRadius(50);
    pPointMonster->SetMonster(3693, 1);         // 蓝怪
	pPointMonster->SetTriggerID(12);                  // Trigger点事件触发
    // 点42
	pPointMonster = CnNew PointMonster(this, 42);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3695, 1);         
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetDelayTime(300);            // 触发后延时500ms出怪
	pPointMonster->SetTriggerID(12);                  // Trigger点事件触发
	// 点45
	pPointMonster = CnNew PointMonster(this, 45);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3695, 1);         
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetDelayTime(300);            // 触发后延时500ms出怪
	pPointMonster->SetTriggerID(12);                  // Trigger点事件触发
	// 点43
	pPointMonster = CnNew PointMonster(this, 43);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3695, 1);         
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetDelayTime(600);            // 触发后延时500ms出怪
	pPointMonster->SetTriggerID(12);                  // Trigger点事件触发
	// 点44
	pPointMonster = CnNew PointMonster(this, 44);
	pPointMonster->SetRadius(50);
	pPointMonster->SetMonster(3695, 1);         
	pPointMonster->SetDefaultMovie();                 // 出场演绎
	pPointMonster->SetDelayTime(600);            // 触发后延时500ms出怪
	pPointMonster->SetTriggerID(12);                  // Trigger点事件触发


   /////////////////////////////////////////////////////////////////////////////////
	// 点52
	pPointMonster12 = CnNew PointMonster(this, 52);
	pPointMonster12->SetRadius(600);
	pPointMonster12->SetMonster(3698, 10);         
	pPointMonster12->SetDefaultMovie();                 // 出场演绎
    pPointMonster12->SetSpawnSpeed(1000);                // 出怪间隔300ms
	pPointMonster12->SetTriggerID(13);                  // Trigger点事件触发
	// 点53
	pPointMonster13 = CnNew PointMonster(this, 53);
	pPointMonster13->SetRadius(50);
	pPointMonster13->SetMonster(3696, 1);         
	pPointMonster13->SetDefaultMovie();                 // 出场演绎
	pPointMonster13->SetSpawnSpeed(500);                // 出怪间隔300ms
    pPointMonster13->LinkTrigger(pPointMonster12);


    // 点51
    pPointMonster = CnNew PointMonster(this, 51);
    pPointMonster->SetRadius(0);
    pPointMonster->SetMonster(3694, 1);         // boss
    pPointMonster->SetQuality(QUALITY_GREEN);
	pPointMonster->SetDelayTime(4500);            // 触发后延时500ms出怪
	pPointMonster->LinkTrigger(pPointMonster13);


}

void RegularFirst::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    RegularROGDungeon::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
    case BATTLE_EVENT_MONSTER_DEAD:
        {
            uint32 monsterObjId = arg1;
            uint32 playerID = arg2;

            Player* pPlayer = CnDynamicCast(Player, GetScene()->GetBattleUnit(playerID));
            Monster* pMonster = CnDynamicCast(Monster, GetScene()->GetBattleUnit(monsterObjId));
            FirstDrop(pPlayer, pMonster);
        }
        break;
    case BATTLE_EVENT_PLAYER_DEAD:
        {
            OnBattleEventPlayerKill(arg2, arg1);
        }
        break;
    case EVENT_PLAYER_RESURGENCE_REQ:       // 接收复活请求
        {
            OnPlayerResurgenceReq(arg1);
        }
        break;
    default:
        break;
    }
}

void RegularFirst::OnBattleEventPlayerKill( uint32 attackerID, uint32 defencerID )
{
    Player* attackPlayer = 0;
    //玩家死亡
    attackPlayer = GetScene()->GetPlayerByObjectId(defencerID);
    if(!attackPlayer)
        return;

    NONE none;
    attackPlayer->SendToGate(MSG_PLAYER_DEAD_CAN_RELIVE, &none);
}

void RegularFirst::OnPlayerResurgenceReq(int32 arg1)
{
    Player* player = m_pScene->FindPlayer(arg1);
    if(!player || !player->IsDead())
        return;

    Resurgence((BattleUnit *)player, 0, 0);  // 原地复活
    Resurgence((BattleUnit *)player->GetPetMgr()->GetSoldier(), 0, 0); // 复活佣兵
}

void RegularFirst::FirstDrop(Player* pPlayer, Monster* pMonster)
{
    if (!pPlayer || !pMonster)
    {
        return;
    }

    std::vector<uint32> itemIdList;
    ItemBase* pItemBase(NULL);

    switch(pMonster->GetCreatureId())
    {
    case 3692:  // 塞壬
        {
            // 强制掉落一件衣服
            switch(pPlayer->GetCareer())
            {
            case CAREER_WARRIOR:    // 战士
                {
                    itemIdList.push_back(11301); // 1级身：力
                }
                break;
            case CAREER_MAGE:       // 法师
                {
                    itemIdList.push_back(11401); // 1级身：智
                }
                break;
            case CAREER_ARCHER:     // 弓手
                {
                    itemIdList.push_back(11501); // 1级身：敏
                }
                break;
            default:
                return;
            }
        }
        break;
    case 3693:  // 海族萨满
        {
            // 强制掉落一个技能
            switch(pPlayer->GetCareer())
            {
            case CAREER_WARRIOR:    // 战士
                {
                    itemIdList.push_back(17001); // 劈砍
                }
                break;
            case CAREER_MAGE:       // 法师
                {
                    itemIdList.push_back(17003); // 连锁闪电
                }
                break;
            case CAREER_ARCHER:     // 弓手
                {
                    itemIdList.push_back(17013); // 多重箭
                }
                break;
            default:
                return;
            }
        }
        break;
    case 3694:  // 斯库拉
        {
            // 强制掉落三个装备，一蓝药，一红药
            switch(pPlayer->GetCareer())
            {
            case CAREER_WARRIOR:    // 战士
                {
                    itemIdList.push_back(11001); // 1级头：力
                    itemIdList.push_back(11601); // 1级肩：力
                    itemIdList.push_back(12201); // 1级鞋：力
                }
                break;
            case CAREER_MAGE:       // 法师
                {
                    itemIdList.push_back(11101); // 1级头：智
                    itemIdList.push_back(11701); // 1级肩：智
                    itemIdList.push_back(12301); // 1级鞋：智
                }
                break;
            case CAREER_ARCHER:     // 弓手
                {
                    itemIdList.push_back(11201); // 1级头：敏
                    itemIdList.push_back(11801); // 1级肩：敏
                    itemIdList.push_back(12401); // 1级鞋：敏
                }
                break;
            default:
                return;
            }

            itemIdList.push_back(13001); // 生命药水
            itemIdList.push_back(13002); // 魔法药水
        }
        break;
    default:
        return;
    }

    for (size_t i=0; i<itemIdList.size(); ++i)
    {
        pItemBase = ItemFactory::CreateItem(itemIdList[i], ItemConfigManager::Instance()->GetItemTypeById(itemIdList[i]));
        if (!pItemBase)
        {
            continue;;
        }

        DropData dropdata;
        dropdata.from_pos = pMonster->GetPosition();
        dropdata.pos = GetScene()->FindValidDropItemPostion(dropdata.from_pos, 250);            // 掉落范围
        dropdata.owner_id = 0;
        dropdata.lifetime = 20000;           // 20秒,掉落物品消失时间

        GetScene()->GetRegular()->AddDropObject(dropdata, pItemBase);
    }
}
