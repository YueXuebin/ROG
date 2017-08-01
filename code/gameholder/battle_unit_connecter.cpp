#include "gameholder_pch.h"
#include "battle_unit_connecter.h"
#include "world_prop_communicator.h"
#include "guild_boss_communicator.h"
#include "twilight_war_communicator.h"
#include "boss.h"
#include "dungeon_set.h"
#include "guild_config.h"
#include "world_boss_manager.h"
#include "dungeon_data.h"

BattleUnitConnecter::BattleUnitConnecter(DungeonSet* dungeonSet) :
m_dungeonSet(dungeonSet)
{

}

BattleUnitConnecter::~BattleUnitConnecter()
{
    for(BattleUnitCommunicatorMap::iterator iter = m_battleUnitDataMap.begin(); iter != m_battleUnitDataMap.end(); iter++)
    {
        BattleUnitCommunicator* pCommunicator = iter->second;
        CnDelete pCommunicator;
    }
    m_battleUnitDataMap.clear();
}

void BattleUnitConnecter::ConnectBattleUnit(uint32 dungeonID, const std::string& name, BattleUnit* pBattleUnit)
{
    BattleUnitCommunicatorMap::iterator battleUnitIt;
    BattleUnitCommunicator* pCommunicator = 0;
    Creature* creature = 0;

    if(pBattleUnit->GetObjectType() == OBJ_BOSS)
    {
        battleUnitIt = m_battleUnitDataMap.find(name);
        if(battleUnitIt == m_battleUnitDataMap.end())
        {
            // 根据副本的ID判断当前将要创建的BOSS属性连接器
            //if(GetDungeonClass(dungeonID) == DUNGEON_CLASS_GUILD_ID)
            //{
            //    if(dungeonID == GuildConfig::Instance()->GetCrusadeBossDungeonID())
            //    {
            //        pCommunicator = CnNew GuildBossCommunicator(m_dungeonSet);
            //    }
            //    else if(dungeonID == GuildConfig::Instance()->GetTwilightWarDungeonID())
            //    {
            //        pCommunicator = CnNew TwilightWarCommunicator(m_dungeonSet);
            //    }
            //    else
            //    {
            //        CnAssert(false);
            //    }
            //}
            //else
            {
                // 创建该世界boss的共享数据
                pCommunicator = CnNew WorldPropCommunicator(m_dungeonSet);
            }

            // 判断连通器的类型，只有部分连通器中的BOSS会根据等级加强
            switch(pCommunicator->GetType())
            {
            case BattleUnitCommunicator::TWILIGHT_WAR_COMMUNICATOR:
            case BattleUnitCommunicator::WORLD_BOSS_COMMUNICATOR:
                {
                    // 根据Boss的强壮等级，调整数据
                    creature = dynamic_cast<Creature*>(pBattleUnit);
                    if(creature)
                    {
                        WorldBossManager::Instance()->StrongWorldBoss(creature);
                    }
                }
            default:
                break;
            }

            // 共享器设置为 BattleUnit 的 HP值
            pCommunicator->SetMaxHp(pBattleUnit->GetMaxHp());
            pCommunicator->SetHp(pBattleUnit->GetHP());

            // 保存新的联动器
            m_battleUnitDataMap[name] = pCommunicator;

            // 将新的战斗单位给联动器
            pCommunicator->Connect(pBattleUnit);

            // 将联动器复制给战斗单位
            pBattleUnit->SetCommunicator(pCommunicator);
        }
        else
        {
            if(battleUnitIt->second)
            {
                // 判断连通器的类型，只有部分连通器中的BOSS会根据等级加强
                switch(battleUnitIt->second->GetType())
                {
                case BattleUnitCommunicator::TWILIGHT_WAR_COMMUNICATOR:
                case BattleUnitCommunicator::WORLD_BOSS_COMMUNICATOR:
                    {
                        // 根据Boss的强壮等级，调整数据
                        creature = dynamic_cast<Creature*>(pBattleUnit);
                        if(creature)
                        {
                            WorldBossManager::Instance()->StrongWorldBoss(creature);
                        }
                    }
                default:
                    break;
                }

                // 同步hp和maxhp给战斗单位
                pBattleUnit->SetMaxHp(battleUnitIt->second->GetMaxHp());
                pBattleUnit->SetHP(battleUnitIt->second->GetHp());

                // 将新的战斗单位给联动器
                battleUnitIt->second->Connect(pBattleUnit);

                // 将联动器赋值给战斗单位
                pBattleUnit->SetCommunicator(battleUnitIt->second);
            }
        }
    }
}

void BattleUnitConnecter::DisconnectBattleUnit(std::string name, BattleUnit* pBattleUnit)
{
    if(pBattleUnit->GetObjectType() == OBJ_BOSS)
    {
        BattleUnitCommunicatorMap::iterator delIter = m_battleUnitDataMap.find(name);
        CnAssert(delIter != m_battleUnitDataMap.end());

        BattleUnitCommunicator* pCommunicator = delIter->second;
        CnAssert(pCommunicator);

        if(!pCommunicator->HasBattleUnit(pBattleUnit))
        {
            CnAssert(false);
            return;
        }

        pCommunicator->DelBattleUnit(pBattleUnit);
        pBattleUnit->SetCommunicator(NULL);

        if(pCommunicator->GetBattleUnitNum() < 1)
        {
            CnDelete pCommunicator;
            m_battleUnitDataMap.erase(delIter);
        }
    }
}

void BattleUnitConnecter::EndConnecter(bool success, std::string* name)
{
    if(name)
    {
        BattleUnitCommunicatorMap::iterator iter = m_battleUnitDataMap.find(*name);
        if(iter != m_battleUnitDataMap.end())
        {
            iter->second->OnEnd(success);
        }
    }
    else
    {
        for(BattleUnitCommunicatorMap::iterator iter = m_battleUnitDataMap.begin();
            iter != m_battleUnitDataMap.end(); iter++)
        {
            iter->second->OnEnd(success);
        }
    }
}

void BattleUnitConnecter::Update()
{
    for(BattleUnitCommunicatorMap::iterator iter = m_battleUnitDataMap.begin(); 
            iter != m_battleUnitDataMap.end(); iter++)
    {
        BattleUnitCommunicator* pCommunicator = iter->second;
        pCommunicator->Update();
    }
}
