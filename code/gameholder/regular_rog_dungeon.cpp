#include "gameholder_pch.h"
#include "regular_rog_dungeon.h"
#include "battle_unit.h"
#include "point_monster.h"
#include "player.h"
#include "event_define.h"
#include "scene.h"

CnImplementRTTI(RegularROGDungeon, RegularSingle);

RegularROGDungeon::RegularROGDungeon(RegularDoc *pDoc) : 
RegularSingle(pDoc),
m_doFirstSend(false)
{
}

RegularROGDungeon::~RegularROGDungeon()
{
}

void RegularROGDungeon::OneSecondUpdate(int32 deltatime)
{
    RegularSingle::OneSecondUpdate(deltatime);
}

void RegularROGDungeon::Update(uint32 curr_tick)
{
    RegularSingle::Update(curr_tick);
   
    if(!m_doFirstSend)
    {
        m_doFirstSend = true;
        CnAssert(m_MonsterIdList.size() == 0);

        // 获取场景里面所有怪的ID, 去重后发送给客户端
        std::vector<PointBase*> pPointBaseList = GetBusinessPoints();

        for (uint32 i=0; i<pPointBaseList.size(); i++)
        {
            PointBase* pPointBase = pPointBaseList[i];

            // 是不是出怪点
            if (!pPointBase->IsKindOf(&PointMonster::ms_RTTI))
                continue;

            PointMonster* pPointMonster = CnDynamicCast(PointMonster, pPointBase);
            if(!pPointMonster) 
            {
                CnAssert(false);
                return;
            }

            m_MonsterIdList.push_back(pPointMonster->GetMonsterId());
        }

        // ID去重
        std::sort(m_MonsterIdList.begin(), m_MonsterIdList.end());
        std::vector<uint32>::iterator newEnd = unique(m_MonsterIdList.begin(), m_MonsterIdList.end());
        m_MonsterIdList.erase(newEnd, m_MonsterIdList.end());
    }

}

void RegularROGDungeon::OnCreatureEnter(BattleUnit* creature)
{
    RegularSingle::OnCreatureEnter(creature);

    // 如果是玩家
    if (creature->IsKindOf(&Player::ms_RTTI))
    {
        Player* pPlayer = CnDynamicCast(Player, creature);
        if(!pPlayer) 
        {
            CnAssert(false);
            return;
        }

        if(m_MonsterIdList.size() > 0)
        {   // 通知客户端预加载副本中的怪物
            DUNGEON_MOSTER_ID_LIST MosterIdList;
            MosterIdList.id_list = m_MonsterIdList;
            pPlayer->SendToGate(MSG_DUNGEON_MONSTER_ID_INFO, &MosterIdList);
        }
    }


}

void RegularROGDungeon::OnCreatureLeave(BattleUnit* creatrue)
{
    RegularSingle::OnCreatureLeave(creatrue);
}

void RegularROGDungeon::SceneClear(bool success, std::vector<std::string>* arglist)
{
    if (success)
    {
        // 杀死所有怪
        KillAll();
        
        std::vector<PointBase*> pBusinessPoints = GetBusinessPoints();
        uint32 count =  pBusinessPoints.size();

        // 所有出怪点停止出怪
        for(uint32 i=0; i<count; i++)
        {
            PointBase* pPointBase = pBusinessPoints[i];
            CnAssert(pPointBase);

            PointMonster* pPointMonster = CnDynamicCast(PointMonster, pPointBase);
            if(!pPointMonster)
                continue;
            
            // 停止出怪
            pPointMonster->GetSpawn()->SetClearAllMosters();
        }
    }

    RegularSingle::SceneClear(success);        // 场景通关
}