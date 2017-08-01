#include "gameholder_pch.h"
#include "sub_regular_monster.h"
#include "pet.h"
#include "event_define.h"
#include "scene.h"
#include "mail_manager.h"
#include "mail_title_config.h"
#include "game_util.h"
#include "gate_handler.h"
#include "player_manager.h"
#include "item_manager.h"
#include "trigger.h"
#include "monster.h"
#include "spawn.h"
#include "setting_manager.h"
#include "drop_object.h"
#include "world_map_config.h"
#include "boss.h"
#include "regular_event.h"
#include "event_define.h"
#include "battle_unit.h"
#include "scene.h"
#include "checkpoint.h"
#include "player.h"
#include "boss.h"
#include "ai_controller.h"
#include "enemy_manager.h"
#include "mail_manager.h"
#include "mail_title_config.h"
#include "guild_manager.h"
#include "world_boss_manager.h"
#include "game_util.h"
#include "void_field_config.h"
#include "regular_field_boss.h"

SubRegularMonster::SubRegularMonster(Regular* pRegular) :
SubRegular(pRegular)
{
    m_masterRegular = CnDynamicCast(RegularFiledBoss, pRegular);
    CnAssert(m_masterRegular);

}

SubRegularMonster::~SubRegularMonster()
{
}

void SubRegularMonster::OneSecondUpdate( int32 deltatime )
{
    SubRegular::OneSecondUpdate(deltatime);
}

void SubRegularMonster::OnCreatureEnter( BattleUnit* creature )
{
    SubRegular::OnCreatureEnter(creature);
}

void SubRegularMonster::OnCreatureLeave( BattleUnit* creature )
{
    SubRegular::OnCreatureLeave(creature);
}

void SubRegularMonster::ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3)
{
    SubRegular::ReceiveEvent(frameCount, eventId, arg1, arg2, arg3);

    switch(eventId)
    {
    case BATTLE_EVENT_MONSTER_DEAD:
        {
           // std::vector<std::string> strList = Crown::SDSplitStrings(arg3, '&');
           ////atoi
           // uint32 monseterid = Crown::SDAtou(strList[0].c_str());
           //OnBattleEventMonsterDead((uint32)arg1, (uint32)arg2, monseterid);
        }
        break;
    default:
        break;
    }
}

void SubRegularMonster::Update(uint32 curr_tick)
{
    SubRegular::Update(curr_tick);

    CheckDropItemTime();
}


void SubRegularMonster::CheckDropItemTime()
{
    //FIELD_BOSS_UPDATE_DROP_STATE_ACK sendMsg;

    //uint32 curtime = (uint32)Crown::SDNow().GetTimeValue();
    //Scene::DropObjectMap& dropmap = GetScene()->GetDropItemsMap();
    //Scene::DropObjectMap::iterator dropiter;
    //for(dropiter = dropmap.begin(); dropiter != dropmap.end(); dropiter++)
    //{
    //    DropObject* dropObj = dropiter->second;
    //    if(!dropObj) continue; 
    //    if(dropObj->GetFromtype() == OBJ_MONSTER)
    //    {
    //        if(!dropObj->isDestory() &&  curtime >= dropObj->GetCanPickTime() && dropObj->GetPlayerObjectID())
    //        {
    //            dropObj->SetArriveTime(1); 
    //            sendMsg.object_id = dropObj->GetId();
    //            sendMsg.arrive_time = dropObj->GetArriveTime();
    //            Scene* curscene = GetScene();
    //            if(curscene)
    //                curscene->NotifyMessage(dropObj,MSG_FIELD_BOSS_UPDATE_DROP_STATE_ACK, &sendMsg);//某对象的范围通知
    //        }
    //    }
    //}
}

void SubRegularMonster::OnBattleEventMonsterDead(uint32 monstercreateID, uint32 killerID,uint32 monseterid)
{
    //Player* player = m_masterRegular->GetPlayerByUnitID(killerID);

    //if(!player) return;

    //BattleUnit*  battleunit = GetScene()->GetBattleUnit(monseterid);
    //if(battleunit)
    //{
    //   CoVec2 vec = battleunit->GetPosition();

    //   int32 randNum = Crown::SDRandom(BASE_RATE_NUM);
    //   const REWARD_ITEM* rewardItem = WorldMapConfig::Instance()->GetMonsterDeadRandItem(randNum);

    //   if(!rewardItem) return;

    //   CoVec2 endVec;  //终点
    //   endVec.x =vec.x +  rand() % 300;
    //   endVec.y= vec.y + rand() %  300; 

    //   DropData dropdata;
    //   dropdata.dropid = 1;
    //   dropdata.x = (int32)vec.x;
    //   dropdata.y = (int32) vec.y;
    //   dropdata.fromid =  monseterid;
    //   dropdata.pos = endVec;
    //   dropdata.playerobjid = player->GetId();
    //   dropdata.picktime = WorldMapConfig::Instance()->GetKillMosterPickTime();
    //   dropdata.sysappeartime = WorldMapConfig::Instance()->GetkillMosterLifeTime();
    //   dropdata.arrvietime = 0;
    //   dropdata.fromtype = OBJ_MONSTER;
    //   dropdata.reward_item.id = rewardItem->id;
    //   dropdata.reward_item.type = rewardItem->type;
    //   dropdata.reward_item.num = rewardItem->num;
    //   m_Regular->AddDropObject(dropdata);
    //}

}
